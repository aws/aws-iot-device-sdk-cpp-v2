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
        class EventStreamHeader;
        class EventstreamRpcClient;
        class EventstreamRpcConnection;
        class MessageAmendment;

        using HeaderType = aws_event_stream_header_value_type;
        using MessageType = aws_event_stream_rpc_message_type;

        using OnMessageFlush = std::function<void(int errorCode)>;

        /**
         * This callback is only invoked upon receiving a `CONNECT_ACK` from the
         * server with the `CONNECTION_ACCEPTED` flag set. Therefore, the connection
         * pointer is always guaranteed to be valid during invocation.
         */
        using OnConnect = std::function<void(const std::shared_ptr<EventstreamRpcConnection> &connection)>;

        /**
         * Invoked upon connection shutdown. `connection` will always be a valid pointer. `errorCode` will specify
         * shutdown reason. A graceful connection close will set `errorCode` to AWS_ERROR_SUCCESS.
         * Internally, the connection pointer will be unreferenced immediately after this call; if you took a
         * reference to it in OnConnect(), you'll need to release your reference before the underlying
         * memory is released. If you never took a reference to it, the resources for the connection will be
         * immediately released after completion of this callback.
         */
        using OnDisconnect =
            std::function<void(const std::shared_ptr<EventstreamRpcConnection> &newConnection, int errorCode)>;

        using OnError = std::function<bool(int errorCode)>;

        using OnPing = std::function<
            void(const Crt::List<EventStreamHeader> &headers, const Crt::Optional<Crt::ByteBuf> &payload)>;

        using ConnectMessageAmender = std::function<MessageAmendment &(void)>;

        class AWS_EVENTSTREAMRPC_API EventStreamHeader final
        {
          public:
            EventStreamHeader(const EventStreamHeader &lhs) noexcept;
            EventStreamHeader(EventStreamHeader &&rhs) noexcept;
            ~EventStreamHeader() noexcept;
            EventStreamHeader(const struct aws_event_stream_header_value_pair &header);
            EventStreamHeader(const Crt::String &name, bool value);
            EventStreamHeader(const Crt::String &name, int8_t value);
            EventStreamHeader(const Crt::String &name, int16_t value);
            EventStreamHeader(const Crt::String &name, int32_t value);
            EventStreamHeader(const Crt::String &name, int64_t value);
            EventStreamHeader(const Crt::String &name, Crt::DateTime &value);
            EventStreamHeader(
                const Crt::String &name,
                const Crt::String &value,
                Crt::Allocator *allocator = Crt::g_allocator) noexcept;
            EventStreamHeader(const Crt::String &name, Crt::ByteBuf &value);
            EventStreamHeader(const Crt::String &name, Crt::UUID value);

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

            bool operator==(const EventStreamHeader &other) const noexcept;

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
                const Crt::List<EventStreamHeader> &headers,
                Crt::Optional<Crt::ByteBuf> &payload) noexcept;
            MessageAmendment(const Crt::List<EventStreamHeader> &headers) noexcept;
            MessageAmendment(Crt::List<EventStreamHeader> &&headers) noexcept;
            MessageAmendment(const Crt::ByteBuf &payload) noexcept;
            Crt::List<EventStreamHeader> &GetHeaders() noexcept;
            void AddHeader(EventStreamHeader &&header) noexcept;
            Crt::Optional<Crt::ByteBuf> &GetPayload() noexcept;
            void SetPayload(const Crt::Optional<Crt::ByteBuf> &payload) noexcept;

          private:
            Crt::List<EventStreamHeader> m_headers;
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
            OnConnect OnConnectCallback;
            OnDisconnect OnDisconnectCallback;
            OnError OnErrorCallback;
            OnPing OnPingCallback;
            ConnectMessageAmender ConnectMessageAmenderCallback;
        };

        class AWS_EVENTSTREAMRPC_API EventstreamRpcConnection
            : public std::enable_shared_from_this<EventstreamRpcConnection>
        {
          public:
            virtual ~EventstreamRpcConnection() = default;
            EventstreamRpcConnection(const EventstreamRpcConnection &) = delete;
            EventstreamRpcConnection(EventstreamRpcConnection &&) = delete;
            EventstreamRpcConnection &operator=(const EventstreamRpcConnection &) = delete;
            EventstreamRpcConnection &operator=(EventstreamRpcConnection &&) = delete;

            static bool CreateConnection(
                const EventstreamRpcConnectionOptions &config,
                Crt::Allocator *allocator) noexcept;

            void SendPing(
                const Crt::List<EventStreamHeader> &headers,
                Crt::Optional<Crt::ByteBuf> &payload,
                OnMessageFlush onMessageFlushCallback) noexcept;

            void SendPingResponse(
                const Crt::List<EventStreamHeader> &headers,
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

          protected:
            EventstreamRpcConnection(
                struct aws_event_stream_rpc_client_connection *connection,
                Crt::Allocator *allocator) noexcept;
            struct aws_event_stream_rpc_client_connection *m_underlyingConnection;
            Crt::List<EventStreamHeader> m_defaultConnectHeaders;

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
            ClientState m_clientState;

            void SendProtocolMessage(
                const Crt::List<EventStreamHeader> &headers,
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
                std::weak_ptr<EventstreamRpcConnection> connection,
                const Crt::List<EventStreamHeader> &headers,
                Crt::Optional<Crt::ByteBuf> &payload,
                MessageType messageType,
                uint32_t flags,
                OnMessageFlush onMessageFlushCallback) noexcept;

            static void s_sendPing(
                std::weak_ptr<EventstreamRpcConnection> connection,
                const Crt::List<EventStreamHeader> &headers,
                Crt::Optional<Crt::ByteBuf> &payload,
                OnMessageFlush onMessageFlushCallback) noexcept;

            static void s_sendPingResponse(
                std::weak_ptr<EventstreamRpcConnection> connection,
                const Crt::List<EventStreamHeader> &headers,
                Crt::Optional<Crt::ByteBuf> &payload,
                OnMessageFlush onMessageFlushCallback) noexcept;
        };
    } // namespace Eventstreamrpc
} // namespace Aws
