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
        /* Because `std::function` cannot be typecasted to void *, we must contain it in a struct. */
        struct OnMessageFlushCallbackContainer
        {
            explicit OnMessageFlushCallbackContainer(Crt::Allocator *allocator) : allocator(allocator) {}
            Crt::Allocator *allocator;
            OnMessageFlushCallback onMessageFlushCallback;
            std::promise<RpcError> onFlushPromise;
        };

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
                    Crt::String ret = "Failed with EVENT_STREAM_RPC_CRT_ERROR, the CRT error was ";
                    ret += Crt::ErrorDebugString(crtError);
                    return ret;
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
 * 1. No callback is made while a lock is held.  We perform callbacks by having a transactional callback context that
 *    is moved out of shared state (under the lock), but the callback context does not perform its work until the lock
 *    is released.
 * 2. No destructor blocking or synchronization.  In order to provide the best behavioral backwards compatibility, we
 *    "synthesize" the callbacks that would occur at destruction when we kick off the async cleanup process.  When the
 *    asynchronous events occur that would normally trigger a callback occur, we ignore them via the has_shut_down flag.
 * 3. A self-reference (via shared_ptr member) guarantees the binding impl stays alive longer than the C objects.   The
 *    public binding object also keeps a shared_ptr to the impl, so final destruction only occurs once both the public
 *    binding object's destructor has run and no C connection object is still alive.
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
            ConnectionCallbackContext() : m_action(ConnectionCallbackActionType::None), m_error{EVENT_STREAM_RPC_SUCCESS, AWS_ERROR_SUCCESS} {}

            ConnectionCallbackContext(const std::function<void(RpcError)> &disconnectionCallback, const std::function<bool(RpcError)> &errorCallback, const std::function<void()> &connectionSuccessCallback) :
                m_action(ConnectionCallbackActionType::CompleteConnectPromise),
                m_error{EVENT_STREAM_RPC_SUCCESS, AWS_ERROR_SUCCESS},
                m_connectPromise(),
                m_disconnectionCallback(disconnectionCallback),
                m_errorCallback(errorCallback),
                m_connectionSuccessCallback(connectionSuccessCallback)
            {}

            ConnectionCallbackContext(ConnectionCallbackContext &&rhs) noexcept :
                m_action(rhs.m_action),
                m_error(rhs.m_error),
                m_connectPromise(std::move(rhs.m_connectPromise)),
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

            ClientContinuation NewStream(ClientContinuationHandler &clientContinuationHandler) noexcept;

            void Close() noexcept;

            bool IsOpen() const noexcept;

            void Shutdown() noexcept;

            std::future<RpcError> SendProtocolMessage(
                const Crt::List<EventStreamHeader> &headers,
                const Crt::Optional<Crt::ByteBuf> &payload,
                MessageType messageType,
                uint32_t messageFlags,
                OnMessageFlushCallback onMessageFlushCallback) noexcept;

          private:

            void CloseInternal(bool isShutdown) noexcept;
            void MoveToDisconnected(RpcError error) noexcept;

            int SendProtocolMessageAux(
                const Crt::List<EventStreamHeader> &headers,
                const Crt::Optional<Crt::ByteBuf> &payload,
                MessageType messageType,
                uint32_t messageFlags,
                OnMessageFlushCallback &&onMessageFlushCallback,
                std::promise<RpcError> *sendOutcomePromise,
                bool takeLock) noexcept;

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
            : m_allocator(allocator),  m_lifecycleHandler(nullptr), m_connectMessageAmender(nullptr),
            m_bootstrap(aws_client_bootstrap_acquire(bootstrap)), m_eventLoop(nullptr),
            m_sharedState { nullptr, ClientState::Disconnected, ClientState::Disconnected, {}, false}
        {
            m_eventLoop = aws_event_loop_group_get_next_loop(bootstrap->event_loop_group);
        }

        ClientConnectionImpl::~ClientConnectionImpl() noexcept
        {
            aws_client_bootstrap_release(m_bootstrap);
        }

        // We use a task to zero out the self reference to make sure we are not in a call stack that includes
        // a member function of the connection impl itself when potentially releasing the final reference.
        struct AwsEventstreamConnectionImplClearSharedTask {
            AwsEventstreamConnectionImplClearSharedTask(Aws::Crt::Allocator *allocator, ClientConnectionImpl *clientConnectionImpl) noexcept;

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

        AwsEventstreamConnectionImplClearSharedTask::AwsEventstreamConnectionImplClearSharedTask(Aws::Crt::Allocator *allocator, ClientConnectionImpl *clientConnectionImpl) noexcept :
            m_task{},
            m_allocator(allocator),
            m_impl(clientConnectionImpl->shared_from_this())
        {
            aws_task_init(&m_task, s_zeroSharedReference, this, "AwsEventstreamConnectionImplClearSharedTask");
        }

        void ClientConnectionImpl::MoveToDisconnected(RpcError error) noexcept
        {
            auto *clearSharedTask = Aws::Crt::New<AwsEventstreamConnectionImplClearSharedTask>(m_allocator, m_allocator, this);
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
            {
                std::lock_guard<std::mutex> lock(m_sharedStateLock);
                m_sharedState.m_desiredState = ClientState::Disconnected;
                m_sharedState.m_callbackContext.SetError({EVENT_STREAM_RPC_CONNECTION_CLOSED, AWS_ERROR_EVENT_STREAM_RPC_CONNECTION_CLOSED});
                if (isShutdown)
                {
                    m_sharedState.m_hasShutDown = true;
                }

                if (m_sharedState.m_currentState == ClientState::Connected || m_sharedState.m_currentState == ClientState::PendingConnack)
                {
                    m_sharedState.m_currentState = ClientState::Disconnecting;
                    aws_event_stream_rpc_client_connection_close(m_sharedState.m_underlyingConnection, AWS_ERROR_EVENT_STREAM_RPC_CONNECTION_CLOSED);
                }

                if (isShutdown)
                {
                    localContext = std::move(m_sharedState.m_callbackContext);
                }
            }

            localContext.InvokeCallbacks();
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
                std::function<void(RpcError)> disconnectCallback = [this](RpcError error) { this->m_lifecycleHandler->OnDisconnectCallback(error); };
                std::function<bool(RpcError)> errorCallback = [this](RpcError error){ return this->m_lifecycleHandler->OnErrorCallback(error); };
                std::function<void()> connectionSuccessCallback = [this](){ this->m_lifecycleHandler->OnConnectCallback(); };

                m_connectionConfig = connectionConfig;
                m_selfReference = shared_from_this();
                m_sharedState.m_desiredState = ClientState::Connected;
                m_sharedState.m_currentState = ClientState::PendingConnect;
                m_sharedState.m_callbackContext = {std::move(disconnectCallback), std::move(errorCallback), std::move(connectionSuccessCallback)};
                localFuture =  m_sharedState.m_callbackContext.GetConnectPromiseFuture();
            }

            Crt::Io::SocketOptions socketOptions;
            if (m_connectionConfig.GetSocketOptions().has_value())
            {
                socketOptions = m_connectionConfig.GetSocketOptions().value();
            }

            struct aws_event_stream_rpc_client_connection_options connectOptions = {
                .host_name = connectionConfig.GetHostName().value().c_str(),
                .port = connectionConfig.GetPort().value(),
                .bootstrap = m_bootstrap,
                .socket_options = &socketOptions.GetImpl(),
                .on_connection_setup = ClientConnectionImpl::s_onConnectionSetup,
                .on_connection_protocol_message = ClientConnectionImpl::s_onProtocolMessage,
                .on_connection_shutdown = ClientConnectionImpl::s_onConnectionShutdown,
                .user_data = reinterpret_cast<void *>(this),
            };

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

        static void s_protocolMessageCallback(int errorCode, void *userData) noexcept
        {
            auto *callbackData = static_cast<OnMessageFlushCallbackContainer *>(userData);

            if (errorCode)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_EVENT_STREAM_RPC_CLIENT,
                    "A CRT error occurred while attempting to send a message: %s",
                    Crt::ErrorDebugString(errorCode));
                callbackData->onFlushPromise.set_value({EVENT_STREAM_RPC_CRT_ERROR, errorCode});
            }
            else
            {
                callbackData->onFlushPromise.set_value({EVENT_STREAM_RPC_SUCCESS, 0});
            }

            /* Call the user-provided callback. */
            if (callbackData->onMessageFlushCallback)
            {
                callbackData->onMessageFlushCallback(errorCode);
            }

            Crt::Delete(callbackData, callbackData->allocator);
        }

        ClientContinuation ClientConnectionImpl::NewStream(ClientContinuationHandler &clientContinuationHandler) noexcept
        {
            std::lock_guard<std::mutex> lock(m_sharedStateLock);
            return {m_sharedState.m_underlyingConnection, clientContinuationHandler, m_allocator};
        }

        int ClientConnectionImpl::SendProtocolMessageAux(
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            MessageType messageType,
            uint32_t messageFlags,
            OnMessageFlushCallback &&onMessageFlushCallback,
            std::promise<RpcError> *sendOutcomePromise,
            bool takeLock) noexcept
        {
            OnMessageFlushCallbackContainer *callbackContainer = nullptr;
            struct aws_array_list headersArray;

            s_fillNativeHeadersArray(headers, &headersArray, m_allocator);
            struct aws_event_stream_rpc_message_args msg_args;
            msg_args.headers = (struct aws_event_stream_header_value_pair *)headersArray.data;
            msg_args.headers_count = headers.size();
            msg_args.payload = payload.has_value() ? (aws_byte_buf *)(&(payload.value())) : nullptr;
            msg_args.message_type = messageType;
            msg_args.message_flags = messageFlags;

            callbackContainer = Crt::New<OnMessageFlushCallbackContainer>(m_allocator, m_allocator);
            callbackContainer->onMessageFlushCallback = std::move(onMessageFlushCallback);
            if (sendOutcomePromise)
            {
                callbackContainer->onFlushPromise = std::move(*sendOutcomePromise);
            }

            int errorCode = AWS_ERROR_SUCCESS;
            {
                if (takeLock)
                {
                    m_sharedStateLock.lock();
                }

                if (aws_event_stream_rpc_client_connection_send_protocol_message(
                    m_sharedState.m_underlyingConnection,
                    &msg_args,
                    s_protocolMessageCallback,
                    reinterpret_cast<void *>(callbackContainer)))
                {
                    errorCode = aws_last_error();
                }

                if (takeLock)
                {
                    m_sharedStateLock.unlock();
                }
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
                if (sendOutcomePromise)
                {
                    sendOutcomePromise->set_value({EVENT_STREAM_RPC_CRT_ERROR, errorCode});
                }

                Crt::Delete(callbackContainer, m_allocator);
            }

            return (errorCode == AWS_ERROR_SUCCESS) ? AWS_OP_SUCCESS : AWS_OP_ERR;
        }

        std::future<RpcError> ClientConnectionImpl::SendProtocolMessage(
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            MessageType messageType,
            uint32_t messageFlags,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            std::promise<RpcError> sendOutcomePromise;
            auto sendOutcomeFuture = sendOutcomePromise.get_future();
            SendProtocolMessageAux(headers, payload, messageType, messageFlags, std::move(onMessageFlushCallback), &sendOutcomePromise, true);

            return sendOutcomeFuture;
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
                    aws_event_stream_rpc_client_connection_close(connection, AWS_ERROR_EVENT_STREAM_RPC_CONNECTION_CLOSED);
                    return;
                }

                impl->m_sharedState.m_currentState = ClientState::PendingConnack;
                MessageAmendment messageAmendment;

                if (impl->m_connectionConfig.GetConnectAmendment().has_value())
                {
                    MessageAmendment connectAmendment(impl->m_connectionConfig.GetConnectAmendment().value());
                    // The version header is necessary for establishing the connection.
                    messageAmendment.AddHeader(EventStreamHeader(
                        Crt::String(EVENTSTREAM_VERSION_HEADER),
                        Crt::String(EVENTSTREAM_VERSION_STRING),
                        impl->m_allocator));
                    messageAmendment.PrependHeaders(std::move(connectAmendment).GetHeaders());
                    messageAmendment.SetPayload(std::move(connectAmendment).GetPayload());
                }

                if (impl->SendProtocolMessageAux(                    messageAmendment.GetHeaders(),
                    messageAmendment.GetPayload(),
                    AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_CONNECT,
                    0U,
                    impl->m_connectionConfig.GetConnectRequestCallback(), nullptr, false))
                {
                    impl->m_sharedState.m_callbackContext.SetError({EVENT_STREAM_RPC_CRT_ERROR, aws_last_error()});
                    impl->m_sharedState.m_currentState = ClientState::Disconnecting;
                    aws_event_stream_rpc_client_connection_close(connection, AWS_ERROR_EVENT_STREAM_RPC_CONNECTION_CLOSED);
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
                    bool successfulAck = (messageArgs->message_flags & AWS_EVENT_STREAM_RPC_MESSAGE_FLAG_CONNECTION_ACCEPTED) != 0;
                    {
                        std::lock_guard<std::mutex> lock(impl->m_sharedStateLock);
                        if (impl->m_sharedState.m_currentState != ClientState::PendingConnack || !successfulAck)
                        {
                            if (!impl->m_sharedState.m_hasShutDown)
                            {
                                impl->m_sharedState.m_callbackContext.SetError(
                                    (successfulAck) ? RpcError{EVENT_STREAM_RPC_CONNECTION_CLOSED, 0} : RpcError{EVENT_STREAM_RPC_CONNECTION_ACCESS_DENIED, 0}
                                    );
                            }
                            impl->m_sharedState.m_desiredState = ClientState::Disconnected;
                            if (impl->m_sharedState.m_currentState != ClientState::Disconnecting)
                            {
                                impl->m_sharedState.m_currentState = ClientState::Disconnecting;
                                aws_event_stream_rpc_client_connection_close(impl->m_sharedState.m_underlyingConnection, AWS_ERROR_EVENT_STREAM_RPC_CONNECTION_CLOSED);
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
                        pingHeaders.emplace_back(
                            EventStreamHeader(messageArgs->headers[i], impl->m_allocator));
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
            :m_impl(Aws::Crt::MakeShared<ClientConnectionImpl>(allocator, allocator, bootstrap))
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

        ClientContinuation ClientConnection::NewStream(ClientContinuationHandler &clientContinuationHandler) noexcept
        {
            return m_impl->NewStream(clientContinuationHandler);
        }

        void ClientConnection::Close() noexcept
        {
            m_impl->Close();
        }

        bool ClientConnection::IsOpen() const noexcept
        {
            return m_impl->IsOpen();
        }

        void AbstractShapeBase::s_customDeleter(AbstractShapeBase *shape) noexcept
        {
            if (shape->m_allocator != nullptr)
                Crt::Delete<AbstractShapeBase>(shape, shape->m_allocator);
        }

        struct RawContinuationCallbackDataWrapper
        {
            RawContinuationCallbackDataWrapper(Aws::Crt::Allocator *allocator, const std::shared_ptr<ContinuationCallbackData> &callbackData) :
                m_allocator(allocator),
                m_callbackData(callbackData)
            {}

            Aws::Crt::Allocator *m_allocator;
            std::shared_ptr<ContinuationCallbackData> m_callbackData;
        };

        static void s_onContinuationTerminated(void *user_data)
        {
            if (user_data == nullptr)
            {
                return;
            }

            struct RawContinuationCallbackDataWrapper *wrapper = static_cast<struct RawContinuationCallbackDataWrapper *>(user_data);
            Aws::Crt::Delete(wrapper, wrapper->m_allocator);
        }

        ClientContinuation::ClientContinuation(
            struct aws_event_stream_rpc_client_connection *connection,
            ClientContinuationHandler &continuationHandler,
            Crt::Allocator *allocator) noexcept
            : m_allocator(allocator), m_continuationHandler(continuationHandler), m_continuationToken(nullptr)
        {
            struct aws_event_stream_rpc_client_stream_continuation_options options;
            options.on_continuation = ClientContinuation::s_onContinuationMessage;
            options.on_continuation_closed = ClientContinuation::s_onContinuationClosed;
            options.on_continuation_terminated = s_onContinuationTerminated;

            m_callbackData = Crt::MakeShared<ContinuationCallbackData>(m_allocator, this, m_allocator);

            m_continuationHandler.m_callbackData = m_callbackData;
            options.user_data = reinterpret_cast<void *>(Aws::Crt::New<RawContinuationCallbackDataWrapper>(allocator, allocator, m_callbackData));

            if (connection)
            {
                m_continuationToken =
                    aws_event_stream_rpc_client_connection_new_stream(connection, &options);
                if (m_continuationToken == nullptr)
                {
                    m_continuationHandler.m_callbackData = nullptr;
                    m_callbackData = nullptr;
                }
            }
        }

        ClientContinuation::~ClientContinuation() noexcept
        {
            Release();
        }

        void ClientContinuation::Release()
        {
            if (m_callbackData != nullptr)
            {
                {
                    const std::lock_guard<std::mutex> lock(m_callbackData->callbackMutex);
                    m_callbackData->continuationDestroyed = true;
                }
            }

            if (m_continuationToken)
            {
                aws_event_stream_rpc_client_continuation_release(m_continuationToken);
                m_continuationToken = nullptr;
            }
        }

        void ClientContinuation::s_onContinuationMessage(
            struct aws_event_stream_rpc_client_continuation_token *continuationToken,
            const struct aws_event_stream_rpc_message_args *messageArgs,
            void *userData) noexcept
        {
            (void)continuationToken;
            /* The `userData` pointer is used to pass a `ContinuationCallbackData` object. */
            auto *callbackData = static_cast<RawContinuationCallbackDataWrapper *>(userData)->m_callbackData.get();
            auto *thisContinuation = callbackData->clientContinuation;

            Crt::List<EventStreamHeader> continuationMessageHeaders;
            for (size_t i = 0; i < messageArgs->headers_count; ++i)
            {
                continuationMessageHeaders.emplace_back(
                    EventStreamHeader(messageArgs->headers[i], thisContinuation->m_allocator));
            }

            Crt::Optional<Crt::ByteBuf> payload;

            if (messageArgs->payload)
            {
                payload = Crt::Optional<Crt::ByteBuf>(*messageArgs->payload);
            }
            else
            {
                payload = Crt::Optional<Crt::ByteBuf>();
            }

            const std::lock_guard<std::mutex> lock(callbackData->callbackMutex);
            if (callbackData->continuationDestroyed)
                return;
            thisContinuation->m_continuationHandler.OnContinuationMessage(
                continuationMessageHeaders, payload, messageArgs->message_type, messageArgs->message_flags);
        }

        void ClientContinuation::s_onContinuationClosed(
            struct aws_event_stream_rpc_client_continuation_token *continuationToken,
            void *userData) noexcept
        {
            (void)continuationToken;

            /* The `userData` pointer is used to pass a `ContinuationCallbackData` object. */
            auto *callbackData = static_cast<RawContinuationCallbackDataWrapper *>(userData)->m_callbackData.get();

            const std::lock_guard<std::mutex> lock(callbackData->callbackMutex);
            if (callbackData->continuationDestroyed)
                return;

            auto *thisContinuation = callbackData->clientContinuation;
            thisContinuation->m_continuationHandler.OnContinuationClosed();
        }

        std::future<RpcError> ClientContinuation::Activate(
            const Crt::String &operationName,
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            MessageType messageType,
            uint32_t messageFlags,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            struct aws_array_list headersArray;
            OnMessageFlushCallbackContainer *callbackContainer = nullptr;
            std::promise<RpcError> onFlushPromise;

            if (m_continuationToken == nullptr)
            {
                onFlushPromise.set_value({EVENT_STREAM_RPC_CONNECTION_CLOSED, 0});
                return onFlushPromise.get_future();
            }

            if (IsClosed())
            {
                onFlushPromise.set_value({EVENT_STREAM_RPC_CONTINUATION_CLOSED, 0});
                return onFlushPromise.get_future();
            }

            s_fillNativeHeadersArray(headers, &headersArray, m_allocator);

            /*
             * Regardless of how the promise gets moved around (or not), this future should stay valid as a return
             * value.
             *
             * We pull it out early because the call to aws_event_stream_rpc_client_continuation_activate() may complete
             * and delete the promise before we pull out the future afterwords.
             */
            std::future<RpcError> retValue = onFlushPromise.get_future();

            struct aws_event_stream_rpc_message_args msg_args;
            msg_args.headers = (struct aws_event_stream_header_value_pair *)headersArray.data;
            msg_args.headers_count = headers.size();
            msg_args.payload = payload.has_value() ? (aws_byte_buf *)(&(payload.value())) : nullptr;
            msg_args.message_type = messageType;
            msg_args.message_flags = messageFlags;

            /* This heap allocation is necessary so that the flush callback can still be invoked when this function
             * returns. */
            callbackContainer = Crt::New<OnMessageFlushCallbackContainer>(m_allocator, m_allocator);
            callbackContainer->onMessageFlushCallback = onMessageFlushCallback;
            callbackContainer->onFlushPromise = std::move(onFlushPromise);

            int errorCode = aws_event_stream_rpc_client_continuation_activate(
                m_continuationToken,
                Crt::ByteCursorFromCString(operationName.c_str()),
                &msg_args,
                s_protocolMessageCallback,
                reinterpret_cast<void *>(callbackContainer));

            /* Cleanup. */
            if (aws_array_list_is_valid(&headersArray))
            {
                aws_array_list_clean_up(&headersArray);
            }

            if (errorCode)
            {
                onFlushPromise = std::move(callbackContainer->onFlushPromise);
                onFlushPromise.set_value({EVENT_STREAM_RPC_CRT_ERROR, errorCode});
                Crt::Delete(callbackContainer, m_allocator);
            }

            return retValue;
        }

        std::future<RpcError> ClientContinuation::SendMessage(
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            MessageType messageType,
            uint32_t messageFlags,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            struct aws_array_list headersArray;
            OnMessageFlushCallbackContainer *callbackContainer = nullptr;
            std::promise<RpcError> onFlushPromise;

            if (IsClosed())
            {
                onFlushPromise.set_value({EVENT_STREAM_RPC_CONTINUATION_CLOSED, 0});
                return onFlushPromise.get_future();
            }

            s_fillNativeHeadersArray(headers, &headersArray, m_allocator);

            struct aws_event_stream_rpc_message_args msg_args;
            msg_args.headers = (struct aws_event_stream_header_value_pair *)headersArray.data;
            msg_args.headers_count = headers.size();
            msg_args.payload = payload.has_value() ? (aws_byte_buf *)(&(payload.value())) : nullptr;
            msg_args.message_type = messageType;
            msg_args.message_flags = messageFlags;

            /* This heap allocation is necessary so that the flush callback can still be invoked when this function
             * returns. */
            callbackContainer = Crt::New<OnMessageFlushCallbackContainer>(m_allocator, m_allocator);
            callbackContainer->onMessageFlushCallback = onMessageFlushCallback;
            callbackContainer->onFlushPromise = std::move(onFlushPromise);

            int errorCode = AWS_OP_SUCCESS;
            if (m_continuationToken)
            {
                if (aws_event_stream_rpc_client_continuation_send_message(
                    m_continuationToken,
                    &msg_args,
                    s_protocolMessageCallback,
                    reinterpret_cast<void *>(callbackContainer)))
                {
                    errorCode = aws_last_error();
                }
            }

            /* Cleanup. */
            if (aws_array_list_is_valid(&headersArray))
            {
                aws_array_list_clean_up(&headersArray);
            }

            if (errorCode)
            {
                onFlushPromise = std::move(callbackContainer->onFlushPromise);
                AWS_LOGF_ERROR(
                    AWS_LS_EVENT_STREAM_RPC_CLIENT,
                    "A CRT error occurred while queueing a message to be sent on a stream: %s",
                    Crt::ErrorDebugString(errorCode));
                onFlushPromise.set_value({EVENT_STREAM_RPC_CRT_ERROR, errorCode});
                Crt::Delete(callbackContainer, m_allocator);
            }
            else
            {
                return callbackContainer->onFlushPromise.get_future();
            }

            return onFlushPromise.get_future();
        }

        bool ClientContinuation::IsClosed() noexcept
        {
            if (!m_continuationToken)
            {
                return true;
            }
            else
            {
                return aws_event_stream_rpc_client_continuation_is_closed(m_continuationToken);
            }
        }

        OperationModelContext::OperationModelContext(const ServiceModel &serviceModel) noexcept
            : m_serviceModel(serviceModel)
        {
        }

        void OperationError::SerializeToJsonObject(Crt::JsonObject &payloadObject) const
        {
            (void)payloadObject;
        }

        AbstractShapeBase::AbstractShapeBase() noexcept : m_allocator(nullptr) {}

        ClientOperation::ClientOperation(
            ClientConnection &connection,
            std::shared_ptr<StreamResponseHandler> streamHandler,
            const OperationModelContext &operationModelContext,
            Crt::Allocator *allocator) noexcept
            : m_operationModelContext(operationModelContext), m_asyncLaunchMode(std::launch::deferred),
              m_messageCount(0), m_allocator(allocator), m_streamHandler(streamHandler),
              m_clientContinuation(connection.NewStream(*this)), m_expectingClose(false), m_streamClosedCalled(false)
        {
        }

        ClientOperation::~ClientOperation() noexcept
        {
            Close().wait();
            m_clientContinuation.Release();
            //std::unique_lock<std::mutex> lock(m_continuationMutex);
            //m_closeReady.wait(lock, [this] { return m_expectingClose == false; });

        }

        TaggedResult::TaggedResult(Crt::ScopedResource<AbstractShapeBase> operationResponse) noexcept
            : m_responseType(OPERATION_RESPONSE)
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

        std::future<TaggedResult> ClientOperation::GetOperationResult() noexcept
        {
            {
                const std::lock_guard<std::mutex> lock(m_continuationMutex);

                if (m_clientContinuation.IsClosed() && !m_resultReceived)
                {
                    AWS_LOGF_ERROR(AWS_LS_EVENT_STREAM_RPC_CLIENT, "The underlying stream is already closed.");
                    m_initialResponsePromise.set_value(TaggedResult({EVENT_STREAM_RPC_CONNECTION_CLOSED, 0}));
                    m_resultReceived = true;
                }
            }

            return m_initialResponsePromise.get_future();
        }

        const EventStreamHeader *ClientOperation::GetHeaderByName(
            const Crt::List<EventStreamHeader> &headers,
            const Crt::String &name) noexcept
        {
            for (auto it = headers.begin(); it != headers.end(); ++it)
            {
                if (name == it->GetHeaderName())
                {
                    return &(*it);
                }
            }
            return nullptr;
        }

        EventStreamRpcStatusCode ClientOperation::HandleData(const Crt::Optional<Crt::ByteBuf> &payload)
        {
            Crt::StringView payloadStringView;
            if (payload.has_value())
            {
                payloadStringView = Crt::ByteCursorToStringView(Crt::ByteCursorFromByteBuf(payload.value()));
            }

            /* The value of this hashmap contains the function that allocates the response object from the
             * payload. */
            /* Responses after the first message don't necessarily have the same shape as the first. */
            Crt::ScopedResource<AbstractShapeBase> response;
            if (m_messageCount == 1)
            {
                response = m_operationModelContext.AllocateInitialResponseFromPayload(payloadStringView, m_allocator);
            }
            else
            {
                response = m_operationModelContext.AllocateStreamingResponseFromPayload(payloadStringView, m_allocator);
            }

            if (response.get() == nullptr)
            {
                AWS_LOGF_ERROR(AWS_LS_EVENT_STREAM_RPC_CLIENT, "Failed to allocate a response from the payload.");
                return EVENT_STREAM_RPC_ALLOCATION_ERROR;
            }

            if (m_messageCount == 1)
            {
                const std::lock_guard<std::mutex> lock(m_continuationMutex);
                m_resultReceived = true;
                m_initialResponsePromise.set_value(TaggedResult(std::move(response)));
            }
            else
            {
                if (m_streamHandler)
                    m_streamHandler->OnStreamEvent(std::move(response));
            }

            return EVENT_STREAM_RPC_SUCCESS;
        }

        EventStreamRpcStatusCode ClientOperation::HandleError(
            const Crt::String &modelName,
            const Crt::Optional<Crt::ByteBuf> &payload,
            uint32_t messageFlags)
        {
            bool streamAlreadyTerminated = (messageFlags & AWS_EVENT_STREAM_RPC_MESSAGE_FLAG_TERMINATE_STREAM) != 0;

            Crt::StringView payloadStringView;
            if (payload.has_value())
            {
                payloadStringView = Crt::ByteCursorToStringView(Crt::ByteCursorFromByteBuf(payload.value()));
            }

            /* The value of this hashmap contains the function that allocates the error from the
             * payload. */
            Crt::ScopedResource<OperationError> error =
                m_operationModelContext.AllocateOperationErrorFromPayload(modelName, payloadStringView, m_allocator);
            if (error.get() == nullptr)
                return EVENT_STREAM_RPC_UNMAPPED_DATA;
            if (error->GetMessage().has_value())
            {
                AWS_LOGF_ERROR(
                    AWS_LS_EVENT_STREAM_RPC_CLIENT,
                    "An error was received from the server: %s",
                    error->GetMessage().value().c_str());
            }
            TaggedResult taggedResult(std::move(error));
            if (m_messageCount == 1)
            {
                {
                    const std::lock_guard<std::mutex> lock(m_continuationMutex);
                    m_resultReceived = true;
                    m_initialResponsePromise.set_value(std::move(taggedResult));
                }
                /* Close the stream unless the server already closed it for us. This condition is checked
                 * so that TERMINATE_STREAM messages aren't resent by the client. */
                if (!streamAlreadyTerminated && !m_clientContinuation.IsClosed())
                {
                    Close().wait();
                }
            }
            else
            {
                bool shouldCloseNow = true;
                if (m_streamHandler)
                    shouldCloseNow = m_streamHandler->OnStreamError(std::move(error), {EVENT_STREAM_RPC_SUCCESS, 0});
                if (!streamAlreadyTerminated && shouldCloseNow && !m_clientContinuation.IsClosed())
                {
                    Close().wait();
                }
            }

            return EVENT_STREAM_RPC_SUCCESS;
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

        void ClientOperation::OnContinuationMessage(
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            MessageType messageType,
            uint32_t messageFlags)
        {
            EventStreamRpcStatusCode errorCode = EVENT_STREAM_RPC_SUCCESS;
            const EventStreamHeader *modelHeader = nullptr;
            const EventStreamHeader *contentHeader = nullptr;
            Crt::String modelName;

            if (messageFlags & AWS_EVENT_STREAM_RPC_MESSAGE_FLAG_TERMINATE_STREAM)
            {
                const std::lock_guard<std::mutex> lock(m_continuationMutex);
                m_expectingClose = true;
            }

            m_messageCount += 1;

            modelHeader = GetHeaderByName(headers, Crt::String(SERVICE_MODEL_TYPE_HEADER));
            if (modelHeader == nullptr)
            {
                /* Missing required service model type header. */
                AWS_LOGF_ERROR(
                    AWS_LS_EVENT_STREAM_RPC_CLIENT,
                    "A required header (%s) could not be found in the message.",
                    SERVICE_MODEL_TYPE_HEADER);
                errorCode = EVENT_STREAM_RPC_UNMAPPED_DATA;
            }

            /* Verify that the model name matches. */
            if (!errorCode)
            {
                modelHeader->GetValueAsString(modelName);
                if (messageType == AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_APPLICATION_MESSAGE)
                {
                    if (m_messageCount == 1 && m_operationModelContext.GetInitialResponseModelName() != modelName)
                    {
                        AWS_LOGF_ERROR(
                            AWS_LS_EVENT_STREAM_RPC_CLIENT,
                            "The model name of the initial response did not match its expected model name.");
                        errorCode = EVENT_STREAM_RPC_UNMAPPED_DATA;
                    }
                    else if (
                        m_messageCount > 1 && m_operationModelContext.GetStreamingResponseModelName().has_value() &&
                        m_operationModelContext.GetStreamingResponseModelName().value() != modelName)
                    {
                        AWS_LOGF_ERROR(
                            AWS_LS_EVENT_STREAM_RPC_CLIENT,
                            "The model name of a subsequent response did not match its expected model name.");
                        errorCode = EVENT_STREAM_RPC_UNMAPPED_DATA;
                    }
                }
            }

            if (!errorCode)
            {
                Crt::String contentType;
                contentHeader = GetHeaderByName(headers, Crt::String(CONTENT_TYPE_HEADER));
                if (contentHeader == nullptr)
                {
                    /* Missing required content type header. */
                    AWS_LOGF_ERROR(
                        AWS_LS_EVENT_STREAM_RPC_CLIENT,
                        "A required header (%s) could not be found in the message.",
                        CONTENT_TYPE_HEADER);
                    errorCode = EVENT_STREAM_RPC_UNSUPPORTED_CONTENT_TYPE;
                }
                else if (contentHeader->GetValueAsString(contentType) && contentType != CONTENT_TYPE_APPLICATION_JSON)
                {
                    /* Missing required content type header. */
                    AWS_LOGF_ERROR(
                        AWS_LS_EVENT_STREAM_RPC_CLIENT,
                        "The content type (%s) header was specified with an unsupported value (%s).",
                        CONTENT_TYPE_HEADER,
                        contentType.c_str());
                    errorCode = EVENT_STREAM_RPC_UNSUPPORTED_CONTENT_TYPE;
                }
            }

            if (!errorCode)
            {
                if (messageType == AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_APPLICATION_MESSAGE)
                {
                    errorCode = HandleData(payload);
                }
                else
                {
                    errorCode = HandleError(modelName, payload, messageFlags);
                }
            }

            if (errorCode)
            {
                if (m_messageCount == 1)
                {
                    const std::lock_guard<std::mutex> lock(m_continuationMutex);
                    m_resultReceived = true;
                    RpcError promiseValue = {(EventStreamRpcStatusCode)errorCode, 0};
                    m_initialResponsePromise.set_value(TaggedResult(promiseValue));
                }
                else
                {
                    bool shouldClose = true;
                    if (m_streamHandler)
                        shouldClose = m_streamHandler->OnStreamError(nullptr, {errorCode, 0});
                    if (!m_clientContinuation.IsClosed() && shouldClose)
                    {
                        Close().wait();
                    }
                }
            }
        }

        std::future<RpcError> ClientOperation::Activate(
            const AbstractShapeBase *shape,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            /* Promises must be reset in case the client would like to send a subsequent request with the same
             * `ClientOperation`. */
            m_initialResponsePromise = {};
            {
                const std::lock_guard<std::mutex> lock(m_continuationMutex);
                m_resultReceived = false;
            }

            Crt::List<EventStreamHeader> headers;
            headers.emplace_back(EventStreamHeader(
                Crt::String(CONTENT_TYPE_HEADER), Crt::String(CONTENT_TYPE_APPLICATION_JSON), m_allocator));
            headers.emplace_back(
                EventStreamHeader(Crt::String(SERVICE_MODEL_TYPE_HEADER), GetModelName(), m_allocator));
            Crt::JsonObject payloadObject;
            shape->SerializeToJsonObject(payloadObject);
            Crt::String payloadString = payloadObject.View().WriteCompact();
            return m_clientContinuation.Activate(
                GetModelName(),
                headers,
                Crt::ByteBufFromCString(payloadString.c_str()),
                AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_APPLICATION_MESSAGE,
                0,
                onMessageFlushCallback);
        }

        void ClientOperation::OnContinuationClosed()
        {
            const std::lock_guard<std::mutex> lock(m_continuationMutex);
            if (!m_resultReceived)
            {
                m_initialResponsePromise.set_value(TaggedResult({EVENT_STREAM_RPC_CONTINUATION_CLOSED, 0}));
                m_resultReceived = true;
            }

            if (m_expectingClose)
            {
                m_expectingClose = false;
                if (!m_streamClosedCalled && m_streamHandler)
                {
                    m_streamHandler->OnStreamClosed();
                    m_streamClosedCalled = true;
                }
                m_closeReady.notify_one();
            }
        }

        void ClientOperation::WithLaunchMode(std::launch mode) noexcept
        {
            m_asyncLaunchMode = mode;
        }

        std::future<RpcError> ClientOperation::Close(OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            const std::lock_guard<std::mutex> lock(m_continuationMutex);
            if (m_expectingClose || m_clientContinuation.IsClosed())
            {
                std::promise<RpcError> errorPromise;
                errorPromise.set_value({EVENT_STREAM_RPC_CONTINUATION_CLOSED, 0});
                return errorPromise.get_future();
            }
            else
            {
                std::promise<RpcError> onTerminatePromise;

                int errorCode = AWS_OP_ERR;
                struct aws_event_stream_rpc_message_args msg_args;
                msg_args.headers = nullptr;
                msg_args.headers_count = 0;
                msg_args.payload = nullptr;
                msg_args.message_type = AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_APPLICATION_MESSAGE;
                msg_args.message_flags = AWS_EVENT_STREAM_RPC_MESSAGE_FLAG_TERMINATE_STREAM;

                /* This heap allocation is necessary so that the flush callback can still be invoked when this function
                 * returns. */
                OnMessageFlushCallbackContainer *callbackContainer =
                    Crt::New<OnMessageFlushCallbackContainer>(m_allocator, m_allocator);
                callbackContainer->onMessageFlushCallback = onMessageFlushCallback;
                callbackContainer->onFlushPromise = std::move(onTerminatePromise);

                if (m_clientContinuation.m_continuationToken)
                {
                    errorCode = aws_event_stream_rpc_client_continuation_send_message(
                        m_clientContinuation.m_continuationToken,
                        &msg_args,
                        s_protocolMessageCallback,
                        reinterpret_cast<void *>(callbackContainer));
                }

                if (errorCode)
                {
                    onTerminatePromise = std::move(callbackContainer->onFlushPromise);
                    std::promise<RpcError> errorPromise;
                    AWS_LOGF_ERROR(
                        AWS_LS_EVENT_STREAM_RPC_CLIENT,
                        "A CRT error occurred while closing the stream: %s",
                        Crt::ErrorDebugString(errorCode));
                    onTerminatePromise.set_value({EVENT_STREAM_RPC_CRT_ERROR, errorCode});
                    Crt::Delete(callbackContainer, m_allocator);
                }
                else
                {
                    m_expectingClose = true;
                    return callbackContainer->onFlushPromise.get_future();
                }

                return onTerminatePromise.get_future();
            }
        }

        void OperationError::s_customDeleter(OperationError *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(shape);
        }

    } /* namespace Eventstreamrpc */
} // namespace Aws
