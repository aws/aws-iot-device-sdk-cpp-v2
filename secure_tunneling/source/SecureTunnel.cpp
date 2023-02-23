/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/crt/Api.h>
#include <aws/iotsecuretunneling/SecureTunnel.h>

namespace Aws
{
    namespace Iotsecuretunneling
    {
        void setPacketByteBufOptional(
            Crt::Optional<Crt::ByteCursor> &optional,
            Crt::ByteBuf &optionalStorage,
            Crt::Allocator *allocator,
            const Crt::ByteCursor *value)
        {
            aws_byte_buf_clean_up(&optionalStorage);
            AWS_ZERO_STRUCT(optionalStorage);
            if (value != nullptr)
            {
                aws_byte_buf_init_copy_from_cursor(&optionalStorage, allocator, *value);
                optional = aws_byte_cursor_from_buf(&optionalStorage);
            }
            else
            {
                optional.reset();
            }
        }

        void setPacketStringOptional(
            Crt::Optional<Crt::ByteCursor> &optional,
            Crt::String &optionalStorage,
            const aws_byte_cursor *value)
        {
            if (value != nullptr)
            {
                optionalStorage = Crt::String((const char *)value->ptr, value->len);
                struct aws_byte_cursor optional_cursor;
                optional_cursor.ptr = (uint8_t *)optionalStorage.c_str();
                optional_cursor.len = optionalStorage.size();
                optional = optional_cursor;
            }
        }

        //***********************************************************************************************************************
        /*                                              Message */
        //***********************************************************************************************************************

        Message::Message(const aws_secure_tunnel_message_view &message, Crt::Allocator *allocator) noexcept
            : m_allocator(allocator)
        {
            AWS_ZERO_STRUCT(m_payloadStorage);
            AWS_ZERO_STRUCT(m_serviceIdStorage);

            setPacketByteBufOptional(m_payload, m_payloadStorage, m_allocator, message.payload);
            setPacketByteBufOptional(m_serviceId, m_serviceIdStorage, m_allocator, message.service_id);
        }

        /* Default constructor */
        Message::Message(Crt::Allocator *allocator) noexcept : m_allocator(allocator)
        {
            AWS_ZERO_STRUCT(m_payloadStorage);
            AWS_ZERO_STRUCT(m_serviceIdStorage);
        }

        Message::Message(Crt::ByteCursor payload, Crt::Allocator *allocator) noexcept : m_allocator(allocator)
        {
            AWS_ZERO_STRUCT(m_payloadStorage);
            AWS_ZERO_STRUCT(m_serviceIdStorage);

            aws_byte_buf_clean_up(&m_payloadStorage);
            aws_byte_buf_init_copy_from_cursor(&m_payloadStorage, m_allocator, payload);
            m_payload = aws_byte_cursor_from_buf(&m_payloadStorage);
        }

        Message::Message(Crt::ByteCursor serviceId, Crt::ByteCursor payload, Crt::Allocator *allocator) noexcept
            : m_allocator(allocator)
        {
            AWS_ZERO_STRUCT(m_payloadStorage);
            AWS_ZERO_STRUCT(m_serviceIdStorage);

            aws_byte_buf_clean_up(&m_payloadStorage);
            aws_byte_buf_init_copy_from_cursor(&m_payloadStorage, m_allocator, payload);
            m_payload = aws_byte_cursor_from_buf(&m_payloadStorage);

            aws_byte_buf_clean_up(&m_serviceIdStorage);
            aws_byte_buf_init_copy_from_cursor(&m_serviceIdStorage, m_allocator, serviceId);
            m_serviceId = aws_byte_cursor_from_buf(&m_serviceIdStorage);
        }

        Message &Message::withPayload(Crt::ByteCursor payload) noexcept
        {
            aws_byte_buf_clean_up(&m_payloadStorage);
            aws_byte_buf_init_copy_from_cursor(&m_payloadStorage, m_allocator, payload);
            m_payload = aws_byte_cursor_from_buf(&m_payloadStorage);
            return *this;
        }

        Message &Message::withServiceId(Crt::ByteCursor serviceId) noexcept
        {
            aws_byte_buf_clean_up(&m_serviceIdStorage);
            aws_byte_buf_init_copy_from_cursor(&m_serviceIdStorage, m_allocator, serviceId);
            m_serviceId = aws_byte_cursor_from_buf(&m_serviceIdStorage);
            return *this;
        }

        bool Message::initializeRawOptions(aws_secure_tunnel_message_view &raw_options) noexcept
        {
            AWS_ZERO_STRUCT(raw_options);
            if (m_payload.has_value())
            {
                raw_options.payload = &m_payload.value();
            }
            if (m_serviceId.has_value())
            {
                raw_options.service_id = &m_serviceId.value();
            }

            return true;
        }

        const Crt::Optional<Crt::ByteCursor> &Message::getPayload() const noexcept { return m_payload; }

        const Crt::Optional<Crt::ByteCursor> &Message::getServiceId() const noexcept { return m_serviceId; }

        Message::~Message()
        {
            aws_byte_buf_clean_up(&m_payloadStorage);
            aws_byte_buf_clean_up(&m_serviceIdStorage);
        }

        //***********************************************************************************************************************
        /*                                              ConnectionData */
        //***********************************************************************************************************************

        ConnectionData::ConnectionData(
            const aws_secure_tunnel_connection_view &connection,
            Crt::Allocator *allocator) noexcept
            : m_allocator(allocator)
        {
            AWS_ZERO_STRUCT(m_serviceId1Storage);
            AWS_ZERO_STRUCT(m_serviceId2Storage);
            AWS_ZERO_STRUCT(m_serviceId3Storage);

            setPacketByteBufOptional(m_serviceId1, m_serviceId1Storage, m_allocator, connection.service_id_1);
            setPacketByteBufOptional(m_serviceId2, m_serviceId2Storage, m_allocator, connection.service_id_2);
            setPacketByteBufOptional(m_serviceId3, m_serviceId3Storage, m_allocator, connection.service_id_3);
        }

        const Crt::Optional<Crt::ByteCursor> &ConnectionData::getServiceId1() const noexcept { return m_serviceId1; }
        const Crt::Optional<Crt::ByteCursor> &ConnectionData::getServiceId2() const noexcept { return m_serviceId2; }
        const Crt::Optional<Crt::ByteCursor> &ConnectionData::getServiceId3() const noexcept { return m_serviceId3; }

        ConnectionData::~ConnectionData()
        {
            aws_byte_buf_clean_up(&m_serviceId1Storage);
            aws_byte_buf_clean_up(&m_serviceId2Storage);
            aws_byte_buf_clean_up(&m_serviceId3Storage);
        }

        //***********************************************************************************************************************
        /*                                              StreamStartData */
        //***********************************************************************************************************************

        StreamStartedData::StreamStartedData(
            const aws_secure_tunnel_message_view &message,
            Crt::Allocator *allocator) noexcept
            : m_allocator(allocator)
        {
            AWS_ZERO_STRUCT(m_serviceIdStorage);

            setPacketByteBufOptional(m_serviceId, m_serviceIdStorage, m_allocator, message.service_id);
        }

        const Crt::Optional<Crt::ByteCursor> &StreamStartedData::getServiceId() const noexcept { return m_serviceId; }

        StreamStartedData::~StreamStartedData() { aws_byte_buf_clean_up(&m_serviceIdStorage); }

        //***********************************************************************************************************************
        /*                                          SecureTunnelBuilder */
        //***********************************************************************************************************************
        SecureTunnelBuilder::SecureTunnelBuilder(
            Crt::Allocator *allocator,                        // Should out live this object
            Aws::Crt::Io::ClientBootstrap &clientBootstrap,   // Should out live this object
            const Aws::Crt::Io::SocketOptions &socketOptions, // Make a copy and save in this object
            const std::string &accessToken,                   // Make a copy and save in this object
            aws_secure_tunneling_local_proxy_mode localProxyMode,
            const std::string &endpointHost) // Make a copy and save in this object
            : m_allocator(allocator), m_clientBootstrap(&clientBootstrap), m_socketOptions(socketOptions),
              m_accessToken(accessToken), m_localProxyMode(localProxyMode), m_endpointHost(endpointHost), m_rootCa(""),
              m_httpClientConnectionProxyOptions(), m_OnConnectionShutdown(), m_OnSendDataComplete(), m_OnStreamReset(),
              m_OnSessionReset(), m_OnConnectionComplete(), m_OnDataReceive(), m_OnStreamStart()
        {
        }

        SecureTunnelBuilder::SecureTunnelBuilder(
            Crt::Allocator *allocator,                        // Should out live this object
            const Aws::Crt::Io::SocketOptions &socketOptions, // Make a copy and save in this object
            const std::string &accessToken,                   // Make a copy and save in this object
            aws_secure_tunneling_local_proxy_mode localProxyMode,
            const std::string &endpointHost) // Make a copy and save in this object
            : m_allocator(allocator), m_clientBootstrap(Crt::ApiHandle::GetOrCreateStaticDefaultClientBootstrap()),
              m_socketOptions(socketOptions), m_accessToken(accessToken), m_localProxyMode(localProxyMode),
              m_endpointHost(endpointHost), m_rootCa(""), m_httpClientConnectionProxyOptions(),
              m_OnConnectionShutdown(), m_OnSendDataComplete(), m_OnStreamReset(), m_OnSessionReset(),
              m_OnConnectionComplete(), m_OnDataReceive(), m_OnStreamStart()
        {
        }

        SecureTunnelBuilder::SecureTunnelBuilder(
            Crt::Allocator *allocator,      // Should out live this object
            const std::string &accessToken, // Make a copy and save in this object
            aws_secure_tunneling_local_proxy_mode localProxyMode,
            const std::string &endpointHost) // Make a copy and save in this object
            : m_allocator(allocator), m_clientBootstrap(Crt::ApiHandle::GetOrCreateStaticDefaultClientBootstrap()),
              m_socketOptions(Crt::Io::SocketOptions()), m_accessToken(accessToken), m_localProxyMode(localProxyMode),
              m_endpointHost(endpointHost), m_rootCa(""), m_httpClientConnectionProxyOptions(),
              m_OnConnectionShutdown(), m_OnSendDataComplete(), m_OnStreamReset(), m_OnSessionReset(),
              m_OnConnectionComplete(), m_OnDataReceive(), m_OnStreamStart()
        {
        }

        SecureTunnelBuilder &SecureTunnelBuilder::WithRootCa(const std::string &rootCa)
        {
            m_rootCa = rootCa;
            return *this;
        }

        SecureTunnelBuilder &SecureTunnelBuilder::WithHttpClientConnectionProxyOptions(
            const Aws::Crt::Http::HttpClientConnectionProxyOptions &httpClientConnectionProxyOptions)
        {
            m_httpClientConnectionProxyOptions = httpClientConnectionProxyOptions;
            return *this;
        }

        SecureTunnelBuilder &SecureTunnelBuilder::WithOnConnectionSuccess(OnConnectionSuccess onConnectionSuccess)
        {
            m_OnConnectionSuccess = std::move(onConnectionSuccess);
            return *this;
        }

        SecureTunnelBuilder &SecureTunnelBuilder::WithOnConnectionFailure(OnConnectionFailure onConnectionFailure)
        {
            m_OnConnectionFailure = std::move(onConnectionFailure);
            return *this;
        }

        SecureTunnelBuilder &SecureTunnelBuilder::WithOnConnectionShutdown(OnConnectionShutdown onConnectionShutdown)
        {
            m_OnConnectionShutdown = std::move(onConnectionShutdown);
            return *this;
        }

        SecureTunnelBuilder &SecureTunnelBuilder::WithOnSendDataComplete(OnSendDataComplete onSendDataComplete)
        {
            m_OnSendDataComplete = std::move(onSendDataComplete);
            return *this;
        }

        SecureTunnelBuilder &SecureTunnelBuilder::WithOnMessageReceived(OnMessageReceived OnMessageReceived)
        {
            m_OnMessageReceived = std::move(OnMessageReceived);
            return *this;
        }

        SecureTunnelBuilder &SecureTunnelBuilder::WithOnStreamStarted(OnStreamStarted onStreamStarted)
        {
            m_OnStreamStarted = std::move(onStreamStarted);
            return *this;
        }

        SecureTunnelBuilder &SecureTunnelBuilder::WithOnStreamReset(OnStreamReset onStreamReset)
        {
            m_OnStreamReset = std::move(onStreamReset);
            return *this;
        }

        SecureTunnelBuilder &SecureTunnelBuilder::WithOnSessionReset(OnSessionReset onSessionReset)
        {
            m_OnSessionReset = std::move(onSessionReset);
            return *this;
        }

        SecureTunnelBuilder &SecureTunnelBuilder::WithOnStopped(OnStopped onStopped)
        {
            m_OnStopped = std::move(onStopped);
            return *this;
        }

        SecureTunnelBuilder &SecureTunnelBuilder::WithClientToken(const std::string &clientToken)
        {
            m_clientToken = clientToken;
            return *this;
        }

        /* Deprecated - Use WithOnConnectionSuccess and WithOnConnectionFailure */
        SecureTunnelBuilder &SecureTunnelBuilder::WithOnConnectionComplete(OnConnectionComplete onConnectionComplete)
        {
            m_OnConnectionComplete = std::move(onConnectionComplete);
            return *this;
        }
        /* Deprecated - Use WithOnStreamStarted */
        SecureTunnelBuilder &SecureTunnelBuilder::WithOnStreamStart(OnStreamStart onStreamStart)
        {
            m_OnStreamStart = std::move(onStreamStart);
            return *this;
        }
        /* Deprecated - Use WithOnMessageReceived */
        SecureTunnelBuilder &SecureTunnelBuilder::WithOnDataReceive(OnDataReceive onDataReceive)
        {
            m_OnDataReceive = std::move(onDataReceive);
            return *this;
        }

        std::shared_ptr<SecureTunnel> SecureTunnelBuilder::Build() noexcept
        {
            auto tunnel = std::shared_ptr<SecureTunnel>(new SecureTunnel(
                m_allocator,
                m_clientBootstrap,
                m_socketOptions,
                m_accessToken,
                m_clientToken,
                m_localProxyMode,
                m_endpointHost,
                m_rootCa,
                m_httpClientConnectionProxyOptions.has_value() ? &m_httpClientConnectionProxyOptions.value() : nullptr,
                m_OnConnectionSuccess,
                m_OnConnectionFailure,
                m_OnConnectionComplete,
                m_OnConnectionShutdown,
                m_OnSendDataComplete,
                m_OnMessageReceived,
                m_OnDataReceive,
                m_OnStreamStarted,
                m_OnStreamStart,
                m_OnStreamReset,
                m_OnSessionReset,
                m_OnStopped));

            if (tunnel->m_secure_tunnel == nullptr)
            {
                return nullptr;
            }

            return tunnel;
        }

        //***********************************************************************************************************************
        /*                                             SecureTunnel */
        //***********************************************************************************************************************

        /**
         * Private SecureTunnel constructor used by SecureTunnelBuilder on SecureTunnelBuilder::Build() and by old
         * SecureTunnel constructor which should be deprecated
         */
        SecureTunnel::SecureTunnel(
            Crt::Allocator *allocator,
            Aws::Crt::Io::ClientBootstrap *clientBootstrap,
            const Aws::Crt::Io::SocketOptions &socketOptions,
            const std::string &accessToken,
            const std::string &clientToken,
            aws_secure_tunneling_local_proxy_mode localProxyMode,
            const std::string &endpointHost,

            const std::string &rootCa,
            Aws::Crt::Http::HttpClientConnectionProxyOptions *httpClientConnectionProxyOptions,

            OnConnectionSuccess onConnectionSuccess,
            OnConnectionFailure onConnectionFailure,
            OnConnectionComplete onConnectionComplete,
            OnConnectionShutdown onConnectionShutdown,
            OnSendDataComplete onSendDataComplete,
            OnMessageReceived onMessageReceived,
            OnDataReceive onDataReceive,
            OnStreamStarted onStreamStarted,
            OnStreamStart onStreamStart,
            OnStreamReset onStreamReset,
            OnSessionReset onSessionReset,
            OnStopped onStopped)
        {
            // Client callbacks
            m_OnConnectionSuccess = std::move(onConnectionSuccess);
            m_OnConnectionFailure = std::move(onConnectionFailure);
            m_OnConnectionComplete = std::move(onConnectionComplete);
            m_OnConnectionShutdown = std::move(onConnectionShutdown);
            m_OnSendDataComplete = std::move(onSendDataComplete);
            m_OnMessageReceived = std::move(onMessageReceived);
            m_OnDataReceive = std::move(onDataReceive);
            m_OnStreamStarted = std::move(onStreamStarted);
            m_OnStreamStart = std::move(onStreamStart);
            m_OnStreamReset = std::move(onStreamReset);
            m_OnSessionReset = std::move(onSessionReset);
            m_OnStopped = std::move(onStopped);

            // Initialize aws_secure_tunnel_options
            aws_secure_tunnel_options config;
            AWS_ZERO_STRUCT(config);

            config.bootstrap = clientBootstrap ? clientBootstrap->GetUnderlyingHandle() : nullptr;
            config.socket_options = &socketOptions.GetImpl();

            config.access_token = aws_byte_cursor_from_c_str(accessToken.c_str());
            config.local_proxy_mode = localProxyMode;
            config.endpoint_host = aws_byte_cursor_from_c_str(endpointHost.c_str());

            if (rootCa.length() > 0)
            {
                config.root_ca = rootCa.c_str();
            }

            if (clientToken.length() > 0)
            {
                config.client_token = aws_byte_cursor_from_c_str(clientToken.c_str());
            }

            /* callbacks for native secure tunnel */
            config.on_message_received = s_OnMessageReceived;
            config.on_connection_complete = s_OnConnectionComplete;
            config.on_connection_shutdown = s_OnConnectionShutdown;
            config.on_send_data_complete = s_OnSendDataComplete;
            config.on_stream_start = s_OnStreamStarted;
            config.on_stream_reset = s_OnStreamReset;
            config.on_session_reset = s_OnSessionReset;
            config.on_stopped = s_OnStopped;

            config.user_data = this;

            aws_http_proxy_options temp;
            AWS_ZERO_STRUCT(temp);
            if (httpClientConnectionProxyOptions != NULL)
            {
                httpClientConnectionProxyOptions->InitializeRawProxyOptions(temp);
                config.http_proxy_options = &temp;
            }

            // Create the secure tunnel
            m_secure_tunnel = aws_secure_tunnel_new(allocator, &config);
            m_allocator = allocator;
        }

        /**
         * Deprecated - Use SecureTunnelBuilder
         */
        SecureTunnel::SecureTunnel(
            Crt::Allocator *allocator,
            Aws::Crt::Io::ClientBootstrap *clientBootstrap,
            const Aws::Crt::Io::SocketOptions &socketOptions,

            const std::string &accessToken,
            aws_secure_tunneling_local_proxy_mode localProxyMode,
            const std::string &endpointHost,
            const std::string &rootCa,

            OnConnectionComplete onConnectionComplete,
            OnConnectionShutdown onConnectionShutdown,
            OnSendDataComplete onSendDataComplete,
            OnDataReceive onDataReceive,
            OnStreamStart onStreamStart,
            OnStreamReset onStreamReset,
            OnSessionReset onSessionReset)
            : SecureTunnel(
                  allocator,
                  clientBootstrap,
                  socketOptions,
                  accessToken,
                  nullptr,
                  localProxyMode,
                  endpointHost,
                  rootCa,
                  nullptr,
                  nullptr,
                  nullptr,
                  onConnectionComplete,
                  onConnectionShutdown,
                  onSendDataComplete,
                  nullptr,
                  onDataReceive,
                  nullptr,
                  onStreamStart,
                  onStreamReset,
                  onSessionReset,
                  nullptr)
        {
        }

        /**
         * Deprecated - Use SecureTunnelBuilder
         */
        SecureTunnel::SecureTunnel(
            Crt::Allocator *allocator,
            const Aws::Crt::Io::SocketOptions &socketOptions,

            const std::string &accessToken,
            aws_secure_tunneling_local_proxy_mode localProxyMode,
            const std::string &endpointHost,
            const std::string &rootCa,

            OnConnectionComplete onConnectionComplete,
            OnConnectionShutdown onConnectionShutdown,
            OnSendDataComplete onSendDataComplete,
            OnDataReceive onDataReceive,
            OnStreamStart onStreamStart,
            OnStreamReset onStreamReset,
            OnSessionReset onSessionReset)
            : SecureTunnel(
                  allocator,
                  Crt::ApiHandle::GetOrCreateStaticDefaultClientBootstrap(),
                  socketOptions,
                  accessToken,
                  nullptr,
                  localProxyMode,
                  endpointHost,
                  rootCa,
                  nullptr,
                  nullptr,
                  nullptr,
                  onConnectionComplete,
                  onConnectionShutdown,
                  onSendDataComplete,
                  nullptr,
                  onDataReceive,
                  nullptr,
                  onStreamStart,
                  onStreamReset,
                  onSessionReset,
                  nullptr)
        {
        }

        SecureTunnel::~SecureTunnel()
        {
            if (m_secure_tunnel)
            {
                aws_secure_tunnel_release(m_secure_tunnel);
                m_secure_tunnel = nullptr;
            }
        }

        SecureTunnel::SecureTunnel(SecureTunnel &&other) noexcept
        {
            m_OnConnectionSuccess = std::move(other.m_OnConnectionSuccess);
            m_OnConnectionFailure = std::move(other.m_OnConnectionFailure);
            m_OnConnectionShutdown = std::move(other.m_OnConnectionShutdown);
            m_OnSendDataComplete = std::move(other.m_OnSendDataComplete);
            m_OnMessageReceived = std::move(other.m_OnMessageReceived);
            m_OnStreamStarted = std::move(other.m_OnStreamStarted);
            m_OnStreamReset = std::move(other.m_OnStreamReset);
            m_OnSessionReset = std::move(other.m_OnSessionReset);
            m_OnStopped = std::move(other.m_OnStopped);

            /* Deprecated - Use m_OnConnectionSuccess and m_OnConnectionFailure */
            m_OnConnectionComplete = std::move(other.m_OnConnectionComplete);
            /* Deprecated - Use m_OnMessageReceived */
            m_OnDataReceive = std::move(other.m_OnDataReceive);
            /* Deprecated - Use m_OnStreamStarted */
            m_OnStreamStart = std::move(other.m_OnStreamStart);

            m_secure_tunnel = other.m_secure_tunnel;

            other.m_secure_tunnel = nullptr;
        }

        SecureTunnel &SecureTunnel::operator=(SecureTunnel &&other) noexcept
        {
            if (this != &other)
            {
                this->~SecureTunnel();

                m_OnConnectionSuccess = std::move(other.m_OnConnectionSuccess);
                m_OnConnectionFailure = std::move(other.m_OnConnectionFailure);
                m_OnConnectionShutdown = std::move(other.m_OnConnectionShutdown);
                m_OnSendDataComplete = std::move(other.m_OnSendDataComplete);
                m_OnMessageReceived = std::move(other.m_OnMessageReceived);
                m_OnStreamStarted = std::move(other.m_OnStreamStarted);
                m_OnStreamReset = std::move(other.m_OnStreamReset);
                m_OnSessionReset = std::move(other.m_OnSessionReset);
                m_OnStopped = std::move(other.m_OnStopped);

                /* Deprecated - Use m_OnConnectionSuccess and m_OnConnectionFailure */
                m_OnConnectionComplete = std::move(other.m_OnConnectionComplete);
                /* Deprecated - Use m_OnMessageReceived */
                m_OnDataReceive = std::move(other.m_OnDataReceive);
                /* Deprecated - Use m_OnStreamStarted */
                m_OnStreamStart = std::move(other.m_OnStreamStart);

                m_secure_tunnel = other.m_secure_tunnel;

                other.m_secure_tunnel = nullptr;
            }

            return *this;
        }

        bool SecureTunnel::IsValid() { return m_secure_tunnel ? true : false; }

        int SecureTunnel::Start()
        {
            // if (m_selfRef == nullptr)
            // {
            //     m_selfRef = this->getptr();
            // }
            return aws_secure_tunnel_start(m_secure_tunnel);
        }

        int SecureTunnel::Stop() { return aws_secure_tunnel_stop(m_secure_tunnel); }

        /* Deprecated - Use Start() */
        int SecureTunnel::Connect() { return Start(); }

        /* Deprecated - Use Stop() */
        int SecureTunnel::Close() { return Stop(); }

        /* Deprecated - Use SendMessage() */
        int SecureTunnel::SendData(const Crt::ByteCursor &data)
        {
            // return SendData("", data);
            std::shared_ptr<Message> message = std::make_shared<Message>(data);
            return SendMessage(message);
        }

        int SecureTunnel::SendMessage(std::shared_ptr<Message> messageOptions) noexcept
        {
            if (messageOptions == nullptr)
            {
                return AWS_OP_ERR;
            }

            aws_secure_tunnel_message_view message;
            messageOptions->initializeRawOptions(message);
            return aws_secure_tunnel_send_message(m_secure_tunnel, &message);
        }

        int SecureTunnel::SendStreamStart() { return SendStreamStart(""); }
        int SecureTunnel::SendStreamStart(std::string serviceId)
        {
            struct aws_byte_cursor service_id_cur;
            AWS_ZERO_STRUCT(service_id_cur);
            if (serviceId.length() > 0)
            {
                service_id_cur = aws_byte_cursor_from_c_str(serviceId.c_str());
            }
            return SendStreamStart(service_id_cur);
        }
        int SecureTunnel::SendStreamStart(Crt::ByteCursor serviceId)
        {
            struct aws_secure_tunnel_message_view messageView;
            AWS_ZERO_STRUCT(messageView);
            messageView.service_id = &serviceId;
            return aws_secure_tunnel_stream_start(m_secure_tunnel, &messageView);
        }

        int SecureTunnel::SendStreamReset() { return aws_secure_tunnel_stream_reset(m_secure_tunnel, NULL); }

        aws_secure_tunnel *SecureTunnel::GetUnderlyingHandle() { return m_secure_tunnel; }

        void SecureTunnel::s_OnConnectionComplete(
            const struct aws_secure_tunnel_connection_view *connection,
            int error_code,
            void *user_data)
        {
            auto *secureTunnel = static_cast<SecureTunnel *>(user_data);

            if (!error_code)
            {
                /* Check for full callback */
                if (secureTunnel->m_OnConnectionSuccess)
                {
                    std::shared_ptr<ConnectionData> packet =
                        std::make_shared<ConnectionData>(*connection, secureTunnel->m_allocator);
                    ConnectionSuccessEventData eventData;
                    eventData.connectionData = packet;
                    secureTunnel->m_OnConnectionSuccess(secureTunnel, eventData);
                    return;
                }

                /* Fall back on deprecated complete callback */
                if (secureTunnel->m_OnConnectionComplete)
                {
                    secureTunnel->m_OnConnectionComplete();
                }
            }
            else
            {
                if (secureTunnel->m_OnConnectionFailure)
                {
                    secureTunnel->m_OnConnectionFailure(secureTunnel, error_code);
                }
            }
        }

        void SecureTunnel::s_OnConnectionShutdown(int error_code, void *user_data)
        {
            SecureTunnel *secureTunnel = static_cast<SecureTunnel *>(user_data);
            if (secureTunnel->m_OnConnectionShutdown)
            {
                secureTunnel->m_OnConnectionShutdown();
            }
        }

        void SecureTunnel::s_OnSendDataComplete(int error_code, void *user_data)
        {
            SecureTunnel *secureTunnel = static_cast<SecureTunnel *>(user_data);
            if (secureTunnel->m_OnSendDataComplete)
            {
                secureTunnel->m_OnSendDataComplete(error_code);
            }
        }

        void SecureTunnel::s_OnMessageReceived(const struct aws_secure_tunnel_message_view *message, void *user_data)
        {
            SecureTunnel *secureTunnel = static_cast<SecureTunnel *>(user_data);
            if (secureTunnel != nullptr)
            {
                if (message != NULL)
                {
                    /* V2 Protocol API */
                    if (secureTunnel->m_OnMessageReceived != nullptr)
                    {
                        std::shared_ptr<Message> packet =
                            std::make_shared<Message>(*message, secureTunnel->m_allocator);
                        MessageReceivedEventData eventData;
                        eventData.message = packet;
                        secureTunnel->m_OnMessageReceived(secureTunnel, eventData);
                        return;
                    }

                    /* V1 Protocol API */
                    if (secureTunnel->m_OnDataReceive != nullptr)
                    {
                        /*
                         * Old API (V1) expects an aws_byte_buf. Temporarily creating one from an aws_byte_cursor. The
                         * data will be managed syncronous here with the expectation that the user copies what they
                         * need as it is cleared as soon as this function completes
                         */
                        struct aws_byte_buf payload_buf;
                        AWS_ZERO_STRUCT(payload_buf);
                        payload_buf.allocator = NULL;
                        payload_buf.buffer = message->payload->ptr;
                        payload_buf.len = message->payload->len;
                        secureTunnel->m_OnDataReceive(payload_buf);
                        return;
                    }
                }
                else
                {
                    AWS_LOGF_ERROR(AWS_LS_IOTDEVICE_SECURE_TUNNELING, "Failed to access message view.");
                }
            }
        }

        void SecureTunnel::s_OnStreamStarted(
            const struct aws_secure_tunnel_message_view *message,
            int error_code,
            void *user_data)
        {
            SecureTunnel *secureTunnel = static_cast<SecureTunnel *>(user_data);
            if (!error_code)
            {
                if (secureTunnel->m_OnStreamStarted)
                {
                    std::shared_ptr<StreamStartedData> packet =
                        std::make_shared<StreamStartedData>(*message, secureTunnel->m_allocator);
                    StreamStartedEventData eventData;
                    eventData.streamStartedData = packet;
                    secureTunnel->m_OnStreamStarted(secureTunnel, error_code, eventData);
                    return;
                }

                /* Fall back on deprecated stream start callback */
                if (secureTunnel->m_OnStreamStart)
                {
                    secureTunnel->m_OnStreamStart();
                }
            }
        }

        void SecureTunnel::s_OnStreamReset(
            const struct aws_secure_tunnel_message_view *message,
            int error_code,
            void *user_data)
        {
            SecureTunnel *secureTunnel = static_cast<SecureTunnel *>(user_data);
            if (secureTunnel->m_OnStreamReset)
            {
                secureTunnel->m_OnStreamReset();
            }
        }

        void SecureTunnel::s_OnSessionReset(void *user_data)
        {
            SecureTunnel *secureTunnel = static_cast<SecureTunnel *>(user_data);
            if (secureTunnel->m_OnSessionReset)
            {
                secureTunnel->m_OnSessionReset();
            }
        }

        void SecureTunnel::s_OnStopped(void *user_data)
        {
            SecureTunnel *secureTunnel = static_cast<SecureTunnel *>(user_data);
            secureTunnel->m_selfRef = nullptr;
            if (secureTunnel->m_OnStopped)
            {
                secureTunnel->m_OnStopped(secureTunnel);
            }
        }

        void SecureTunnel::Shutdown() { Stop(); }
    } // namespace Iotsecuretunneling
} // namespace Aws
