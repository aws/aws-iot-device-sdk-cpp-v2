#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/crt/http/HttpConnection.h>
#include <aws/crt/io/Bootstrap.h>
#include <aws/crt/io/SocketOptions.h>
#include <aws/iotdevice/secure_tunneling.h>
#include <aws/iotsecuretunneling/Exports.h>

#include <future>

namespace Aws
{
    namespace Iotsecuretunneling
    {
        /**
         * Data model for Secure Tunnel messages.
         */
        class AWS_IOTSECURETUNNELING_API Message
        {
          public:
            Message(
                const aws_secure_tunnel_message_view &raw_options,
                Crt::Allocator *allocator = Crt::ApiAllocator()) noexcept;
            Message(Crt::Allocator *allocator = Crt::ApiAllocator()) noexcept;
            Message(Crt::ByteCursor payload, Crt::Allocator *allocator = Crt::ApiAllocator()) noexcept;
            Message(
                Crt::ByteCursor serviceId,
                Crt::ByteCursor payload,
                Crt::Allocator *allocator = Crt::ApiAllocator()) noexcept;

            /**
             * Sets the service id for the secure tunnel message.
             *
             * @param serviceId The service id for the secure tunnel message.
             * @return The Message Object after setting the payload.
             */
            Message &withServiceId(Crt::ByteCursor serviceId) noexcept;

            /**
             * Sets the payload for the secure tunnel message.
             *
             * @param payload The payload for the secure tunnel message.
             * @return The Message Object after setting the payload.
             */
            Message &withPayload(Crt::ByteCursor payload) noexcept;

            bool initializeRawOptions(aws_secure_tunnel_message_view &raw_options) noexcept;

            /**
             * The service id of the secure tunnel message.
             *
             * @return The service id of the secure tunnel message.
             */
            const Crt::Optional<Crt::ByteCursor> &getServiceId() const noexcept;

            /**
             * The payload of the secure tunnel message.
             *
             * @return The payload of the secure tunnel message.
             */
            const Crt::Optional<Crt::ByteCursor> &getPayload() const noexcept;

            virtual ~Message();
            /* Do not allow direct copy or move */
            Message(const Message &) = delete;
            Message(Message &&) noexcept = delete;
            Message &operator=(const Message &) = delete;
            Message &operator=(Message &&) noexcept = delete;

          private:
            Crt::Allocator *m_allocator;

            /**
             * The service id used for multiplexing.
             *
             * If left empty, a V1 protocol message is assumed.
             */
            Crt::Optional<Crt::ByteCursor> m_serviceId;

            /**
             * The payload of the secure tunnel message.
             */
            Crt::Optional<Crt::ByteCursor> m_payload;

            ///////////////////////////////////////////////////////////////////////////
            // Underlying data storage for internal use
            ///////////////////////////////////////////////////////////////////////////
            Crt::ByteBuf m_payloadStorage;
            Crt::ByteBuf m_serviceIdStorage;
        };

        /**
         * The data returned when a message is received on the secure tunnel.
         */
        struct AWS_IOTSECURETUNNELING_API MessageReceivedEventData
        {
            MessageReceivedEventData() : message(nullptr) {}
            std::shared_ptr<Message> message;
        };

        /**
         * Data model for Secure Tunnel connection view.
         */
        class AWS_IOTSECURETUNNELING_API ConnectionData
        {
          public:
            ConnectionData(
                const aws_secure_tunnel_connection_view &raw_options,
                Crt::Allocator *allocator = Crt::ApiAllocator()) noexcept;

            /**
             * Service id 1 of the secure tunnel.
             *
             * @return Service id 1 of the secure tunnel.
             */
            const Crt::Optional<Crt::ByteCursor> &getServiceId1() const noexcept;

            /**
             * Service id 2 of the secure tunnel.
             *
             * @return Service id 2 of the secure tunnel.
             */
            const Crt::Optional<Crt::ByteCursor> &getServiceId2() const noexcept;

            /**
             * Service id 3 of the secure tunnel.
             *
             * @return Service id 3 of the secure tunnel.
             */
            const Crt::Optional<Crt::ByteCursor> &getServiceId3() const noexcept;

            virtual ~ConnectionData();
            /* Do not allow direct copy or move */
            ConnectionData(const ConnectionData &) = delete;
            ConnectionData(ConnectionData &&) noexcept = delete;
            ConnectionData &operator=(const ConnectionData &) = delete;
            ConnectionData &operator=(ConnectionData &&) noexcept = delete;

          private:
            Crt::Allocator *m_allocator;

            /**
             * Service id 1 used for multiplexing.
             *
             * If left empty, a V1 protocol message is assumed.
             */
            Crt::Optional<Crt::ByteCursor> m_serviceId1;

            /**
             * Service id 2 used for multiplexing.
             */
            Crt::Optional<Crt::ByteCursor> m_serviceId2;

            /**
             * Service id 2 used for multiplexing.
             */
            Crt::Optional<Crt::ByteCursor> m_serviceId3;

            ///////////////////////////////////////////////////////////////////////////
            // Underlying data storage for internal use
            ///////////////////////////////////////////////////////////////////////////
            Crt::ByteBuf m_serviceId1Storage;
            Crt::ByteBuf m_serviceId2Storage;
            Crt::ByteBuf m_serviceId3Storage;
        };

        /**
         * The data returned when a connection with secure tunnel service is established.
         */
        struct AWS_IOTSECURETUNNELING_API ConnectionEstablishedEventData
        {
            ConnectionEstablishedEventData() : connectionData(nullptr) {}
            std::shared_ptr<ConnectionData> connectionData;
        };

        /**
         * Data model for started Secure Tunnel streams.
         */
        class AWS_IOTSECURETUNNELING_API StreamStartedData
        {
          public:
            StreamStartedData(
                const aws_secure_tunnel_message_view &raw_options,
                Crt::Allocator *allocator = Crt::ApiAllocator()) noexcept;

            /**
             * Service id of the started stream.
             *
             * @return Service id of the started stream.
             */
            const Crt::Optional<Crt::ByteCursor> &getServiceId() const noexcept;

            virtual ~StreamStartedData();
            /* Do not allow direct copy or move */
            StreamStartedData(const StreamStartedData &) = delete;
            StreamStartedData(StreamStartedData &&) noexcept = delete;
            StreamStartedData &operator=(const StreamStartedData &) = delete;
            StreamStartedData &operator=(StreamStartedData &&) noexcept = delete;

          private:
            Crt::Allocator *m_allocator;

            /**
             * Service id of started stream.
             *
             * If left empty, a V1 protocolstream is assumed.
             */
            Crt::Optional<Crt::ByteCursor> m_serviceId;

            ///////////////////////////////////////////////////////////////////////////
            // Underlying data storage for internal use
            ///////////////////////////////////////////////////////////////////////////
            Crt::ByteBuf m_serviceIdStorage;
        };

        /**
         * The data returned when a stream is started on the Secure Tunnel.
         */
        struct AWS_IOTSECURETUNNELING_API StreamStartedEventData
        {
            StreamStartedEventData() : streamStartedData(nullptr) {}
            std::shared_ptr<StreamStartedData> streamStartedData;
        };

        class SecureTunnel;

        // Client callback type definitions

        /**
         * Type signature of the callback invoked when connection is established with the secure tunnel service and
         * available service ids are returned.
         */
        using OnConnectionEstablished =
            std::function<void(SecureTunnel &, int errorCode, const ConnectionEstablishedEventData &)>;
        /**
         * Type signature of the callback invoked when connection is shutdown.
         */
        using OnConnectionShutdown = std::function<void(void)>;

        /**
         * Type signature of the callback invoked when data has been sent through the secure tunnel connection.
         */
        using OnSendDataComplete = std::function<void(int errorCode)>;

        /**
         * Type signature of the callback invoked when a message is received through the secure tunnel connection.
         */
        using OnMessageReceived = std::function<void(SecureTunnel &, const MessageReceivedEventData &)>;

        /**
         * Type signature of the callback invoked when a stream has been started with a source through the secure tunnel
         * connection.
         */
        using OnStreamStarted = std::function<void(SecureTunnel &, int errorCode, const StreamStartedEventData &)>;
        using OnStreamReset = std::function<void(void)>;
        using OnSessionReset = std::function<void(void)>;

        /* Deprecate */
        using OnConnectionComplete = std::function<void(void)>;
        using OnDataReceive = std::function<void(const Crt::ByteBuf &data)>;
        using OnStreamStart = std::function<void()>;

        class AWS_IOTSECURETUNNELING_API SecureTunnelBuilder final
        {
          public:
            /**
             * Constructor arguments are the minimum required to create a secure tunnel
             */
            SecureTunnelBuilder(
                Crt::Allocator *allocator,                   // Should out live this object
                Crt::Io::ClientBootstrap &clientBootstrap,   // Should out live this object
                const Crt::Io::SocketOptions &socketOptions, // Make a copy and save in this object
                const std::string &accessToken,              // Make a copy and save in this object
                aws_secure_tunneling_local_proxy_mode localProxyMode,
                const std::string &endpointHost); // Make a copy and save in this object

            /**
             * Constructor arguments are the minimum required to create a secure tunnel
             */
            SecureTunnelBuilder(
                Crt::Allocator *allocator,                   // Should out live this object
                const Crt::Io::SocketOptions &socketOptions, // Make a copy and save in this object
                const std::string &accessToken,              // Make a copy and save in this object
                aws_secure_tunneling_local_proxy_mode localProxyMode,
                const std::string &endpointHost); // Make a copy and save in this object

            /**
             * Optional members
             */
            SecureTunnelBuilder &WithRootCa(const std::string &rootCa);
            SecureTunnelBuilder &WithHttpClientConnectionProxyOptions(
                const Crt::Http::HttpClientConnectionProxyOptions &httpClientConnectionProxyOptions);
            SecureTunnelBuilder &WithOnConnectionEstablished(OnConnectionEstablished onConnectionEstablished);
            SecureTunnelBuilder &WithOnConnectionShutdown(OnConnectionShutdown onConnectionShutdown);
            SecureTunnelBuilder &WithOnSendDataComplete(OnSendDataComplete onSendDataComplete);
            SecureTunnelBuilder &WithOnMessageReceived(OnMessageReceived onMessageReceived);
            SecureTunnelBuilder &WithOnStreamStarted(OnStreamStarted onStreamStarted);
            SecureTunnelBuilder &WithOnStreamReset(OnStreamReset onStreamReset);
            SecureTunnelBuilder &WithOnSessionReset(OnSessionReset onSessionReset);
            SecureTunnelBuilder &WithClientToken(const std::string &clientToken);
            /* Deprecate */
            SecureTunnelBuilder &WithOnDataReceive(OnDataReceive onDataReceive);
            SecureTunnelBuilder &WithOnConnectionComplete(OnConnectionComplete onConnectionComplete);
            SecureTunnelBuilder &WithOnStreamStart(OnStreamStart onStreamStart);

            /**
             * Will return a shared pointer to a new SecureTunnel that countains a
             * new aws_secure_tunnel that is generated using the set members of SecureTunnelBuilder.
             * On failure, the shared_ptr will contain a nullptr, call Crt::LastErrorOrUnknown(); to get the reason
             * for failure.
             */
            std::shared_ptr<SecureTunnel> Build() noexcept;

          private:
            /**
             * Required members
             */
            Crt::Allocator *m_allocator;
            Crt::Io::ClientBootstrap *m_clientBootstrap;
            Crt::Io::SocketOptions m_socketOptions;
            std::string m_accessToken;
            aws_secure_tunneling_local_proxy_mode m_localProxyMode;
            std::string m_endpointHost;

            /**
             * Optional members
             */
            std::string m_clientToken;
            std::string m_rootCa;
            Crt::Optional<Crt::Http::HttpClientConnectionProxyOptions> m_httpClientConnectionProxyOptions;

            /**
             * Callbacks
             */
            OnConnectionEstablished m_OnConnectionEstablished;
            OnConnectionShutdown m_OnConnectionShutdown;
            OnSendDataComplete m_OnSendDataComplete;
            OnMessageReceived m_OnMessageReceived;
            OnStreamStarted m_OnStreamStarted;
            OnStreamReset m_OnStreamReset;
            OnSessionReset m_OnSessionReset;

            /* Depricate */
            OnConnectionComplete m_OnConnectionComplete;
            /* Depricate */
            OnDataReceive m_OnDataReceive;
            /* Depricate */
            OnStreamStart m_OnStreamStart;

            friend class SecureTunnel;
        };

        class AWS_IOTSECURETUNNELING_API SecureTunnel final
        {
          public:
            SecureTunnel(
                Crt::Allocator *allocator,                   // Should out live this object
                Crt::Io::ClientBootstrap *clientBootstrap,   // Should out live this object
                const Crt::Io::SocketOptions &socketOptions, // Make a copy and save in this object

                const std::string &accessToken, // Make a copy and save in this object
                aws_secure_tunneling_local_proxy_mode localProxyMode,
                const std::string &endpointHost, // Make a copy and save in this object
                const std::string &rootCa,       // Make a copy and save in this object

                OnConnectionComplete onConnectionComplete,
                OnConnectionShutdown onConnectionShutdown,
                OnSendDataComplete onSendDataComplete,
                OnDataReceive onDataReceive,
                OnStreamStart onStreamStart,
                OnStreamReset onStreamReset,
                OnSessionReset onSessionReset);

            SecureTunnel(
                Crt::Allocator *allocator,                   // Should out live this object
                const Crt::Io::SocketOptions &socketOptions, // Make a copy and save in this object

                const std::string &accessToken, // Make a copy and save in this object
                aws_secure_tunneling_local_proxy_mode localProxyMode,
                const std::string &endpointHost, // Make a copy and save in this object
                const std::string &rootCa,       // Make a copy and save in this object

                OnConnectionComplete onConnectionComplete,
                OnConnectionShutdown onConnectionShutdown,
                OnSendDataComplete onSendDataComplete,
                OnDataReceive onDataReceive,
                OnStreamStart onStreamStart,
                OnStreamReset onStreamReset,
                OnSessionReset onSessionReset);

            virtual ~SecureTunnel();
            /* Do not allow direct copy or move */
            SecureTunnel(const SecureTunnel &) = delete;
            SecureTunnel(SecureTunnel &&) noexcept;
            SecureTunnel &operator=(const SecureTunnel &) = delete;
            SecureTunnel &operator=(SecureTunnel &&) noexcept;

            bool IsValid();

            int Start();
            int Stop();
            int SendMessage(std::shared_ptr<Message> messageOptions) noexcept;

            /* SOURCE MODE ONLY */
            int SendStreamStart();
            int SendStreamStart(std::string serviceId);
            int SendStreamStart(Crt::ByteCursor serviceId);

            aws_secure_tunnel *GetUnderlyingHandle();

            /* These are to be deprecated */
            int Connect();
            int Close();
            void Shutdown();
            int SendData(const Crt::ByteCursor &data);
            /* Should not be exposed. Under the hood only operation. */
            int SendStreamReset();

          private:
            /**
             * This constructor is used with SecureTunnelBuilder and should be modified when members are added or
             * removed.
             */
            SecureTunnel(
                Crt::Allocator *allocator,
                Crt::Io::ClientBootstrap *clientBootstrap,
                const Crt::Io::SocketOptions &socketOptions,
                const std::string &accessToken,
                const std::string &clientToken,
                aws_secure_tunneling_local_proxy_mode localProxyMode,
                const std::string &endpointHost,

                const std::string &rootCa,
                Crt::Http::HttpClientConnectionProxyOptions *httpClientConnectionProxyOptions,

                OnConnectionEstablished onConnectionEstablished,
                OnConnectionComplete onConnectionComplete,
                OnConnectionShutdown onConnectionShutdown,
                OnSendDataComplete onSendDataComplete,
                OnMessageReceived onMessageReceived,
                OnDataReceive onDataReceive,
                OnStreamStarted onStreamStarted,
                OnStreamStart onStreamStart,
                OnStreamReset onStreamReset,
                OnSessionReset onSessionReset);

            // aws-c-iot callbacks
            static void s_OnMessageReceived(const struct aws_secure_tunnel_message_view *message, void *user_data);
            static void s_OnConnectionEstablished(
                const struct aws_secure_tunnel_connection_view *connection,
                int error_code,
                void *user_data);
            static void s_OnConnectionShutdown(int error_code, void *user_data);
            static void s_OnSendDataComplete(int error_code, void *user_data);
            static void s_OnStreamReset(
                const struct aws_secure_tunnel_message_view *message,
                int error_code,
                void *user_data);
            static void s_OnSessionReset(void *user_data);
            static void s_OnTerminationComplete(void *user_data);
            static void s_OnStreamStarted(
                const struct aws_secure_tunnel_message_view *message,
                int error_code,
                void *user_data);

            void OnTerminationComplete();

            // Client callbacks
            OnMessageReceived m_OnMessageReceived;
            OnConnectionEstablished m_OnConnectionEstablished;
            OnConnectionShutdown m_OnConnectionShutdown;
            OnSendDataComplete m_OnSendDataComplete;
            OnStreamStarted m_OnStreamStarted;
            OnStreamReset m_OnStreamReset;
            OnSessionReset m_OnSessionReset;

            aws_secure_tunnel *m_secure_tunnel;
            Crt::Allocator *m_allocator;

            /* Deprecate these callbacks */
            OnDataReceive m_OnDataReceive;
            OnConnectionComplete m_OnConnectionComplete;
            OnStreamStart m_OnStreamStart;

            std::promise<void> m_TerminationComplete;

            friend class SecureTunnelBuilder;
        };
    } // namespace Iotsecuretunneling
} // namespace Aws
