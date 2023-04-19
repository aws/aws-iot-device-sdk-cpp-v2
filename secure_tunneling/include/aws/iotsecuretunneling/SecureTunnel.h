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
                Crt::ByteCursor payload,
                uint32_t connectionId,
                Crt::Allocator *allocator = Crt::ApiAllocator()) noexcept;
            Message(
                Crt::ByteCursor serviceId,
                Crt::ByteCursor payload,
                Crt::Allocator *allocator = Crt::ApiAllocator()) noexcept;
            Message(
                Crt::ByteCursor serviceId,
                uint32_t connectionId,
                Crt::ByteCursor payload,
                Crt::Allocator *allocator = Crt::ApiAllocator()) noexcept;

            /**
             * Sets the service id for the secure tunnel message.
             *
             * @param serviceId The service id for the secure tunnel message.
             * @return The Message Object after setting the payload.
             */
            Message &WithServiceId(Crt::ByteCursor serviceId) noexcept;

            /**
             * Sets the connection id for the secure tunnel message.
             *
             * @param connectionId The connection id for the secure tunnel message.
             * @return The Message Object after setting the payload.
             */
            Message &WithConnectionId(uint32_t connectionId) noexcept;

            /**
             * Sets the payload for the secure tunnel message.
             *
             * @param payload The payload for the secure tunnel message.
             * @return The Message Object after setting the payload.
             */
            Message &WithPayload(Crt::ByteCursor payload) noexcept;

            bool initializeRawOptions(aws_secure_tunnel_message_view &raw_options) noexcept;

            /**
             * The service id of the secure tunnel message.
             *
             * @return The service id of the secure tunnel message.
             */
            const Crt::Optional<Crt::ByteCursor> &getServiceId() const noexcept;

            /**
             * The connection id of the secure tunnel message.
             *
             * @return The connection id of the secure tunnel message.
             */
            const uint32_t &getConnectionId() const noexcept;

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
             * The connection id used for simultaneous TCP connections.
             *
             * If left empty, a V1 or V2 protocol message is assumed.
             */
            uint32_t m_connectionId;

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
         * Data model for messages sent out to the WebSocket
         */
        class AWS_IOTSECURETUNNELING_API SendMessageCompleteData
        {
          public:
            SendMessageCompleteData(
                enum aws_secure_tunnel_message_type type,
                Crt::Allocator *allocator = Crt::ApiAllocator()) noexcept;

            /**
             * Message Type of sent message.
             *
             * @return Message Type of sent message.
             */
            const Crt::ByteCursor &getMessageType() const noexcept;

            virtual ~SendMessageCompleteData();
            /* Do not allow direct copy or move */
            SendMessageCompleteData(const SendMessageCompleteData &) = delete;
            SendMessageCompleteData(SendMessageCompleteData &&) noexcept = delete;
            SendMessageCompleteData &operator=(const SendMessageCompleteData &) = delete;
            SendMessageCompleteData &operator=(SendMessageCompleteData &&) noexcept = delete;

          private:
            Crt::Allocator *m_allocator;

            /**
             * Message Type of sent message.
             *
             */
            Crt::ByteCursor m_messageType;

            ///////////////////////////////////////////////////////////////////////////
            // Underlying data storage for internal use
            ///////////////////////////////////////////////////////////////////////////
            Crt::ByteBuf m_messageTypeStorage;
        };

        /**
         * The data returned when a message is sent on the secure tunnel.
         */
        struct AWS_IOTSECURETUNNELING_API SendMessageCompleteEventData
        {
            SendMessageCompleteEventData() : sendMessageCompleteData(nullptr) {}
            std::shared_ptr<SendMessageCompleteData> sendMessageCompleteData;
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
        struct AWS_IOTSECURETUNNELING_API ConnectionSuccessEventData
        {
            ConnectionSuccessEventData() : connectionData(nullptr) {}
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

            /**
             * The connection id of the secure tunnel message.
             *
             * @return The connection id of the secure tunnel message.
             */
            const uint32_t &getConnectionId() const noexcept;

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

            /**
             * The connection id used for simultaneous TCP connections.
             *
             * If left empty, a V1 or V2 protocol message is assumed.
             */
            uint32_t m_connectionId;

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

        /**
         * Data model for stopped Secure Tunnel streams.
         */
        class AWS_IOTSECURETUNNELING_API StreamStoppedData
        {
          public:
            StreamStoppedData(
                const aws_secure_tunnel_message_view &raw_options,
                Crt::Allocator *allocator = Crt::ApiAllocator()) noexcept;

            /**
             * Service id of the stopped stream.
             *
             * @return Service id of the stopped stream.
             */
            const Crt::Optional<Crt::ByteCursor> &getServiceId() const noexcept;

            virtual ~StreamStoppedData();
            /* Do not allow direct copy or move */
            StreamStoppedData(const StreamStoppedData &) = delete;
            StreamStoppedData(StreamStoppedData &&) noexcept = delete;
            StreamStoppedData &operator=(const StreamStoppedData &) = delete;
            StreamStoppedData &operator=(StreamStoppedData &&) noexcept = delete;

          private:
            Crt::Allocator *m_allocator;

            /**
             * Service id of stopped stream.
             *
             * If left empty, a V1 protocol stream is assumed.
             */
            Crt::Optional<Crt::ByteCursor> m_serviceId;

            ///////////////////////////////////////////////////////////////////////////
            // Underlying data storage for internal use
            ///////////////////////////////////////////////////////////////////////////
            Crt::ByteBuf m_serviceIdStorage;
        };

        /**
         * The data returned when a stream is closed on the Secure Tunnel.
         */
        struct AWS_IOTSECURETUNNELING_API StreamStoppedEventData
        {
            StreamStoppedEventData() : streamStoppedData(nullptr) {}
            std::shared_ptr<StreamStoppedData> streamStoppedData;
        };

        /**
         * Data model for opened Secure Tunnel connection.
         */
        class AWS_IOTSECURETUNNELING_API ConnectionStartedData
        {
          public:
            ConnectionStartedData(
                const aws_secure_tunnel_message_view &raw_options,
                Crt::Allocator *allocator = Crt::ApiAllocator()) noexcept;

            /**
             * Service id the connection is using.
             *
             * @return Service id the connection is using.
             */
            const Crt::Optional<Crt::ByteCursor> &getServiceId() const noexcept;

            /**
             * The connection id of the opened connection.
             *
             * @return The connection id of the opened connection.
             */
            const uint32_t &getConnectionId() const noexcept;

            virtual ~ConnectionStartedData();
            /* Do not allow direct copy or move */
            ConnectionStartedData(const ConnectionStartedData &) = delete;
            ConnectionStartedData(ConnectionStartedData &&) noexcept = delete;
            ConnectionStartedData &operator=(const ConnectionStartedData &) = delete;
            ConnectionStartedData &operator=(ConnectionStartedData &&) noexcept = delete;

          private:
            Crt::Allocator *m_allocator;

            /**
             * Service id the connection is using.
             *
             * If left empty, a V1 protocolstream is assumed.
             */
            Crt::Optional<Crt::ByteCursor> m_serviceId;

            /**
             * The connection id of the opened connection.
             *
             */
            uint32_t m_connectionId;

            ///////////////////////////////////////////////////////////////////////////
            // Underlying data storage for internal use
            ///////////////////////////////////////////////////////////////////////////
            Crt::ByteBuf m_serviceIdStorage;
        };

        /**
         * The data returned when a connection is started on the Secure Tunnel.
         */
        struct AWS_IOTSECURETUNNELING_API ConnectionStartedEventData
        {
            ConnectionStartedEventData() : connectionStartedData(nullptr) {}
            std::shared_ptr<ConnectionStartedData> connectionStartedData;
        };

        /**
         * Data model for reset Secure Tunnel connection.
         */
        class AWS_IOTSECURETUNNELING_API ConnectionResetData
        {
          public:
            ConnectionResetData(
                const aws_secure_tunnel_message_view &raw_options,
                Crt::Allocator *allocator = Crt::ApiAllocator()) noexcept;

            /**
             * Service id used by reset connection.
             *
             * @return Service id used by reset connection.
             */
            const Crt::Optional<Crt::ByteCursor> &getServiceId() const noexcept;

            /**
             * Connection id of the reset connection.
             *
             * @return Connection id of the reset connection.
             */
            const uint32_t &getConnectionId() const noexcept;

            virtual ~ConnectionResetData();
            /* Do not allow direct copy or move */
            ConnectionResetData(const ConnectionResetData &) = delete;
            ConnectionResetData(ConnectionResetData &&) noexcept = delete;
            ConnectionResetData &operator=(const ConnectionResetData &) = delete;
            ConnectionResetData &operator=(ConnectionResetData &&) noexcept = delete;

          private:
            Crt::Allocator *m_allocator;

            /**
             * Service id used by the reset connection.
             *
             * If left empty, a V1 protocol stream is assumed.
             */
            Crt::Optional<Crt::ByteCursor> m_serviceId;

            /**
             * The connection id of the reset connection.
             *
             */
            uint32_t m_connectionId;

            ///////////////////////////////////////////////////////////////////////////
            // Underlying data storage for internal use
            ///////////////////////////////////////////////////////////////////////////
            Crt::ByteBuf m_serviceIdStorage;
        };

        /**
         * The data returned when a stream is reset on the Secure Tunnel.
         */
        struct AWS_IOTSECURETUNNELING_API ConnectionResetEventData
        {
            ConnectionResetEventData() : connectionResetData(nullptr) {}
            std::shared_ptr<ConnectionResetData> connectionResetData;
        };

        class SecureTunnel;

        // Client callback type definitions

        /**
         * Type signature of the callback invoked when connection is established with the secure tunnel service and
         * available service ids are returned.
         */
        using OnConnectionSuccess = std::function<void(SecureTunnel *secureTunnel, const ConnectionSuccessEventData &)>;

        /**
         * Type signature of the callback invoked when connection is established with the secure tunnel service and
         * available service ids are returned.
         */
        using OnConnectionFailure = std::function<void(SecureTunnel *secureTunnel, int errorCode)>;

        /**
         * Type signature of the callback invoked when connection is shutdown.
         */
        using OnConnectionShutdown = std::function<void(void)>;

        /**
         * Type signature of the callback invoked when message has been sent through the secure tunnel connection.
         */
        using OnSendMessageComplete =
            std::function<void(SecureTunnel *secureTunnel, int errorCode, const SendMessageCompleteEventData &)>;

        /**
         * Type signature of the callback invoked when a message is received through the secure tunnel connection.
         */
        using OnMessageReceived = std::function<void(SecureTunnel *secureTunnel, const MessageReceivedEventData &)>;

        /**
         * Type signature of the callback invoked when a stream has been started with a source through the secure tunnel
         * connection.
         */
        using OnStreamStarted =
            std::function<void(SecureTunnel *secureTunnel, int errorCode, const StreamStartedEventData &)>;

        /**
         * Type signature of the callback invoked when a stream has been closed
         */
        using OnStreamStopped = std::function<void(SecureTunnel *secureTunnel, const StreamStoppedEventData &)>;

        /**
         * Type signature of the callback invoked when a connection has been started with a source through the secure
         * tunnel connection.
         */
        using OnConnectionStarted =
            std::function<void(SecureTunnel *secureTunnel, int errorCode, const ConnectionStartedEventData &)>;

        /**
         * Type signature of the callback invoked when a connection has been reset
         */
        using OnConnectionReset =
            std::function<void(SecureTunnel *secureTunnel, int errorCode, const ConnectionResetEventData &)>;

        /**
         * Type signature of the callback invoked when the secure tunnel receives a Session Reset.
         */
        using OnSessionReset = std::function<void(void)>;

        /**
         * Type signature of the callback invoked when the secure tunnel completes transitioning to a stopped state.
         */
        using OnStopped = std::function<void(SecureTunnel *secureTunnel)>;

        /**
         * Deprecated - OnConnectionSuccess and OnConnectionFailure
         */
        using OnConnectionComplete = std::function<void(void)>;
        /**
         * Deprecated - Use OnMessageReceived
         */
        using OnDataReceive = std::function<void(const Crt::ByteBuf &data)>;
        /**
         * Deprecated - Use OnStreamStarted
         */
        using OnStreamStart = std::function<void()>;
        /**
         * Deprecated - Use OnStreamStopped
         */
        using OnStreamReset = std::function<void(void)>;
        /**
         * Deprecated - Use OnSendMessageComplete
         */
        using OnSendDataComplete = std::function<void(int errorCode)>;

        /**
         * Represents a unique configuration for a secure tunnel
         */
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
             * Constructor arguments are the minimum required to create a secure tunnel
             */
            SecureTunnelBuilder(
                Crt::Allocator *allocator,      // Should out live this object
                const std::string &accessToken, // Make a copy and save in this object
                aws_secure_tunneling_local_proxy_mode localProxyMode,
                const std::string &endpointHost); // Make a copy and save in this object

            /* Optional members */

            /**
             * Sets TLS options to be used by secure tunnel connection.
             *
             * @param tslOptions TLS options to use for secure tunnel connection. If provided, the rootCA settings in
             * these options will override any rootCA provided to the builder.
             *
             * @return this builder object
             */
            SecureTunnelBuilder &WithTlsConnectionOptions(const Crt::Io::TlsConnectionOptions &tslOptions);

            /**
             * Sets rootCA to be used for this secure tunnel connection overriding the default trust store.
             *
             * @param rootCa string to use as rootCA for secure tunnel connection
             *
             * @return this builder object
             */
            SecureTunnelBuilder &WithRootCa(const std::string &rootCa);

            /**
             * Sets Client Token to a specified value rather than allowing the secure tunnel to auto-generate one.
             *
             * @param clientToken string to use as unique client token for secure tunnel connection
             *
             * @return this builder object
             */
            SecureTunnelBuilder &WithClientToken(const std::string &clientToken);

            /**
             * Sets http proxy options.
             *
             * @param httpClientConnectionProxyOptions http proxy configuration for connection establishment
             *
             * @return this builder object
             */
            SecureTunnelBuilder &WithHttpClientConnectionProxyOptions(
                const Crt::Http::HttpClientConnectionProxyOptions &httpClientConnectionProxyOptions);

            /**
             * Setup callback handler trigged when an Secure Tunnel establishes a connection and receives available
             * service ids.
             *
             * @param onConnectionSuccess
             *
             * @return this builder object
             */
            SecureTunnelBuilder &WithOnConnectionSuccess(OnConnectionSuccess onConnectionSuccess);

            /**
             * Setup callback handler trigged when an Secure Tunnel fails a connection attempt.
             *
             * @param onConnectionFailure
             *
             * @return this builder object
             */
            SecureTunnelBuilder &WithOnConnectionFailure(OnConnectionFailure onConnectionFailure);

            /**
             * Setup callback handler trigged when an Secure Tunnel shuts down connection to the secure tunnel service.
             *
             * @param onConnectionShutdown
             *
             * @return this builder object
             */
            SecureTunnelBuilder &WithOnConnectionShutdown(OnConnectionShutdown onConnectionShutdown);

            /**
             * Setup callback handler trigged when an Secure Tunnel completes sending a message to the secure tunnel
             * service.
             *
             * @param onSendMessageComplete
             *
             * @return this builder object
             */
            SecureTunnelBuilder &WithOnSendMessageComplete(OnSendMessageComplete onSendMessageComplete);

            /**
             * Setup callback handler trigged when an Secure Tunnel receives a Message through the secure tunnel
             * service.
             *
             * @param onMessageReceived
             *
             * @return this builder object
             */
            SecureTunnelBuilder &WithOnMessageReceived(OnMessageReceived onMessageReceived);

            /**
             * Setup callback handler trigged when an Secure Tunnel starts a stream with a source through the secure
             * tunnel service.
             *
             * @param onStreamStarted
             *
             * @return this builder object
             */
            SecureTunnelBuilder &WithOnStreamStarted(OnStreamStarted onStreamStarted);

            /**
             * Setup callback handler trigged when an Secure Tunnel stops a stream.
             *
             * @param onStreamStopped
             *
             * @return this builder object
             */
            SecureTunnelBuilder &WithOnStreamStopped(OnStreamStopped onStreamStopped);

            /**
             * Setup callback handler trigged when an Secure Tunnel starts a connection with a source through the secure
             * tunnel service.
             *
             * @param onConnectionStarted
             *
             * @return this builder object
             */
            SecureTunnelBuilder &WithOnConnectionStarted(OnConnectionStarted onConnectionStarted);

            /**
             * Setup callback handler trigged when an Secure Tunnel resets a connection.
             *
             * @param onConnectionReset
             *
             * @return this builder object
             */
            SecureTunnelBuilder &WithOnConnectionReset(OnConnectionReset onConnectionReset);

            /**
             * Setup callback handler trigged when an Secure Tunnel receives a stream reset.
             *
             * @param onStreamReset
             *
             * @return this builder object
             */
            SecureTunnelBuilder &WithOnStreamReset(OnStreamReset onStreamReset);

            /**
             * Setup callback handler trigged when an Secure Tunnel receives a session reset from the secure tunnel
             * service.
             *
             * @param onSessionReset
             *
             * @return this builder object
             */
            SecureTunnelBuilder &WithOnSessionReset(OnSessionReset onSessionReset);

            /**
             * Setup callback handler trigged when an Secure Tunnel completes entering a stopped state
             *
             * @param onStopped
             *
             * @return this builder object
             */
            SecureTunnelBuilder &WithOnStopped(OnStopped onStopped);

            /**
             * Deprecated - Use WithOnMessageReceived()
             */
            SecureTunnelBuilder &WithOnDataReceive(OnDataReceive onDataReceive);
            /**
             * Deprecated - Use WithOnConnectionSuccess() and WithOnConnectionFailure()
             */
            SecureTunnelBuilder &WithOnConnectionComplete(OnConnectionComplete onConnectionComplete);
            /**
             * Deprecated - Use WithOnStreamStarted
             */
            SecureTunnelBuilder &WithOnStreamStart(OnStreamStart onStreamStart);
            /**
             * Deprecated - Use WithOnSendMessageComplete
             */
            SecureTunnelBuilder &WithOnSendDataComplete(OnSendDataComplete onSendDataComplete);

            /**
             * Will return a shared pointer to a new SecureTunnel that countains a
             * new aws_secure_tunnel that is generated using the set members of SecureTunnelBuilder.
             * On failure, the shared_ptr will contain a nullptr, call Crt::LastErrorOrUnknown(); to get the reason
             * for failure.
             */
            std::shared_ptr<SecureTunnel> Build() noexcept;

          private:
            /* Required Memebers */

            Crt::Allocator *m_allocator;

            /**
             * Client bootstrap to use.  In almost all cases, this can be left undefined.
             */
            Crt::Io::ClientBootstrap *m_clientBootstrap;

            /**
             * Controls socket properties of the underlying connections made by the secure tunnel.  Leave undefined to
             * use defaults (no TCP keep alive, 10 second socket timeout).
             */
            Crt::Io::SocketOptions m_socketOptions;

            /**
             * Token used to establish a WebSocket connection with the secure tunnel service. This token is one time use
             * and must be rotated to establish a new connection to the secure tunnel unless using a unique client
             * token.
             */
            std::string m_accessToken;

            /**
             * Proxy mode to use.
             */
            aws_secure_tunneling_local_proxy_mode m_localProxyMode;

            /**
             * AWS Secure Tunnel endpoint to connect to.
             */
            std::string m_endpointHost;

            /* Optional members */
            /**
             * Client token is used to reconnect to a secure tunnel after initial connection. If this is not set by the
             * user, one will be automatically generated and used to maintain a connection as long as the secure tunnel
             * has the desired state of CONNECTED.
             */
            std::string m_clientToken;

            /**
             * If set, TLS context for secure socket connections.
             * If undefined, then default options will be used.
             */
            Crt::Optional<Crt::Io::TlsConnectionOptions> m_tlsConnectionOptions;

            /**
             * If set, this will be used to override the default trust store.
             */
            std::string m_rootCa;

            /**
             * If set, http proxy configuration will be used for connection establishment
             */
            Crt::Optional<Crt::Http::HttpClientConnectionProxyOptions> m_httpClientConnectionProxyOptions;

            /* Callbacks */
            /**
             * Callback handler trigged when secure tunnel establishes connection with secure tunnel service and
             * receives available service ids.
             */
            OnConnectionSuccess m_OnConnectionSuccess;

            /* Callbacks */
            /**
             * Callback handler trigged when secure tunnel establishes fails a connection attempt with secure tunnel
             * service.
             */
            OnConnectionFailure m_OnConnectionFailure;

            /**
             * Callback handler trigged when secure tunnel connection to secure tunnel service is closed.
             */
            OnConnectionShutdown m_OnConnectionShutdown;

            /**
             * Callback handler trigged when secure tunnel completes sending message to the secure tunnel service.
             */
            OnSendMessageComplete m_OnSendMessageComplete;

            /**
             * Callback handler trigged when secure tunnel receives a message from the secure tunnel service.
             *
             * @param SecureTunnel: The shared secure tunnel
             * @param MessageReceivedEventData: Data received
             */
            OnMessageReceived m_OnMessageReceived;

            /**
             * Callback handler trigged when secure tunnel receives a stream start from a source device.
             *
             * @param SecureTunnel: The shared secure tunnel
             * @param int: error code
             * @param StreamStartedEventData: Stream Started data
             */
            OnStreamStarted m_OnStreamStarted;

            /**
             * Callback handler trigged when secure tunnel receives a stream reset.
             *
             * @param SecureTunnel: The shared secure tunnel
             * @param StreamStoppedEventData: Stream stopped data
             */
            OnStreamStopped m_OnStreamStopped;

            /**
             * Callback handler trigged when secure tunnel receives a connection start from a source device.
             *
             * @param SecureTunnel: The shared secure tunnel
             * @param int: error code
             * @param ConnectionStartedEventData: Connection Started data
             */
            OnConnectionStarted m_OnConnectionStarted;

            /**
             * Callback handler trigged when secure tunnel receives a connection reset.
             *
             * @param SecureTunnel: The shared secure tunnel
             * @param ConnectionResetEventData: Connection reset data
             */
            OnConnectionReset m_OnConnectionReset;

            /**
             * Callback handler trigged when secure tunnel receives a session reset from the secure tunnel service.
             */
            OnSessionReset m_OnSessionReset;

            /**
             * Callback handler trigged when secure tunnel completes transition to stopped state.
             */
            OnStopped m_OnStopped;

            /**
             * Deprecated - Use m_OnConnectionSuccess and m_OnConnectionFailure
             */
            OnConnectionComplete m_OnConnectionComplete;
            /**
             * Deprecated - Use m_OnMessageReceived
             */
            OnDataReceive m_OnDataReceive;
            /**
             * Deprecated - Use m_OnStreamStarted
             */
            OnStreamStart m_OnStreamStart;
            /**
             * Deprecated - Use m_OnStreamStopped
             */
            OnStreamReset m_OnStreamReset;
            /**
             * Deprecated - Use m_OnSendMessageComplete
             */
            OnSendDataComplete m_OnSendDataComplete;

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
            SecureTunnel(const SecureTunnel &) = delete;
            SecureTunnel &operator=(const SecureTunnel &) = delete;

            SecureTunnel(SecureTunnel &&) noexcept;
            SecureTunnel &operator=(SecureTunnel &&) noexcept;

            bool IsValid();

            /**
             * Notifies the secure tunnel that you want it to attempt to connect to the configured endpoint.
             * The secure tunnel will attempt to stay connected and attempt to reconnect if disconnected.
             *
             * @return success/failure in the synchronous logic that kicks off the start process
             */
            int Start();

            /**
             * Notifies the secure tunnel that you want it to transition to the stopped state, disconnecting any
             * existing connection and stopping subsequent reconnect attempts.
             *
             * @return success/failure in the synchronous logic that kicks off the stop process
             */
            int Stop();

            /**
             * Tells the secure tunnel to attempt to send a Message
             *
             * @param messageOptions: Message to send to the secure tunnel service.
             *
             * @return success/failure in the synchronous logic that kicks off the Send Message operation
             */
            int SendMessage(std::shared_ptr<Message> messageOptions) noexcept;

            /* SOURCE MODE ONLY */
            /**
             * Notifies the secure tunnel that you want to start a stream with the Destination device. This will result
             * in a V1 stream.
             *
             * @return success/failure in the synchronous logic that kicks off the Stream Start operation
             */
            int SendStreamStart();

            /**
             * Notifies the secure tunnel that you want to start a stream with the Destination device on a specific
             * service id. This will result in a V2 stream.
             *
             * @param serviceId: The Service Id to start a stream on.
             *
             * @return success/failure in the synchronous logic that kicks off the Stream Start operation
             */
            int SendStreamStart(std::string serviceId);

            /**
             * Notifies the secure tunnel that you want to start a stream with the Destination device on a specific
             * service id. This will result in a V2 stream.
             *
             * @param serviceId: The Service Id to start a stream on.
             *
             * @return success/failure in the synchronous logic that kicks off the Stream Start operation
             */
            int SendStreamStart(Crt::ByteCursor serviceId);

            /**
             * Notifies the secure tunnel that you want to start a stream with the Destination device on a specific
             * service id. This will result in a V2 stream.
             *
             * @param serviceId: The Service Id to start a stream on.
             *
             * @param connectionId: The Connection Id to start the stream on.
             *
             * @return success/failure in the synchronous logic that kicks off the Stream Start operation
             */
            int SendStreamStart(std::string serviceId, uint32_t connectionId);

            /**
             * Notifies the secure tunnel that you want to start a stream with the Destination device on a specific
             * service id. This will result in a V2 stream.
             *
             * @param serviceId: The Service Id to start a stream on.
             *
             * @param connectionId: The Connection Id to start the stream on.
             *
             * @return success/failure in the synchronous logic that kicks off the Stream Start operation
             */
            int SendStreamStart(Crt::ByteCursor serviceId, uint32_t connectionId);

            /**
             * Notifies the secure tunnel that you want to start a connection with the Destination device.
             *
             * @param connectionId: The connection id to start the connection on.
             *
             * @return success/failure in the synchronous logic that kicks off the Stream Start operation
             */
            int SendConnectionStart(uint32_t connectionId);

            /**
             * Notifies the secure tunnel that you want to start a connection with the Destination device.
             *
             * @param serviceId: The Service Id to start the connection on.
             *
             * @param connectionId: The connection id to start the connection on.
             *
             * @return success/failure in the synchronous logic that kicks off the Stream Start operation
             */
            int SendConnectionStart(std::string serviceId, uint32_t connectionId);

            /**
             * Notifies the secure tunnel that you want to start a connection with the Destination device.
             *
             * @param serviceId: The Service Id to start the connection on.
             *
             * @param connectionId: The connection id to start the connection on.
             *
             * @return success/failure in the synchronous logic that kicks off the Stream Start operation
             */
            int SendConnectionStart(Crt::ByteCursor serviceId, uint32_t connectionId);

            aws_secure_tunnel *GetUnderlyingHandle();

            /**
             * Deprecated - use Start()
             */
            int Connect();

            /**
             * Deprecated - Use Stop()
             */
            int Close();

            /**
             * Deprecated - Use Stop()
             */
            void Shutdown();

            /**
             * Deprecated - Use SendMessage()
             */
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

                Crt::Io::TlsConnectionOptions *tslOptions,
                const std::string &rootCa,
                Crt::Http::HttpClientConnectionProxyOptions *httpClientConnectionProxyOptions,

                OnConnectionSuccess onConnectionSuccess,
                OnConnectionFailure onConnectionFailure,
                OnConnectionComplete onConnectionComplete, /* Deprecated */
                OnConnectionShutdown onConnectionShutdown,
                OnSendMessageComplete onSendMessageComplete,
                OnSendDataComplete onSendDataComplete, /* Deprecated */
                OnMessageReceived onMessageReceived,
                OnDataReceive onDataReceive, /* Deprecated */
                OnStreamStarted onStreamStarted,
                OnStreamStart onStreamStart, /* Deprecated */
                OnStreamStopped onStreamStopped,
                OnStreamReset onStreamReset, /* Deprecated */
                OnConnectionStarted onConnectionStarted,
                OnConnectionReset onConnectionReset,
                OnSessionReset onSessionReset,
                OnStopped onStopped);

            /* Static Callbacks */
            static void s_OnMessageReceived(const struct aws_secure_tunnel_message_view *message, void *user_data);
            static void s_OnConnectionComplete(
                const struct aws_secure_tunnel_connection_view *connection,
                int error_code,
                void *user_data);
            static void s_OnConnectionFailure(int error_code, void *user_data);
            static void s_OnConnectionShutdown(int error_code, void *user_data);
            static void s_OnSendDataComplete(int error_code, void *user_data);
            static void s_OnSendMessageComplete(
                enum aws_secure_tunnel_message_type type,
                int error_code,
                void *user_data);
            static void s_OnStreamStopped(
                const struct aws_secure_tunnel_message_view *message,
                int error_code,
                void *user_data);
            static void s_OnSessionReset(void *user_data);
            static void s_OnStopped(void *user_data);
            static void s_OnTerminationComplete(void *user_data);
            static void s_OnStreamStarted(
                const struct aws_secure_tunnel_message_view *message,
                int error_code,
                void *user_data);
            static void s_OnConnectionStarted(
                const struct aws_secure_tunnel_message_view *message,
                int error_code,
                void *user_data);
            static void s_OnConnectionReset(
                const struct aws_secure_tunnel_message_view *message,
                int error_code,
                void *user_data);

            void OnTerminationComplete();

            /**
             * Callback handler trigged when secure tunnel receives a Message.
             */
            OnMessageReceived m_OnMessageReceived;

            /**
             * Callback handler trigged when secure tunnel establishes connection with the secure tunnel service and
             * receives service ids.
             */
            OnConnectionSuccess m_OnConnectionSuccess;

            /**
             * Callback handler trigged when secure tunnel fails a connection attempt with the secure tunnel service.
             */
            OnConnectionFailure m_OnConnectionFailure;

            /**
             * Callback handler trigged when secure tunnel shuts down connection.
             */
            OnConnectionShutdown m_OnConnectionShutdown;

            /**
             * Callback handler trigged when secure tunnel completes sending message to the secure tunnel service.
             */
            OnSendMessageComplete m_OnSendMessageComplete;

            /**
             * Callback handler trigged when secure tunnel starts a stream with a source device through the secure
             * tunnel service.
             */
            OnStreamStarted m_OnStreamStarted;

            /**
             * Callback handler trigged when secure tunnel closes a stream
             */
            OnStreamStopped m_OnStreamStopped;

            /**
             * Callback handler trigged when secure tunnel starts a connection with a source device through the secure
             * tunnel service.
             */
            OnConnectionStarted m_OnConnectionStarted;

            /**
             * Callback handler trigged when secure tunnel resets a connection
             */
            OnConnectionReset m_OnConnectionReset;

            /**
             * Callback handler trigged when secure tunnel receives a session reset from the secure tunnel service.
             */
            OnSessionReset m_OnSessionReset;

            /**
             * Callback handler trigged when secure tunnel finishes entering a stopped state.
             */
            OnStopped m_OnStopped;

            aws_secure_tunnel *m_secure_tunnel;
            Crt::Allocator *m_allocator;

            /**
             * Deprecated - Use m_OnMessageReceived
             */
            OnDataReceive m_OnDataReceive;
            /**
             * Deprecated - Use m_OnConnectionSuccess and m_OnConnectionFailure
             */
            OnConnectionComplete m_OnConnectionComplete;
            /**
             * Deprecated - Use m_OnStreamStarted
             */
            OnStreamStart m_OnStreamStart;
            /**
             * Deprecated - Use m_OnStreamStopped
             */
            OnStreamReset m_OnStreamReset;
            /**
             * Deprecated - Use m_OnMessageSendComplete
             */
            OnSendDataComplete m_OnSendDataComplete;

            std::shared_ptr<SecureTunnel> m_selfRef;

            friend class SecureTunnelBuilder;
        };
    } // namespace Iotsecuretunneling
} // namespace Aws
