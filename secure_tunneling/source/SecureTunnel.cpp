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
        Message::Message(const aws_secure_tunnel_message_view &message, Crt::Allocator *allocator) noexcept
            : m_allocator(allocator)
        {
        }

        Message::~Message() { aws_byte_buf_clean_up(&m_payloadStorage); }

        SecureTunnelBuilder::SecureTunnelBuilder(
            Crt::Allocator *allocator,                        // Should out live this object
            Aws::Crt::Io::ClientBootstrap &clientBootstrap,   // Should out live this object
            const Aws::Crt::Io::SocketOptions &socketOptions, // Make a copy and save in this object
            const std::string &accessToken,                   // Make a copy and save in this object
            aws_secure_tunneling_local_proxy_mode localProxyMode,
            const std::string &endpointHost) // Make a copy and save in this object
            : m_allocator(allocator), m_clientBootstrap(&clientBootstrap), m_socketOptions(socketOptions),
              m_accessToken(accessToken), m_localProxyMode(localProxyMode), m_endpointHost(endpointHost), m_rootCa(""),
              m_httpClientConnectionProxyOptions(), m_OnConnectionComplete(), m_OnConnectionShutdown(),
              m_OnSendDataComplete(), m_OnDataReceive(), m_OnStreamStart(), m_OnStreamReset(), m_OnSessionReset()
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
              m_OnConnectionComplete(), m_OnConnectionShutdown(), m_OnSendDataComplete(), m_OnDataReceive(),
              m_OnStreamStart(), m_OnStreamReset(), m_OnSessionReset()
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

        SecureTunnelBuilder &SecureTunnelBuilder::WithOnConnectionComplete(OnConnectionComplete onConnectionComplete)
        {
            m_OnConnectionComplete = std::move(onConnectionComplete);
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

        SecureTunnelBuilder &SecureTunnelBuilder::WithOnDataReceive(OnDataReceive onDataReceive)
        {
            m_OnDataReceive = std::move(onDataReceive);
            return *this;
        }

        SecureTunnelBuilder &SecureTunnelBuilder::WithOnStreamStart(OnStreamStart onStreamStart)
        {
            m_OnStreamStart = std::move(onStreamStart);
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

        SecureTunnelBuilder &SecureTunnelBuilder::WithClientToken(const std::string &clientToken)
        {
            m_clientToken = clientToken;
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
                m_OnConnectionComplete,
                m_OnConnectionShutdown,
                m_OnSendDataComplete,
                m_OnDataReceive,
                m_OnStreamStart,
                m_OnStreamReset,
                m_OnSessionReset));

            if (tunnel->m_secure_tunnel == nullptr)
            {
                return nullptr;
            }

            return tunnel;
        }

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

            OnConnectionComplete onConnectionComplete,
            OnConnectionShutdown onConnectionShutdown,
            OnSendDataComplete onSendDataComplete,
            OnDataReceive onDataReceive,
            OnStreamStart onStreamStart,
            OnStreamReset onStreamReset,
            OnSessionReset onSessionReset)
        {
            // Client callbacks
            m_OnConnectionComplete = std::move(onConnectionComplete);
            m_OnConnectionShutdown = std::move(onConnectionShutdown);
            m_OnSendDataComplete = std::move(onSendDataComplete);
            m_OnDataReceive = std::move(onDataReceive);
            m_OnStreamStart = std::move(onStreamStart);
            m_OnStreamReset = std::move(onStreamReset);
            m_OnSessionReset = std::move(onSessionReset);

            // Initialize aws_secure_tunnel_options
            aws_secure_tunnel_options config;
            AWS_ZERO_STRUCT(config);

            config.allocator = allocator;
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

            config.on_message_received = s_OnMessageReceived;
            config.on_connection_complete = s_OnConnectionComplete;
            config.on_connection_shutdown = s_OnConnectionShutdown;
            config.on_send_data_complete = s_OnSendDataComplete;
            // config.on_data_receive = s_OnDataReceive;
            config.on_stream_start = s_OnStreamStart;
            config.on_stream_reset = s_OnStreamReset;
            config.on_session_reset = s_OnSessionReset;
            config.on_termination_complete = s_OnTerminationComplete;

            config.user_data = this;

            aws_http_proxy_options temp;
            AWS_ZERO_STRUCT(temp);
            if (httpClientConnectionProxyOptions != NULL)
            {
                httpClientConnectionProxyOptions->InitializeRawProxyOptions(temp);
                config.http_proxy_options = &temp;
            }

            // Create the secure tunnel
            m_secure_tunnel = aws_secure_tunnel_new(&config);
        }

        /**
         * Should be deprecated when possible.
         * SecureTunnelBuilder::Build() should be used to generate new SecureTunnels
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
                  onConnectionComplete,
                  onConnectionShutdown,
                  onSendDataComplete,
                  onDataReceive,
                  onStreamStart,
                  onStreamReset,
                  onSessionReset)
        {
        }

        /**
         * Should be deprecated when possible.
         * SecureTunnelBuilder::Build() should be used to generate new SecureTunnels
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
                  onConnectionComplete,
                  onConnectionShutdown,
                  onSendDataComplete,
                  onDataReceive,
                  onStreamStart,
                  onStreamReset,
                  onSessionReset)
        {
        }

        SecureTunnel::SecureTunnel(SecureTunnel &&other) noexcept
        {
            m_OnConnectionComplete = std::move(other.m_OnConnectionComplete);
            m_OnConnectionShutdown = std::move(other.m_OnConnectionShutdown);
            m_OnSendDataComplete = std::move(other.m_OnSendDataComplete);
            m_OnDataReceive = std::move(other.m_OnDataReceive);
            m_OnStreamStart = std::move(other.m_OnStreamStart);
            m_OnStreamReset = std::move(other.m_OnStreamReset);
            m_OnSessionReset = std::move(other.m_OnSessionReset);

            m_TerminationComplete = std::move(other.m_TerminationComplete);

            m_secure_tunnel = other.m_secure_tunnel;

            other.m_secure_tunnel = nullptr;
        }

        SecureTunnel::~SecureTunnel()
        {
            if (m_secure_tunnel)
            {
                aws_secure_tunnel_release(m_secure_tunnel);
            }
        }

        SecureTunnel &SecureTunnel::operator=(SecureTunnel &&other) noexcept
        {
            if (this != &other)
            {
                this->~SecureTunnel();

                m_OnConnectionComplete = std::move(other.m_OnConnectionComplete);
                m_OnConnectionShutdown = std::move(other.m_OnConnectionShutdown);
                m_OnSendDataComplete = std::move(other.m_OnSendDataComplete);
                m_OnDataReceive = std::move(other.m_OnDataReceive);
                m_OnStreamStart = std::move(other.m_OnStreamStart);
                m_OnStreamReset = std::move(other.m_OnStreamReset);
                m_OnSessionReset = std::move(other.m_OnSessionReset);

                m_TerminationComplete = std::move(other.m_TerminationComplete);

                m_secure_tunnel = other.m_secure_tunnel;

                other.m_secure_tunnel = nullptr;
            }

            return *this;
        }

        bool SecureTunnel::IsValid() { return m_secure_tunnel ? true : false; }

        // Steve TODO Deprecate
        int SecureTunnel::Connect() { return aws_secure_tunnel_start(m_secure_tunnel); }
        int SecureTunnel::Start() { return aws_secure_tunnel_start(m_secure_tunnel); }

        // Steve TODO Deprecate
        int SecureTunnel::Close() { return aws_secure_tunnel_stop(m_secure_tunnel); }
        int SecureTunnel::Stop() { return aws_secure_tunnel_stop(m_secure_tunnel); }

        int SecureTunnel::SendData(const Crt::ByteCursor &data) { return SendData("", data); }
        int SecureTunnel::SendData(std::string serviceId, const Crt::ByteCursor &data)
        {
            struct aws_secure_tunnel_message_view messageView;
            AWS_ZERO_STRUCT(messageView);
            messageView.service_id = aws_byte_cursor_from_c_str(serviceId.c_str());
            messageView.payload = data;
            return aws_secure_tunnel_send_message(m_secure_tunnel, &messageView);
        }

        // Steve TODO
        int SecureTunnel::SendStreamStart() { return SendStreamStart(""); }
        int SecureTunnel::SendStreamStart(std::string serviceId)
        {
            struct aws_secure_tunnel_message_view messageView;
            AWS_ZERO_STRUCT(messageView);
            messageView.service_id = aws_byte_cursor_from_c_str(serviceId.c_str());
            return aws_secure_tunnel_stream_start(m_secure_tunnel, &messageView);
        }

        // Steve TODO
        int SecureTunnel::SendStreamReset() { return aws_secure_tunnel_stream_reset(m_secure_tunnel, NULL); }

        aws_secure_tunnel *SecureTunnel::GetUnderlyingHandle() { return m_secure_tunnel; }

        void SecureTunnel::s_OnConnectionComplete(int error_code, void *user_data)
        {
            auto *secureTunnel = static_cast<SecureTunnel *>(user_data);
            if (!error_code)
            {
                if (secureTunnel->m_OnConnectionComplete)
                {
                    secureTunnel->m_OnConnectionComplete();
                }
            }
        }

        void SecureTunnel::s_OnConnectionShutdown(int error_code, void *user_data)
        {
            auto *secureTunnel = static_cast<SecureTunnel *>(user_data);
            if (secureTunnel->m_OnConnectionShutdown)
            {
                secureTunnel->m_OnConnectionShutdown();
            }
        }

        void SecureTunnel::s_OnSendDataComplete(int error_code, void *user_data)
        {
            auto *secureTunnel = static_cast<SecureTunnel *>(user_data);
            if (secureTunnel->m_OnSendDataComplete)
            {
                secureTunnel->m_OnSendDataComplete(error_code);
            }
        }

        void SecureTunnel::s_OnMessageReceived(const struct aws_secure_tunnel_message_view *message, void *user_data)
        {
            auto *secureTunnel = static_cast<SecureTunnel *>(user_data);
            /* V1 Protocol */
            if (secureTunnel->m_OnDataReceive)
            {
                /*
                 * Old API expects an aws_byte_buf. Temporarily creating one from an aws_byte_cursor. The data will be
                 * managed syncronous here with the expectation that the user copies what they need as it is cleared as
                 * soon as this function completes
                 */
                struct aws_byte_buf payload_buf;
                AWS_ZERO_STRUCT(payload_buf);
                payload_buf.allocator = NULL;
                payload_buf.buffer = message->payload.ptr;
                payload_buf.len = message->payload.len;
                secureTunnel->m_OnDataReceive(payload_buf);
                return;
            }

            if (secureTunnel->m_OnMessageReceived)
            {
                secureTunnel->m_OnMessageReceived(message->service_id, message->payload);
            }
        }

        void SecureTunnel::s_OnStreamStart(
            const struct aws_secure_tunnel_message_view *message,
            int error_code,
            void *user_data)
        {
            auto *secureTunnel = static_cast<SecureTunnel *>(user_data);
            if (secureTunnel->m_OnStreamStart)
            {
                secureTunnel->m_OnStreamStart();
            }
        }

        void SecureTunnel::s_OnStreamReset(
            const struct aws_secure_tunnel_message_view *message,
            int error_code,
            void *user_data)
        {
            auto *secureTunnel = static_cast<SecureTunnel *>(user_data);
            if (secureTunnel->m_OnStreamReset)
            {
                secureTunnel->m_OnStreamReset();
            }
        }

        void SecureTunnel::s_OnSessionReset(void *user_data)
        {
            auto *secureTunnel = static_cast<SecureTunnel *>(user_data);
            if (secureTunnel->m_OnSessionReset)
            {
                secureTunnel->m_OnSessionReset();
            }
        }

        void SecureTunnel::s_OnTerminationComplete(void *user_data)
        {
            auto *secureTunnel = static_cast<SecureTunnel *>(user_data);
            secureTunnel->OnTerminationComplete();
        }

        void SecureTunnel::OnTerminationComplete() { m_TerminationComplete.set_value(); }

        void SecureTunnel::Shutdown()
        {
            Close();
            aws_secure_tunnel_release(m_secure_tunnel);
            m_secure_tunnel = nullptr;

            m_TerminationComplete.get_future().wait();
        }
    } // namespace Iotsecuretunneling
} // namespace Aws
