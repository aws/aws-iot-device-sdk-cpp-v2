#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/eventstreamrpc/Exports.h>

#include <aws/crt/DateTime.h>
#include <aws/crt/StlAllocator.h>
#include <aws/crt/Types.h>
#include <aws/crt/UUID.h>
#include <aws/crt/io/EventLoopGroup.h>
#include <aws/crt/io/SocketOptions.h>
#include <aws/crt/io/TlsOptions.h>

#include <aws/event-stream/event_stream_rpc_client.h>

#include <atomic>
#include <functional>
#include <memory>

namespace Aws
{
    namespace Crt
    {
        namespace Io
        {
            class ClientBootstrap;
        }
    } // namespace Crt
    namespace Eventstreamrpc
    {
        class EventstreamHeader;
        class EventstreamRpcClient;
        class EventstreamRpcConnection;
        class MessageAmendment;

        using HeaderType = aws_event_stream_header_value_type;
        using MessageType = aws_event_stream_rpc_message_type;

        using OnMessageFlush = std::function<void(int errorCode)>;

        /**
         * This callback is only invoked upon receiving a CONNECT_ACK with the
         * CONNECTION_ACCEPTED flag set by the server. Therefore, the `connection`
         * pointer is always guaranteed to be valid during invocation. However, it
         * is guaranteed to be invalidated when `OnDisconnect` is invoked or 
         * when `OnError` is invoked and returns true.
         */
        using OnConnect = std::function<void(EventstreamRpcConnection *connection)>;

        /**
         * Invoked upon connection shutdown. `errorCode` will specify
         * shutdown reason. A graceful connection close will set `errorCode` to
         * `AWS_ERROR_SUCCESS` or 0.
         */
        using OnDisconnect = std::function<void(int errorCode)>;

        /**
         * Invoked upon receiving any error. Use the return value to determine
         * whether or not to force the connection to close. Keep in mind that once
         * closed, the pointer reference from `OnConnect` is no longer safe to use.
         */
        using OnError = std::function<bool(int errorCode)>;

        /**
         * Invoked upon receiving a ping from the server. The `headers` and `payload`
         * refer to what is contained in the ping message.
         */
        using OnPing = std::function<
            void(const Crt::List<EventstreamHeader> &headers, const Crt::Optional<Crt::ByteBuf> &payload)>;

        /**
         * Allows the application to append headers and change the payload of the CONNECT
         * packet being sent out.
         */
        using ConnectMessageAmender = std::function<MessageAmendment &(void)>;

        class AWS_EVENTSTREAMRPC_API EventstreamHeader final
        {
          public:
            EventstreamHeader(const EventstreamHeader &lhs) noexcept;
            EventstreamHeader(EventstreamHeader &&rhs) noexcept;
            ~EventstreamHeader() noexcept;
            EventstreamHeader(const struct aws_event_stream_header_value_pair &header);
            EventstreamHeader(const Crt::String &name, bool value);
            EventstreamHeader(const Crt::String &name, int8_t value);
            EventstreamHeader(const Crt::String &name, int16_t value);
            EventstreamHeader(const Crt::String &name, int32_t value);
            EventstreamHeader(const Crt::String &name, int64_t value);
            EventstreamHeader(const Crt::String &name, Crt::DateTime &value);
            EventstreamHeader(
                const Crt::String &name,
                const Crt::String &value,
                Crt::Allocator *allocator = Crt::g_allocator) noexcept;
            EventstreamHeader(const Crt::String &name, Crt::ByteBuf &value);
            EventstreamHeader(const Crt::String &name, Crt::UUID value);

            HeaderType GetHeaderType();
            Crt::String GetHeaderName() noexcept;
            void SetHeaderName(Crt::String &);

            bool GetValueAsBoolean(bool &);
            bool GetValueAsByte(int8_t &);
            bool GetValueAsShort(int16_t &);
            bool GetValueAsInt(int32_t &);
            bool GetValueAsLong(int64_t &);
            bool GetValueAsTimestamp(Crt::DateTime &);
            bool GetValueAsBytes(Crt::ByteBuf &);
            bool GetValueAsUUID(Crt::UUID &);

            void SetValue(bool value);
            void SetValue(int8_t value);
            void SetValue(int16_t value);
            void SetValue(int32_t value);
            void SetValue(int64_t value);
            void SetValue(Crt::DateTime &value);
            void SetValue(Crt::ByteBuf value);
            void SetValue(Crt::UUID value);

            const struct aws_event_stream_header_value_pair *GetUnderlyingHandle() const;

            bool operator==(const EventstreamHeader &other) const noexcept;

          private:
            Crt::Allocator *m_allocator;
            Crt::ByteBuf m_valueByteBuf;
            struct aws_event_stream_header_value_pair m_underlyingHandle;
        };

        class AWS_EVENTSTREAMRPC_API MessageAmendment final
        {
          public:
            MessageAmendment() = default;
            MessageAmendment(const MessageAmendment &lhs) = default;
            MessageAmendment(MessageAmendment &&rhs) = default;
            MessageAmendment(
                const Crt::List<EventstreamHeader> &headers,
                Crt::Optional<Crt::ByteBuf> &payload) noexcept;
            MessageAmendment(const Crt::List<EventstreamHeader> &headers) noexcept;
            MessageAmendment(Crt::List<EventstreamHeader> &&headers) noexcept;
            MessageAmendment(const Crt::ByteBuf &payload) noexcept;
            void AddHeader(EventstreamHeader &&header) noexcept;
            void SetPayload(const Crt::Optional<Crt::ByteBuf> &payload) noexcept;
            Crt::List<EventstreamHeader> &GetHeaders() noexcept;
            Crt::Optional<Crt::ByteBuf> &GetPayload() noexcept;

          private:
            friend class EventstreamRpcConnection;
            Crt::List<EventstreamHeader> m_headers;
            Crt::Optional<Crt::ByteBuf> m_payload;
        };

        /**
         * Configuration structure holding all options relating to eventstream RPC connection establishment
         */
        class AWS_EVENTSTREAMRPC_API EventstreamRpcConnectionOptions final
        {
          public:
            EventstreamRpcConnectionOptions();
            EventstreamRpcConnectionOptions(const EventstreamRpcConnectionOptions &rhs) = default;
            EventstreamRpcConnectionOptions(EventstreamRpcConnectionOptions &&rhs) = default;

            ~EventstreamRpcConnectionOptions() = default;

            EventstreamRpcConnectionOptions &operator=(const EventstreamRpcConnectionOptions &rhs) = default;
            EventstreamRpcConnectionOptions &operator=(EventstreamRpcConnectionOptions &&rhs) = default;

            Crt::Io::ClientBootstrap *Bootstrap;
            Crt::Io::SocketOptions SocketOptions;
            Crt::Optional<Crt::Io::TlsConnectionOptions> TlsOptions;
            Crt::String HostName;
            uint16_t Port;
        };

        class AWS_EVENTSTREAMRPC_API ConnectionLifecycleHandler
        {
            public:
                virtual void OnConnectCallback();
                virtual void OnDisconnectCallback(int errorCode);
                virtual bool OnErrorCallback(int errorCode);
                virtual void OnPingCallback(const Crt::List<EventstreamHeader> &headers, const Crt::Optional<Crt::ByteBuf> &payload);
        };

        class AWS_EVENTSTREAMRPC_API EventstreamRpcConnection
        {
          public:
            enum ConnectStatus {

            };
            EventstreamRpcConnection(
                Crt::Allocator *allocator) noexcept;
            ~EventstreamRpcConnection() noexcept;
            EventstreamRpcConnection(const EventstreamRpcConnection &) = delete;
            EventstreamRpcConnection(EventstreamRpcConnection &&) = delete;
            EventstreamRpcConnection &operator=(const EventstreamRpcConnection &) = delete;
            EventstreamRpcConnection &operator=(EventstreamRpcConnection &&) = delete;

            bool Connect(
                const EventstreamRpcConnectionOptions &connectionOptions,
                ConnectionLifecycleHandler* connectionLifecycleHandler,
                ConnectMessageAmender connectMessageAmender
                ) noexcept;

            void SendPing(
                const Crt::List<EventstreamHeader> &headers,
                Crt::Optional<Crt::ByteBuf> &payload,
                OnMessageFlush onMessageFlushCallback) noexcept;

            void SendPingResponse(
                const Crt::List<EventstreamHeader> &headers,
                Crt::Optional<Crt::ByteBuf> &payload,
                OnMessageFlush onMessageFlushCallback) noexcept;

            void Close() noexcept;
            void Close(int errorCode) noexcept;

            /**
             * @return true if the instance is in a valid state, false otherwise.
             */
            operator bool() const noexcept;
            /**
             * @return the value of the last aws error encountered by operations on this instance.
             */
            int LastError() const noexcept;

          private:
            enum ClientState
            {
                DISCONNECTED = 1,
                CONNECTING_TO_SOCKET,
                WAITING_FOR_CONNECT_ACK,
                CONNECTED,
                DISCONNECTING,
            };
            Crt::Allocator *m_allocator;
            struct aws_event_stream_rpc_client_connection *m_underlyingConnection;
            ClientState m_clientState;
            ConnectionLifecycleHandler* m_lifecycleHandler;
            ConnectMessageAmender m_connectMessageAmender;
            static void s_customDeleter(EventstreamRpcConnection *connection) noexcept;
            void SendProtocolMessage(
                const Crt::List<EventstreamHeader> &headers,
                Crt::Optional<Crt::ByteBuf> &payload,
                MessageType messageType,
                uint32_t flags,
                OnMessageFlush onMessageFlushCallback) noexcept;

            static void s_onConnectionShutdown(
                struct aws_event_stream_rpc_client_connection *connection,
                int errorCode,
                void *userData);
            static void s_onConnectionSetup(
                struct aws_event_stream_rpc_client_connection *connection,
                int errorCode,
                void *userData);
            static void s_onProtocolMessage(
                struct aws_event_stream_rpc_client_connection *connection,
                const struct aws_event_stream_rpc_message_args *messageArgs,
                void *userData);

            static void s_protocolMessageCallback(int errorCode, void *userData) noexcept;
            static void s_sendProtocolMessage(
                EventstreamRpcConnection *connection,
                const Crt::List<EventstreamHeader> &headers,
                Crt::Optional<Crt::ByteBuf> &payload,
                MessageType messageType,
                uint32_t flags,
                OnMessageFlush onMessageFlushCallback) noexcept;

            static void s_sendPing(
                EventstreamRpcConnection *connection,
                const Crt::List<EventstreamHeader> &headers,
                Crt::Optional<Crt::ByteBuf> &payload,
                OnMessageFlush onMessageFlushCallback) noexcept;

            static void s_sendPingResponse(
                EventstreamRpcConnection *connection,
                const Crt::List<EventstreamHeader> &headers,
                Crt::Optional<Crt::ByteBuf> &payload,
                OnMessageFlush onMessageFlushCallback) noexcept;
        };
    } // namespace Eventstreamrpc
} // namespace Aws
