/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/eventstreamrpc/EventStreamClient.h>

#include <aws/crt/Api.h>
#include <aws/crt/Config.h>
#include <aws/crt/auth/Credentials.h>

#include <stdint.h>
#include <string.h>

#include <algorithm>

constexpr auto EVENTSTREAM_VERSION_HEADER = ":version";
constexpr auto EVENTSTREAM_VERSION_STRING = "0.1.0";
constexpr auto CONTENT_TYPE_HEADER = ":content-type";
constexpr auto CONTENT_TYPE_APPLICATION_JSON = "application/json";
constexpr auto SERVICE_MODEL_TYPE_HEADER = "service-model-type";

namespace Aws
{
    namespace Eventstreamrpc
    {
        static void s_deleteMessageCallbackContainer(void *);

        /*
         * This structure has evolved into a more complex form in order to support some additional requirements
         * in the eventstream refactor.
         *
         * In order to maintain behavioral compatibility while removing blocking destruction, we need to be able to
         * synchronously "cancel" pending callbacks/completions at the C++ level.  The easiest way to accomplish
         * this is by ref-counting the structure (so both the C++ and C continuations can reference it) and using
         * a lock + completion state to control whether or not the completion actually happens (first one wins).
         *
         * Because C callbacks are just function pointers, using shared_ptrs is difficult/awkward.  So instead we
         * just use the C pattern of an internal ref-count that deletes on dropping to zero.
         *
         * We also want to support tracking the whole set of pending callbacks so that we can cancel them all
         * at once while still efficiently completing then one-at-a-time, hence the intrusive linked list node.  The
         * linked list node is not always used (for example, the connect message callback is not added to any list).
         */
        class OnMessageFlushCallbackContainer
        {
          public:
            OnMessageFlushCallbackContainer(Crt::Allocator *allocator, OnMessageFlushCallback &&flushCallback)
                : m_allocator(allocator), m_refCount({}), m_sharedState({})
            {
                aws_ref_count_init(&m_refCount, this, s_deleteMessageCallbackContainer);
                aws_ref_count_acquire(&m_refCount); // always start at 2, one for C++, one for C
                m_sharedState.m_state = CallbackState::Incomplete;
                m_sharedState.m_onMessageFlushCallback = std::move(flushCallback);
            }

            OnMessageFlushCallbackContainer(
                Crt::Allocator *allocator,
                OnMessageFlushCallback &&flushCallback,
                std::promise<RpcError> &&flushPromise)
                : OnMessageFlushCallbackContainer(allocator, std::move(flushCallback))
            {
                m_sharedState.m_onFlushPromise = std::move(flushPromise);
            }

            ~OnMessageFlushCallbackContainer() = default;

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

            static void Release(OnMessageFlushCallbackContainer *callback)
            {
                if (callback == nullptr)
                {
                    return;
                }

                aws_ref_count_release(&callback->m_refCount);
            }

            Aws::Crt::Allocator *GetAllocator() const { return m_allocator; }

          private:
            enum class CallbackState
            {
                Incomplete,
                Finished
            };

            Crt::Allocator *m_allocator;
            struct aws_ref_count m_refCount;

            std::mutex m_sharedStateLock;
            struct
            {
                CallbackState m_state;
                OnMessageFlushCallback m_onMessageFlushCallback;
                std::promise<RpcError> m_onFlushPromise;
            } m_sharedState;
        };

        static void s_deleteMessageCallbackContainer(void *object)
        {
            auto container = reinterpret_cast<OnMessageFlushCallbackContainer *>(object);

            Aws::Crt::Delete(container, container->GetAllocator());
        }

        static void s_protocolMessageCallback(int errorCode, void *userData) noexcept
        {
            auto *callbackData = static_cast<OnMessageFlushCallbackContainer *>(userData);

            auto rpcStatus = (errorCode == AWS_ERROR_SUCCESS) ? EVENT_STREAM_RPC_SUCCESS : EVENT_STREAM_RPC_CRT_ERROR;
            OnMessageFlushCallbackContainer::Complete(callbackData, {rpcStatus, errorCode});
            OnMessageFlushCallbackContainer::Release(callbackData);
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

        MessageAmendment::MessageAmendment(MessageAmendment &&rhs)
            : m_headers(std::move(rhs.m_headers)), m_payload(rhs.m_payload), m_allocator(rhs.m_allocator)
        {
            rhs.m_allocator = nullptr;
            rhs.m_payload = Crt::Optional<Crt::ByteBuf>();
        }

        MessageAmendment &MessageAmendment::operator=(MessageAmendment &&rhs)
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
            /* Check if the connection has expired before attempting to send. */
            aws_event_stream_headers_list_init(headersArray, m_allocator);

            /* Populate the array with the underlying handle of each EventStreamHeader. */
            for (auto &i : headers)
            {
                aws_array_list_push_back(headersArray, i.GetUnderlyingHandle());
            }
        }

        Crt::String RpcError::StatusToString()
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
                case EVENT_STREAM_RPC_CONTINUATION_ALREADY_OPENED:
                    return "EVENT_STREAM_RPC_CONTINUATION_ALREADY_OPENED";
                case EVENT_STREAM_RPC_CONTINUATION_CLOSE_IN_PROGRESS:
                    return "EVENT_STREAM_RPC_CONTINUATION_CLOSE_IN_PROGRESS";
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
                    m_underlyingHandle.header_value_len = m_valueByteBuf.len;
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
            m_underlyingHandle.header_name_len = static_cast<uint8_t>(name.length());
            size_t length;
            if (name.length() > INT8_MAX)
            {
                length = INT8_MAX;
            }
            else
            {
                length = static_cast<size_t>(name.length());
            }
            (void)memcpy(m_underlyingHandle.header_name, name.c_str(), length);
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

        EventStreamHeader &EventStreamHeader::operator=(const EventStreamHeader &lhs) noexcept
        {
            m_allocator = lhs.m_allocator;
            if (aws_byte_buf_is_valid(&m_valueByteBuf))
            {
                Crt::ByteBufDelete(m_valueByteBuf);
            }
            m_valueByteBuf = Crt::ByteBufNewCopy(lhs.m_allocator, lhs.m_valueByteBuf.buffer, lhs.m_valueByteBuf.len);
            m_underlyingHandle = lhs.m_underlyingHandle;
            m_underlyingHandle.header_value.variable_len_val = m_valueByteBuf.buffer;
            m_underlyingHandle.header_value_len = static_cast<uint16_t>(m_valueByteBuf.len);
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
         * Part 1 of an effort to refactor the eventstream bindings to conform to the latest CRT binding guidelines.
         *
         * For connections, we now enforce the following invariants for correctness:
         *
         * 1. No callback is made while a lock is held.  We perform callbacks by having a transactional callback context
         * that is moved out of shared state (under the lock), but the callback context does not perform its work until
         * the lock is released.
         * 2. No destructor blocking or synchronization.  In order to provide the best behavioral backwards
         * compatibility, we "synthesize" the callbacks that would occur at destruction when we kick off the async
         * cleanup process.  When the asynchronous events occur that would normally trigger a callback occur, we ignore
         * them via the has_shut_down flag.
         * 3. A self-reference (via shared_ptr member) guarantees the binding impl stays alive longer than the C
         * objects.   The public binding object also keeps a shared_ptr to the impl, so final destruction only occurs
         * once both the public binding object's destructor has run and no C connection object is still alive.
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
         * We rely heavily on careful move semantics to ensure (2) and (3)
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
                            (void)m_errorCallback(m_error);
                        }
                        else
                        {
                            m_connectionSuccessCallback();
                        }
                        m_connectPromise.set_value(m_error);
                        break;
                    }
                    case ConnectionCallbackActionType::DisconnectionCallback:
                    {
                        m_disconnectionCallback(m_error);
                        break;
                    }
                    default:
                        break;
                }
            }

            /*
             * Returns a callback context that should be invoked, and moves this context into a state that should be
             * invoked when the connection is closed
             */
            ConnectionCallbackContext TransitionToConnected()
            {
                ConnectionCallbackContext context = {};
                context.m_action = ConnectionCallbackActionType::CompleteConnectPromise;
                context.m_connectPromise = std::move(m_connectPromise);
                context.m_connectionSuccessCallback = m_connectionSuccessCallback;

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

            void Shutdown() noexcept;

          private:
            void CloseInternal(bool isShutdown) noexcept;
            void MoveToDisconnected(RpcError error) noexcept;

            int SendConnectMessage(OnMessageFlushCallback &&onMessageFlushCallback) noexcept;

            enum class ClientState
            {
                Disconnected,
                PendingConnect,
                PendingConnack,
                Connected,
                Disconnecting,
            };

            Crt::Allocator *m_allocator;

            ConnectionLifecycleHandler *m_lifecycleHandler; // cannot be made a shared_ptr sadly
            ConnectMessageAmender m_connectMessageAmender;
            ConnectionConfig m_connectionConfig;
            std::shared_ptr<ClientConnectionImpl> m_selfReference;
            aws_client_bootstrap *m_bootstrap;
            aws_event_loop *m_eventLoop;

            std::mutex m_sharedStateLock;
            struct
            {
                aws_event_stream_rpc_client_connection *m_underlyingConnection;
                ClientState m_currentState;
                ClientState m_desiredState;
                ConnectionCallbackContext m_callbackContext;
                OnMessageFlushCallbackContainer *m_connectFlushContainer;
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
                m_selfReference = nullptr;
                if (!m_sharedState.m_hasShutDown)
                {
                    localContext = std::move(m_sharedState.m_callbackContext);
                }
                m_sharedState.m_callbackContext = {};
            }

            localContext.InvokeCallbacks();
            aws_event_loop_schedule_task_now(m_eventLoop, &clearSharedTask->m_task);
        }

        void ClientConnectionImpl::CloseInternal(bool isShutdown) noexcept
        {
            ConnectionCallbackContext localContext = {};
            OnMessageFlushCallbackContainer *connectFlushContainer = nullptr;
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
                    aws_event_stream_rpc_client_connection_close(
                        m_sharedState.m_underlyingConnection, AWS_ERROR_EVENT_STREAM_RPC_CONNECTION_CLOSED);
                }

                if (isShutdown)
                {
                    localContext = std::move(m_sharedState.m_callbackContext);
                    connectFlushContainer = m_sharedState.m_connectFlushContainer;
                    m_sharedState.m_connectFlushContainer = nullptr;
                }
            }

            localContext.InvokeCallbacks();

            OnMessageFlushCallbackContainer::Complete(
                connectFlushContainer,
                {EVENT_STREAM_RPC_CONNECTION_CLOSED, AWS_ERROR_EVENT_STREAM_RPC_CONNECTION_CLOSED});
            OnMessageFlushCallbackContainer::Release(connectFlushContainer);
        }

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
            EventStreamRpcStatusCode status = connectionConfig.Validate();
            if (status != EVENT_STREAM_RPC_SUCCESS)
            {
                std::promise<RpcError> localPromise;
                localPromise.set_value({status, AWS_ERROR_INVALID_ARGUMENT});

                return localPromise.get_future();
            }

            std::future<RpcError> localFuture = {};
            {
                std::lock_guard<std::mutex> lock(m_sharedStateLock);
                if (m_sharedState.m_currentState != ClientState::Disconnected)
                {
                    std::promise<RpcError> localPromise;
                    localPromise.set_value({EVENT_STREAM_RPC_CONNECTION_ALREADY_ESTABLISHED, AWS_ERROR_SUCCESS});
                    return localPromise.get_future();
                }

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

            struct aws_event_stream_rpc_client_connection_options connectOptions = {};
            connectOptions.host_name = connectionConfig.GetHostName().value().c_str();
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
                MoveToDisconnected({EVENT_STREAM_RPC_CONNECTION_SETUP_FAILED, aws_last_error()});
            }

            return localFuture;
        }

        bool ClientConnectionImpl::IsOpen() const noexcept
        {
            std::lock_guard<std::mutex> lock(const_cast<ClientConnectionImpl *>(this)->m_sharedStateLock);
            return m_sharedState.m_currentState == ClientState::Connected;
        }

        int ClientConnectionImpl::SendConnectMessage(OnMessageFlushCallback &&onMessageFlushCallback) noexcept
        {
            MessageAmendment messageAmendment;
            if (m_connectionConfig.GetConnectAmendment().has_value())
            {
                MessageAmendment connectAmendment(m_connectionConfig.GetConnectAmendment().value());
                // The version header is necessary for establishing the connection.
                messageAmendment.AddHeader(EventStreamHeader(
                    Crt::String(EVENTSTREAM_VERSION_HEADER), Crt::String(EVENTSTREAM_VERSION_STRING), m_allocator));
                messageAmendment.PrependHeaders(std::move(connectAmendment).GetHeaders());
                messageAmendment.SetPayload(std::move(connectAmendment).GetPayload());
            }

            struct aws_array_list headersArray;

            s_fillNativeHeadersArray(messageAmendment.GetHeaders(), &headersArray, m_allocator);
            struct aws_event_stream_rpc_message_args msg_args;
            msg_args.headers = (struct aws_event_stream_header_value_pair *)headersArray.data;
            msg_args.headers_count = messageAmendment.GetHeaders().size();
            msg_args.payload = messageAmendment.GetPayload().has_value()
                                   ? (aws_byte_buf *)(&(messageAmendment.GetPayload().value()))
                                   : nullptr;
            msg_args.message_type = AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_CONNECT;
            msg_args.message_flags = 0U;

            // It shouldn't be possible to interrupt a connect with another connect, but let's handle that case anyways
            OnMessageFlushCallbackContainer::Complete(
                m_sharedState.m_connectFlushContainer, {EVENT_STREAM_RPC_CONNECTION_SETUP_FAILED, AWS_ERROR_SUCCESS});
            OnMessageFlushCallbackContainer::Release(m_sharedState.m_connectFlushContainer);

            m_sharedState.m_connectFlushContainer =
                Crt::New<OnMessageFlushCallbackContainer>(m_allocator, m_allocator, std::move(onMessageFlushCallback));

            int errorCode = AWS_ERROR_SUCCESS;
            if (aws_event_stream_rpc_client_connection_send_protocol_message(
                    m_sharedState.m_underlyingConnection,
                    &msg_args,
                    s_protocolMessageCallback,
                    reinterpret_cast<void *>(m_sharedState.m_connectFlushContainer)))
            {
                errorCode = aws_last_error();
            }

            /* Cleanup. */
            if (aws_array_list_is_valid(&headersArray))
            {
                aws_array_list_clean_up(&headersArray);
            }

            if (errorCode)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_EVENT_STREAM_RPC_CLIENT,
                    "A CRT error occurred while queueing a message to be sent on the connection: %s",
                    Crt::ErrorDebugString(errorCode));

                OnMessageFlushCallbackContainer::Complete(
                    m_sharedState.m_connectFlushContainer, {EVENT_STREAM_RPC_CRT_ERROR, errorCode});
                OnMessageFlushCallbackContainer::Release(m_sharedState.m_connectFlushContainer);
                m_sharedState.m_connectFlushContainer = nullptr;
            }

            return (errorCode == AWS_ERROR_SUCCESS) ? AWS_OP_SUCCESS : AWS_OP_ERR;
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

            AWS_FATAL_ASSERT(connection != nullptr);
            {
                std::lock_guard<std::mutex> lock(impl->m_sharedStateLock);
                AWS_FATAL_ASSERT(impl->m_sharedState.m_currentState == ClientState::PendingConnect);

                // the channel owns the initial ref; we have to take our own
                aws_event_stream_rpc_client_connection_acquire(connection);
                impl->m_sharedState.m_underlyingConnection = connection;
                if (impl->m_sharedState.m_desiredState != ClientState::Connected)
                {
                    AWS_FATAL_ASSERT(impl->m_sharedState.m_desiredState == ClientState::Disconnected);
                    impl->m_sharedState.m_currentState = ClientState::Disconnecting;
                    aws_event_stream_rpc_client_connection_close(
                        connection, AWS_ERROR_EVENT_STREAM_RPC_CONNECTION_CLOSED);
                    return;
                }

                impl->m_sharedState.m_currentState = ClientState::PendingConnack;
                auto callback = impl->m_connectionConfig.GetConnectRequestCallback();
                if (impl->SendConnectMessage(std::move(callback)))
                {
                    impl->m_sharedState.m_callbackContext.SetError({EVENT_STREAM_RPC_CRT_ERROR, aws_last_error()});
                    impl->m_sharedState.m_currentState = ClientState::Disconnecting;
                    aws_event_stream_rpc_client_connection_close(
                        connection, AWS_ERROR_EVENT_STREAM_RPC_CONNECTION_CLOSED);
                }
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
                            if (!impl->m_sharedState.m_hasShutDown)
                            {
                                impl->m_sharedState.m_callbackContext.SetError(
                                    (successfulAck) ? RpcError{EVENT_STREAM_RPC_CONNECTION_CLOSED, 0}
                                                    : RpcError{EVENT_STREAM_RPC_CONNECTION_ACCESS_DENIED, 0});
                            }
                            impl->m_sharedState.m_desiredState = ClientState::Disconnected;
                            if (impl->m_sharedState.m_currentState != ClientState::Disconnecting)
                            {
                                impl->m_sharedState.m_currentState = ClientState::Disconnecting;
                                aws_event_stream_rpc_client_connection_close(
                                    impl->m_sharedState.m_underlyingConnection,
                                    AWS_ERROR_EVENT_STREAM_RPC_CONNECTION_CLOSED);
                            }
                            return;
                        }

                        impl->m_sharedState.m_currentState = ClientState::Connected;
                        localCallbackContext = impl->m_sharedState.m_callbackContext.TransitionToConnected();
                    }
                    localCallbackContext.InvokeCallbacks();
                    break;
                }

                case AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_PING:
                {
                    Crt::List<EventStreamHeader> pingHeaders;
                    for (size_t i = 0; i < messageArgs->headers_count; ++i)
                    {
                        pingHeaders.emplace_back(EventStreamHeader(messageArgs->headers[i], impl->m_allocator));
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

        TaggedResult::TaggedResult(Crt::ScopedResource<AbstractShapeBase> operationResponse) noexcept
            : m_responseType(OPERATION_RESPONSE), m_rpcError({})
        {
            m_operationResult.m_response = std::move(operationResponse);
        }

        TaggedResult::~TaggedResult() noexcept
        {
            if (m_responseType == OPERATION_RESPONSE)
            {
                m_operationResult.m_response.~unique_ptr();
            }
            else if (m_responseType == OPERATION_ERROR)
            {
                m_operationResult.m_error.~unique_ptr();
            }
        }

        TaggedResult::TaggedResult(Crt::ScopedResource<OperationError> operationError) noexcept
            : m_responseType(OPERATION_ERROR), m_rpcError({EVENT_STREAM_RPC_UNINITIALIZED, 0})
        {
            m_operationResult.m_error = std::move(operationError);
        }

        TaggedResult &TaggedResult::operator=(TaggedResult &&rhs) noexcept
        {
            m_responseType = rhs.m_responseType;
            if (m_responseType == OPERATION_RESPONSE)
            {
                m_operationResult.m_response = std::move(rhs.m_operationResult.m_response);
            }
            else if (m_responseType == OPERATION_ERROR)
            {
                m_operationResult.m_error = std::move(rhs.m_operationResult.m_error);
            }
            m_rpcError = rhs.m_rpcError;
            rhs.m_rpcError = {EVENT_STREAM_RPC_UNINITIALIZED, 0};

            return *this;
        }

        TaggedResult::TaggedResult(RpcError rpcError) noexcept
            : m_responseType(RPC_ERROR), m_operationResult(), m_rpcError(rpcError)
        {
        }

        TaggedResult::TaggedResult() noexcept
            : m_responseType(RPC_ERROR), m_operationResult(), m_rpcError({EVENT_STREAM_RPC_UNINITIALIZED, 0})
        {
        }

        TaggedResult::TaggedResult(TaggedResult &&rhs) noexcept
        {
            m_responseType = rhs.m_responseType;
            if (m_responseType == OPERATION_RESPONSE)
            {
                m_operationResult.m_response = std::move(rhs.m_operationResult.m_response);
            }
            else if (m_responseType == OPERATION_ERROR)
            {
                m_operationResult.m_error = std::move(rhs.m_operationResult.m_error);
            }
            m_rpcError = rhs.m_rpcError;
            rhs.m_rpcError = {EVENT_STREAM_RPC_UNINITIALIZED, 0};
        }

        TaggedResult::operator bool() const noexcept
        {
            return m_responseType == OPERATION_RESPONSE;
        }

        AbstractShapeBase *TaggedResult::GetOperationResponse() const noexcept
        {
            return (m_responseType == OPERATION_RESPONSE) ? m_operationResult.m_response.get() : nullptr;
        }

        OperationError *TaggedResult::GetOperationError() const noexcept
        {
            return (m_responseType == OPERATION_ERROR) ? m_operationResult.m_error.get() : nullptr;
        }

        RpcError TaggedResult::GetRpcError() const noexcept
        {
            if (m_responseType == RPC_ERROR)
            {
                return m_rpcError;
            }
            else
            {
                /* Assume success since an application response or error was received. */
                return {EVENT_STREAM_RPC_SUCCESS, 0};
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

        void StreamResponseHandler::OnStreamEvent(Crt::ScopedResource<AbstractShapeBase> response) {}

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

        enum class EventStreamMessageRoutingType
        {
            Response,
            Stream,
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

        enum class ContinuationStateType
        {
            None,
            PendingActivate,
            Activated,
            PendingClose,
            Closed,
        };

        struct ContinuationSharedState
        {
            ContinuationSharedState();

            ContinuationStateType m_currentState;
            ContinuationStateType m_desiredState;
            struct aws_event_stream_rpc_client_continuation_token *m_continuation;
            OnMessageFlushCallbackContainer *m_activationCallbackContainer;
            std::function<void(TaggedResult &&)> m_activationResponseCallback;
            OnMessageFlushCallbackContainer *m_closeCallbackContainer;
        };

        ContinuationSharedState::ContinuationSharedState()
            : m_currentState(ContinuationStateType::None), m_desiredState(ContinuationStateType::None),
              m_continuation(nullptr), m_activationCallbackContainer(nullptr), m_activationResponseCallback(),
              m_closeCallbackContainer(nullptr)
        {
        }

        class ClientContinuationImpl : public std::enable_shared_from_this<ClientContinuationImpl>
        {
          public:
            ClientContinuationImpl(
                Aws::Crt::Allocator *allocator,
                struct aws_event_stream_rpc_client_connection *connection) noexcept;
            virtual ~ClientContinuationImpl();

            // Called by ClientOperation::~ClientOperation(); the returned future is completed after the
            // termination callback of the underlying C continuation
            std::future<void> ShutDown() noexcept;

            std::future<RpcError> Activate(
                const Crt::String &operation,
                const Crt::List<EventStreamHeader> &headers,
                const Crt::Optional<Crt::ByteBuf> &payload,
                MessageType messageType,
                uint32_t messageFlags,
                std::function<void(TaggedResult &&)> &&onResultCallback,
                OnMessageFlushCallback &&onMessageFlushCallback,
                bool &synchronousSuccess) noexcept;

            std::future<RpcError> SendStreamMessage(
                const Crt::List<EventStreamHeader> &headers,
                const Crt::Optional<Crt::ByteBuf> &payload,
                MessageType messageType,
                uint32_t messageFlags,
                OnMessageFlushCallback &&onMessageFlushCallback);

            std::future<RpcError> Close(OnMessageFlushCallback &&onMessageFlushCallback = nullptr) noexcept;

            Crt::String GetModelName() const noexcept;

            void Initialize(
                const OperationModelContext *operationModelContext,
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

            int SendCloseMessage(
                OnMessageFlushCallback &&closeFlushCallback,
                std::promise<RpcError> &&closeFlushPromise) noexcept;

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

            std::shared_ptr<ClientContinuationImpl> m_selfReference;

            std::mutex m_sharedStateLock;
            ContinuationSharedState m_sharedState;

            const OperationModelContext *m_operationModelContext;
            std::shared_ptr<StreamResponseHandler> m_streamHandler;

            std::promise<void> m_terminationPromise;
        };

        ClientOperation::ClientOperation(
            ClientConnection &connection,
            std::shared_ptr<StreamResponseHandler> streamHandler,
            const OperationModelContext &operationModelContext,
            Crt::Allocator *allocator) noexcept
            : m_allocator(allocator), m_impl(connection.NewStream())
        {
            m_impl->Initialize(&operationModelContext, std::move(streamHandler));
        }

        ClientOperation::~ClientOperation() noexcept
        {
            auto terminationFuture = m_impl->ShutDown();
            m_impl = nullptr;
            terminationFuture.get();
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
            std::function<void(TaggedResult &&)> &&onResultCallback,
            bool &synchronousSuccess) noexcept
        {
            Crt::List<EventStreamHeader> headers;
            headers.emplace_back(EventStreamHeader(
                Crt::String(CONTENT_TYPE_HEADER), Crt::String(CONTENT_TYPE_APPLICATION_JSON), m_allocator));
            headers.emplace_back(
                EventStreamHeader(Crt::String(SERVICE_MODEL_TYPE_HEADER), GetModelName(), m_allocator));
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
                std::move(onMessageFlushCallback),
                synchronousSuccess);
        }

        std::future<RpcError> ClientOperation::SendStreamMessage(
            const AbstractShapeBase *shape,
            OnMessageFlushCallback &&onMessageFlushCallback) noexcept
        {
            Crt::List<EventStreamHeader> headers;
            headers.emplace_back(EventStreamHeader(
                Crt::String(CONTENT_TYPE_HEADER), Crt::String(CONTENT_TYPE_APPLICATION_JSON), m_allocator));
            headers.emplace_back(
                EventStreamHeader(Crt::String(SERVICE_MODEL_TYPE_HEADER), GetModelName(), m_allocator));
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

        ClientContinuationImpl::ClientContinuationImpl(
            Aws::Crt::Allocator *allocator,
            struct aws_event_stream_rpc_client_connection *connection) noexcept
            : m_allocator(allocator), m_clientBootstrap(aws_client_bootstrap_acquire(
                                          aws_event_stream_rpc_client_connection_get_client_bootstrap(connection))),
              m_sharedState(), m_operationModelContext(nullptr)
        {
            struct aws_event_stream_rpc_client_stream_continuation_options continuation_options = {
                .on_continuation = s_OnContinuationMessage,
                .on_continuation_closed = s_OnContinuationClosed,
                .on_continuation_terminated = s_OnContinuationTerminated,
                .user_data = this,
            };
            m_sharedState.m_continuation =
                aws_event_stream_rpc_client_connection_new_stream(connection, &continuation_options);

            if (!m_sharedState.m_continuation)
            {
                m_sharedState.m_currentState = ContinuationStateType::Closed;
                m_sharedState.m_desiredState = ContinuationStateType::Closed;
            }
        }

        ClientContinuationImpl::~ClientContinuationImpl()
        {
            m_terminationPromise.set_value();
            aws_client_bootstrap_release(m_clientBootstrap);
        }

        // We use a task to release the C continuation to make it impossible to trigger the termination callback
        // in a callstack that includes ClientContinationImpl methods or state.
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

            struct aws_event_loop *event_loop = aws_event_loop_group_get_next_loop(bootstrap->event_loop_group);
            AWS_FATAL_ASSERT(event_loop != NULL);

            auto releaseTask = Aws::Crt::New<AwsEventstreamContinuationReleaseTask>(allocator, allocator, continuation);
            aws_event_loop_schedule_task_now(event_loop, &releaseTask->m_task);
        }

        std::future<void> ClientContinuationImpl::ShutDown() noexcept
        {
            struct aws_event_stream_rpc_client_continuation_token *releaseContinuation = nullptr;
            OnMessageFlushCallbackContainer *closeCallbackContainer = nullptr;
            OnMessageFlushCallbackContainer *activationCallbackContainer = nullptr;
            std::function<void(TaggedResult &&)> activationResponseCallback;

            {
                std::lock_guard<std::mutex> lock(m_sharedStateLock);
                if (m_sharedState.m_currentState == ContinuationStateType::None)
                {
                    releaseContinuation = m_sharedState.m_continuation;
                    m_sharedState.m_continuation = nullptr;
                    m_sharedState.m_currentState = ContinuationStateType::Closed;
                    m_sharedState.m_desiredState = ContinuationStateType::Closed;
                }

                activationCallbackContainer = m_sharedState.m_activationCallbackContainer;
                m_sharedState.m_activationCallbackContainer = nullptr;

                closeCallbackContainer = m_sharedState.m_closeCallbackContainer;
                m_sharedState.m_closeCallbackContainer = nullptr;

                activationResponseCallback = std::move(m_sharedState.m_activationResponseCallback);
                m_sharedState.m_activationResponseCallback = nullptr; // unsure if necessary, let's be sure
            }

            if (activationResponseCallback)
            {
                activationResponseCallback(
                    TaggedResult(RpcError{EVENT_STREAM_RPC_CONTINUATION_CLOSED, AWS_ERROR_SUCCESS}));
            }

            /*
             * Short-circuit and simulate both activate and close callbacks as necessary.  Part of our contract is that
             * when Shutdown returns, no further user-facing callbacks/promise completions will be performed.
             */
            OnMessageFlushCallbackContainer::Complete(
                activationCallbackContainer, {EVENT_STREAM_RPC_CONTINUATION_CLOSED, AWS_ERROR_SUCCESS});
            OnMessageFlushCallbackContainer::Release(activationCallbackContainer);

            OnMessageFlushCallbackContainer::Complete(
                closeCallbackContainer, {EVENT_STREAM_RPC_CONTINUATION_CLOSED, AWS_ERROR_SUCCESS});
            OnMessageFlushCallbackContainer::Release(closeCallbackContainer);

            if (releaseContinuation != nullptr)
            {
                ReleaseContinuation(m_allocator, m_clientBootstrap, releaseContinuation);
            }
            else
            {
                Close();
            }

            return m_terminationPromise.get_future();
        }

        std::future<RpcError> ClientContinuationImpl::Activate(
            const Crt::String &operation,
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            MessageType messageType,
            uint32_t messageFlags,
            std::function<void(TaggedResult &&)> &&onResultCallback,
            OnMessageFlushCallback &&onMessageFlushCallback,
            bool &synchronousSuccess) noexcept
        {
            int result = AWS_OP_SUCCESS;
            synchronousSuccess = false;
            struct aws_array_list headersArray; // guaranteed to be zeroed or valid if we reach the end of the function
            OnMessageFlushCallbackContainer *activationFailureContainer = nullptr;
            std::promise<RpcError> activationPromise;
            std::future<RpcError> activationFuture = activationPromise.get_future();
            {
                std::lock_guard<std::mutex> lock(m_sharedStateLock);
                if (m_sharedState.m_continuation == nullptr)
                {
                    activationPromise.set_value({EVENT_STREAM_RPC_CONTINUATION_CLOSED, 0});
                    return activationFuture;
                }

                switch (m_sharedState.m_currentState)
                {
                    case ContinuationStateType::PendingActivate:
                    case ContinuationStateType::Activated:
                        activationPromise.set_value({EVENT_STREAM_RPC_CONTINUATION_ALREADY_OPENED, 0});
                        return activationFuture;

                    case ContinuationStateType::PendingClose:
                    case ContinuationStateType::Closed:
                        activationPromise.set_value({EVENT_STREAM_RPC_CONTINUATION_CLOSED, 0});
                        return activationFuture;

                    default:
                        break;
                }

                AWS_FATAL_ASSERT(m_sharedState.m_currentState == ContinuationStateType::None);

                // cleanup requirements mean we can't early out from here on
                s_fillNativeHeadersArray(headers, &headersArray, m_allocator);

                struct aws_event_stream_rpc_message_args msg_args;
                msg_args.headers = static_cast<struct aws_event_stream_header_value_pair *>(headersArray.data);
                msg_args.headers_count = headers.size();
                msg_args.payload = payload.has_value() ? (aws_byte_buf *)(&(payload.value())) : nullptr;
                msg_args.message_type = messageType;
                msg_args.message_flags = messageFlags;

                m_sharedState.m_activationCallbackContainer = Crt::New<OnMessageFlushCallbackContainer>(
                    m_allocator, m_allocator, std::move(onMessageFlushCallback), std::move(activationPromise));
                m_sharedState.m_activationResponseCallback = std::move(onResultCallback);

                result = aws_event_stream_rpc_client_continuation_activate(
                    m_sharedState.m_continuation,
                    Crt::ByteCursorFromCString(operation.c_str()),
                    &msg_args,
                    s_protocolMessageCallback,
                    reinterpret_cast<void *>(m_sharedState.m_activationCallbackContainer));

                if (result != AWS_OP_SUCCESS)
                {
                    activationFailureContainer = m_sharedState.m_activationCallbackContainer;
                    m_sharedState.m_activationCallbackContainer = nullptr;
                    m_sharedState.m_activationResponseCallback = std::function<void(TaggedResult &&)>();
                }
                else
                {
                    m_sharedState.m_currentState = ContinuationStateType::PendingActivate;
                    m_sharedState.m_desiredState = ContinuationStateType::Activated;
                    synchronousSuccess = true;
                }
            }

            if (activationFailureContainer)
            {
                OnMessageFlushCallbackContainer::Complete(
                    activationFailureContainer, {EVENT_STREAM_RPC_CRT_ERROR, aws_last_error()});
                OnMessageFlushCallbackContainer::Release(activationFailureContainer);
                OnMessageFlushCallbackContainer::Release(activationFailureContainer);
            }

            if (aws_array_list_is_valid(&headersArray))
            {
                aws_array_list_clean_up(&headersArray);
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
            int result = AWS_OP_SUCCESS;
            struct aws_array_list headersArray; // guaranteed to be zeroed or valid if we reach the end of the function
            OnMessageFlushCallbackContainer *sendFailureContainer = nullptr;
            std::promise<RpcError> sendPromise;
            std::future<RpcError> sendFuture = sendPromise.get_future();
            {
                std::lock_guard<std::mutex> lock(m_sharedStateLock);
                if (m_sharedState.m_continuation == nullptr)
                {
                    sendPromise.set_value({EVENT_STREAM_RPC_CONTINUATION_CLOSED, 0});
                    return sendFuture;
                }

                switch (m_sharedState.m_currentState)
                {
                    case ContinuationStateType::PendingActivate:
                    case ContinuationStateType::None:
                        sendPromise.set_value({EVENT_STREAM_RPC_CONTINUATION_NOT_YET_OPENED, 0});
                        return sendFuture;

                    case ContinuationStateType::PendingClose:
                    case ContinuationStateType::Closed:
                        sendPromise.set_value({EVENT_STREAM_RPC_CONTINUATION_CLOSED, 0});
                        return sendFuture;

                    default:
                        break;
                }

                // cleanup requirements mean we can't early out from here on
                s_fillNativeHeadersArray(headers, &headersArray, m_allocator);

                struct aws_event_stream_rpc_message_args msg_args;
                msg_args.headers = static_cast<struct aws_event_stream_header_value_pair *>(headersArray.data);
                msg_args.headers_count = headers.size();
                msg_args.payload = payload.has_value() ? (aws_byte_buf *)(&(payload.value())) : nullptr;
                msg_args.message_type = messageType;
                msg_args.message_flags = messageFlags;

                auto sendContainer = Crt::New<OnMessageFlushCallbackContainer>(
                    m_allocator, m_allocator, std::move(onMessageFlushCallback), std::move(sendPromise));
                OnMessageFlushCallbackContainer::Release(sendContainer); // release our ref

                result = aws_event_stream_rpc_client_continuation_send_message(
                    m_sharedState.m_continuation,
                    &msg_args,
                    s_protocolMessageCallback,
                    reinterpret_cast<void *>(sendContainer));

                if (result != AWS_OP_SUCCESS)
                {
                    sendFailureContainer = sendContainer;
                }
            }

            if (sendFailureContainer)
            {
                OnMessageFlushCallbackContainer::Complete(
                    sendFailureContainer, {EVENT_STREAM_RPC_CRT_ERROR, aws_last_error()});
                OnMessageFlushCallbackContainer::Release(sendFailureContainer);
            }

            if (aws_array_list_is_valid(&headersArray))
            {
                aws_array_list_clean_up(&headersArray);
            }

            return sendFuture;
        }

        // Intentionally returns an error code and not AWS_OP_ERR/AWS_OP_SUCCESS
        // shared state lock must be held before calling
        int ClientContinuationImpl::SendCloseMessage(
            OnMessageFlushCallback &&closeFlushCallback,
            std::promise<RpcError> &&closeFlushPromise) noexcept
        {
            int errorCode = AWS_ERROR_SUCCESS;

            struct aws_event_stream_rpc_message_args msg_args;
            msg_args.headers = nullptr;
            msg_args.headers_count = 0;
            msg_args.payload = nullptr;
            msg_args.message_type = AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_APPLICATION_MESSAGE;
            msg_args.message_flags = AWS_EVENT_STREAM_RPC_MESSAGE_FLAG_TERMINATE_STREAM;

            m_sharedState.m_closeCallbackContainer = Crt::New<OnMessageFlushCallbackContainer>(
                m_allocator, m_allocator, std::move(closeFlushCallback), std::move(closeFlushPromise));

            int result = aws_event_stream_rpc_client_continuation_send_message(
                m_sharedState.m_continuation,
                &msg_args,
                s_protocolMessageCallback,
                reinterpret_cast<void *>(m_sharedState.m_closeCallbackContainer));

            if (result)
            {
                errorCode = aws_last_error();
                AWS_LOGF_ERROR(
                    AWS_LS_EVENT_STREAM_RPC_CLIENT,
                    "A CRT error occurred while closing the stream: %s",
                    Crt::ErrorDebugString(errorCode));
            }

            return errorCode;
        }

        std::future<RpcError> ClientContinuationImpl::Close(OnMessageFlushCallback &&onMessageFlushCallback) noexcept
        {
            int closeErrorCode = AWS_ERROR_SUCCESS;
            OnMessageFlushCallbackContainer *closeFailureCallbackContainer = nullptr;
            std::promise<RpcError> closePromise;
            std::future<RpcError> closeFuture = closePromise.get_future();
            {
                std::lock_guard<std::mutex> lock(m_sharedStateLock);
                if (m_sharedState.m_continuation == nullptr)
                {
                    closePromise.set_value({EVENT_STREAM_RPC_CONTINUATION_CLOSED, 0});
                    return closeFuture;
                }

                switch (m_sharedState.m_currentState)
                {
                    case ContinuationStateType::PendingActivate:
                    case ContinuationStateType::Activated:
                    {
                        closeErrorCode = SendCloseMessage(std::move(onMessageFlushCallback), std::move(closePromise));
                        if (closeErrorCode != AWS_ERROR_SUCCESS)
                        {
                            closeFailureCallbackContainer = m_sharedState.m_closeCallbackContainer;
                            m_sharedState.m_closeCallbackContainer = nullptr;
                        }
                        else
                        {
                            m_sharedState.m_currentState = ContinuationStateType::PendingClose;
                        }
                        break;
                    }

                    case ContinuationStateType::PendingClose:
                        closePromise.set_value({EVENT_STREAM_RPC_CONTINUATION_CLOSE_IN_PROGRESS, 0});
                        return closeFuture;

                    default:
                        closePromise.set_value({EVENT_STREAM_RPC_CONTINUATION_CLOSED, 0});
                        return closeFuture;
                }
            }

            if (closeFailureCallbackContainer != nullptr)
            {
                OnMessageFlushCallbackContainer::Complete(
                    closeFailureCallbackContainer, {EVENT_STREAM_RPC_CRT_ERROR, closeErrorCode});
                OnMessageFlushCallbackContainer::Release(closeFailureCallbackContainer);
                OnMessageFlushCallbackContainer::Release(closeFailureCallbackContainer);
            }

            return closeFuture;
        }

        void ClientContinuationImpl::OnClosed() noexcept
        {
            struct aws_event_stream_rpc_client_continuation_token *releaseContinuation = nullptr;
            OnMessageFlushCallbackContainer *closeCallbackContainer = nullptr;
            OnMessageFlushCallbackContainer *activationCallbackContainer = nullptr;
            std::function<void(TaggedResult &&)> activationResponseCallback;

            {
                std::lock_guard<std::mutex> lock(m_sharedStateLock);

                m_sharedState.m_currentState = ContinuationStateType::Closed;
                m_sharedState.m_desiredState = ContinuationStateType::Closed;
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
                    TaggedResult(RpcError{EVENT_STREAM_RPC_CONTINUATION_CLOSED, AWS_ERROR_SUCCESS}));
            }

            OnMessageFlushCallbackContainer::Complete(
                activationCallbackContainer, {EVENT_STREAM_RPC_CONTINUATION_CLOSED, AWS_ERROR_SUCCESS});
            OnMessageFlushCallbackContainer::Release(activationCallbackContainer);

            OnMessageFlushCallbackContainer::Complete(
                closeCallbackContainer, {EVENT_STREAM_RPC_CONTINUATION_CLOSED, AWS_ERROR_SUCCESS});
            OnMessageFlushCallbackContainer::Release(closeCallbackContainer);

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
                continuationMessageHeaders.emplace_back(EventStreamHeader(rawMessage->headers[i], m_allocator));
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
                AWS_LOGF_ERROR(
                    AWS_LS_EVENT_STREAM_RPC_CLIENT,
                    "A required header (%s) could not be found in the message.",
                    CONTENT_TYPE_HEADER);
                result.m_statusCode = EVENT_STREAM_RPC_UNSUPPORTED_CONTENT_TYPE;
                return result;
            }

            Crt::String contentType;
            if (contentHeader->GetValueAsString(contentType) && contentType != CONTENT_TYPE_APPLICATION_JSON)
            {
                /* Missing required content type header. */
                AWS_LOGF_ERROR(
                    AWS_LS_EVENT_STREAM_RPC_CLIENT,
                    "The content type (%s) header was specified with an unsupported value (%s).",
                    CONTENT_TYPE_HEADER,
                    contentType.c_str());
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
            std::function<void(TaggedResult &&)> activationResultCallback;

            MessageResult result;
            bool isResponse = false;
            {
                std::lock_guard<std::mutex> lock(m_sharedStateLock);
                isResponse = static_cast<bool>(m_sharedState.m_currentState == ContinuationStateType::PendingActivate);
                if (isResponse)
                {
                    activationResultCallback = std::move(m_sharedState.m_activationResponseCallback);
                    m_sharedState.m_activationResponseCallback = nullptr;
                    m_sharedState.m_currentState = ContinuationStateType::Activated;
                }
            }

            result = DeserializeRawMessage(messageArgs, isResponse);

            if (activationResultCallback)
            {
                if (result.m_statusCode == EVENT_STREAM_RPC_SUCCESS)
                {
                    activationResultCallback(TaggedResult(std::move(result.m_message.value().m_shape)));
                }
                else
                {
                    activationResultCallback(TaggedResult(RpcError{result.m_statusCode, 0}));
                }
            }
            else if (!isResponse)
            {
                if (result.m_message.has_value())
                {
                    AWS_FATAL_ASSERT(result.m_message.value().m_route == EventStreamMessageRoutingType::Stream);
                    auto shape = std::move(result.m_message.value().m_shape);
                    m_streamHandler->OnStreamEvent(std::move(shape));
                }
                else
                {
                    bool shouldClose = m_streamHandler->OnStreamError(nullptr, {result.m_statusCode, 0});
                    if (shouldClose)
                    {
                        Close();
                    }
                }
            }
        }

        void ClientContinuationImpl::s_OnContinuationClosed(
            struct aws_event_stream_rpc_client_continuation_token *token,
            void *user_data) noexcept
        {
            auto impl = reinterpret_cast<ClientContinuationImpl *>(user_data);
            impl->OnClosed();
        }

        void ClientContinuationImpl::s_OnContinuationMessage(
            struct aws_event_stream_rpc_client_continuation_token *token,
            const struct aws_event_stream_rpc_message_args *message_args,
            void *user_data) noexcept
        {
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
                m_allocator, m_allocator, m_sharedState.m_underlyingConnection);
        }

    } /* namespace Eventstreamrpc */
} // namespace Aws
