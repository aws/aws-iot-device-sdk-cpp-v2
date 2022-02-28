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
            m_OnConnectionComplete = onConnectionComplete;
            return *this;
        }

        SecureTunnelBuilder &SecureTunnelBuilder::WithOnConnectionShutdown(OnConnectionShutdown onConnectionShutdown)
        {
            m_OnConnectionShutdown = onConnectionShutdown;
            return *this;
        }

        SecureTunnelBuilder &SecureTunnelBuilder::WithOnSendDataComplete(OnSendDataComplete onSendDataComplete)
        {
            m_OnSendDataComplete = onSendDataComplete;
            return *this;
        }

        SecureTunnelBuilder &SecureTunnelBuilder::WithOnDataReceive(OnDataReceive onDataReceive)
        {
            m_OnDataReceive = onDataReceive;
            return *this;
        }

        SecureTunnelBuilder &SecureTunnelBuilder::WithOnStreamStart(OnStreamStart onStreamStart)
        {
            m_OnStreamStart = onStreamStart;
            return *this;
        }

        SecureTunnelBuilder &SecureTunnelBuilder::WithOnStreamReset(OnStreamReset onStreamReset)
        {
            m_OnStreamReset = onStreamReset;
            return *this;
        }

        SecureTunnelBuilder &SecureTunnelBuilder::WithOnSessionReset(OnSessionReset onSessionReset)
        {
            m_OnSessionReset = onSessionReset;
            return *this;
        }

        std::shared_ptr<SecureTunnel> SecureTunnelBuilder::Build() noexcept
        {
            auto tunnel = std::shared_ptr<SecureTunnel>(new SecureTunnel(
                m_allocator,
                m_clientBootstrap,
                m_socketOptions,
                m_accessToken,
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
            m_OnConnectionComplete = onConnectionComplete;
            m_OnConnectionShutdown = onConnectionShutdown;
            m_OnSendDataComplete = onSendDataComplete;
            m_OnDataReceive = onDataReceive;
            m_OnStreamStart = onStreamStart;
            m_OnStreamReset = onStreamReset;
            m_OnSessionReset = onSessionReset;

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

            config.on_connection_complete = s_OnConnectionComplete;
            config.on_connection_shutdown = s_OnConnectionShutdown;
            config.on_send_data_complete = s_OnSendDataComplete;
            config.on_data_receive = s_OnDataReceive;
            config.on_stream_start = s_OnStreamStart;
            config.on_stream_reset = s_OnStreamReset;
            config.on_session_reset = s_OnSessionReset;

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
            m_OnConnectionComplete = other.m_OnConnectionComplete;
            m_OnConnectionShutdown = other.m_OnConnectionShutdown;
            m_OnSendDataComplete = other.m_OnSendDataComplete;
            m_OnDataReceive = other.m_OnDataReceive;
            m_OnStreamStart = other.m_OnStreamStart;
            m_OnStreamReset = other.m_OnStreamReset;
            m_OnSessionReset = other.m_OnSessionReset;

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

                m_OnConnectionComplete = other.m_OnConnectionComplete;
                m_OnConnectionShutdown = other.m_OnConnectionShutdown;
                m_OnSendDataComplete = other.m_OnSendDataComplete;
                m_OnDataReceive = other.m_OnDataReceive;
                m_OnStreamStart = other.m_OnStreamStart;
                m_OnStreamReset = other.m_OnStreamReset;
                m_OnSessionReset = other.m_OnSessionReset;

                m_secure_tunnel = other.m_secure_tunnel;

                other.m_secure_tunnel = nullptr;
            }

            return *this;
        }

        bool SecureTunnel::IsValid() { return m_secure_tunnel ? true : false; }

        int SecureTunnel::Connect() { return aws_secure_tunnel_connect(m_secure_tunnel); }

        int SecureTunnel::Close() { return aws_secure_tunnel_close(m_secure_tunnel); }

        int SecureTunnel::SendData(const Crt::ByteCursor &data)
        {
            return aws_secure_tunnel_send_data(m_secure_tunnel, &data);
        }

        int SecureTunnel::SendStreamStart() { return aws_secure_tunnel_stream_start(m_secure_tunnel); }

        int SecureTunnel::SendStreamReset() { return aws_secure_tunnel_stream_reset(m_secure_tunnel); }

        aws_secure_tunnel *SecureTunnel::GetUnderlyingHandle() { return m_secure_tunnel; }

        void SecureTunnel::s_OnConnectionComplete(void *user_data)
        {
            auto *secureTunnel = static_cast<SecureTunnel *>(user_data);
            if (secureTunnel->m_OnConnectionComplete)
            {
                secureTunnel->m_OnConnectionComplete();
            }
        }

        void SecureTunnel::s_OnConnectionShutdown(void *user_data)
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

        void SecureTunnel::s_OnDataReceive(const struct aws_byte_buf *data, void *user_data)
        {
            auto *secureTunnel = static_cast<SecureTunnel *>(user_data);
            if (secureTunnel->m_OnDataReceive)
            {
                secureTunnel->m_OnDataReceive(*data);
            }
        }

        void SecureTunnel::s_OnStreamStart(void *user_data)
        {
            auto *secureTunnel = static_cast<SecureTunnel *>(user_data);
            if (secureTunnel->m_OnStreamStart)
            {
                secureTunnel->m_OnStreamStart();
            }
        }

        void SecureTunnel::s_OnStreamReset(void *user_data)
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

    } // namespace Iotsecuretunneling
} // namespace Aws
