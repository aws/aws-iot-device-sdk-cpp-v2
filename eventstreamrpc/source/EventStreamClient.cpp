/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/eventstreamrpc/EventStreamClient.h>

#include <aws/crt/Api.h>
#include <aws/crt/Config.h>
#include <aws/crt/auth/Credentials.h>

#include <algorithm>
#include <iostream>

/* TODO: Change to constexpr */
#define EVENTSTREAM_VERSION_HEADER ":version"
#define EVENTSTREAM_VERSION_STRING "0.1.0"
#define CONTENT_TYPE_HEADER ":content-type"
#define CONTENT_TYPE_APPLICATION_TEXT "text/plain"
#define CONTENT_TYPE_APPLICATION_JSON "application/json"
#define SERVICE_MODEL_TYPE_HEADER "service-model-type"

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
            std::promise<EventStreamRpcStatus> onFlushPromise;
        };

        MessageAmendment::MessageAmendment(const Crt::ByteBuf &payload) noexcept : m_headers(), m_payload(payload) {}

        MessageAmendment::MessageAmendment(const Crt::List<EventStreamHeader> &headers) noexcept
            : m_headers(headers), m_payload()
        {
        }

        MessageAmendment::MessageAmendment(Crt::List<EventStreamHeader> &&headers) noexcept
            : m_headers(headers), m_payload()
        {
        }

        MessageAmendment::MessageAmendment(
            const Crt::List<EventStreamHeader> &headers,
            Crt::Optional<Crt::ByteBuf> &payload) noexcept
            : m_headers(headers), m_payload(payload)
        {
        }

        Crt::List<EventStreamHeader> &MessageAmendment::GetHeaders() noexcept { return m_headers; }

        const Crt::Optional<Crt::ByteBuf> &MessageAmendment::GetPayload() const noexcept { return m_payload; }

        void MessageAmendment::SetPayload(const Crt::Optional<Crt::ByteBuf> &payload) noexcept { m_payload = payload; }

        ClientConnectionOptions::ClientConnectionOptions()
            : Bootstrap(), SocketOptions(), TlsOptions(), HostName(), Port(0)
        {
        }

        template <typename T>
        ProtectedPromise<T>::ProtectedPromise(std::promise<T> &&promise) noexcept
            : m_fulfilled(false), m_promise(std::move(promise))
        {
        }

        template <typename T>
        void ProtectedPromise<T>::Reset() noexcept
        {
            m_mutex.lock();
            m_promise = std::promise<T>();
            m_fulfilled = false;
            m_mutex.unlock();
        }

        template <typename T> ProtectedPromise<T>::ProtectedPromise() noexcept : m_fulfilled(false) {}

        template <typename T> void ProtectedPromise<T>::SetValue(T &&r) noexcept
        {
            if (m_mutex.try_lock())
            {
                if (!m_fulfilled)
                {
                    m_promise.set_value(r);
                    m_fulfilled = true;
                }
                m_mutex.unlock();
            }
        }

        template <typename T> void ProtectedPromise<T>::SetValue(const T &r) noexcept
        {
            if (m_mutex.try_lock())
            {
                if (!m_fulfilled)
                {
                    m_promise.set_value(r);
                    m_fulfilled = true;
                }
                m_mutex.unlock();
            }
        }

        template <typename T> std::future<T> ProtectedPromise<T>::GetFuture() noexcept
        {
            return m_promise.get_future();
        }

        class EventStreamCppToNativeCrtBuilder
        {
          private:
            friend class ClientConnection;
            friend class ClientContinuation;
            static int s_fillNativeHeadersArray(
                const Crt::List<EventStreamHeader> &headers,
                struct aws_array_list *headersArray,
                Crt::Allocator *m_allocator = Crt::g_allocator)
            {
                AWS_ZERO_STRUCT(*headersArray);
                /* Check if the connection has expired before attempting to send. */
                int errorCode = aws_event_stream_headers_list_init(headersArray, m_allocator);

                if (!errorCode)
                {
                    /* Populate the array with the underlying handle of each EventStreamHeader. */
                    for (auto &i : headers)
                    {
                        errorCode = aws_array_list_push_back(headersArray, i.GetUnderlyingHandle());

                        if (errorCode)
                        {
                            break;
                        }
                    }
                }

                return errorCode;
            }
        };

        ClientConnection::ClientConnection(Crt::Allocator *allocator) noexcept : m_allocator(allocator) {}

        ClientConnection::~ClientConnection() noexcept
        {
            if (m_underlyingConnection)
            {
                aws_event_stream_rpc_client_connection_release(m_underlyingConnection);
                m_underlyingConnection = nullptr;
            }
        }

        bool ConnectionLifecycleHandler::OnErrorCallback(int errorCode) { return true; }

        void ConnectionLifecycleHandler::OnPingCallback(
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload)
        {
            (void)headers;
            (void)payload;
        }

        void ConnectionLifecycleHandler::OnConnectCallback() {}

        void ConnectionLifecycleHandler::OnDisconnectCallback(int errorCode) { (void)errorCode; }

        std::future<EventStreamRpcStatus> ClientConnection::Connect(
            const ClientConnectionOptions &connectionOptions,
            ConnectionLifecycleHandler *connectionLifecycleHandler,
            ConnectMessageAmender connectMessageAmender) noexcept
        {
            m_connectAckedPromise.Reset();

            if (connectionLifecycleHandler == NULL)
            {
                m_connectAckedPromise.SetValue({EVENT_STREAM_RPC_NULL_PARAMETER, 0});
                return m_connectAckedPromise.GetFuture();
            }

            struct aws_event_stream_rpc_client_connection_options connOptions;
            AWS_ZERO_STRUCT(connOptions);
            connOptions.host_name = connectionOptions.HostName.c_str();
            connOptions.port = connectionOptions.Port;
            connOptions.socket_options = &connectionOptions.SocketOptions.GetImpl();
            connOptions.bootstrap = connectionOptions.Bootstrap->GetUnderlyingHandle();
            connOptions.on_connection_setup = ClientConnection::s_onConnectionSetup;
            connOptions.on_connection_protocol_message = ClientConnection::s_onProtocolMessage;
            connOptions.on_connection_shutdown = ClientConnection::s_onConnectionShutdown;
            connOptions.user_data = reinterpret_cast<void *>(this);
            m_lifecycleHandler = connectionLifecycleHandler;
            m_connectMessageAmender = connectMessageAmender;

            if (connectionOptions.TlsOptions.has_value())
            {
                connOptions.tls_options = connectionOptions.TlsOptions->GetUnderlyingHandle();
            }

            int crtError = aws_event_stream_rpc_client_connection_connect(m_allocator, &connOptions);
            if (crtError)
            {
                m_connectAckedPromise.SetValue({EVENT_STREAM_RPC_CRT_ERROR, crtError});
                return m_connectAckedPromise.GetFuture();
            }

            return m_connectAckedPromise.GetFuture();
        }

        std::future<EventStreamRpcStatus> ClientConnection::SendPing(
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return s_sendPing(this, headers, payload, onMessageFlushCallback);
        }

        std::future<EventStreamRpcStatus> ClientConnection::SendPingResponse(
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return s_sendPingResponse(this, headers, payload, onMessageFlushCallback);
        }

        std::future<EventStreamRpcStatus> ClientConnection::s_sendPing(
            ClientConnection *connection,
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return s_sendProtocolMessage(
                connection, headers, payload, AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_PING, 0, onMessageFlushCallback);
        }

        std::future<EventStreamRpcStatus> ClientConnection::s_sendPingResponse(
            ClientConnection *connection,
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return s_sendProtocolMessage(
                connection,
                headers,
                payload,
                AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_PING_RESPONSE,
                0,
                onMessageFlushCallback);
        }

        std::future<EventStreamRpcStatus> ClientConnection::SendProtocolMessage(
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            MessageType messageType,
            uint32_t messageFlags,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return s_sendProtocolMessage(this, headers, payload, messageType, messageFlags, onMessageFlushCallback);
        }

        void ClientConnection::s_protocolMessageCallback(int errorCode, void *userData) noexcept
        {
            auto *callbackData = static_cast<OnMessageFlushCallbackContainer *>(userData);

            if (errorCode)
            {
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

        std::future<EventStreamRpcStatus> ClientConnection::s_sendProtocolMessage(
            ClientConnection *connection,
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            MessageType messageType,
            uint32_t messageFlags,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            std::promise<EventStreamRpcStatus> onFlushPromise;
            OnMessageFlushCallbackContainer *callbackContainer = nullptr;
            struct aws_array_list headersArray;

            /* The caller should never pass a NULL connection. */
            AWS_FATAL_ASSERT(connection);

            int errorCode = EventStreamCppToNativeCrtBuilder::s_fillNativeHeadersArray(
                headers, &headersArray, connection->m_allocator);

            if (!errorCode)
            {
                struct aws_event_stream_rpc_message_args msg_args;
                msg_args.headers = (struct aws_event_stream_header_value_pair *)headersArray.data;
                msg_args.headers_count = headers.size();
                msg_args.payload = payload.has_value() ? (aws_byte_buf *)(&(payload.value())) : nullptr;
                msg_args.message_type = messageType;
                msg_args.message_flags = messageFlags;

                /* This heap allocation is necessary so that the callback can still be invoked when this function
                 * returns. */
                callbackContainer =
                    Crt::New<OnMessageFlushCallbackContainer>(connection->m_allocator, connection->m_allocator);
                callbackContainer->onMessageFlushCallback = onMessageFlushCallback;
                callbackContainer->onFlushPromise = std::move(onFlushPromise);

                errorCode = aws_event_stream_rpc_client_connection_send_protocol_message(
                    connection->m_underlyingConnection,
                    &msg_args,
                    ClientConnection::s_protocolMessageCallback,
                    reinterpret_cast<void *>(callbackContainer));
            }

            /* Cleanup. */
            if (aws_array_list_is_valid(&headersArray))
            {
                aws_array_list_clean_up(&headersArray);
            }

            if (errorCode)
            {
                Crt::Delete(callbackContainer, connection->m_allocator);
                onFlushPromise.set_value({EVENT_STREAM_RPC_CRT_ERROR, errorCode});
            }
            else
            {
                return callbackContainer->onFlushPromise.get_future();
            }

            return onFlushPromise.get_future();
        }

        void ClientConnection::Close() noexcept
        {
            aws_event_stream_rpc_client_connection_close(this->m_underlyingConnection, AWS_OP_SUCCESS);
            this->m_clientState = DISCONNECTED;
        }

        EventStreamHeader::EventStreamHeader(const struct aws_event_stream_header_value_pair &header)
            : m_allocator(nullptr), m_underlyingHandle(header)
        {
        }

        EventStreamHeader::EventStreamHeader(
            const Crt::String &name,
            const Crt::String &value,
            Crt::Allocator *allocator) noexcept
            : m_allocator(allocator)
        {
            m_underlyingHandle.header_name_len = (uint8_t)name.length();
            (void)strncpy(m_underlyingHandle.header_name, name.c_str(), std::min((int)name.length(), INT8_MAX));
            m_underlyingHandle.header_value_type = AWS_EVENT_STREAM_HEADER_STRING;
            m_valueByteBuf = Crt::ByteBufNewCopy(allocator, (uint8_t *)value.c_str(), value.length());
            m_underlyingHandle.header_value.variable_len_val = m_valueByteBuf.buffer;
            m_underlyingHandle.header_value_len = (uint16_t)m_valueByteBuf.len;
        }

        EventStreamHeader::~EventStreamHeader()
        {
            if (m_allocator)
            {
                Crt::ByteBufDelete(m_valueByteBuf);
            }
        }

        EventStreamHeader::EventStreamHeader(const EventStreamHeader &lhs) noexcept
            : m_allocator(lhs.m_allocator),
              m_valueByteBuf(Crt::ByteBufNewCopy(lhs.m_allocator, lhs.m_valueByteBuf.buffer, lhs.m_valueByteBuf.len)),
              m_underlyingHandle(lhs.m_underlyingHandle)
        {
            m_underlyingHandle.header_value.variable_len_val = m_valueByteBuf.buffer;
            m_underlyingHandle.header_value_len = m_valueByteBuf.len;
        }

        EventStreamHeader::EventStreamHeader(EventStreamHeader &&rhs) noexcept
            : m_allocator(rhs.m_allocator), m_valueByteBuf(rhs.m_valueByteBuf),
              m_underlyingHandle(rhs.m_underlyingHandle)
        {
            rhs.m_valueByteBuf.allocator = NULL;
            rhs.m_valueByteBuf.buffer = NULL;
        }

        const struct aws_event_stream_header_value_pair *EventStreamHeader::GetUnderlyingHandle() const
        {
            return &m_underlyingHandle;
        }

        Crt::String EventStreamHeader::GetHeaderName() const noexcept
        {
            return Crt::String(m_underlyingHandle.header_name, m_underlyingHandle.header_name_len);
        }

        bool EventStreamHeader::GetValueAsString(Crt::String &value) const noexcept
        {
            if (m_underlyingHandle.header_value_type != AWS_EVENT_STREAM_HEADER_STRING)
            {
                return false;
            }
            Crt::StringView viewFromHere = Crt::ByteCursorToStringView(Crt::ByteCursorFromByteBuf(m_valueByteBuf));
            value = {viewFromHere.begin(), viewFromHere.end()};

            return true;
        }

        ClientContinuation ClientConnection::NewStream(ClientContinuationHandler *clientContinuationHandler) noexcept
        {
            return ClientContinuation(this, clientContinuationHandler, m_allocator);
        }

        void ClientConnection::s_onConnectionSetup(
            struct aws_event_stream_rpc_client_connection *connection,
            int errorCode,
            void *userData) noexcept
        {
            /* The `userData` pointer is used to pass `this` of a `ClientConnection` object. */
            auto *thisConnection = static_cast<ClientConnection *>(userData);

            if (!errorCode)
            {
                thisConnection->m_underlyingConnection = connection;
                MessageAmendment messageAmendment;
                auto &messageAmendmentHeaders = messageAmendment.GetHeaders();

                if (thisConnection->m_connectMessageAmender)
                {
                    MessageAmendment &connectAmendment = thisConnection->m_connectMessageAmender();
                    auto &amenderHeaderList = connectAmendment.GetHeaders();
                    /* The version header is necessary for establishing the connection. */
                    messageAmendment.AddHeader(EventStreamHeader(
                        Crt::String(EVENTSTREAM_VERSION_HEADER),
                        Crt::String(EVENTSTREAM_VERSION_STRING),
                        thisConnection->m_allocator));
                    /* Note that we are prepending headers from the user-provided amender. */
                    messageAmendmentHeaders.splice(messageAmendmentHeaders.end(), amenderHeaderList);
                    messageAmendment.SetPayload(connectAmendment.GetPayload());
                }

                /* Send a CONNECT packet to the server. */
                s_sendProtocolMessage(
                    thisConnection,
                    messageAmendment.GetHeaders(),
                    messageAmendment.GetPayload(),
                    AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_CONNECT,
                    0,
                    thisConnection->m_onConnectRequestCallback);
                thisConnection->m_clientState = WAITING_FOR_CONNECT_ACK;
                return;

                /* Release if we're unable to allocate the connection's containing object. */
                aws_event_stream_rpc_client_connection_release(connection);
                errorCode = aws_last_error();
            }

            thisConnection->m_clientState = DISCONNECTED;

            if (thisConnection->m_lifecycleHandler->OnErrorCallback(errorCode))
            {
                thisConnection->Close();
            }
        }

        void MessageAmendment::AddHeader(EventStreamHeader &&header) noexcept { m_headers.push_back(header); }

        void ClientConnection::s_onConnectionShutdown(
            struct aws_event_stream_rpc_client_connection *connection,
            int errorCode,
            void *userData) noexcept
        {
            (void)connection;
            /* The `userData` pointer is used to pass `this` of a `ClientConnection` object. */
            auto *thisConnection = static_cast<ClientConnection *>(userData);

            /* Protect setting future value since the connection might shut down while processing CONNACK. */
            thisConnection->m_connectAckedPromise.SetValue(
                {EVENT_STREAM_RPC_CONNECTION_CLOSED_BEFORE_CONNACK, errorCode});

            thisConnection->m_lifecycleHandler->OnDisconnectCallback(errorCode);
        }

        void ClientConnection::s_onProtocolMessage(
            struct aws_event_stream_rpc_client_connection *connection,
            const struct aws_event_stream_rpc_message_args *messageArgs,
            void *userData) noexcept
        {
            AWS_FATAL_ASSERT(messageArgs);
            (void)connection;

            /* The `userData` pointer is used to pass `this` of a `ClientConnection` object. */
            auto *thisConnection = static_cast<ClientConnection *>(userData);
            Crt::List<EventStreamHeader> pingHeaders;

            switch (messageArgs->message_type)
            {
                case AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_CONNECT_ACK:

                    if (thisConnection->m_clientState == WAITING_FOR_CONNECT_ACK)
                    {
                        if (messageArgs->message_flags & AWS_EVENT_STREAM_RPC_MESSAGE_FLAG_CONNECTION_ACCEPTED)
                        {
                            thisConnection->m_clientState = CONNECTED;
                            thisConnection->m_connectAckedPromise.SetValue({EVENT_STREAM_RPC_SUCCESS, 0});
                            thisConnection->m_lifecycleHandler->OnConnectCallback();
                        }
                        else
                        {
                            thisConnection->m_clientState = DISCONNECTING;
                            thisConnection->Close();
                        }
                    }

                    break;

                case AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_PING:

                    for (size_t i = 0; i < messageArgs->headers_count; ++i)
                    {
                        pingHeaders.push_back(EventStreamHeader(messageArgs->headers[i]));
                    }

                    if (messageArgs->payload)
                    {
                        thisConnection->m_lifecycleHandler->OnPingCallback(pingHeaders, *messageArgs->payload);
                    }
                    else
                    {
                        thisConnection->m_lifecycleHandler->OnPingCallback(pingHeaders, Crt::Optional<Crt::ByteBuf>());
                    }

                    break;

                case AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_PING_RESPONSE:
                    return;
                    break;

                case AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_PROTOCOL_ERROR:
                case AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_INTERNAL_ERROR:

                    if (thisConnection->m_lifecycleHandler->OnErrorCallback(AWS_ERROR_EVENT_STREAM_RPC_PROTOCOL_ERROR))
                    {
                        thisConnection->Close();
                    }

                    break;

                default:
                    return;

                    if (thisConnection->m_lifecycleHandler->OnErrorCallback(EVENT_STREAM_RPC_UNKNOWN_PROTOCOL_MESSAGE))
                    {
                        thisConnection->Close();
                    }

                    break;
            }
        }

        void AbstractShapeBase::s_customDeleter(AbstractShapeBase *shape) noexcept
        {
            Crt::Delete<AbstractShapeBase>(shape, shape->m_allocator);
        }

        ClientContinuation::ClientContinuation(
            ClientConnection *connection,
            ClientContinuationHandler *handler,
            Crt::Allocator *allocator) noexcept
            : m_allocator(allocator), m_handler(handler)
        {
            struct aws_event_stream_rpc_client_stream_continuation_options options;
            options.on_continuation = ClientContinuation::s_onContinuationMessage;
            options.on_continuation_closed = ClientContinuation::s_onContinuationClosed;
            options.user_data = reinterpret_cast<void *>(connection);

            m_continuationToken =
                aws_event_stream_rpc_client_connection_new_stream(connection->m_underlyingConnection, &options);
        }

        void ClientContinuation::s_onContinuationMessage(
            struct aws_event_stream_rpc_client_continuation_token *continuationToken,
            const struct aws_event_stream_rpc_message_args *messageArgs,
            void *userData) noexcept
        {
            (void)continuationToken;
            /* The `userData` pointer is used to pass `this` of a `ClientContinuation` object. */
            auto *thisContinuationToken = static_cast<ClientContinuation *>(userData);

            Crt::List<EventStreamHeader> continuationMessageHeaders;
            for (size_t i = 0; i < messageArgs->headers_count; ++i)
            {
                continuationMessageHeaders.push_back(EventStreamHeader(messageArgs->headers[i]));
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

            thisContinuationToken->m_handler->OnContinuationMessage(
                continuationMessageHeaders, payload, messageArgs->message_type, messageArgs->message_flags);
        }

        void ClientContinuation::s_onContinuationClosed(
            struct aws_event_stream_rpc_client_continuation_token *continuationToken,
            void *userData) noexcept
        {
            (void)continuationToken;
            /* The `userData` pointer is used to pass `this` of a `ClientContinuation` object. */
            auto *thisContinuationToken = static_cast<ClientContinuation *>(userData);

            thisContinuationToken->m_handler->OnContinuationClosed();
        }

        std::future<EventStreamRpcStatus> ClientContinuation::Activate(
            const Crt::String &operationName,
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            MessageType messageType,
            uint32_t messageFlags,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            struct aws_array_list headersArray;
            OnMessageFlushCallbackContainer *callbackContainer = nullptr;
            std::promise<EventStreamRpcStatus> onFlushPromise;

            if (m_continuationToken == nullptr)
            {
                onFlushPromise.set_value({EVENT_STREAM_RPC_NULL_PARAMETER, 0});
                return onFlushPromise.get_future();
            }

            int errorCode =
                EventStreamCppToNativeCrtBuilder::s_fillNativeHeadersArray(headers, &headersArray, m_allocator);

            if (!errorCode)
            {
                struct aws_event_stream_rpc_message_args msg_args;
                msg_args.headers = (struct aws_event_stream_header_value_pair *)headersArray.data;
                msg_args.headers_count = headers.size();
                msg_args.payload = payload.has_value() ? (aws_byte_buf *)(&(payload.value())) : nullptr;
                msg_args.message_type = messageType;
                msg_args.message_flags = messageFlags;

                /* This heap allocation is necessary so that the callback can still be invoked when this function
                 * returns. */
                callbackContainer = Crt::New<OnMessageFlushCallbackContainer>(m_allocator, m_allocator);
                callbackContainer->onMessageFlushCallback = onMessageFlushCallback;
                callbackContainer->onFlushPromise = std::move(onFlushPromise);

                errorCode = aws_event_stream_rpc_client_continuation_activate(
                    m_continuationToken,
                    Crt::ByteCursorFromCString(operationName.c_str()),
                    &msg_args,
                    ClientConnection::s_protocolMessageCallback,
                    reinterpret_cast<void *>(callbackContainer));
            }

            /* Cleanup. */
            if (aws_array_list_is_valid(&headersArray))
            {
                aws_array_list_clean_up(&headersArray);
            }

            if (errorCode)
            {
                Crt::Delete(callbackContainer, m_allocator);
                onFlushPromise.set_value({EVENT_STREAM_RPC_CRT_ERROR, errorCode});
            }
            else
            {
                return callbackContainer->onFlushPromise.get_future();
            }

            return onFlushPromise.get_future();
        }

        std::future<EventStreamRpcStatus> ClientContinuation::SendMessage(
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            MessageType messageType,
            uint32_t messageFlags,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            struct aws_array_list headersArray;
            OnMessageFlushCallbackContainer *callbackContainer = nullptr;
            std::promise<EventStreamRpcStatus> onFlushPromise;

            if (m_continuationToken == nullptr)
            {
                onFlushPromise.set_value({EVENT_STREAM_RPC_NULL_PARAMETER, 0});
                return onFlushPromise.get_future();
            }

            int errorCode =
                EventStreamCppToNativeCrtBuilder::s_fillNativeHeadersArray(headers, &headersArray, m_allocator);

            if (!errorCode)
            {
                struct aws_event_stream_rpc_message_args msg_args;
                msg_args.headers = (struct aws_event_stream_header_value_pair *)headersArray.data;
                msg_args.headers_count = headers.size();
                msg_args.payload = payload.has_value() ? (aws_byte_buf *)(&(payload.value())) : nullptr;
                msg_args.message_type = messageType;
                msg_args.message_flags = messageFlags;

                /* This heap allocation is necessary so that the callback can still be invoked when this function
                 * returns. */
                callbackContainer = Crt::New<OnMessageFlushCallbackContainer>(m_allocator, m_allocator);
                callbackContainer->onMessageFlushCallback = onMessageFlushCallback;
                callbackContainer->onFlushPromise = std::move(onFlushPromise);

                errorCode = aws_event_stream_rpc_client_continuation_send_message(
                    m_continuationToken,
                    &msg_args,
                    ClientConnection::s_protocolMessageCallback,
                    reinterpret_cast<void *>(callbackContainer));
            }

            /* Cleanup. */
            if (aws_array_list_is_valid(&headersArray))
            {
                aws_array_list_clean_up(&headersArray);
            }

            if (errorCode)
            {
                Crt::Delete(callbackContainer, m_allocator);
                onFlushPromise.set_value({EVENT_STREAM_RPC_CRT_ERROR, errorCode});
            }
            else
            {
                return callbackContainer->onFlushPromise.get_future();
            }

            return onFlushPromise.get_future();
        }

        bool ClientContinuation::IsClosed() noexcept
        {
            return aws_event_stream_rpc_client_continuation_is_closed(m_continuationToken);
        }

        OperationError::OperationError(Crt::Allocator *allocator) noexcept
            : AbstractShapeBase(allocator), m_errorCode(0)
        {
        }
        OperationError::OperationError(int errorCode, Crt::Allocator *allocator) noexcept
            : AbstractShapeBase(allocator), m_errorCode(errorCode)
        {
        }

        void OperationError::SerializeToJsonObject(Crt::JsonObject &payloadObject) const { (void)payloadObject; }

        Crt::String AbstractShapeBase::GetModelName() const noexcept { return Crt::String(""); }

        AbstractShapeBase::AbstractShapeBase(Crt::Allocator *allocator) noexcept : m_allocator(allocator) {}

        OperationResponse::OperationResponse(Crt::Allocator *allocator) noexcept : AbstractShapeBase(allocator) {}

        OperationRequest::OperationRequest(Crt::Allocator *allocator) noexcept : AbstractShapeBase(allocator) {}

        ClientOperation::ClientOperation(
            ClientConnection &connection,
            StreamResponseHandler *streamHandler,
            const ResponseRetriever *responseRetriever,
            Crt::Allocator *allocator) noexcept
            : m_messageCount(0), m_allocator(allocator), m_streamHandler(streamHandler),
              m_responseRetriever(responseRetriever), m_clientContinuation(connection.NewStream(this))
        {
        }

        TaggedResponse::TaggedResponse(Crt::ScopedResource<OperationResponse> response) noexcept
            : m_responseType(EXPECTED_RESPONSE)
        {
            m_responseResult.m_response = std::move(response);
        }

        TaggedResponse::TaggedResponse(Crt::ScopedResource<OperationError> error) noexcept
            : m_responseType(ERROR_RESPONSE)
        {
            m_responseResult.m_error = std::move(error);
        }

        TaggedResponse::TaggedResponse(TaggedResponse &&taggedResponse) noexcept
        {
            if (m_responseType == EXPECTED_RESPONSE)
            {
                m_responseResult.m_response = std::move(taggedResponse.m_responseResult.m_response);
            }
            else if (m_responseType == ERROR_RESPONSE)
            {
                m_responseResult.m_error = std::move(taggedResponse.m_responseResult.m_error);
            }
        }

        TaggedResponse::operator bool() const noexcept
        {
            if (m_responseType == EXPECTED_RESPONSE)
            {
                return true;
            }
            else if (m_responseType == ERROR_RESPONSE)
            {
                return false;
            }
            else
            {
                return false;
            }
        }

        OperationResponse *TaggedResponse::GetResponse()
        {
            if (m_responseType == EXPECTED_RESPONSE)
            {
                return m_responseResult.m_response.get();
            }
            else
            {
                return nullptr;
            }
        }

        OperationError *TaggedResponse::GetError()
        {
            if (m_responseType == ERROR_RESPONSE)
            {
                return m_responseResult.m_error.get();
            }
            else
            {
                return nullptr;
            }
        }

        std::future<TaggedResponse> ClientOperation::GetResponse() noexcept
        {
            return m_initialResponsePromise.GetFuture();
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

        int ClientOperation::HandleData(const Crt::String &modelName, const Crt::Optional<Crt::ByteBuf> &payload)
        {
            ExpectedResponseFactory responseFactory = nullptr;

            /* Responses after the first message don't necessarily have the same shape as the first. */
            if (m_messageCount == 1)
            {
                responseFactory = m_responseRetriever->GetLoneResponseFromModelName(GetModelName());
            }
            else
            {
                responseFactory = m_responseRetriever->GetStreamingResponseFromModelName(GetModelName());
            }

            if (responseFactory == nullptr)
            {
                return EVENT_STREAM_RPC_UNMAPPED_DATA;
            }
            else
            {
                Crt::StringView payloadStringView;
                if (payload.has_value())
                {
                    payloadStringView = Crt::ByteCursorToStringView(Crt::ByteCursorFromByteBuf(payload.value()));
                }
                /* The value of this hashmap contains the function that allocates the response object from the
                 * payload. */
                Crt::ScopedResource<OperationResponse> response = responseFactory(payloadStringView, m_allocator);
                if (modelName != response->GetModelName())
                {
                    /* TODO: Log an error */
                    return EVENT_STREAM_RPC_UNMAPPED_DATA;
                }
                if (m_messageCount == 1)
                {
                    m_initialResponsePromise.SetValue(TaggedResponse(std::move(response)));
                }
                else
                {
                    m_streamHandler->OnStreamEvent(std::move(response));
                }
            }
            return AWS_OP_SUCCESS;
        }

        int ClientOperation::HandleError(
            const Crt::String &modelName,
            const Crt::Optional<Crt::ByteBuf> &payload,
            uint16_t messageFlags)
        {
            bool streamAlreadyTerminated = messageFlags & AWS_EVENT_STREAM_RPC_MESSAGE_FLAG_TERMINATE_STREAM;
            ErrorResponseFactory errorFactory = m_responseRetriever->GetErrorResponseFromModelName(modelName);
            if (errorFactory == nullptr)
            {
                return EVENT_STREAM_RPC_UNMAPPED_DATA;
            }
            else
            {
                Crt::StringView payloadStringView;
                if (payload.has_value())
                {
                    payloadStringView = Crt::ByteCursorToStringView(Crt::ByteCursorFromByteBuf(payload.value()));
                }

                /* The value of this hashmap contains the function that allocates the error from the
                 * payload. */
                Crt::ScopedResource<OperationError> error = errorFactory(payloadStringView, m_allocator);
                if (m_messageCount == 1)
                {
                    m_initialResponsePromise.SetValue(TaggedResponse(std::move(error)));
                    /* Close the stream unless the server already closed it for us. This condition is checked
                     * so that TERMINATE_STREAM messages aren't resent by the client. */
                    if (!streamAlreadyTerminated)
                    {
                        Close();
                    }
                }
                else
                {
                    if (!streamAlreadyTerminated && m_streamHandler->OnStreamError(std::move(error)))
                    {
                        Close();
                    }
                }

                m_streamHandler->OnStreamError(std::move(error));
            }
            return AWS_OP_SUCCESS;
        }

        bool StreamResponseHandler::OnStreamError(Crt::ScopedResource<OperationError> response) { return true; }

        void StreamResponseHandler::OnStreamEvent(Crt::ScopedResource<OperationResponse> response) {}

        void StreamResponseHandler::OnStreamClosed() {}

        void ClientOperation::OnContinuationMessage(
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            MessageType messageType,
            uint32_t messageFlags)
        {
            int errorCode = AWS_OP_SUCCESS;
            const EventStreamHeader *modelHeader = nullptr;
            const EventStreamHeader *contentHeader = nullptr;

            m_messageCount += 1;

            modelHeader = GetHeaderByName(headers, Crt::String(SERVICE_MODEL_TYPE_HEADER));
            if (modelHeader == nullptr)
            {
                /* TERMINATE_STREAM message should be empty. */
                if (messageFlags & AWS_EVENT_STREAM_RPC_MESSAGE_FLAG_TERMINATE_STREAM)
                    errorCode = EVENT_STREAM_RPC_STREAM_CLOSED_ERROR;
                else
                    /* Missing required service model type header. */
                    errorCode = EVENT_STREAM_RPC_UNMAPPED_DATA;
            }

            if (!errorCode)
            {
                Crt::String contentType;
                contentHeader = GetHeaderByName(headers, Crt::String(CONTENT_TYPE_HEADER));
                if (contentHeader == nullptr)
                {
                    /* Missing required content type header. */
                    errorCode = EVENT_STREAM_RPC_UNMAPPED_DATA;
                }
                else if (contentHeader->GetValueAsString(contentType) && contentType != CONTENT_TYPE_APPLICATION_JSON)
                {
                    errorCode = EVENT_STREAM_RPC_UNSUPPORTED_CONTENT_TYPE;
                }
            }

            if (!errorCode)
            {
                Crt::String modelName;
                modelHeader->GetValueAsString(modelName);
                if (messageType == AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_APPLICATION_MESSAGE)
                {
                    errorCode = HandleData(modelName, payload);
                }
                else
                {
                    errorCode = HandleError(modelName, payload, messageFlags);
                }
            }

            /* Cleanup. */
            if (errorCode)
            {
                /* Generate an error code here. */
                Crt::ScopedResource<OperationError> operationError(
                    Crt::New<OperationError>(m_allocator, errorCode, m_allocator), OperationError::s_customDeleter);
                if (m_streamHandler->OnStreamError(std::move(operationError)))
                {
                    this->Close();
                }
            }
        }

        std::future<EventStreamRpcStatus> ClientOperation::Activate(
            const OperationRequest *shape,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            Crt::List<EventStreamHeader> headers;
            headers.push_back(EventStreamHeader(
                Crt::String(CONTENT_TYPE_HEADER), Crt::String(CONTENT_TYPE_APPLICATION_JSON), m_allocator));
            headers.push_back(EventStreamHeader(Crt::String(SERVICE_MODEL_TYPE_HEADER), GetModelName(), m_allocator));
            Crt::JsonObject payloadObject;
            
            /* Promise must be reset in case the client would like to send a subsequent request with the same object. */
            m_initialResponsePromise.Reset();

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
            /* Because m_initialResponsePromise is a ProtectedPromise, setting its value when it's already set
             * will just do nothing. */
            Crt::ScopedResource<OperationError> operationError(
                Crt::New<OperationError>(m_allocator, EVENT_STREAM_RPC_STREAM_CLOSED_ERROR, m_allocator),
                OperationError::s_customDeleter);
            TaggedResponse taggedResponse(std::move(operationError));
            m_initialResponsePromise.SetValue(std::move(taggedResponse));

            m_closedPromise.set_value();

            if (m_streamHandler)
            {
                m_streamHandler->OnStreamClosed();
            }
        }

        std::future<void> ClientOperation::Close(OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            m_clientContinuation.SendMessage(
                Crt::List<EventStreamHeader>(),
                Crt::Optional<Crt::ByteBuf>(),
                AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_APPLICATION_MESSAGE,
                AWS_EVENT_STREAM_RPC_MESSAGE_FLAG_TERMINATE_STREAM,
                onMessageFlushCallback);
            return m_closedPromise.get_future();
        }

        void OperationError::s_customDeleter(OperationError *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(shape);
        }

        void OperationResponse::s_customDeleter(OperationResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(shape);
        }

        void OperationResponse::SerializeToJsonObject(Crt::JsonObject &payloadObject) const {}

    } /* namespace Eventstreamrpc */
} // namespace Aws
