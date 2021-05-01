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
#define EVENTSTREAM_VERSION "0.1.0"

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

        EventstreamHeader::EventstreamHeader(const struct aws_event_stream_header_value_pair &header)
            : m_underlyingHandle(header)
        {
        }

        MessageAmendment::MessageAmendment(const Crt::ByteBuf &payload) noexcept : m_headers(), m_payload(payload) {}

        MessageAmendment::MessageAmendment(const Crt::List<EventstreamHeader> &headers) noexcept
            : m_headers(headers), m_payload()
        {
        }

        MessageAmendment::MessageAmendment(Crt::List<EventstreamHeader> &&headers) noexcept
            : m_headers(headers), m_payload()
        {
        }

        MessageAmendment::MessageAmendment(
            const Crt::List<EventstreamHeader> &headers,
            Crt::Optional<Crt::ByteBuf> &payload) noexcept
            : m_headers(headers), m_payload(payload)
        {
        }

        Crt::List<EventstreamHeader> &MessageAmendment::GetHeaders() noexcept { return m_headers; }

        Crt::Optional<Crt::ByteBuf> &MessageAmendment::GetPayload() noexcept { return m_payload; }

        void MessageAmendment::SetPayload(const Crt::Optional<Crt::ByteBuf> &payload) noexcept { m_payload = payload; }

        EventstreamRpcConnectionOptions::EventstreamRpcConnectionOptions()
            : Bootstrap(), SocketOptions(), TlsOptions(), HostName(), Port(0)
        {
        }

        EventstreamRpcConnection::EventstreamRpcConnection(Crt::Allocator *allocator) noexcept : m_allocator(allocator)
        {
        }

        EventstreamRpcConnection::~EventstreamRpcConnection() noexcept
        {
            if (m_underlyingConnection)
            {
                aws_event_stream_rpc_client_connection_release(m_underlyingConnection);
                m_underlyingConnection = nullptr;
            }
        }

        bool ConnectionLifecycleHandler::OnErrorCallback(int errorCode) { return true; }

        void ConnectionLifecycleHandler::OnPingCallback(
            const Crt::List<EventstreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload)
        {
            (void)headers;
            (void)payload;
        }

        void ConnectionLifecycleHandler::OnConnectCallback() {}

        void ConnectionLifecycleHandler::OnDisconnectCallback(int errorCode) { (void)errorCode; }

        bool EventstreamRpcConnection::Connect(
            const EventstreamRpcConnectionOptions &connectionOptions,
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
            connOptions.on_connection_setup = EventstreamRpcConnection::s_onConnectionSetup;
            connOptions.on_connection_protocol_message = EventstreamRpcConnection::s_onProtocolMessage;
            connOptions.on_connection_shutdown = EventstreamRpcConnection::s_onConnectionShutdown;
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

        const struct aws_event_stream_header_value_pair *EventstreamHeader::GetUnderlyingHandle() const
        {
            return &m_underlyingHandle;
        }

        void EventstreamRpcConnection::SendPing(
            const Crt::List<EventstreamHeader> &headers,
            Crt::Optional<Crt::ByteBuf> &payload,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            s_sendPing(this, headers, payload, onMessageFlushCallback);
        }

        void EventstreamRpcConnection::SendPingResponse(
            const Crt::List<EventstreamHeader> &headers,
            Crt::Optional<Crt::ByteBuf> &payload,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            s_sendPingResponse(this, headers, payload, onMessageFlushCallback);
        }

        void EventstreamRpcConnection::s_sendPing(
            EventstreamRpcConnection *connection,
            const Crt::List<EventstreamHeader> &headers,
            Crt::Optional<Crt::ByteBuf> &payload,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            s_sendProtocolMessage(
                connection, headers, payload, AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_PING, 0, onMessageFlushCallback);
        }

        void EventstreamRpcConnection::s_sendPingResponse(
            EventstreamRpcConnection *connection,
            const Crt::List<EventstreamHeader> &headers,
            Crt::Optional<Crt::ByteBuf> &payload,
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

        void EventstreamRpcConnection::SendProtocolMessage(
            const Crt::List<EventstreamHeader> &headers,
            Crt::Optional<Crt::ByteBuf> &payload,
            MessageType messageType,
            uint32_t flags,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            s_sendProtocolMessage(this, headers, payload, messageType, flags, onMessageFlushCallback);
        }

        void EventstreamRpcConnection::s_protocolMessageCallback(int errorCode, void *userData) noexcept
        {
            auto *callbackData = static_cast<OnMessageFlushCallbackContainer *>(userData);

            /* Call the user-provided callback. */
            if (callbackData->onMessageFlushCallback)
                callbackData->onMessageFlushCallback(errorCode);

            Crt::Delete(callbackData, callbackData->allocator);
        }

        void EventstreamRpcConnection::s_sendProtocolMessage(
            EventstreamRpcConnection *connection,
            const Crt::List<EventstreamHeader> &headers,
            Crt::Optional<Crt::ByteBuf> &payload,
            MessageType messageType,
            uint32_t flags,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            struct aws_event_stream_rpc_message_args msg_args;
            AWS_ZERO_STRUCT(msg_args);
            struct aws_array_list headersArray;
            AWS_ZERO_STRUCT(headersArray);

            /* Check if the connection has expired before attempting to send. */
            if (connection)
            {
                OnMessageFlushCallbackContainer *callbackContainer = nullptr;
                int errorCode = aws_event_stream_headers_list_init(&headersArray, connection->m_allocator);
                if (!errorCode)
                {
                    /* Populate the array with the underlying handle of each EventstreamHeader. */
                    for (auto &i : headers)
                    {
                        errorCode = aws_array_list_push_back(&headersArray, i.GetUnderlyingHandle());
                        if (errorCode)
                            break;
                    }
                }

                if (!errorCode)
                {
                    msg_args.message_flags = flags;
                    msg_args.message_type = messageType;
                    msg_args.headers = (struct aws_event_stream_header_value_pair *)headersArray.data;
                    msg_args.headers_count = aws_array_list_length(&headersArray);

                    if (payload.has_value())
                    {
                        msg_args.payload = &payload.value();
                    }

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

        void EventstreamRpcConnection::Close() noexcept
        {
            aws_event_stream_rpc_client_connection_close(this->m_underlyingConnection, AWS_OP_SUCCESS);
            this->m_clientState = DISCONNECTED;
        }

        void EventstreamRpcConnection::Close(int errorCode) noexcept
        {
            aws_event_stream_rpc_client_connection_close(this->m_underlyingConnection, errorCode);
            this->m_clientState = DISCONNECTED;
        }

        EventstreamHeader::EventstreamHeader(
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

        EventstreamHeader::~EventstreamHeader() { Crt::ByteBufDelete(m_valueByteBuf); }

        EventstreamHeader::EventstreamHeader(const EventstreamHeader &lhs) noexcept
            : m_allocator(lhs.m_allocator),
              m_valueByteBuf(
                  Crt::ByteBufNewCopy(lhs.m_valueByteBuf.allocator, lhs.m_valueByteBuf.buffer, lhs.m_valueByteBuf.len)),
              m_underlyingHandle(lhs.m_underlyingHandle)
        {
            m_underlyingHandle.header_value.variable_len_val = m_valueByteBuf.buffer;
            m_underlyingHandle.header_value_len = m_valueByteBuf.len;
        }

        EventstreamHeader::EventstreamHeader(EventstreamHeader &&rhs) noexcept
            : m_allocator(rhs.m_allocator), m_valueByteBuf(rhs.m_valueByteBuf),
              m_underlyingHandle(rhs.m_underlyingHandle)
        {
            rhs.m_valueByteBuf.allocator = NULL;
            rhs.m_valueByteBuf.buffer = NULL;
        }

        Crt::String EventstreamHeader::GetHeaderName() noexcept
        {
            return Crt::String(m_underlyingHandle.header_name, m_underlyingHandle.header_name_len);
        }

        void EventstreamRpcConnection::s_onConnectionSetup(
            struct aws_event_stream_rpc_client_connection *connection,
            int errorCode,
            void *userData)
        {
            /* The `userData` pointer is used to pass `this`. */
            auto *thisConnection = static_cast<EventstreamRpcConnection *>(userData);

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
                    messageAmendment.AddHeader(EventstreamHeader(
                        Crt::String(EVENTSTREAM_VERSION_HEADER),
                        Crt::String(EVENTSTREAM_VERSION),
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

        void MessageAmendment::AddHeader(EventstreamHeader &&header) noexcept { m_headers.push_back(header); }

        void EventstreamRpcConnection::s_onConnectionShutdown(
            struct aws_event_stream_rpc_client_connection *connection,
            int errorCode,
            void *userData)
        {
            (void)connection;
            /* The `userData` pointer is used to pass `this`. */
            auto *thisConnection = static_cast<EventstreamRpcConnection *>(userData);

            thisConnection->m_lifecycleHandler->OnDisconnectCallback(errorCode);
        }

        void EventstreamRpcConnection::s_onProtocolMessage(
            struct aws_event_stream_rpc_client_connection *connection,
            const struct aws_event_stream_rpc_message_args *messageArgs,
            void *userData)
        {
            AWS_FATAL_ASSERT(messageArgs);
            (void)connection;

            /* The `userData` pointer is used to pass `this`. */
            auto *thisConnection = static_cast<EventstreamRpcConnection *>(userData);
            Crt::List<EventstreamHeader> pingHeaders;

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
                        pingHeaders.push_back(EventstreamHeader(messageArgs->headers[i]));
                    }
                    if (messageArgs->payload)
                        thisConnection->m_lifecycleHandler->OnPingCallback(pingHeaders, *messageArgs->payload);
                    else
                        thisConnection->m_lifecycleHandler->OnPingCallback(pingHeaders, Crt::Optional<Crt::ByteBuf>());
                    break;
                case AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_PING_RESPONSE:
                    return;
                    break;
                case AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_PROTOCOL_ERROR:
                case AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_INTERNAL_ERROR:
                    if (thisConnection->m_lifecycleHandler->OnErrorCallback(AWS_ERROR_EVENT_STREAM_RPC_PROTOCOL_ERROR))
                        thisConnection->Close(AWS_ERROR_EVENT_STREAM_RPC_PROTOCOL_ERROR);
                    break;
                default:
                    return;
                    if (thisConnection->m_lifecycleHandler->OnErrorCallback(
                            AWS_ERROR_EVENT_STREAM_RPC_UNKNOWN_PROTOCOL_MESSAGE))
                        thisConnection->Close(AWS_ERROR_EVENT_STREAM_RPC_UNKNOWN_PROTOCOL_MESSAGE);
                    break;
            }
        }
    } // namespace Eventstreamrpc
} // namespace Aws
