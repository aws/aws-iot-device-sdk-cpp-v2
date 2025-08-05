/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/eventstreamrpc/EventStreamClient.h>

#include <aws/crt/Api.h>
#include <aws/crt/auth/Credentials.h>

#include <algorithm>
#include <cstdint>
#include <cstring>

constexpr auto EVENTSTREAM_VERSION_HEADER = ":version";
constexpr auto EVENTSTREAM_VERSION_STRING = "0.1.0";
constexpr auto CONTENT_TYPE_HEADER = ":content-type";
constexpr auto CONTENT_TYPE_APPLICATION_JSON = "application/json";
constexpr auto SERVICE_MODEL_TYPE_HEADER = "service-model-type";

namespace Aws
{
    namespace Eventstreamrpc
    {
        /*
         * User-data class for completing message flush callbacks.  The C++ API supports both callbacks and
         * promise completion and so in turn this must support both.
         *
         * Additionally, because we synthesize callbacks (in order to have non-blocking destruction), this class
         * must also support multiple users trying to complete it (with the first one winning) in a thread-safe
         * manner.
         *
         * The general pattern, when needed, results in two possibilities:
         *
         * (1) Create a (shared_ptr) instance and submit it to the C API via activate()/connect()/send_message()
         * (2) Keep a shared_ptr back in the connection/continuation as needed
         * (3) C client sends message and invokes completion.  A later completion by the binding does nothing.
         *
         * or
         *
         * (1) Create a (shared_ptr) instance and submit it to the C API via activate()/connect()/send_message()
         * (2) Keep a shared_ptr back in the connection/continuation as needed
         * (3) Connection/continuation shutdown happens before the C client does the message flush, so the callbacks
         *   get triggered by the shutdown.  When the C client goes to complete, the completion does nothing (but
         *   is memory-safe).

         */
        class OnMessageFlushCallbackContainer
        {
          public:
            explicit OnMessageFlushCallbackContainer(OnMessageFlushCallback &&flushCallback) : m_sharedState({})
            {
                m_sharedState.m_state = CallbackState::Incomplete;
                m_sharedState.m_onMessageFlushCallback = std::move(flushCallback);
            }

            OnMessageFlushCallbackContainer(
                OnMessageFlushCallback &&flushCallback,
                std::promise<RpcError> &&flushPromise)
                : OnMessageFlushCallbackContainer(std::move(flushCallback))
            {
                m_sharedState.m_onFlushPromise = std::move(flushPromise);
            }

            static void Complete(OnMessageFlushCallbackContainer *callback, RpcError error)
            {
                if (callback == nullptr)
                {
                    return;
                }

                bool performCallback = false;
                OnMessageFlushCallback onMessageFlushCallback;
                std::promise<RpcError> onFlushPromise;
                {
                    std::lock_guard<std::mutex> lock(callback->m_sharedStateLock);

                    if (callback->m_sharedState.m_state == CallbackState::Incomplete)
                    {
                        performCallback = true;
                        callback->m_sharedState.m_state = CallbackState::Finished;
                        onMessageFlushCallback = std::move(callback->m_sharedState.m_onMessageFlushCallback);
                        onFlushPromise = std::move(callback->m_sharedState.m_onFlushPromise);
                    }
                }

                if (performCallback)
                {
                    if (error.crtError != AWS_ERROR_SUCCESS)
                    {
                        AWS_LOGF_ERROR(
                            AWS_LS_EVENT_STREAM_RPC_CLIENT,
                            "A CRT error occurred while attempting to send a message: %s",
                            Crt::ErrorDebugString(error.crtError));
                    }
                    onFlushPromise.set_value(error);
                    if (onMessageFlushCallback)
                    {
                        onMessageFlushCallback(error.crtError);
                    }
                }
            }

          private:
            enum class CallbackState
            {
                Incomplete,
                Finished
            };

            std::mutex m_sharedStateLock;
            struct
            {
                CallbackState m_state;
                OnMessageFlushCallback m_onMessageFlushCallback;
                std::promise<RpcError> m_onFlushPromise;
            } m_sharedState;
        };

        /*
         * We can't submit a shared_ptr to the C layer, only a raw pointer.  So this class just wraps a shared_ptr
         * to a callback container.
         */
        class OnMessageFlushCallbackContainerWrapper
        {
          public:
            OnMessageFlushCallbackContainerWrapper(Crt::Allocator *allocator, OnMessageFlushCallback &&flushCallback)
                : m_allocator(allocator),
                  m_container(
                      Aws::Crt::MakeShared<OnMessageFlushCallbackContainer>(allocator, std::move(flushCallback)))
            {
            }

            OnMessageFlushCallbackContainerWrapper(
                Crt::Allocator *allocator,
                OnMessageFlushCallback &&flushCallback,
                std::promise<RpcError> &&flushPromise)
                : m_allocator(allocator), m_container(Aws::Crt::MakeShared<OnMessageFlushCallbackContainer>(
                                              allocator,
                                              std::move(flushCallback),
                                              std::move(flushPromise)))
            {
            }

            static void Complete(OnMessageFlushCallbackContainerWrapper *wrapper, RpcError error)
            {
                if (wrapper == nullptr)
                {
                    return;
                }

                OnMessageFlushCallbackContainer::Complete(wrapper->GetContainer().get(), error);
            }

            std::shared_ptr<OnMessageFlushCallbackContainer> GetContainer() const { return m_container; }
            Crt::Allocator *GetAllocator() { return m_allocator; }

          private:
            Crt::Allocator *m_allocator;
            std::shared_ptr<OnMessageFlushCallbackContainer> m_container;
        };

        /* The flush completion callback function that the low-level C client will invoke */
        static void s_protocolMessageCallback(int errorCode, void *userData) noexcept
        {
            auto *wrapper = static_cast<OnMessageFlushCallbackContainerWrapper *>(userData);

            auto rpcStatus = (errorCode == AWS_ERROR_SUCCESS) ? EVENT_STREAM_RPC_SUCCESS : EVENT_STREAM_RPC_CRT_ERROR;
            OnMessageFlushCallbackContainerWrapper::Complete(wrapper, {rpcStatus, errorCode});
            Aws::Crt::Delete(wrapper, wrapper->GetAllocator());
        }

        MessageAmendment::MessageAmendment(Crt::Allocator *allocator) noexcept
            : m_headers(), m_payload(), m_allocator(allocator)
        {
        }

        MessageAmendment::MessageAmendment(const Crt::ByteBuf &payload, Crt::Allocator *allocator) noexcept
            : m_headers(), m_payload(Crt::ByteBufNewCopy(allocator, payload.buffer, payload.len)),
              m_allocator(allocator)
        {
        }

        MessageAmendment::MessageAmendment(
            const Crt::List<EventStreamHeader> &headers,
            Crt::Allocator *allocator) noexcept
            : m_headers(headers), m_payload(), m_allocator(allocator)
        {
        }

        MessageAmendment::MessageAmendment(Crt::List<EventStreamHeader> &&headers, Crt::Allocator *allocator) noexcept
            : m_headers(std::move(headers)), m_payload(), m_allocator(allocator)
        {
        }

        MessageAmendment::MessageAmendment(
            const Crt::List<EventStreamHeader> &headers,
            Crt::Optional<Crt::ByteBuf> &payload,
            Crt::Allocator *allocator) noexcept
            : m_headers(headers), m_payload(), m_allocator(allocator)
        {
            if (payload.has_value())
            {
                m_payload = Crt::ByteBufNewCopy(allocator, payload.value().buffer, payload.value().len);
            }
        }

        MessageAmendment::MessageAmendment(const MessageAmendment &lhs)
            : m_headers(lhs.m_headers), m_payload(), m_allocator(lhs.m_allocator)
        {
            if (lhs.m_payload.has_value())
            {
                m_payload =
                    Crt::ByteBufNewCopy(lhs.m_allocator, lhs.m_payload.value().buffer, lhs.m_payload.value().len);
            }
        }

        MessageAmendment &MessageAmendment::operator=(const MessageAmendment &lhs)
        {
            if (this != &lhs)
            {
                m_headers = lhs.m_headers;
                if (lhs.m_payload.has_value())
                {
                    m_payload =
                        Crt::ByteBufNewCopy(lhs.m_allocator, lhs.m_payload.value().buffer, lhs.m_payload.value().len);
                }
                m_allocator = lhs.m_allocator;
            }
            return *this;
        }

        MessageAmendment::MessageAmendment(MessageAmendment &&rhs) noexcept
            : m_headers(std::move(rhs.m_headers)), m_payload(std::move(rhs.m_payload)), m_allocator(rhs.m_allocator)
        {
            rhs.m_allocator = nullptr;
            rhs.m_payload = Crt::Optional<Crt::ByteBuf>();
        }

        MessageAmendment &MessageAmendment::operator=(MessageAmendment &&rhs) noexcept
        {
            if (this != &rhs)
            {
                m_headers = std::move(rhs.m_headers);
                m_payload = std::move(rhs.m_payload);
                m_allocator = rhs.m_allocator;

                rhs.m_allocator = nullptr;
                rhs.m_payload = Crt::Optional<Crt::ByteBuf>();
            }
            return *this;
        }

        const Crt::List<EventStreamHeader> &MessageAmendment::GetHeaders() const & noexcept
        {
            return m_headers;
        }

        Crt::List<EventStreamHeader> &&MessageAmendment::GetHeaders() &&
        {
            return std::move(m_headers);
        }

        const Crt::Optional<Crt::ByteBuf> &MessageAmendment::GetPayload() const & noexcept
        {
            return m_payload;
        }

        Crt::Optional<Crt::ByteBuf> &&MessageAmendment::GetPayload() &&
        {
            return std::move(m_payload);
        }

        void MessageAmendment::AddHeader(EventStreamHeader &&header) noexcept
        {
            m_headers.emplace_back(std::move(header));
        }

        void MessageAmendment::PrependHeaders(Crt::List<EventStreamHeader> &&headers)
        {
            m_headers.splice(m_headers.begin(), std::move(headers));
        }

        void MessageAmendment::SetPayload(const Crt::Optional<Crt::ByteBuf> &payload) noexcept
        {
            if (payload.has_value())
            {
                m_payload = Crt::ByteBufNewCopy(m_allocator, payload.value().buffer, payload.value().len);
            }
        }

        void MessageAmendment::SetPayload(Crt::Optional<Crt::ByteBuf> &&payload)
        {
            m_payload = std::move(payload);
        }

        MessageAmendment::~MessageAmendment() noexcept
        {
            if (m_payload.has_value())
            {
                Crt::ByteBufDelete(m_payload.value());
            }
        }

        static void s_fillNativeHeadersArray(
            const Crt::List<EventStreamHeader> &headers,
            struct aws_array_list *headersArray,
            Crt::Allocator *m_allocator = Crt::g_allocator)
        {
            AWS_ZERO_STRUCT(*headersArray);
            aws_event_stream_headers_list_init(headersArray, m_allocator);

            /* Populate the array with the underlying handle of each EventStreamHeader. */
            for (auto &i : headers)
            {
                aws_array_list_push_back(headersArray, i.GetUnderlyingHandle());
            }
        }

        Crt::String RpcError::StatusToString() const
        {
            switch (baseStatus)
            {
                case EVENT_STREAM_RPC_SUCCESS:
                    return "EVENT_STREAM_RPC_SUCCESS";
                case EVENT_STREAM_RPC_NULL_PARAMETER:
                    return "EVENT_STREAM_RPC_NULL_PARAMETER";
                case EVENT_STREAM_RPC_UNINITIALIZED:
                    return "EVENT_STREAM_RPC_UNINITIALIZED";
                case EVENT_STREAM_RPC_ALLOCATION_ERROR:
                    return "EVENT_STREAM_RPC_ALLOCATION_ERROR";
                case EVENT_STREAM_RPC_CONNECTION_SETUP_FAILED:
                    return "EVENT_STREAM_RPC_CONNECTION_SETUP_FAILED";
                case EVENT_STREAM_RPC_CONNECTION_ACCESS_DENIED:
                    return "EVENT_STREAM_RPC_CONNECTION_ACCESS_DENIED";
                case EVENT_STREAM_RPC_CONNECTION_ALREADY_ESTABLISHED:
                    return "EVENT_STREAM_RPC_CONNECTION_ALREADY_ESTABLISHED";
                case EVENT_STREAM_RPC_CONNECTION_CLOSED:
                    return "EVENT_STREAM_RPC_CONNECTION_CLOSED";
                case EVENT_STREAM_RPC_CONTINUATION_CLOSED:
                    return "EVENT_STREAM_RPC_CONTINUATION_CLOSED";
                case EVENT_STREAM_RPC_UNKNOWN_PROTOCOL_MESSAGE:
                    return "EVENT_STREAM_RPC_UNKNOWN_PROTOCOL_MESSAGE";
                case EVENT_STREAM_RPC_UNMAPPED_DATA:
                    return "EVENT_STREAM_RPC_UNMAPPED_DATA";
                case EVENT_STREAM_RPC_UNSUPPORTED_CONTENT_TYPE:
                    return "EVENT_STREAM_RPC_UNSUPPORTED_CONTENT_TYPE";
                case EVENT_STREAM_RPC_CRT_ERROR:
                {
                    Crt::String ret = "Failed with EVENT_STREAM_RPC_CRT_ERROR, the CRT error was ";
                    ret += Crt::ErrorDebugString(crtError);
                    return ret;
                }
                case EVENT_STREAM_RPC_CONTINUATION_NOT_YET_OPENED:
                    return "EVENT_STREAM_RPC_CONTINUATION_NOT_YET_OPENED";
            }
            return "Unknown status code";
        }

        EventStreamRpcStatusCode ConnectionConfig::Validate() const noexcept
        {
            if (!m_hostName.has_value() || !m_port.has_value())
            {
                return EVENT_STREAM_RPC_NULL_PARAMETER;
            }

            return EVENT_STREAM_RPC_SUCCESS;
        }

        bool ConnectionLifecycleHandler::OnErrorCallback(RpcError error)
        {
            (void)error;
            /* Returning true implies that the connection should close as a result of encountering this error. */
            return true;
        }

        void ConnectionLifecycleHandler::OnPingCallback(
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload)
        {
            (void)headers;
            (void)payload;
        }

        void ConnectionLifecycleHandler::OnConnectCallback() {}

        void ConnectionLifecycleHandler::OnDisconnectCallback(RpcError error)
        {
            (void)error;
        }

        EventStreamHeader::EventStreamHeader(
            const struct aws_event_stream_header_value_pair &header,
            Crt::Allocator *allocator)
            : m_allocator(allocator), m_valueByteBuf({}), m_underlyingHandle(header)
        {
            switch (header.header_value_type)
            {
                case AWS_EVENT_STREAM_HEADER_STRING:
                case AWS_EVENT_STREAM_HEADER_BYTE_BUF:
                    // Unsafe to copy C struct by value.  Copy the referenced buffer and fix up pointers.
                    m_valueByteBuf =
                        Crt::ByteBufNewCopy(allocator, header.header_value.variable_len_val, header.header_value_len);
                    m_underlyingHandle.header_value.variable_len_val = m_valueByteBuf.buffer;
                    m_underlyingHandle.header_value_len = static_cast<uint16_t>(m_valueByteBuf.len);
                    break;

                default:
                    // C struct can be copied by value safely
                    break;
            }
        }

        EventStreamHeader::EventStreamHeader(
            const Crt::String &name,
            const Crt::String &value,
            Crt::Allocator *allocator) noexcept
            : m_allocator(allocator),
              m_valueByteBuf(Crt::ByteBufNewCopy(allocator, (uint8_t *)value.c_str(), value.length()))
        {
            size_t length;
            if (name.length() > AWS_EVENT_STREAM_HEADER_NAME_LEN_MAX)
            {
                length = AWS_EVENT_STREAM_HEADER_NAME_LEN_MAX;
            }
            else
            {
                length = static_cast<size_t>(name.length());
            }
            (void)memcpy(m_underlyingHandle.header_name, name.c_str(), length);
            m_underlyingHandle.header_name_len = static_cast<uint8_t>(length);
            m_underlyingHandle.header_value_type = AWS_EVENT_STREAM_HEADER_STRING;
            m_underlyingHandle.header_value.variable_len_val = m_valueByteBuf.buffer;
            m_underlyingHandle.header_value_len = (uint16_t)m_valueByteBuf.len;
        }

        EventStreamHeader::~EventStreamHeader() noexcept
        {
            if (aws_byte_buf_is_valid(&m_valueByteBuf))
                Crt::ByteBufDelete(m_valueByteBuf);
        }

        EventStreamHeader::EventStreamHeader(const EventStreamHeader &lhs) noexcept
            : m_allocator(lhs.m_allocator),
              m_valueByteBuf(Crt::ByteBufNewCopy(lhs.m_allocator, lhs.m_valueByteBuf.buffer, lhs.m_valueByteBuf.len)),
              m_underlyingHandle(lhs.m_underlyingHandle)
        {
            m_underlyingHandle.header_value.variable_len_val = m_valueByteBuf.buffer;
            m_underlyingHandle.header_value_len = static_cast<uint16_t>(m_valueByteBuf.len);
        }

        EventStreamHeader &EventStreamHeader::operator=(const EventStreamHeader &rhs) noexcept
        {
            if (this != &rhs)
            {
                m_allocator = rhs.m_allocator;
                if (aws_byte_buf_is_valid(&m_valueByteBuf))
                {
                    Crt::ByteBufDelete(m_valueByteBuf);
                }
                m_valueByteBuf =
                    Crt::ByteBufNewCopy(rhs.m_allocator, rhs.m_valueByteBuf.buffer, rhs.m_valueByteBuf.len);
                m_underlyingHandle = rhs.m_underlyingHandle;
                m_underlyingHandle.header_value.variable_len_val = m_valueByteBuf.buffer;
                m_underlyingHandle.header_value_len = static_cast<uint16_t>(m_valueByteBuf.len);
            }
            return *this;
        }

        EventStreamHeader::EventStreamHeader(EventStreamHeader &&rhs) noexcept
            : m_allocator(rhs.m_allocator), m_valueByteBuf(rhs.m_valueByteBuf),
              m_underlyingHandle(rhs.m_underlyingHandle)
        {
            rhs.m_valueByteBuf.allocator = nullptr;
            rhs.m_valueByteBuf.buffer = nullptr;
        }

        const struct aws_event_stream_header_value_pair *EventStreamHeader::GetUnderlyingHandle() const
        {
            return &m_underlyingHandle;
        }

        Crt::String EventStreamHeader::GetHeaderName() const noexcept
        {
            return Crt::String(m_underlyingHandle.header_name, m_underlyingHandle.header_name_len, m_allocator);
        }

        bool EventStreamHeader::GetValueAsString(Crt::String &value) const noexcept
        {
            if (m_underlyingHandle.header_value_type != AWS_EVENT_STREAM_HEADER_STRING)
            {
                return false;
            }
            value = Crt::String(
                reinterpret_cast<char *>(m_underlyingHandle.header_value.variable_len_val),
                m_underlyingHandle.header_value_len,
                m_allocator);

            return true;
        }

        /*
         * Eventstream Connection Refactor
         *
         * For connections, we now enforce the following invariants for correctness:
         *
         * 1. No callback is made while a lock is held.  We perform callbacks by having a transactional callback context
         * that is moved out of shared state (under the lock), but the callback context does not perform its work until
         * the lock is released.
         *
         * 2. No C API call is made while the lock is held.  This prevents deadlock in degenerate cases where callbacks
         * get invoked synchronously (for example, trying to send a message on a closed connection).
         *
         * 3. No destructor blocking or synchronization.  In order to provide the best behavioral backwards
         * compatibility, we "synthesize" the callbacks that would occur at destruction when we kick off the async
         * cleanup process.  Later, when the asynchronous events occur that would normally trigger a callback occur, we
         * ignore them.
         *
         * 4. A self-reference (via shared_ptr member) guarantees the binding impl stays alive longer than the C
         * objects.   The public binding object also keeps a shared_ptr to the impl, so final destruction only occurs
         * once both the public binding object's destructor has run and no C connection object is still alive.
         *
         *
         * Invariant (2) lead to a common pattern throughout the bindings:
         *
         * (1) Lock the shared state
         * (2) Optimistically update the shared state as if action-to-take was successful
         * (3) Unlock the shared state
         * (4) Invoke the C API
         * (5) If the C API was unsuccessful (rare case), lock the shared state, rollback the optimistic update, unlock
         *
         * For this to be correct, the shared state update in (2) must exclude other pathways from trying to do the same
         * action.  For example, if a continuation is created but not activated, calling activate optimistically
         * updates the state to PENDING_ACTIVATE, which prevents concurrent callers from doing anything.  If the C
         * call to activate() fails, then we roll back from PENDING_ACTIVATE, allowing future callers to again try
         * and activate the continuation.
         */

        /* What kind of callback should this context trigger? */
        enum class ConnectionCallbackActionType
        {
            None,
            CompleteConnectPromise,
            DisconnectionCallback
        };

        /*
         * The purpose of this type is to hide much of the conditional complexity around callbacks that the
         * connection implementation faces.  Issues include:
         *   (1) When to complete a promise vs. calling a function
         *   (2) Tracking what promise to complete
         *   (3) Helping ensure callbacks always occur once and only once
         *   (4) Passing delayed state (like an error code) forward until the callback can safely be triggered
         *
         * We rely on move semantics to ensure (2) and (3)
         */
        class ConnectionCallbackContext
        {
          public:
            ConnectionCallbackContext()
                : m_action(ConnectionCallbackActionType::None), m_error{EVENT_STREAM_RPC_SUCCESS, AWS_ERROR_SUCCESS}
            {
            }

            ConnectionCallbackContext(
                const std::function<void(RpcError)> &disconnectionCallback,
                const std::function<bool(RpcError)> &errorCallback,
                const std::function<void()> &connectionSuccessCallback)
                : m_action(ConnectionCallbackActionType::CompleteConnectPromise),
                  m_error{EVENT_STREAM_RPC_SUCCESS, AWS_ERROR_SUCCESS}, m_connectPromise(),
                  m_disconnectionCallback(disconnectionCallback), m_errorCallback(errorCallback),
                  m_connectionSuccessCallback(connectionSuccessCallback)
            {
            }

            ConnectionCallbackContext(
                std::function<void(RpcError)> &&disconnectionCallback,
                std::function<bool(RpcError)> &&errorCallback,
                std::function<void()> &&connectionSuccessCallback)
                : m_action(ConnectionCallbackActionType::CompleteConnectPromise),
                  m_error{EVENT_STREAM_RPC_SUCCESS, AWS_ERROR_SUCCESS}, m_connectPromise(),
                  m_disconnectionCallback(std::move(disconnectionCallback)), m_errorCallback(std::move(errorCallback)),
                  m_connectionSuccessCallback(std::move(connectionSuccessCallback))
            {
            }

            ConnectionCallbackContext(ConnectionCallbackContext &&rhs) noexcept
                : m_action(rhs.m_action), m_error(rhs.m_error), m_connectPromise(std::move(rhs.m_connectPromise)),
                  m_disconnectionCallback(std::move(rhs.m_disconnectionCallback)),
                  m_errorCallback(std::move(rhs.m_errorCallback)),
                  m_connectionSuccessCallback(std::move(rhs.m_connectionSuccessCallback))
            {
                rhs.ClearContext();
            }

            ConnectionCallbackContext &operator=(ConnectionCallbackContext &&rhs) noexcept
            {
                m_action = rhs.m_action;
                m_error = rhs.m_error;
                m_connectPromise = std::move(rhs.m_connectPromise);
                m_disconnectionCallback = std::move(rhs.m_disconnectionCallback);
                m_errorCallback = std::move(rhs.m_errorCallback);
                m_connectionSuccessCallback = std::move(rhs.m_connectionSuccessCallback);

                rhs.ClearContext();

                return *this;
            }

            void InvokeCallbacks()
            {
                switch (m_action)
                {
                    case ConnectionCallbackActionType::CompleteConnectPromise:
                    {
                        if (m_error.crtError != AWS_ERROR_SUCCESS)
                        {
                            if (m_errorCallback)
                            {
                                (void)m_errorCallback(m_error);
                            }
                        }
                        else
                        {
                            if (m_connectionSuccessCallback)
                            {
                                m_connectionSuccessCallback();
                            }
                        }
                        m_connectPromise.set_value(m_error);
                        break;
                    }
                    case ConnectionCallbackActionType::DisconnectionCallback:
                    {
                        if (m_disconnectionCallback)
                        {
                            m_disconnectionCallback(m_error);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            /*
             * Returns a callback context that should be invoked now (indicating successful connection), and moves
             * this context into a state that should be invoked when the connection is closed.
             */
            ConnectionCallbackContext TransitionToConnected()
            {
                // connection success context caller should invoke
                ConnectionCallbackContext context = {};
                context.m_action = ConnectionCallbackActionType::CompleteConnectPromise;
                context.m_connectPromise = std::move(m_connectPromise);
                context.m_connectionSuccessCallback = m_connectionSuccessCallback;

                // we're now a disconnect context
                m_action = ConnectionCallbackActionType::DisconnectionCallback;
                m_error = {EVENT_STREAM_RPC_SUCCESS, AWS_ERROR_SUCCESS};
                m_connectPromise = {};

                return context;
            }

            void SetAction(ConnectionCallbackActionType action) { m_action = action; }
            void SetError(RpcError error)
            {
                if (m_error.baseStatus == EVENT_STREAM_RPC_SUCCESS)
                {
                    m_error = error;
                }
            }

            std::future<RpcError> GetConnectPromiseFuture() { return m_connectPromise.get_future(); }

          private:
            /* Wipes out state in a context to guarantee it does nothing if someone tries to InvokeCallbacks on it */
            void ClearContext()
            {
                m_action = ConnectionCallbackActionType::None;
                m_error = {EVENT_STREAM_RPC_SUCCESS, AWS_ERROR_SUCCESS};
                m_connectPromise = {};
                m_disconnectionCallback = {};
                m_errorCallback = {};
                m_connectionSuccessCallback = {};
            }

            ConnectionCallbackActionType m_action;
            RpcError m_error;
            std::promise<RpcError> m_connectPromise;
            std::function<void(RpcError)> m_disconnectionCallback;
            std::function<bool(RpcError)> m_errorCallback;
            std::function<void()> m_connectionSuccessCallback;
        };

        class ClientConnectionImpl final : public std::enable_shared_from_this<ClientConnectionImpl>
        {
          public:
            explicit ClientConnectionImpl(Crt::Allocator *allocator, aws_client_bootstrap *bootstrap) noexcept;
            ~ClientConnectionImpl() noexcept;

            std::future<RpcError> Connect(
                const ConnectionConfig &connectionOptions,
                ConnectionLifecycleHandler *connectionLifecycleHandler) noexcept;

            std::shared_ptr<ClientContinuationImpl> NewStream() noexcept;

            void Close() noexcept;

            bool IsOpen() const noexcept;

            // Exclusively called by ClientConnection's destructor.
            void Shutdown() noexcept;

          private:
            void CloseInternal(bool isShutdown) noexcept;
            void MoveToDisconnected(RpcError error) noexcept;

            void ConstructConnectMessage(
                struct aws_event_stream_rpc_message_args *msg_args,
                struct aws_array_list *headersArray,
                MessageAmendment &amendmentStorage) noexcept;

            enum class ClientState
            {
                // There is no C connection and no pending attempt to create one
                Disconnected,

                // There is a pending attempt to create a C connection, but it has not completed yet
                PendingConnect,

                // We have a C connection, but the Connect->Connack handshake has not completed yet
                PendingConnack,

                // We have a C connection ready to be used
                Connected,

                // Our formerly-connected C connection is in the process of shutting down
                Disconnecting,
            };

            Crt::Allocator *m_allocator;

            ConnectionLifecycleHandler *m_lifecycleHandler; // cannot be made a shared_ptr sadly
            ConnectMessageAmender m_connectMessageAmender;
            ConnectionConfig m_connectionConfig;

            // Non-null as long as something connection-related is going on in the C layer.
            std::shared_ptr<ClientConnectionImpl> m_selfReference;
            aws_client_bootstrap *m_bootstrap;
            aws_event_loop *m_eventLoop;

            mutable std::mutex m_sharedStateLock;
            struct
            {
                aws_event_stream_rpc_client_connection *m_underlyingConnection;
                ClientState m_currentState;
                ClientState m_desiredState;
                ConnectionCallbackContext m_callbackContext;
                std::shared_ptr<OnMessageFlushCallbackContainer> m_connectFlushContainer;
                bool m_hasShutDown;
            } m_sharedState;

            static void s_onConnectionShutdown(
                struct aws_event_stream_rpc_client_connection *connection,
                int errorCode,
                void *userData) noexcept;

            static void s_onConnectionSetup(
                struct aws_event_stream_rpc_client_connection *connection,
                int errorCode,
                void *userData) noexcept;

            static void s_onProtocolMessage(
                struct aws_event_stream_rpc_client_connection *connection,
                const struct aws_event_stream_rpc_message_args *messageArgs,
                void *userData) noexcept;
        };

        ClientConnectionImpl::ClientConnectionImpl(Crt::Allocator *allocator, aws_client_bootstrap *bootstrap) noexcept
            : m_allocator(allocator), m_lifecycleHandler(nullptr), m_connectMessageAmender(nullptr),
              m_bootstrap(aws_client_bootstrap_acquire(bootstrap)), m_eventLoop(nullptr),
              m_sharedState{nullptr, ClientState::Disconnected, ClientState::Disconnected, {}, nullptr, false}
        {
            m_eventLoop = aws_event_loop_group_get_next_loop(bootstrap->event_loop_group);
        }

        ClientConnectionImpl::~ClientConnectionImpl() noexcept
        {
            aws_client_bootstrap_release(m_bootstrap);
        }

        // We use a task to zero out the self reference to make sure we are not in a call stack that includes
        // a member function of the connection impl itself when potentially releasing the final reference.
        struct AwsEventstreamConnectionImplClearSharedTask
        {
            AwsEventstreamConnectionImplClearSharedTask(
                Aws::Crt::Allocator *allocator,
                ClientConnectionImpl *clientConnectionImpl) noexcept;

            struct aws_task m_task;
            struct aws_allocator *m_allocator;
            std::shared_ptr<ClientConnectionImpl> m_impl;
        };

        static void s_zeroSharedReference(struct aws_task *task, void *arg, enum aws_task_status status)
        {
            (void)task;
            (void)status;

            auto clearSharedTask = static_cast<AwsEventstreamConnectionImplClearSharedTask *>(arg);

            // implicit destructor does all the work
            Aws::Crt::Delete(clearSharedTask, clearSharedTask->m_allocator);
        }

        AwsEventstreamConnectionImplClearSharedTask::AwsEventstreamConnectionImplClearSharedTask(
            Aws::Crt::Allocator *allocator,
            ClientConnectionImpl *clientConnectionImpl) noexcept
            : m_task{}, m_allocator(allocator), m_impl(clientConnectionImpl->shared_from_this())
        {
            aws_task_init(&m_task, s_zeroSharedReference, this, "AwsEventstreamConnectionImplClearSharedTask");
        }

        /*
         * This gets called in the following cases:
         *
         * (1) Synchronous failure to initiate a connection
         * (2) Connection setup failure (async callback)
         * (3) Connection (that was successfully established) shutdown completion (async callback)
         */
        void ClientConnectionImpl::MoveToDisconnected(RpcError error) noexcept
        {
            auto *clearSharedTask =
                Aws::Crt::New<AwsEventstreamConnectionImplClearSharedTask>(m_allocator, m_allocator, this);
            ConnectionCallbackContext localContext = {};
            {
                std::lock_guard<std::mutex> lock(m_sharedStateLock);

                aws_event_stream_rpc_client_connection_release(m_sharedState.m_underlyingConnection);
                m_sharedState.m_underlyingConnection = nullptr;
                m_sharedState.m_currentState = ClientState::Disconnected;
                m_sharedState.m_desiredState = ClientState::Disconnected;
                m_sharedState.m_callbackContext.SetError(error);
                m_selfReference = nullptr; // doesn't actually drop, since the clear shared task just took a ref

                // If we've called shutdown, then no further callbacks should be done
                if (!m_sharedState.m_hasShutDown)
                {
                    localContext = std::move(m_sharedState.m_callbackContext);
                }
                m_sharedState.m_callbackContext = {};
                m_sharedState.m_connectFlushContainer = nullptr;
            }

            localContext.InvokeCallbacks();
            aws_event_loop_schedule_task_now(m_eventLoop, &clearSharedTask->m_task);
        }

        /*
         * Common logic to kick off the process of shutting down the connection
         */
        void ClientConnectionImpl::CloseInternal(bool isShutdown) noexcept
        {
            struct aws_event_stream_rpc_client_connection *closeConnection = nullptr;
            ConnectionCallbackContext localContext = {};
            std::shared_ptr<OnMessageFlushCallbackContainer> connectFlushContainer = nullptr;
            {
                std::lock_guard<std::mutex> lock(m_sharedStateLock);
                m_sharedState.m_desiredState = ClientState::Disconnected;
                m_sharedState.m_callbackContext.SetError(
                    {EVENT_STREAM_RPC_CONNECTION_CLOSED, AWS_ERROR_EVENT_STREAM_RPC_CONNECTION_CLOSED});
                if (isShutdown)
                {
                    m_sharedState.m_hasShutDown = true;
                }

                if (m_sharedState.m_currentState == ClientState::Connected ||
                    m_sharedState.m_currentState == ClientState::PendingConnack)
                {
                    m_sharedState.m_currentState = ClientState::Disconnecting;
                    closeConnection = m_sharedState.m_underlyingConnection;
                    aws_event_stream_rpc_client_connection_acquire(closeConnection);
                }

                if (isShutdown)
                {
                    /*
                     * When shutting down (ClientConnection::~ClientConnection only) we synthesize any disconnect/
                     * connection-failure callbacks that might happen in the future, and ignore them when they do.
                     * This gives us behavioral compatibility with the old implementation (that blocked in
                     * destructors) while still removing the blocking logic.
                     */
                    localContext = std::move(m_sharedState.m_callbackContext);
                    connectFlushContainer = m_sharedState.m_connectFlushContainer;
                    m_sharedState.m_connectFlushContainer = nullptr;
                }
            }

            // only does something on shutdown
            localContext.InvokeCallbacks();

            // only does something on shutdown
            OnMessageFlushCallbackContainer::Complete(
                connectFlushContainer.get(),
                {EVENT_STREAM_RPC_CONNECTION_CLOSED, AWS_ERROR_EVENT_STREAM_RPC_CONNECTION_CLOSED});

            if (closeConnection != nullptr)
            {
                aws_event_stream_rpc_client_connection_close(
                    closeConnection, AWS_ERROR_EVENT_STREAM_RPC_CONNECTION_CLOSED);
                aws_event_stream_rpc_client_connection_release(closeConnection);
            }
        }

        /*
         * Only called by the destructor of the public-facing ClientConnection object.  When this returns, no
         * further callbacks will be made.
         */
        void ClientConnectionImpl::Shutdown() noexcept
        {
            CloseInternal(true);
        }

        void ClientConnectionImpl::Close() noexcept
        {
            CloseInternal(false);
        }

        std::future<RpcError> ClientConnectionImpl::Connect(
            const ConnectionConfig &connectionConfig,
            ConnectionLifecycleHandler *connectionLifecycleHandler) noexcept
        {
            std::promise<RpcError> localPromise;
            std::future<RpcError> localFuture = localPromise.get_future();

            EventStreamRpcStatusCode status = connectionConfig.Validate();
            if (status != EVENT_STREAM_RPC_SUCCESS)
            {
                localPromise.set_value({status, AWS_ERROR_INVALID_ARGUMENT});
                return localFuture;
            }

            {
                std::lock_guard<std::mutex> lock(m_sharedStateLock);
                if (m_sharedState.m_currentState != ClientState::Disconnected)
                {
                    localPromise.set_value({EVENT_STREAM_RPC_CONNECTION_ALREADY_ESTABLISHED, AWS_ERROR_SUCCESS});
                    return localFuture;
                }

                // Optimistic state update under the lock.  Nothing can alter a PendingConnect (other than a
                // connection callback).
                m_lifecycleHandler = connectionLifecycleHandler;
                std::function<void(RpcError)> disconnectCallback = [connectionLifecycleHandler](RpcError error)
                { connectionLifecycleHandler->OnDisconnectCallback(error); };
                std::function<bool(RpcError)> errorCallback = [connectionLifecycleHandler](RpcError error)
                { return connectionLifecycleHandler->OnErrorCallback(error); };
                std::function<void()> connectionSuccessCallback = [connectionLifecycleHandler]()
                { connectionLifecycleHandler->OnConnectCallback(); };

                m_connectionConfig = connectionConfig;
                m_selfReference = shared_from_this();
                m_sharedState.m_desiredState = ClientState::Connected;
                m_sharedState.m_currentState = ClientState::PendingConnect;
                m_sharedState.m_callbackContext = {
                    std::move(disconnectCallback), std::move(errorCallback), std::move(connectionSuccessCallback)};
                localFuture = m_sharedState.m_callbackContext.GetConnectPromiseFuture();
            }

            Crt::Io::SocketOptions socketOptions;
            if (m_connectionConfig.GetSocketOptions().has_value())
            {
                socketOptions = m_connectionConfig.GetSocketOptions().value();
            }

            auto hostName = connectionConfig.GetHostName().value();
            AWS_LOGF_INFO(AWS_LS_COMMON_GENERAL, "EventStreamClient - Host name %d bytes long", (int)hostName.length());
            AWS_LOGF_INFO(AWS_LS_COMMON_GENERAL, "EventStreamClient - Host name : %s", hostName.c_str());

            struct aws_event_stream_rpc_client_connection_options connectOptions;
            AWS_ZERO_STRUCT(connectOptions);

            connectOptions.host_name = hostName.c_str();
            connectOptions.port = connectionConfig.GetPort().value();
            connectOptions.bootstrap = m_bootstrap;
            connectOptions.socket_options = &socketOptions.GetImpl();
            connectOptions.on_connection_setup = ClientConnectionImpl::s_onConnectionSetup;
            connectOptions.on_connection_protocol_message = ClientConnectionImpl::s_onProtocolMessage;
            connectOptions.on_connection_shutdown = ClientConnectionImpl::s_onConnectionShutdown;

            connectOptions.user_data = reinterpret_cast<void *>(this);

            if (m_connectionConfig.GetTlsConnectionOptions().has_value())
            {
                connectOptions.tls_options = m_connectionConfig.GetTlsConnectionOptions()->GetUnderlyingHandle();
            }

            if (aws_event_stream_rpc_client_connection_connect(m_allocator, &connectOptions))
            {
                // Rollback our optimistic state update
                MoveToDisconnected({EVENT_STREAM_RPC_CONNECTION_SETUP_FAILED, aws_last_error()});
            }

            return localFuture;
        }

        bool ClientConnectionImpl::IsOpen() const noexcept
        {
            std::lock_guard<std::mutex> lock(m_sharedStateLock);
            return m_sharedState.m_currentState == ClientState::Connected;
        }

        void ClientConnectionImpl::ConstructConnectMessage(
            struct aws_event_stream_rpc_message_args *msg_args,
            struct aws_array_list *headersArray,
            MessageAmendment &amendmentStorage) noexcept
        {
            // The version header is necessary for establishing the connection.
            amendmentStorage.AddHeader(EventStreamHeader(
                Crt::String(EVENTSTREAM_VERSION_HEADER), Crt::String(EVENTSTREAM_VERSION_STRING), m_allocator));

            if (m_connectionConfig.GetConnectAmendment().has_value())
            {
                MessageAmendment connectAmendment(m_connectionConfig.GetConnectAmendment().value());
                amendmentStorage.PrependHeaders(std::move(connectAmendment).GetHeaders());
                amendmentStorage.SetPayload(std::move(connectAmendment).GetPayload());
            }

            s_fillNativeHeadersArray(amendmentStorage.GetHeaders(), headersArray, m_allocator);

            msg_args->headers = (struct aws_event_stream_header_value_pair *)headersArray->data;
            msg_args->headers_count = amendmentStorage.GetHeaders().size();
            msg_args->payload = amendmentStorage.GetPayload().has_value()
                                    ? (aws_byte_buf *)(&(amendmentStorage.GetPayload().value()))
                                    : nullptr;
            msg_args->message_type = AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_CONNECT;
            msg_args->message_flags = 0U;
        }

        void ClientConnectionImpl::s_onConnectionSetup(
            struct aws_event_stream_rpc_client_connection *connection,
            int errorCode,
            void *userData) noexcept
        {
            auto *impl = static_cast<ClientConnectionImpl *>(userData);
            if (errorCode != AWS_ERROR_SUCCESS)
            {
                AWS_FATAL_ASSERT(connection == nullptr);
                impl->MoveToDisconnected({EVENT_STREAM_RPC_CRT_ERROR, errorCode});
                return;
            }

            struct aws_event_stream_rpc_client_connection *closeConnection = nullptr;
            struct aws_event_stream_rpc_client_connection *connectConnection = nullptr;
            OnMessageFlushCallbackContainerWrapper *connectCallbackWrapper = nullptr;

            AWS_FATAL_ASSERT(connection != nullptr);
            {
                std::lock_guard<std::mutex> lock(impl->m_sharedStateLock);
                AWS_FATAL_ASSERT(impl->m_sharedState.m_currentState == ClientState::PendingConnect);
                AWS_FATAL_ASSERT(impl->m_sharedState.m_connectFlushContainer == nullptr);

                // the channel owns the initial ref; we have to take our own
                aws_event_stream_rpc_client_connection_acquire(connection);
                impl->m_sharedState.m_underlyingConnection = connection;

                // we also temporarily take a reference because either closeConnection or clientConnection will
                // also hold a local reference outside the lock.  Not truly needed in this case due to event loop
                // pinning, but do it anyways as a pattern to follow.
                aws_event_stream_rpc_client_connection_acquire(connection);

                if (impl->m_sharedState.m_desiredState != ClientState::Connected)
                {
                    // Someone called Close/Shutdown during the connection process.  We couldn't do anything at the
                    // time, but now we can.  So start the close process.
                    AWS_FATAL_ASSERT(impl->m_sharedState.m_desiredState == ClientState::Disconnected);
                    impl->m_sharedState.m_currentState = ClientState::Disconnecting;
                    closeConnection = connection;
                }
                else
                {
                    // optimistic state update to waiting on a connack from the server
                    impl->m_sharedState.m_currentState = ClientState::PendingConnack;
                    connectConnection = connection;

                    auto callback = impl->m_connectionConfig.GetConnectRequestCallback();
                    connectCallbackWrapper = Aws::Crt::New<OnMessageFlushCallbackContainerWrapper>(
                        impl->m_allocator, impl->m_allocator, std::move(callback));
                    impl->m_sharedState.m_connectFlushContainer = connectCallbackWrapper->GetContainer();
                }
            }

            if (connectConnection)
            {
                AWS_FATAL_ASSERT(connectCallbackWrapper);

                MessageAmendment amendmentStorage;
                struct aws_event_stream_rpc_message_args msgArgs;
                struct aws_array_list headersArray;

                impl->ConstructConnectMessage(&msgArgs, &headersArray, amendmentStorage);

                int connectErrorCode = AWS_ERROR_SUCCESS;
                if (aws_event_stream_rpc_client_connection_send_protocol_message(
                        connectConnection,
                        &msgArgs,
                        s_protocolMessageCallback,
                        reinterpret_cast<void *>(connectCallbackWrapper)))
                {
                    connectErrorCode = aws_last_error();
                }

                if (aws_array_list_is_valid(&headersArray))
                {
                    aws_array_list_clean_up(&headersArray);
                }

                if (connectErrorCode != AWS_ERROR_SUCCESS)
                {
                    // rollback our optimistic connack update since we failed to send the connect.
                    // We dont need to release connectConnection because we're "transferring" it to closeConnection
                    std::lock_guard<std::mutex> lock(impl->m_sharedStateLock);

                    impl->m_sharedState.m_callbackContext.SetError({EVENT_STREAM_RPC_CRT_ERROR, connectErrorCode});
                    impl->m_sharedState.m_currentState = ClientState::Disconnecting;
                    closeConnection = connection;

                    impl->m_sharedState.m_connectFlushContainer = nullptr;
                    Aws::Crt::Delete(connectCallbackWrapper, connectCallbackWrapper->GetAllocator());
                }
                else
                {
                    aws_event_stream_rpc_client_connection_release(connectConnection);
                }
            }

            if (closeConnection)
            {
                aws_event_stream_rpc_client_connection_close(
                    closeConnection, AWS_ERROR_EVENT_STREAM_RPC_CONNECTION_CLOSED);
                aws_event_stream_rpc_client_connection_release(closeConnection);
            }
        }

        void ClientConnectionImpl::s_onConnectionShutdown(
            struct aws_event_stream_rpc_client_connection *connection,
            int errorCode,
            void *userData) noexcept
        {
            (void)connection;
            auto *impl = static_cast<ClientConnectionImpl *>(userData);

            impl->MoveToDisconnected({EVENT_STREAM_RPC_CONNECTION_CLOSED, errorCode});
        }

        void ClientConnectionImpl::s_onProtocolMessage(
            struct aws_event_stream_rpc_client_connection *connection,
            const struct aws_event_stream_rpc_message_args *messageArgs,
            void *userData) noexcept
        {
            AWS_PRECONDITION(messageArgs != nullptr);
            (void)connection;

            struct aws_event_stream_rpc_client_connection *closeConnection = nullptr;
            auto *impl = static_cast<ClientConnectionImpl *>(userData);
            switch (messageArgs->message_type)
            {
                case AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_CONNECT_ACK:
                {
                    ConnectionCallbackContext localCallbackContext = {};
                    bool successfulAck =
                        (messageArgs->message_flags & AWS_EVENT_STREAM_RPC_MESSAGE_FLAG_CONNECTION_ACCEPTED) != 0;
                    {
                        std::lock_guard<std::mutex> lock(impl->m_sharedStateLock);
                        if (impl->m_sharedState.m_currentState != ClientState::PendingConnack || !successfulAck)
                        {
                            // triggered if closed/shutdown or failed the handshake
                            if (!impl->m_sharedState.m_hasShutDown)
                            {
                                impl->m_sharedState.m_callbackContext.SetError(RpcError{
                                    successfulAck ? EVENT_STREAM_RPC_CONNECTION_CLOSED
                                                  : EVENT_STREAM_RPC_CONNECTION_ACCESS_DENIED,
                                    0});
                            }
                            impl->m_sharedState.m_desiredState = ClientState::Disconnected;
                            if (impl->m_sharedState.m_currentState != ClientState::Disconnecting)
                            {
                                impl->m_sharedState.m_currentState = ClientState::Disconnecting;
                                closeConnection = connection;
                                aws_event_stream_rpc_client_connection_acquire(closeConnection);
                            }
                        }
                        else
                        {
                            // everything's good; transition into the connected state
                            impl->m_sharedState.m_currentState = ClientState::Connected;
                            localCallbackContext = impl->m_sharedState.m_callbackContext.TransitionToConnected();
                        }
                    }
                    localCallbackContext.InvokeCallbacks();

                    if (closeConnection)
                    {
                        aws_event_stream_rpc_client_connection_close(
                            closeConnection, AWS_ERROR_EVENT_STREAM_RPC_CONNECTION_CLOSED);
                        aws_event_stream_rpc_client_connection_release(closeConnection);
                    }
                    break;
                }

                case AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_PING:
                {
                    Crt::List<EventStreamHeader> pingHeaders;
                    for (size_t i = 0; i < messageArgs->headers_count; ++i)
                    {
                        pingHeaders.emplace_back(messageArgs->headers[i], impl->m_allocator);
                    }

                    if (messageArgs->payload)
                    {
                        impl->m_lifecycleHandler->OnPingCallback(pingHeaders, *messageArgs->payload);
                    }
                    else
                    {
                        impl->m_lifecycleHandler->OnPingCallback(pingHeaders, Crt::Optional<Crt::ByteBuf>());
                    }

                    break;
                }

                case AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_PING_RESPONSE:
                    break;

                default:
                    impl->m_lifecycleHandler->OnErrorCallback(
                        {EVENT_STREAM_RPC_CRT_ERROR, AWS_ERROR_EVENT_STREAM_RPC_PROTOCOL_ERROR});
                    impl->Close();
                    break;
            }
        }

        ClientConnection::ClientConnection(Crt::Allocator *allocator, aws_client_bootstrap *bootstrap) noexcept
            : m_impl(Aws::Crt::MakeShared<ClientConnectionImpl>(allocator, allocator, bootstrap))
        {
        }

        ClientConnection::~ClientConnection() noexcept
        {
            m_impl->Shutdown();
            m_impl = nullptr;
        }

        std::future<RpcError> ClientConnection::Connect(
            const ConnectionConfig &connectionOptions,
            ConnectionLifecycleHandler *connectionLifecycleHandler) noexcept
        {
            return m_impl->Connect(connectionOptions, connectionLifecycleHandler);
        }

        std::shared_ptr<ClientContinuationImpl> ClientConnection::NewStream() noexcept
        {
            return m_impl->NewStream();
        }

        void ClientConnection::Close() noexcept
        {
            m_impl->Close();
        }

        bool ClientConnection::IsOpen() const noexcept
        {
            return m_impl->IsOpen();
        }

        OperationModelContext::OperationModelContext(const ServiceModel &serviceModel) noexcept
            : m_serviceModel(serviceModel)
        {
        }

        void OperationError::SerializeToJsonObject(Crt::JsonObject &payloadObject) const
        {
            (void)payloadObject;
        }

        void OperationError::s_customDeleter(OperationError *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(shape);
        }

        AbstractShapeBase::AbstractShapeBase() noexcept : m_allocator(nullptr) {}

        void AbstractShapeBase::s_customDeleter(AbstractShapeBase *shape) noexcept
        {
            if (shape->m_allocator != nullptr)
            {
                Crt::Delete<AbstractShapeBase>(shape, shape->m_allocator);
            }
        }

        bool StreamResponseHandler::OnStreamError(Crt::ScopedResource<OperationError> operationError, RpcError rpcError)
        {
            (void)operationError;
            (void)rpcError;
            /* Note: Always returning true implies that the stream should close
             * as a result of encountering this error. */
            return true;
        }

        void StreamResponseHandler::OnStreamEvent(Crt::ScopedResource<AbstractShapeBase> response)
        {
            (void)response;
        }

        void StreamResponseHandler::OnStreamClosed() {}

        static const EventStreamHeader *s_GetHeaderByName(
            const Crt::List<EventStreamHeader> &headers,
            const Crt::String &name) noexcept
        {
            for (const auto &header : headers)
            {
                if (name == header.GetHeaderName())
                {
                    return &header;
                }
            }
            return nullptr;
        }

        // Depending on the current state of the continuation, each inbound message falls into one of three types
        enum class EventStreamMessageRoutingType
        {
            // The inbound message should be turned into a modeled response for an Activate request
            Response,

            // The inbound message should be turned into a modeled streaming event
            Stream,

            // The inbound message should be turned into a modeled error for an Activate request
            Error
        };

        struct MessageDeserialization
        {
            MessageDeserialization() : m_route(EventStreamMessageRoutingType::Error) {}

            MessageDeserialization(EventStreamMessageRoutingType route, Crt::ScopedResource<AbstractShapeBase> shape)
                : m_route(route), m_shape(std::move(shape))
            {
            }

            MessageDeserialization(MessageDeserialization &&) = default;

            MessageDeserialization &operator=(MessageDeserialization &&) = default;

            EventStreamMessageRoutingType m_route;
            Crt::ScopedResource<AbstractShapeBase> m_shape;
        };

        struct MessageResult
        {
            MessageResult() : m_statusCode(EVENT_STREAM_RPC_SUCCESS) {}

            MessageResult(MessageResult &&) = default;

            MessageResult &operator=(MessageResult &&) = default;

            EventStreamRpcStatusCode m_statusCode;
            Crt::Optional<MessageDeserialization> m_message;
        };

        // One-way, sequential state sequence for continuations
        enum class ContinuationStateType
        {
            // Continuation has been created, but Activate has not been called yet
            None,

            // The continuation is in the process of being activated
            PendingActivate,

            // The continuation has been successfully activated
            Activated,

            // The continuation is closing
            PendingClose,

            // The continuation is closed
            Closed,
        };

        struct ContinuationSharedState
        {
            ContinuationSharedState();

            ContinuationStateType m_currentState;
            struct aws_event_stream_rpc_client_continuation_token *m_continuation;
            std::shared_ptr<OnMessageFlushCallbackContainer> m_activationCallbackContainer;
            std::function<void(EventstreamResultVariantType &&)> m_activationResponseCallback;
            std::shared_ptr<OnMessageFlushCallbackContainer> m_closeCallbackContainer;
        };

        ContinuationSharedState::ContinuationSharedState()
            : m_currentState(ContinuationStateType::None), m_continuation(nullptr),
              m_activationCallbackContainer(nullptr), m_activationResponseCallback(), m_closeCallbackContainer(nullptr)
        {
        }

        class ClientContinuationImpl : public std::enable_shared_from_this<ClientContinuationImpl>
        {
          public:
            ClientContinuationImpl(
                Aws::Crt::Allocator *allocator,
                struct aws_client_bootstrap *bootstrap,
                struct aws_event_stream_rpc_client_connection *connection) noexcept;
            virtual ~ClientContinuationImpl();

            // Only called by ClientContinuation::~ClientContinuation
            void ShutDown() noexcept;

            std::future<RpcError> Activate(
                const Crt::String &operation,
                const Crt::List<EventStreamHeader> &headers,
                const Crt::Optional<Crt::ByteBuf> &payload,
                MessageType messageType,
                uint32_t messageFlags,
                std::function<void(EventstreamResultVariantType &&)> &&onResultCallback,
                OnMessageFlushCallback &&onMessageFlushCallback) noexcept;

            std::future<RpcError> SendStreamMessage(
                const Crt::List<EventStreamHeader> &headers,
                const Crt::Optional<Crt::ByteBuf> &payload,
                MessageType messageType,
                uint32_t messageFlags,
                OnMessageFlushCallback &&onMessageFlushCallback);

            std::future<RpcError> Close(OnMessageFlushCallback &&onMessageFlushCallback = nullptr) noexcept;

            Crt::String GetModelName() const noexcept;

            void Initialize(
                const std::shared_ptr<OperationModelContext> &operationModelContext,
                std::shared_ptr<StreamResponseHandler> streamHandler) noexcept
            {
                m_operationModelContext = operationModelContext;
                m_streamHandler = std::move(streamHandler);
                if (m_sharedState.m_continuation != nullptr)
                {
                    m_selfReference = shared_from_this();
                }
            }

          private:
            MessageResult DeserializeRawMessage(
                const struct aws_event_stream_rpc_message_args *rawMessage,
                bool shouldBeActivationResponse) noexcept;

            void OnClosed() noexcept;

            void OnMessage(const struct aws_event_stream_rpc_message_args *messageArgs) noexcept;

            EventStreamRpcStatusCode GetEmptyContinuationStatusCode() const;

            static void s_OnContinuationClosed(
                struct aws_event_stream_rpc_client_continuation_token *token,
                void *user_data) noexcept;

            static void s_OnContinuationMessage(
                struct aws_event_stream_rpc_client_continuation_token *token,
                const struct aws_event_stream_rpc_message_args *message_args,
                void *user_data) noexcept;

            static void s_OnContinuationTerminated(void *user_data) noexcept;

            static void ReleaseContinuation(
                Aws::Crt::Allocator *allocator,
                const struct aws_client_bootstrap *bootstrap,
                struct aws_event_stream_rpc_client_continuation_token *continuation) noexcept;

            Aws::Crt::Allocator *m_allocator;

            struct aws_client_bootstrap *m_clientBootstrap;
            struct aws_event_loop *m_connectionEventLoop;

            std::shared_ptr<ClientContinuationImpl> m_selfReference;

            std::mutex m_sharedStateLock;
            ContinuationSharedState m_sharedState;

            std::shared_ptr<OperationModelContext> m_operationModelContext;

            /*
             * A small exception to our "never hold a lock while making callbacks" rule.
             *
             * The handler lock is "one-way"; it is held while invoking callbacks on the stream handler.  We also
             * conditionally hold it while zeroing the handler on shutdown.  Because we do not need the lock for
             * user-facing functions outside ShutDown, deadlock is not a risk.
             *
             * THis allows us to guarantee that streaming events/errors are not emitted after ShutDown is called,
             * even while asynchronous C shutdown is in-progress.
             */
            std::mutex m_streamHandlerLock;
            std::shared_ptr<StreamResponseHandler> m_streamHandler;

            /*
             * An unfortunate bit of state needed to differentiate between two cases:
             *  (1) Failure to create the original continuation for any reason
             *  (2) A fully-closed continuation
             *
             * We want to differentiate these cases in order to communicate clear error codes when attempting
             * to operate on the continuation.
             *
             * This is always true in case (2) and false in case (1)
             */
            bool m_continuationValid;
        };

        ClientContinuationImpl::ClientContinuationImpl(
            Aws::Crt::Allocator *allocator,
            struct aws_client_bootstrap *bootstrap,
            struct aws_event_stream_rpc_client_connection *connection) noexcept
            : m_allocator(allocator), m_clientBootstrap(aws_client_bootstrap_acquire(bootstrap)),
              m_connectionEventLoop(aws_event_stream_rpc_client_connection_get_event_loop(connection)), m_sharedState(),
              m_operationModelContext(nullptr), m_continuationValid(true)
        {
            if (connection != nullptr)
            {
                struct aws_event_stream_rpc_client_stream_continuation_options continuation_options;
                AWS_ZERO_STRUCT(continuation_options);
                continuation_options.on_continuation = s_OnContinuationMessage;
                continuation_options.on_continuation_closed = s_OnContinuationClosed;
                continuation_options.on_continuation_terminated = s_OnContinuationTerminated;
                continuation_options.user_data = this;

                m_sharedState.m_continuation =
                    aws_event_stream_rpc_client_connection_new_stream(connection, &continuation_options);
            }

            if (!m_sharedState.m_continuation)
            {
                m_continuationValid = false;
                m_sharedState.m_currentState = ContinuationStateType::Closed;
            }
        }

        ClientContinuationImpl::~ClientContinuationImpl()
        {
            aws_client_bootstrap_release(m_clientBootstrap);
        }

        // We use a task to release the C continuation to make it impossible to trigger the termination callback
        // in a callstack that includes ClientContinuationImpl methods or state.
        //
        // If we were the last ref holder then release would trigger the termination callback further down the
        // callstack.  This isn't necessarily unsafe, but for peace-of-mind, it's best for destruction to never
        // get invoked on an object that has methods within the callstack.
        struct AwsEventstreamContinuationReleaseTask
        {
            AwsEventstreamContinuationReleaseTask(
                Aws::Crt::Allocator *allocator,
                struct aws_event_stream_rpc_client_continuation_token *continuation) noexcept;

            struct aws_task m_task;
            struct aws_allocator *m_allocator;
            struct aws_event_stream_rpc_client_continuation_token *m_continuation;
        };

        static void s_releaseContinuation(struct aws_task *task, void *arg, enum aws_task_status status)
        {
            (void)task;
            (void)status;

            auto releaseTask = static_cast<AwsEventstreamContinuationReleaseTask *>(arg);

            aws_event_stream_rpc_client_continuation_release(releaseTask->m_continuation);

            Aws::Crt::Delete(releaseTask, releaseTask->m_allocator);
        }

        AwsEventstreamContinuationReleaseTask::AwsEventstreamContinuationReleaseTask(
            Aws::Crt::Allocator *allocator,
            struct aws_event_stream_rpc_client_continuation_token *continuation) noexcept
            : m_task{}, m_allocator(allocator), m_continuation(continuation)
        {
            aws_task_init(&m_task, s_releaseContinuation, this, "AwsEventstreamContinuationReleaseTask");
        }

        void ClientContinuationImpl::ReleaseContinuation(
            Aws::Crt::Allocator *allocator,
            const struct aws_client_bootstrap *bootstrap,
            struct aws_event_stream_rpc_client_continuation_token *continuation) noexcept
        {
            if (continuation == nullptr)
            {
                return;
            }

            AWS_FATAL_ASSERT(bootstrap != NULL);

            // We use the bootstrap rather than the connection's event loop because we might not have a connection
            struct aws_event_loop *event_loop = aws_event_loop_group_get_next_loop(bootstrap->event_loop_group);
            AWS_FATAL_ASSERT(event_loop != NULL);

            auto releaseTask = Aws::Crt::New<AwsEventstreamContinuationReleaseTask>(allocator, allocator, continuation);
            aws_event_loop_schedule_task_now(event_loop, &releaseTask->m_task);
        }

        void ClientContinuationImpl::ShutDown() noexcept
        {
            struct aws_event_stream_rpc_client_continuation_token *releaseContinuation = nullptr;
            std::shared_ptr<OnMessageFlushCallbackContainer> closeCallbackContainer = nullptr;
            std::shared_ptr<OnMessageFlushCallbackContainer> activationCallbackContainer = nullptr;
            std::function<void(EventstreamResultVariantType &&)> activationResponseCallback = nullptr;

            // This block prevents streaming event callbacks from triggering after scope exit
            {
                if (m_connectionEventLoop != nullptr)
                {
                    // If we're on the connection's event loop thread then we risk deadlock by trying to use
                    // the handler lock.  But in that case, it's safe not to use it, because we only use the lock from
                    // the event loop (or the single call to ShutDown)
                    bool useHandlerLock = !aws_event_loop_thread_is_callers_thread(m_connectionEventLoop);
                    if (useHandlerLock)
                    {
                        m_streamHandlerLock.lock();
                    }

                    m_streamHandler = nullptr;

                    if (useHandlerLock)
                    {
                        m_streamHandlerLock.unlock();
                    }
                }
            }

            {
                // update state and gather up all the callbacks we need to synthesize
                std::lock_guard<std::mutex> lock(m_sharedStateLock);
                if (m_sharedState.m_currentState == ContinuationStateType::None)
                {
                    releaseContinuation = m_sharedState.m_continuation;
                    m_sharedState.m_continuation = nullptr;
                    m_sharedState.m_currentState = ContinuationStateType::Closed;
                }

                activationCallbackContainer = m_sharedState.m_activationCallbackContainer;
                m_sharedState.m_activationCallbackContainer = nullptr;

                closeCallbackContainer = m_sharedState.m_closeCallbackContainer;
                m_sharedState.m_closeCallbackContainer = nullptr;

                activationResponseCallback = std::move(m_sharedState.m_activationResponseCallback);
                m_sharedState.m_activationResponseCallback = nullptr;
            }

            // synthesize callbacks

            if (activationResponseCallback)
            {
                activationResponseCallback(
                    EventstreamResultVariantType(RpcError{EVENT_STREAM_RPC_CONTINUATION_CLOSED, AWS_ERROR_SUCCESS}));
            }

            // Short-circuit and simulate both activate and close callbacks as necessary.
            OnMessageFlushCallbackContainer::Complete(
                activationCallbackContainer.get(), {EVENT_STREAM_RPC_CONTINUATION_CLOSED, AWS_ERROR_SUCCESS});

            OnMessageFlushCallbackContainer::Complete(
                closeCallbackContainer.get(), {EVENT_STREAM_RPC_CONTINUATION_CLOSED, AWS_ERROR_SUCCESS});

            if (releaseContinuation != nullptr)
            {
                // An unactivated continuation should be released directly
                ReleaseContinuation(m_allocator, m_clientBootstrap, releaseContinuation);
            }
            else
            {
                // A continuation that has at least started the activation process should be closed first
                Close();
            }
        }

        EventStreamRpcStatusCode ClientContinuationImpl::GetEmptyContinuationStatusCode() const
        {
            if (m_continuationValid)
            {
                return EVENT_STREAM_RPC_CONTINUATION_CLOSED;
            }

            return EVENT_STREAM_RPC_CONNECTION_CLOSED;
        }

        std::future<RpcError> ClientContinuationImpl::Activate(
            const Crt::String &operation,
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            MessageType messageType,
            uint32_t messageFlags,
            std::function<void(EventstreamResultVariantType &&)> &&onResultCallback,
            OnMessageFlushCallback &&onMessageFlushCallback) noexcept
        {
            AWS_FATAL_ASSERT(static_cast<bool>(onResultCallback));

            int result = AWS_OP_ERR;
            std::promise<RpcError> activationPromise;
            std::future<RpcError> activationFuture = activationPromise.get_future();
            auto activationContainerWrapper = Crt::New<OnMessageFlushCallbackContainerWrapper>(
                m_allocator, m_allocator, std::move(onMessageFlushCallback), std::move(activationPromise));
            RpcError activationError = {};
            struct aws_event_stream_rpc_client_continuation_token *activateContinuation = nullptr;

            {
                std::lock_guard<std::mutex> lock(m_sharedStateLock);
                if (m_sharedState.m_continuation != nullptr)
                {
                    if (m_sharedState.m_currentState == ContinuationStateType::None)
                    {
                        // optimistically update shared state to pending activate
                        activateContinuation = m_sharedState.m_continuation;
                        aws_event_stream_rpc_client_continuation_acquire(activateContinuation);
                        m_sharedState.m_currentState = ContinuationStateType::PendingActivate;
                        m_sharedState.m_activationCallbackContainer = activationContainerWrapper->GetContainer();
                        m_sharedState.m_activationResponseCallback = std::move(onResultCallback);
                    }
                    else
                    {
                        // we're not in a state we can activate
                        switch (m_sharedState.m_currentState)
                        {
                            case ContinuationStateType::PendingActivate:
                            case ContinuationStateType::Activated:
                                // simulate what would be returned if we invoked the C API
                                activationError = {EVENT_STREAM_RPC_CRT_ERROR, AWS_ERROR_INVALID_STATE};
                                break;

                            case ContinuationStateType::PendingClose:
                            case ContinuationStateType::Closed:
                                activationError = {GetEmptyContinuationStatusCode(), 0};
                                break;

                            default:
                                AWS_FATAL_ASSERT(false);
                                break;
                        }
                    }
                }
                else
                {
                    activationError = {GetEmptyContinuationStatusCode(), 0};
                }
            }

            if (activateContinuation)
            {
                struct aws_array_list
                    headersArray; // guaranteed to be zeroed or valid if we reach the end of the function

                // cleanup requirements mean we can't early out from here on
                s_fillNativeHeadersArray(headers, &headersArray, m_allocator);

                struct aws_event_stream_rpc_message_args msg_args;
                msg_args.headers = static_cast<struct aws_event_stream_header_value_pair *>(headersArray.data);
                msg_args.headers_count = headers.size();
                msg_args.payload = payload.has_value() ? (aws_byte_buf *)(&(payload.value())) : nullptr;
                msg_args.message_type = messageType;
                msg_args.message_flags = messageFlags;

                result = aws_event_stream_rpc_client_continuation_activate(
                    activateContinuation,
                    Crt::ByteCursorFromCString(operation.c_str()),
                    &msg_args,
                    s_protocolMessageCallback,
                    reinterpret_cast<void *>(activationContainerWrapper));

                if (result != AWS_OP_SUCCESS)
                {
                    // ah shucks, we failed, rollback our optimistic shared state update
                    std::lock_guard<std::mutex> lock(m_sharedStateLock);
                    m_sharedState.m_currentState = ContinuationStateType::None;
                    m_sharedState.m_activationCallbackContainer = nullptr;
                    m_sharedState.m_activationResponseCallback = nullptr;

                    activationError = {EVENT_STREAM_RPC_CRT_ERROR, aws_last_error()};
                }

                if (aws_array_list_is_valid(&headersArray))
                {
                    aws_array_list_clean_up(&headersArray);
                }

                aws_event_stream_rpc_client_continuation_release(activateContinuation);
            }

            if (result != AWS_OP_SUCCESS)
            {
                AWS_FATAL_ASSERT(activationContainerWrapper != nullptr);

                OnMessageFlushCallbackContainerWrapper::Complete(activationContainerWrapper, activationError);
                Aws::Crt::Delete(activationContainerWrapper, activationContainerWrapper->GetAllocator());
            }

            return activationFuture;
        }

        std::future<RpcError> ClientContinuationImpl::SendStreamMessage(
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            MessageType messageType,
            uint32_t messageFlags,
            OnMessageFlushCallback &&onMessageFlushCallback)
        {
            int result = AWS_OP_ERR;
            std::promise<RpcError> sendPromise;
            std::future<RpcError> sendFuture = sendPromise.get_future();
            auto sendContainerWrapper = Crt::New<OnMessageFlushCallbackContainerWrapper>(
                m_allocator, m_allocator, std::move(onMessageFlushCallback), std::move(sendPromise));
            RpcError sendError = {};
            struct aws_event_stream_rpc_client_continuation_token *sendContinuation = nullptr;
            {
                std::lock_guard<std::mutex> lock(m_sharedStateLock);
                if (m_sharedState.m_continuation != nullptr)
                {
                    if (m_sharedState.m_currentState == ContinuationStateType::Activated)
                    {
                        // it's safe to send a stream message
                        sendContinuation = m_sharedState.m_continuation;
                        aws_event_stream_rpc_client_continuation_acquire(sendContinuation);
                    }
                    else
                    {
                        // we're not in a state to send a stream message
                        switch (m_sharedState.m_currentState)
                        {
                            case ContinuationStateType::PendingActivate:
                            case ContinuationStateType::None:
                                sendError = {EVENT_STREAM_RPC_CONTINUATION_NOT_YET_OPENED, 0};
                                break;

                            case ContinuationStateType::PendingClose:
                            case ContinuationStateType::Closed:
                                sendError = {EVENT_STREAM_RPC_CONTINUATION_CLOSED, 0};
                                break;

                            default:
                                AWS_FATAL_ASSERT(false);
                                break;
                        }
                    }
                }
                else
                {
                    sendError = {GetEmptyContinuationStatusCode(), 0};
                }
            }

            if (sendContinuation)
            {
                struct aws_array_list headersArray;
                s_fillNativeHeadersArray(headers, &headersArray, m_allocator);

                struct aws_event_stream_rpc_message_args msg_args;
                msg_args.headers = static_cast<struct aws_event_stream_header_value_pair *>(headersArray.data);
                msg_args.headers_count = headers.size();
                msg_args.payload = payload.has_value() ? (aws_byte_buf *)(&(payload.value())) : nullptr;
                msg_args.message_type = messageType;
                msg_args.message_flags = messageFlags;

                result = aws_event_stream_rpc_client_continuation_send_message(
                    sendContinuation,
                    &msg_args,
                    s_protocolMessageCallback,
                    reinterpret_cast<void *>(sendContainerWrapper));

                if (result != AWS_OP_SUCCESS)
                {
                    sendError = {EVENT_STREAM_RPC_CRT_ERROR, aws_last_error()};
                }

                if (aws_array_list_is_valid(&headersArray))
                {
                    aws_array_list_clean_up(&headersArray);
                }

                aws_event_stream_rpc_client_continuation_release(sendContinuation);
            }

            if (result == AWS_OP_ERR)
            {
                OnMessageFlushCallbackContainerWrapper::Complete(sendContainerWrapper, sendError);
                Aws::Crt::Delete(sendContainerWrapper, sendContainerWrapper->GetAllocator());
            }

            return sendFuture;
        }

        std::future<RpcError> ClientContinuationImpl::Close(OnMessageFlushCallback &&onMessageFlushCallback) noexcept
        {
            int closeResult = AWS_OP_ERR;
            std::promise<RpcError> closePromise;
            std::future<RpcError> closeFuture = closePromise.get_future();
            auto closeCallbackContainerWrapper = Crt::New<OnMessageFlushCallbackContainerWrapper>(
                m_allocator, m_allocator, std::move(onMessageFlushCallback), std::move(closePromise));
            RpcError closeError = {};
            struct aws_event_stream_rpc_client_continuation_token *closeContinuation = nullptr;

            // needed to correctly roll back on failure to send the end-stream message
            ContinuationStateType previousState = ContinuationStateType::PendingClose;

            {
                std::lock_guard<std::mutex> lock(m_sharedStateLock);
                previousState = m_sharedState.m_currentState;

                if (m_sharedState.m_continuation != nullptr)
                {
                    if (m_sharedState.m_currentState == ContinuationStateType::PendingActivate ||
                        m_sharedState.m_currentState == ContinuationStateType::Activated)
                    {
                        m_sharedState.m_currentState = ContinuationStateType::PendingClose;

                        closeContinuation = m_sharedState.m_continuation;
                        aws_event_stream_rpc_client_continuation_acquire(closeContinuation);

                        m_sharedState.m_closeCallbackContainer = closeCallbackContainerWrapper->GetContainer();
                    }
                    else
                    {
                        // not in a valid state to close
                        closeError = {EVENT_STREAM_RPC_CONTINUATION_CLOSED, 0};
                    }
                }
                else
                {
                    closeError = {GetEmptyContinuationStatusCode(), 0};
                }
            }

            if (closeContinuation)
            {
                struct aws_event_stream_rpc_message_args msg_args;
                msg_args.headers = nullptr;
                msg_args.headers_count = 0;
                msg_args.payload = nullptr;
                msg_args.message_type = AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_APPLICATION_MESSAGE;
                msg_args.message_flags = AWS_EVENT_STREAM_RPC_MESSAGE_FLAG_TERMINATE_STREAM;

                closeResult = aws_event_stream_rpc_client_continuation_send_message(
                    closeContinuation,
                    &msg_args,
                    s_protocolMessageCallback,
                    reinterpret_cast<void *>(closeCallbackContainerWrapper));

                if (closeResult == AWS_OP_ERR)
                {
                    closeError = {EVENT_STREAM_RPC_CRT_ERROR, aws_last_error()};

                    std::lock_guard<std::mutex> lock(m_sharedStateLock);
                    m_sharedState.m_closeCallbackContainer = nullptr;
                    if (m_sharedState.m_currentState != ContinuationStateType::Closed)
                    {
                        // Rollback to previous state
                        m_sharedState.m_currentState = previousState;
                    }
                }

                aws_event_stream_rpc_client_continuation_release(closeContinuation);
            }

            if (closeResult != AWS_OP_SUCCESS)
            {
                OnMessageFlushCallbackContainerWrapper::Complete(closeCallbackContainerWrapper, closeError);
                Aws::Crt::Delete(closeCallbackContainerWrapper, closeCallbackContainerWrapper->GetAllocator());
            }

            return closeFuture;
        }

        void ClientContinuationImpl::OnClosed() noexcept
        {
            struct aws_event_stream_rpc_client_continuation_token *releaseContinuation = nullptr;
            std::shared_ptr<OnMessageFlushCallbackContainer> closeCallbackContainer = nullptr;
            std::shared_ptr<OnMessageFlushCallbackContainer> activationCallbackContainer = nullptr;
            std::function<void(EventstreamResultVariantType &&)> activationResponseCallback;

            {
                std::lock_guard<std::mutex> lock(m_sharedStateLock);

                m_sharedState.m_currentState = ContinuationStateType::Closed;
                releaseContinuation = m_sharedState.m_continuation;
                m_sharedState.m_continuation = nullptr;

                activationCallbackContainer = m_sharedState.m_activationCallbackContainer;
                m_sharedState.m_activationCallbackContainer = nullptr;

                closeCallbackContainer = m_sharedState.m_closeCallbackContainer;
                m_sharedState.m_closeCallbackContainer = nullptr;

                activationResponseCallback = std::move(m_sharedState.m_activationResponseCallback);
                m_sharedState.m_activationResponseCallback = nullptr;
            }

            if (activationResponseCallback)
            {
                activationResponseCallback(
                    EventstreamResultVariantType(RpcError{EVENT_STREAM_RPC_CONTINUATION_CLOSED, AWS_ERROR_SUCCESS}));
            }

            OnMessageFlushCallbackContainer::Complete(
                activationCallbackContainer.get(), {EVENT_STREAM_RPC_CONTINUATION_CLOSED, AWS_ERROR_SUCCESS});

            OnMessageFlushCallbackContainer::Complete(
                closeCallbackContainer.get(), {EVENT_STREAM_RPC_CONTINUATION_CLOSED, AWS_ERROR_SUCCESS});

            ReleaseContinuation(m_allocator, m_clientBootstrap, releaseContinuation);
        }

        MessageResult ClientContinuationImpl::DeserializeRawMessage(
            const struct aws_event_stream_rpc_message_args *rawMessage,
            bool shouldBeActivationResponse) noexcept
        {
            MessageResult result;

            Crt::List<EventStreamHeader> continuationMessageHeaders;
            for (size_t i = 0; i < rawMessage->headers_count; ++i)
            {
                continuationMessageHeaders.emplace_back(rawMessage->headers[i], m_allocator);
            }

            Crt::Optional<Crt::ByteBuf> payload;
            if (rawMessage->payload)
            {
                payload = Crt::Optional<Crt::ByteBuf>(*rawMessage->payload);
            }
            else
            {
                payload = Crt::Optional<Crt::ByteBuf>();
            }

            const EventStreamHeader *modelHeader =
                s_GetHeaderByName(continuationMessageHeaders, Crt::String(SERVICE_MODEL_TYPE_HEADER));
            if (modelHeader == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_EVENT_STREAM_RPC_CLIENT,
                    "A required header (%s) could not be found in the message.",
                    SERVICE_MODEL_TYPE_HEADER);
                result.m_statusCode = EVENT_STREAM_RPC_UNMAPPED_DATA;
                return result;
            }

            Crt::String modelName;
            modelHeader->GetValueAsString(modelName);
            if (rawMessage->message_type == AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_APPLICATION_MESSAGE)
            {
                if (shouldBeActivationResponse)
                {
                    if (m_operationModelContext->GetInitialResponseModelName() != modelName)
                    {
                        AWS_LOGF_ERROR(
                            AWS_LS_EVENT_STREAM_RPC_CLIENT,
                            "The model name of the initial response did not match its expected model name.");
                        result.m_statusCode = EVENT_STREAM_RPC_UNMAPPED_DATA;
                        return result;
                    }
                }
                else
                {
                    if (m_operationModelContext->GetStreamingResponseModelName().has_value() &&
                        m_operationModelContext->GetStreamingResponseModelName().value() != modelName)
                    {
                        AWS_LOGF_ERROR(
                            AWS_LS_EVENT_STREAM_RPC_CLIENT,
                            "The model name of a subsequent response did not match its expected model name.");
                        result.m_statusCode = EVENT_STREAM_RPC_UNMAPPED_DATA;
                        return result;
                    }
                }
            }

            const EventStreamHeader *contentHeader =
                s_GetHeaderByName(continuationMessageHeaders, Crt::String(CONTENT_TYPE_HEADER));
            if (contentHeader == nullptr)
            {
                /* Missing required content type header. */
                AWS_LOGF_ERROR(
                    AWS_LS_EVENT_STREAM_RPC_CLIENT,
                    "A required header (%s) could not be found in the message.",
                    CONTENT_TYPE_HEADER);
                result.m_statusCode = EVENT_STREAM_RPC_UNSUPPORTED_CONTENT_TYPE;
                return result;
            }

            Crt::String contentType;
            if (contentHeader->GetValueAsString(contentType))
            {
                if (contentType != CONTENT_TYPE_APPLICATION_JSON)
                {
                    /* Unrecognized content type header value. */
                    AWS_LOGF_ERROR(
                        AWS_LS_EVENT_STREAM_RPC_CLIENT,
                        "The content type (%s) header was specified with an unsupported value (%s).",
                        CONTENT_TYPE_HEADER,
                        contentType.c_str());
                    result.m_statusCode = EVENT_STREAM_RPC_UNSUPPORTED_CONTENT_TYPE;
                    return result;
                }
            }
            else
            {
                /* Invalid content type header value type. */
                AWS_LOGF_ERROR(
                    AWS_LS_EVENT_STREAM_RPC_CLIENT,
                    "The content type (%s) header value was not a string type (%d)",
                    CONTENT_TYPE_HEADER,
                    (int)(contentHeader->GetUnderlyingHandle()->header_value_type));
                result.m_statusCode = EVENT_STREAM_RPC_UNSUPPORTED_CONTENT_TYPE;
                return result;
            }

            Crt::StringView payloadStringView;
            if (payload.has_value())
            {
                payloadStringView = Crt::ByteCursorToStringView(Crt::ByteCursorFromByteBuf(payload.value()));
            }

            if (rawMessage->message_type == AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_APPLICATION_MESSAGE)
            {
                if (shouldBeActivationResponse)
                {
                    result.m_message = MessageDeserialization{
                        EventStreamMessageRoutingType::Response,
                        m_operationModelContext->AllocateInitialResponseFromPayload(payloadStringView, m_allocator)};
                }
                else
                {
                    result.m_message = MessageDeserialization{
                        EventStreamMessageRoutingType::Stream,
                        m_operationModelContext->AllocateStreamingResponseFromPayload(payloadStringView, m_allocator)};
                }
            }
            else
            {
                auto errorShape = m_operationModelContext
                                      ->AllocateOperationErrorFromPayload(modelName, payloadStringView, m_allocator)
                                      .release();
                Crt::Allocator *allocator = m_allocator;
                result.m_message = MessageDeserialization{
                    EventStreamMessageRoutingType::Error,
                    Crt::ScopedResource<AbstractShapeBase>(
                        errorShape, [allocator](AbstractShapeBase *shape) { Crt::Delete(shape, allocator); })};
            }

            if (result.m_message.value().m_shape.get() == nullptr)
            {
                result.m_statusCode = EVENT_STREAM_RPC_UNMAPPED_DATA;
            }

            return result;
        }

        void ClientContinuationImpl::OnMessage(const struct aws_event_stream_rpc_message_args *messageArgs) noexcept
        {
            MessageResult result;
            bool isResponse = false;
            bool shouldClose = false;
            std::function<void(EventstreamResultVariantType &&)> activationResultCallback = nullptr;

            {
                std::lock_guard<std::mutex> lock(m_sharedStateLock);
                isResponse = static_cast<bool>(m_sharedState.m_currentState == ContinuationStateType::PendingActivate);
                result = DeserializeRawMessage(messageArgs, isResponse);

                if (isResponse)
                {
                    activationResultCallback = std::move(m_sharedState.m_activationResponseCallback);
                    m_sharedState.m_activationResponseCallback = nullptr;
                    m_sharedState.m_activationCallbackContainer = nullptr;

                    if (result.m_statusCode == EVENT_STREAM_RPC_SUCCESS &&
                        result.m_message.value().m_route == EventStreamMessageRoutingType::Response)
                    {
                        if ((messageArgs->message_flags & AWS_EVENT_STREAM_RPC_MESSAGE_FLAG_TERMINATE_STREAM) == 0)
                        {
                            m_sharedState.m_currentState = ContinuationStateType::Activated;
                        }
                        else
                        {
                            /*
                             * The underlying implementation is going to close underneath us.  No need to send
                             * an empty terminate stream message, which older server versions can fail on.
                             */
                            m_sharedState.m_currentState = ContinuationStateType::PendingClose;
                        }
                    }
                    else
                    {
                        shouldClose = true;
                    }
                }
            }

            if (activationResultCallback)
            {
                if (result.m_statusCode == EVENT_STREAM_RPC_SUCCESS)
                {
                    const auto &message = result.m_message.value();
                    if (message.m_route == EventStreamMessageRoutingType::Response)
                    {
                        activationResultCallback(
                            EventstreamResultVariantType(std::move(result.m_message.value().m_shape)));
                    }
                    else
                    {
                        auto errorResponse = Crt::SafeSubCast<AbstractShapeBase, OperationError>(
                            std::move(result.m_message.value().m_shape));
                        activationResultCallback(EventstreamResultVariantType(std::move(errorResponse)));
                    }
                }
                else
                {
                    activationResultCallback(EventstreamResultVariantType(RpcError{result.m_statusCode, 0}));
                }
            }

            if (!isResponse)
            {
                std::lock_guard<std::mutex> handlerLock(m_streamHandlerLock);

                auto streamHandler = m_streamHandler;
                if (streamHandler)
                {
                    if (result.m_message.has_value())
                    {
                        auto shape = std::move(result.m_message.value().m_shape);
                        auto route = result.m_message.value().m_route;
                        switch (route)
                        {
                            case EventStreamMessageRoutingType::Stream:
                            {

                                streamHandler->OnStreamEvent(std::move(shape));
                                break;
                            }

                            case EventStreamMessageRoutingType::Error:
                            {
                                auto errorResponse =
                                    Crt::SafeSubCast<AbstractShapeBase, OperationError>(std::move(shape));
                                shouldClose = streamHandler->OnStreamError(
                                    std::move(errorResponse), {EVENT_STREAM_RPC_SUCCESS, 0});
                                break;
                            }

                            default:
                                AWS_FATAL_ASSERT(false);
                        }
                    }
                    else
                    {
                        shouldClose = streamHandler->OnStreamError(nullptr, {result.m_statusCode, 0});
                    }
                }
            }

            if (shouldClose)
            {
                Close();
            }
        }

        void ClientContinuationImpl::s_OnContinuationClosed(
            struct aws_event_stream_rpc_client_continuation_token *token,
            void *user_data) noexcept
        {
            (void)token;

            auto impl = reinterpret_cast<ClientContinuationImpl *>(user_data);
            impl->OnClosed();
        }

        void ClientContinuationImpl::s_OnContinuationMessage(
            struct aws_event_stream_rpc_client_continuation_token *token,
            const struct aws_event_stream_rpc_message_args *message_args,
            void *user_data) noexcept
        {
            (void)token;

            auto impl = reinterpret_cast<ClientContinuationImpl *>(user_data);
            impl->OnMessage(message_args);
        }

        void ClientContinuationImpl::s_OnContinuationTerminated(void *user_data) noexcept
        {
            auto impl = reinterpret_cast<ClientContinuationImpl *>(user_data);

            // We don't need to put this on an event loop task since we release the continuation on an event loop task
            impl->m_selfReference = nullptr;
        }

        Crt::String ClientContinuationImpl::GetModelName() const noexcept
        {
            return m_operationModelContext->GetOperationName();
        }

        std::shared_ptr<ClientContinuationImpl> ClientConnectionImpl::NewStream() noexcept
        {
            std::lock_guard<std::mutex> lock(m_sharedStateLock);
            return Aws::Crt::MakeShared<ClientContinuationImpl>(
                m_allocator, m_allocator, m_bootstrap, m_sharedState.m_underlyingConnection);
        }

        ClientOperation::ClientOperation(
            ClientConnection &connection,
            std::shared_ptr<StreamResponseHandler> streamHandler,
            const std::shared_ptr<OperationModelContext> &operationModelContext,
            Crt::Allocator *allocator) noexcept
            : m_allocator(allocator), m_impl(connection.NewStream())
        {
            m_impl->Initialize(operationModelContext, std::move(streamHandler));
        }

        ClientOperation::~ClientOperation() noexcept
        {
            m_impl->ShutDown();
            m_impl = nullptr;
        }

        std::future<RpcError> ClientOperation::Close(OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return m_impl->Close(std::move(onMessageFlushCallback));
        }

        void ClientOperation::WithLaunchMode(std::launch mode) noexcept
        {
            (void)mode;
        }

        Crt::String ClientOperation::GetModelName() const noexcept
        {
            return m_impl->GetModelName();
        }

        std::future<RpcError> ClientOperation::Activate(
            const AbstractShapeBase *shape,
            OnMessageFlushCallback &&onMessageFlushCallback,
            std::function<void(EventstreamResultVariantType &&)> &&onResultCallback) noexcept
        {
            Crt::List<EventStreamHeader> headers;
            headers.emplace_back(
                Crt::String(CONTENT_TYPE_HEADER), Crt::String(CONTENT_TYPE_APPLICATION_JSON), m_allocator);
            headers.emplace_back(Crt::String(SERVICE_MODEL_TYPE_HEADER), GetModelName(), m_allocator);
            Crt::JsonObject payloadObject;
            shape->SerializeToJsonObject(payloadObject);
            Crt::String payloadString = payloadObject.View().WriteCompact();

            return m_impl->Activate(
                GetModelName(),
                headers,
                Crt::ByteBufFromCString(payloadString.c_str()),
                AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_APPLICATION_MESSAGE,
                0,
                std::move(onResultCallback),
                std::move(onMessageFlushCallback));
        }

        std::future<RpcError> ClientOperation::SendStreamMessage(
            const AbstractShapeBase *shape,
            OnMessageFlushCallback &&onMessageFlushCallback) noexcept
        {
            Crt::List<EventStreamHeader> headers;
            headers.emplace_back(
                Crt::String(CONTENT_TYPE_HEADER), Crt::String(CONTENT_TYPE_APPLICATION_JSON), m_allocator);
            headers.emplace_back(Crt::String(SERVICE_MODEL_TYPE_HEADER), GetModelName(), m_allocator);
            Crt::JsonObject payloadObject;
            shape->SerializeToJsonObject(payloadObject);
            Crt::String payloadString = payloadObject.View().WriteCompact();

            return m_impl->SendStreamMessage(
                headers,
                Crt::ByteBufFromCString(payloadString.c_str()),
                AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_APPLICATION_MESSAGE,
                0,
                std::move(onMessageFlushCallback));
        }

        EventstreamResultVariantType::EventstreamResultVariantType()
            : m_type(ResultType::NONE), m_modeledResult(nullptr), m_modeledError(nullptr),
              m_rpcError({EventStreamRpcStatusCode::EVENT_STREAM_RPC_SUCCESS, AWS_ERROR_SUCCESS})
        {
        }

        EventstreamResultVariantType::EventstreamResultVariantType(
            Crt::ScopedResource<AbstractShapeBase> &&modeledResult) noexcept
            : m_type(ResultType::OPERATION_RESPONSE), m_modeledResult(std::move(modeledResult)),
              m_modeledError(nullptr),
              m_rpcError({EventStreamRpcStatusCode::EVENT_STREAM_RPC_SUCCESS, AWS_ERROR_SUCCESS})
        {
        }

        EventstreamResultVariantType::EventstreamResultVariantType(
            Crt::ScopedResource<OperationError> &&modeledError) noexcept
            : m_type(ResultType::OPERATION_ERROR), m_modeledResult(nullptr), m_modeledError(std::move(modeledError)),
              m_rpcError({EventStreamRpcStatusCode::EVENT_STREAM_RPC_SUCCESS, AWS_ERROR_SUCCESS})
        {
        }

        EventstreamResultVariantType::EventstreamResultVariantType(RpcError rpcError) noexcept
            : m_type(ResultType::RPC_ERROR), m_modeledResult(nullptr), m_modeledError(nullptr), m_rpcError(rpcError)
        {
        }

        EventstreamResultVariantType::EventstreamResultVariantType(EventstreamResultVariantType &&rhs) noexcept
            : m_type(rhs.m_type), m_modeledResult(std::move(rhs.m_modeledResult)),
              m_modeledError(std::move(rhs.m_modeledError)), m_rpcError(rhs.m_rpcError)
        {
        }

        EventstreamResultVariantType &EventstreamResultVariantType::operator=(
            EventstreamResultVariantType &&rhs) noexcept
        {
            if (this != &rhs)
            {
                m_type = rhs.m_type;
                m_modeledResult = std::move(rhs.m_modeledResult);
                m_modeledError = std::move(rhs.m_modeledError);
                m_rpcError = rhs.m_rpcError;
            }

            return *this;
        }

        AbstractShapeBase *EventstreamResultVariantType::GetModeledResult() const
        {
            return m_modeledResult.get();
        }

        OperationError *EventstreamResultVariantType::GetModeledError() const
        {
            return m_modeledError.get();
        }

        RpcError EventstreamResultVariantType::GetRpcError() const
        {
            return m_rpcError;
        }

    } /* namespace Eventstreamrpc */
} // namespace Aws
