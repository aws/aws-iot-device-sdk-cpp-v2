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
        };

        EventStreamHeader::EventStreamHeader(const struct aws_event_stream_header_value_pair &header)
            : m_underlyingHandle(header)
        {
        }

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

        Crt::Optional<Crt::ByteBuf> &MessageAmendment::GetPayload() noexcept { return m_payload; }

        void MessageAmendment::SetPayload(const Crt::Optional<Crt::ByteBuf> &payload) noexcept { m_payload = payload; }

        ClientConnectionOptions::ClientConnectionOptions()
            : Bootstrap(), SocketOptions(), TlsOptions(), HostName(), Port(0)
        {
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

        bool ClientConnection::Connect(
            const ClientConnectionOptions &connectionOptions,
            ConnectionLifecycleHandler *connectionLifecycleHandler,
            ConnectMessageAmender connectMessageAmender) noexcept
        {
            if (connectionLifecycleHandler == NULL)
            {
                return false;
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

            if (aws_event_stream_rpc_client_connection_connect(m_allocator, &connOptions))
            {
                return false;
            }

            return true;
        }

        const struct aws_event_stream_header_value_pair *EventStreamHeader::GetUnderlyingHandle() const
        {
            return &m_underlyingHandle;
        }

        void ClientConnection::SendPing(
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            s_sendPing(this, headers, payload, onMessageFlushCallback);
        }

        void ClientConnection::SendPingResponse(
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            s_sendPingResponse(this, headers, payload, onMessageFlushCallback);
        }

        void ClientConnection::s_sendPing(
            ClientConnection *connection,
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            s_sendProtocolMessage(
                connection, headers, payload, AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_PING, 0, onMessageFlushCallback);
        }

        void ClientConnection::s_sendPingResponse(
            ClientConnection *connection,
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            s_sendProtocolMessage(
                connection,
                headers,
                payload,
                AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_PING_RESPONSE,
                0,
                onMessageFlushCallback);
        }

        void ClientConnection::SendProtocolMessage(
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            MessageType messageType,
            uint32_t messageFlags,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            s_sendProtocolMessage(this, headers, payload, messageType, messageFlags, onMessageFlushCallback);
        }

        void ClientConnection::s_protocolMessageCallback(int errorCode, void *userData) noexcept
        {
            auto *callbackData = static_cast<OnMessageFlushCallbackContainer *>(userData);

            /* Call the user-provided callback. */
            if (callbackData->onMessageFlushCallback)
            {
                callbackData->onMessageFlushCallback(errorCode);
            }

            Crt::Delete(callbackData, callbackData->allocator);
        }

        void ClientConnection::s_sendProtocolMessage(
            ClientConnection *connection,
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            MessageType messageType,
            uint32_t messageFlags,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            /* Check if the connection has expired before attempting to send. */
            if (connection)
            {
                struct aws_array_list headersArray;
                AWS_ZERO_STRUCT(headersArray);
                OnMessageFlushCallbackContainer *callbackContainer = nullptr;
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

                    /* This heap allocation is necessary so that the callback can still be invoked after this function
                     * returns. */
                    callbackContainer =
                        Crt::New<OnMessageFlushCallbackContainer>(connection->m_allocator, connection->m_allocator);
                    callbackContainer->onMessageFlushCallback = onMessageFlushCallback;

                    errorCode = aws_event_stream_rpc_client_connection_send_protocol_message(
                        connection->m_underlyingConnection,
                        &msg_args,
                        s_protocolMessageCallback,
                        reinterpret_cast<void *>(callbackContainer));
                }

                /* Cleanup. */
                if (errorCode)
                {
                    onMessageFlushCallback(errorCode);
                    Crt::Delete(callbackContainer, connection->m_allocator);
                }

                if (aws_array_list_is_valid(&headersArray))
                {
                    aws_array_list_clean_up(&headersArray);
                }
            }
        }

        void ClientConnection::Close() noexcept
        {
            aws_event_stream_rpc_client_connection_close(this->m_underlyingConnection, AWS_OP_SUCCESS);
            this->m_clientState = DISCONNECTED;
        }

        void ClientConnection::Close(int errorCode) noexcept
        {
            aws_event_stream_rpc_client_connection_close(this->m_underlyingConnection, errorCode);
            this->m_clientState = DISCONNECTED;
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

        EventStreamHeader::~EventStreamHeader() { Crt::ByteBufDelete(m_valueByteBuf); }

        EventStreamHeader::EventStreamHeader(const EventStreamHeader &lhs) noexcept
            : m_allocator(lhs.m_allocator),
              m_valueByteBuf(
                  Crt::ByteBufNewCopy(lhs.m_valueByteBuf.allocator, lhs.m_valueByteBuf.buffer, lhs.m_valueByteBuf.len)),
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

        Crt::String EventStreamHeader::GetHeaderName() noexcept
        {
            return Crt::String(m_underlyingHandle.header_name, m_underlyingHandle.header_name_len);
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
                    nullptr);
                thisConnection->m_clientState = WAITING_FOR_CONNECT_ACK;
                return;

                /* Release if we're unable to allocate the connection's containing object. */
                aws_event_stream_rpc_client_connection_release(connection);
                errorCode = aws_last_error();
            }

            thisConnection->m_clientState = DISCONNECTED;

            if (thisConnection->m_lifecycleHandler->OnErrorCallback(errorCode))
            {
                thisConnection->Close(errorCode);
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
                            thisConnection->m_lifecycleHandler->OnConnectCallback();
                        }
                        else
                        {
                            thisConnection->m_clientState = DISCONNECTING;
                            thisConnection->Close(AWS_ERROR_EVENT_STREAM_RPC_CONNECTION_CLOSED);
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
                        thisConnection->Close(AWS_ERROR_EVENT_STREAM_RPC_PROTOCOL_ERROR);
                    }

                    break;

                default:
                    return;

                    if (thisConnection->m_lifecycleHandler->OnErrorCallback(
                            AWS_ERROR_EVENT_STREAM_RPC_UNKNOWN_PROTOCOL_MESSAGE))
                    {
                        thisConnection->Close(AWS_ERROR_EVENT_STREAM_RPC_UNKNOWN_PROTOCOL_MESSAGE);
                    }

                    break;
            }
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

        void ClientContinuation::Activate(
            const Crt::String &operationName,
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            MessageType messageType,
            uint32_t messageFlags,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            struct aws_array_list headersArray;
            OnMessageFlushCallbackContainer *callbackContainer = nullptr;
            int errorCode = AWS_OP_SUCCESS;

            if (m_continuationToken == nullptr)
            {
                errorCode = AWS_ERROR_INVALID_STATE;
            }

            if (!errorCode)
            {
                AWS_ZERO_STRUCT(headersArray);
                errorCode =
                    EventStreamCppToNativeCrtBuilder::s_fillNativeHeadersArray(headers, &headersArray, m_allocator);
            }

            if (!errorCode)
            {
                struct aws_event_stream_rpc_message_args msg_args;
                msg_args.headers = (struct aws_event_stream_header_value_pair *)headersArray.data;
                msg_args.headers_count = headers.size();
                msg_args.payload = payload.has_value() ? (aws_byte_buf *)(&(payload.value())) : nullptr;
                msg_args.message_type = messageType;
                msg_args.message_flags = messageFlags;

                /* This heap allocation is necessary so that the callback can still be invoked after this function
                 * returns. */
                callbackContainer = Crt::New<OnMessageFlushCallbackContainer>(m_allocator, m_allocator);
                callbackContainer->onMessageFlushCallback = onMessageFlushCallback;
                errorCode = aws_event_stream_rpc_client_continuation_activate(
                    m_continuationToken,
                    Crt::ByteCursorFromCString(operationName.c_str()),
                    &msg_args,
                    ClientConnection::s_protocolMessageCallback,
                    reinterpret_cast<void *>(callbackContainer));
            }

            /* Cleanup. */
            if (errorCode)
            {
                onMessageFlushCallback(errorCode);
                Crt::Delete(callbackContainer, m_allocator);
            }

            if (aws_array_list_is_valid(&headersArray))
            {
                aws_array_list_clean_up(&headersArray);
            }
        }

        void ClientContinuation::SendMessage(
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            MessageType messageType,
            uint32_t messageFlags,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            struct aws_array_list headersArray;
            OnMessageFlushCallbackContainer *callbackContainer = nullptr;
            int errorCode = AWS_OP_SUCCESS;

            if (m_continuationToken == nullptr)
            {
                errorCode = AWS_ERROR_INVALID_STATE;
            }

            if (!errorCode)
            {
                AWS_ZERO_STRUCT(headersArray);
                errorCode =
                    EventStreamCppToNativeCrtBuilder::s_fillNativeHeadersArray(headers, &headersArray, m_allocator);
            }

            if (!errorCode)
            {
                struct aws_event_stream_rpc_message_args msg_args;
                msg_args.headers = (struct aws_event_stream_header_value_pair *)headersArray.data;
                msg_args.headers_count = headers.size();
                msg_args.payload = payload.has_value() ? (aws_byte_buf *)(&(payload.value())) : nullptr;
                msg_args.message_type = messageType;
                msg_args.message_flags = messageFlags;

                /* This heap allocation is necessary so that the callback can still be invoked after this function
                 * returns. */
                callbackContainer = Crt::New<OnMessageFlushCallbackContainer>(m_allocator, m_allocator);
                callbackContainer->onMessageFlushCallback = onMessageFlushCallback;

                errorCode = aws_event_stream_rpc_client_continuation_send_message(
                    m_continuationToken,
                    &msg_args,
                    ClientConnection::s_protocolMessageCallback,
                    reinterpret_cast<void *>(callbackContainer));
            }

            /* Cleanup. */
            if (errorCode)
            {
                onMessageFlushCallback(errorCode);
                Crt::Delete(callbackContainer, m_allocator);
            }

            if (aws_array_list_is_valid(&headersArray))
            {
                aws_array_list_clean_up(&headersArray);
            }
        }

        bool ClientContinuation::IsClosed() noexcept
        {
            return aws_event_stream_rpc_client_continuation_is_closed(m_continuationToken);
        }

        ClientOperation::ClientOperation(ClientConnection &connection, StreamResponseHandler *streamHandler) noexcept
            : m_messageCount(0), m_streamHandler(streamHandler), m_operationContinuationHandler(),
              m_clientContinuation(connection.NewStream(&m_operationContinuationHandler))
        {
        }
    } /* namespace Eventstreamrpc */
} /* namespace Aws */
