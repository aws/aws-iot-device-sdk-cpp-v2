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

namespace Aws
{
    namespace Eventstreamrpc
    {
        /* This exists to handle aws_event_stream_rpc_client_connection's shutdown callback, which might fire after
         * EventstreamRpcConnection has been destroyed. */
        struct ConnectionCallbackData
        {
            explicit ConnectionCallbackData(Crt::Allocator *allocator) : allocator(allocator) {}
            std::shared_ptr<EventstreamRpcConnection> connection;
            Crt::Allocator *allocator;
            OnConnect onConnect;
            OnDisconnect onDisconnect;
            OnError onError;
            OnPing onPing;
            ConnectMessageAmender connectMessageAmender;
        };

        struct ProtocolMessageCallbackData
        {
            explicit ProtocolMessageCallbackData(Crt::Allocator *allocator) : allocator(allocator) {}
            std::weak_ptr<EventstreamRpcConnection> connection;
            Crt::Allocator *allocator;
            OnMessageFlush onMessageFlush;
            struct aws_array_list *headersArray;
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

        EventstreamRpcConnectionOptions::EventstreamRpcConnectionOptions()
            : Bootstrap(), SocketOptions(), TlsOptions(), HostName(), Port(0), OnDisconnectCallback(),
              OnErrorCallback(), OnPingCallback(), ConnectMessageAmenderCallback()
        {
        }

        EventstreamRpcConnection::EventstreamRpcConnection(
            struct aws_event_stream_rpc_client_connection *connection,
            Crt::Allocator *allocator) noexcept
            : m_underlyingConnection(connection), m_allocator(allocator)
        {
        }

        class UnmanagedConnection final : public EventstreamRpcConnection
        {
          public:
            UnmanagedConnection(struct aws_event_stream_rpc_client_connection *connection, Crt::Allocator *allocator)
                : EventstreamRpcConnection(connection, allocator)
            {
            }

            ~UnmanagedConnection() override
            {
                this->Close();
                if (m_underlyingConnection)
                {
                    aws_event_stream_rpc_client_connection_release(m_underlyingConnection);
                    m_underlyingConnection = nullptr;
                }
                m_defaultConnectHeaders.clear();
            }
        };

        bool EventstreamRpcConnection::CreateConnection(
            const EventstreamRpcConnectionOptions &connectionOptions,
            Crt::Allocator *allocator) noexcept
        {
            auto *callbackData = Crt::New<ConnectionCallbackData>(allocator, allocator);
            callbackData->allocator = allocator;
            callbackData->connection = nullptr;
            callbackData->onConnect = connectionOptions.OnConnectCallback;
            callbackData->onDisconnect = connectionOptions.OnDisconnectCallback;
            callbackData->onError = connectionOptions.OnErrorCallback;
            callbackData->onPing = connectionOptions.OnPingCallback;
            callbackData->connectMessageAmender = connectionOptions.ConnectMessageAmenderCallback;

            struct aws_event_stream_rpc_client_connection_options connOptions;
            AWS_ZERO_STRUCT(connOptions);
            connOptions.host_name = connectionOptions.HostName.c_str();
            connOptions.port = connectionOptions.Port;
            connOptions.socket_options = &connectionOptions.SocketOptions.GetImpl();
            connOptions.bootstrap = connectionOptions.Bootstrap->GetUnderlyingHandle();
            connOptions.on_connection_setup = EventstreamRpcConnection::s_onConnectionSetup;
            connOptions.on_connection_protocol_message = EventstreamRpcConnection::s_onProtocolMessage;
            connOptions.on_connection_shutdown = EventstreamRpcConnection::s_onConnectionShutdown;
            connOptions.user_data = callbackData;

            if (connectionOptions.TlsOptions.has_value())
            {
                connOptions.tls_options = connectionOptions.TlsOptions->GetUnderlyingHandle();
            }

            if (aws_event_stream_rpc_client_connection_connect(allocator, &connOptions))
            {
                Crt::Delete(callbackData, allocator);
                return false;
            }

            return true;
        }

        const struct aws_event_stream_header_value_pair *EventStreamHeader::GetUnderlyingHandle() const
        {
            return &m_underlyingHandle;
        }

        void EventstreamRpcConnection::SendPing(
            const Crt::List<EventStreamHeader> &headers,
            Crt::Optional<Crt::ByteBuf> &payload,
            OnMessageFlush onMessageFlushCallback) noexcept
        {
            s_sendPing(shared_from_this(), headers, payload, onMessageFlushCallback);
        }

        void EventstreamRpcConnection::SendPingResponse(
            const Crt::List<EventStreamHeader> &headers,
            Crt::Optional<Crt::ByteBuf> &payload,
            OnMessageFlush onMessageFlushCallback) noexcept
        {
            s_sendPingResponse(shared_from_this(), headers, payload, onMessageFlushCallback);
        }

        void EventstreamRpcConnection::s_sendPing(
            std::weak_ptr<EventstreamRpcConnection> connection,
            const Crt::List<EventStreamHeader> &headers,
            Crt::Optional<Crt::ByteBuf> &payload,
            OnMessageFlush onMessageFlushCallback) noexcept
        {
            s_sendProtocolMessage(
                connection, headers, payload, AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_PING, 0, onMessageFlushCallback);
        }

        void EventstreamRpcConnection::s_sendPingResponse(
            std::weak_ptr<EventstreamRpcConnection> connection,
            const Crt::List<EventStreamHeader> &headers,
            Crt::Optional<Crt::ByteBuf> &payload,
            OnMessageFlush onMessageFlushCallback) noexcept
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
            const Crt::List<EventStreamHeader> &headers,
            Crt::Optional<Crt::ByteBuf> &payload,
            MessageType messageType,
            uint32_t flags,
            OnMessageFlush onMessageFlushCallback) noexcept
        {
            s_sendProtocolMessage(shared_from_this(), headers, payload, messageType, flags, onMessageFlushCallback);
        }

        void EventstreamRpcConnection::s_protocolMessageCallback(int errorCode, void *userData) noexcept
        {
            auto *callbackData = static_cast<ProtocolMessageCallbackData *>(userData);

            /* Call the user-provided callback. */
            if (callbackData->onMessageFlush)
                callbackData->onMessageFlush(errorCode);

            Crt::Delete(callbackData, callbackData->allocator);
        }

        void EventstreamRpcConnection::s_sendProtocolMessage(
            std::weak_ptr<EventstreamRpcConnection> connection,
            const Crt::List<EventStreamHeader> &headers,
            Crt::Optional<Crt::ByteBuf> &payload,
            MessageType messageType,
            uint32_t flags,
            OnMessageFlush onMessageFlushCallback) noexcept
        {
            struct aws_event_stream_rpc_message_args msg_args;
            AWS_ZERO_STRUCT(msg_args);
            struct aws_array_list headersArray;
            AWS_ZERO_STRUCT(headersArray);

            /* Check if the connection has expired before attempting to send. */
            if (auto connectionPtr = connection.lock())
            {
                if (aws_event_stream_headers_list_init(&headersArray, connectionPtr->m_allocator))
                {
                    onMessageFlushCallback(AWS_OP_ERR);
                }
                else
                {
                    msg_args.message_flags = flags;
                    msg_args.message_type = messageType;

                    /* Populate the array with the underlying handle of each EventStreamHeader. */
                    for (auto &i : headers)
                    {
                        aws_array_list_push_back(&headersArray, i.GetUnderlyingHandle());
                    }

                    msg_args.headers = (struct aws_event_stream_header_value_pair *)headersArray.data;
                    msg_args.headers_count = aws_array_list_length(&headersArray);

                    if (payload.has_value())
                    {
                        msg_args.payload = &payload.value();
                    }

                    auto *callbackData =
                        Crt::New<ProtocolMessageCallbackData>(connectionPtr->m_allocator, connectionPtr->m_allocator);
                    callbackData->connection = connectionPtr;
                    callbackData->onMessageFlush = onMessageFlushCallback;
                    callbackData->headersArray = &headersArray;

                    if (aws_event_stream_rpc_client_connection_send_protocol_message(
                            connectionPtr->m_underlyingConnection, &msg_args, s_protocolMessageCallback, callbackData))
                    {
                    }

                    if (aws_array_list_is_valid(callbackData->headersArray))
                    {
                        aws_array_list_clean_up(callbackData->headersArray);
                    }
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

        bool EventStreamHeader::operator==(const EventStreamHeader &other) const noexcept
        {
            if (other.m_underlyingHandle.header_name_len != m_underlyingHandle.header_name_len)
            {
                return false;
            }
            for (size_t i = 0; i < m_underlyingHandle.header_name_len; i++)
            {
                if (tolower(m_underlyingHandle.header_name[i]) != tolower(other.m_underlyingHandle.header_name[i]))
                {
                    return false;
                }
            }
            return true;
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

        void EventstreamRpcConnection::s_onConnectionSetup(
            struct aws_event_stream_rpc_client_connection *connection,
            int errorCode,
            void *userData)
        {
            /**
             * Allocate an EventstreamRpcConnection and seat it to `ConnectionCallbackData`'s shared_ptr.
             */
            auto *callbackData = static_cast<ConnectionCallbackData *>(userData);

            if (!errorCode)
            {
                auto connectionObj = std::allocate_shared<UnmanagedConnection>(
                    Crt::StlAllocator<UnmanagedConnection>(), connection, callbackData->allocator);
                if (connectionObj)
                {
                    connectionObj->m_defaultConnectHeaders.push_back(
                        EventStreamHeader(Crt::String(":version"), Crt::String("0.1.0"), callbackData->allocator));
                    MessageAmendment messageAmendment(connectionObj->m_defaultConnectHeaders);
                    if (callbackData->connectMessageAmender)
                    {
                        MessageAmendment &connectAmendment = callbackData->connectMessageAmender();
                        auto &defaultHeaderList = connectionObj->m_defaultConnectHeaders;
                        auto &amenderHeaderList = connectAmendment.GetHeaders();
                        auto it = amenderHeaderList.begin();
                        while (it != amenderHeaderList.end())
                        {
                            /* The connect amender must not add headers with the same name. */
                            if (std::find(defaultHeaderList.begin(), defaultHeaderList.end(), *it) ==
                                defaultHeaderList.end())
                            {
                                /* Since the header is being moved, the entry from its original list
                                 * must also be erased. */
                                messageAmendment.AddHeader(std::move(*it));
                                amenderHeaderList.erase(it++);
                            }
                            else
                            {
                                ++it;
                            }
                        }
                        messageAmendment.SetPayload(connectAmendment.GetPayload());
                    }
                    callbackData->connection = connectionObj;
                    /* Send a CONNECT packet to the server. */
                    s_sendProtocolMessage(
                        connectionObj,
                        messageAmendment.GetHeaders(),
                        messageAmendment.GetPayload(),
                        AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_CONNECT,
                        0,
                        nullptr);
                    connectionObj->m_clientState = WAITING_FOR_CONNECT_ACK;
                    return;
                }

                /* Release if we're unable to allocate the connection's containing object. */
                aws_event_stream_rpc_client_connection_release(connection);
                errorCode = aws_last_error();
            }

            callbackData->connection->m_clientState = DISCONNECTED;
            if (callbackData->onError(errorCode))
            {
                callbackData->connection->Close(errorCode);
            }
            Crt::Delete(callbackData, callbackData->allocator);
        }

        void MessageAmendment::AddHeader(EventStreamHeader &&header) noexcept { m_headers.push_back(header); }

        void EventstreamRpcConnection::s_onConnectionShutdown(
            struct aws_event_stream_rpc_client_connection *connection,
            int errorCode,
            void *userData)
        {
            (void)connection;
            auto *callbackData = static_cast<ConnectionCallbackData *>(userData);

            callbackData->onDisconnect(errorCode);

            Crt::Delete(callbackData, callbackData->allocator);
        }

        void EventstreamRpcConnection::s_onProtocolMessage(
            struct aws_event_stream_rpc_client_connection *connection,
            const struct aws_event_stream_rpc_message_args *messageArgs,
            void *userData)
        {
            AWS_FATAL_ASSERT(messageArgs);
            (void)connection;

            auto *callbackData = static_cast<ConnectionCallbackData *>(userData);
            std::shared_ptr<EventstreamRpcConnection> connectionObj = callbackData->connection;

            switch (messageArgs->message_type)
            {
                case AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_CONNECT_ACK:
                    if (connectionObj->m_clientState == WAITING_FOR_CONNECT_ACK)
                    {
                        if (messageArgs->message_flags & AWS_EVENT_STREAM_RPC_MESSAGE_FLAG_CONNECTION_ACCEPTED)
                        {
                            connectionObj->m_clientState = CONNECTED;
                            callbackData->onConnect(std::move(connectionObj));
                        }
                        else
                        {
                            connectionObj->m_clientState = DISCONNECTING;
                            connectionObj->Close(AWS_ERROR_EVENT_STREAM_RPC_CONNECTION_CLOSED);
                        }
                    }
                case AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_PING:
                    if (callbackData->onPing)
                    {
                        Crt::List<EventStreamHeader> pingHeaders;
                        for (size_t i = 0; i < messageArgs->headers_count; ++i)
                        {
                            pingHeaders.push_back(EventStreamHeader(messageArgs->headers[i]));
                        }
                        if (messageArgs->payload)
                            callbackData->onPing(pingHeaders, *messageArgs->payload);
                        else
                            callbackData->onPing(pingHeaders, Crt::Optional<Crt::ByteBuf>());
                    }
                    break;
                case AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_PING_RESPONSE:
                    return;
                    break;
                case AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_PROTOCOL_ERROR:
                case AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_INTERNAL_ERROR:
                    callbackData->onError(AWS_ERROR_EVENT_STREAM_RPC_PROTOCOL_ERROR);
                    connectionObj->Close(AWS_ERROR_EVENT_STREAM_RPC_PROTOCOL_ERROR);
                    break;
                default:
                    return;
                    break;
            }
        }
    } // namespace Eventstreamrpc
} // namespace Aws
