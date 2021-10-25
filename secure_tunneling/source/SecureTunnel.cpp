/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/iotsecuretunneling/SecureTunnel.h>

namespace Aws
{
    namespace Iotsecuretunneling
    {
        SecureTunnelConfig::SecureTunnelConfig(int lastError) noexcept : m_lastError(lastError) {}

        SecureTunnelConfig SecureTunnelConfig::CreateInvalid(int lastError) noexcept
        {
            return SecureTunnelConfig(lastError);
        }

        SecureTunnelConfigBuilder::SecureTunnelConfigBuilder() : m_lastError(AWS_ERROR_INVALID_STATE) {}

        SecureTunnelConfigBuilder::SecureTunnelConfigBuilder(
            Crt::Allocator *allocator,                        // Should out live this object          req
            Aws::Crt::Io::ClientBootstrap *clientBootstrap,   // Should out live this object          req
            const Aws::Crt::Io::SocketOptions &socketOptions, // Make a copy and save in this object  req
            const std::string &accessToken, // Make a copy and save in this object                    req
            aws_secure_tunneling_local_proxy_mode localProxyMode, //                                  req
            const std::string &endpointHost) // Make a copy and save in this object                  req
            : m_allocator(allocator), m_clientBootstrap(clientBootstrap), m_socketOptions(socketOptions),
              m_accessToken(accessToken), m_localProxyMode(localProxyMode), m_endpointHost(endpointHost), m_lastError(0)
        {
        }

        SecureTunnelConfigBuilder &SecureTunnelConfigBuilder::WithRootCa(const std::string &rootCa)
        {
            m_rootCa = rootCa;
            return *this;
        }
        SecureTunnelConfigBuilder &SecureTunnelConfigBuilder::WithHttpClientConnectionProxyOptions(
            Aws::Crt::Http::HttpClientConnectionProxyOptions *httpClientConnectionProxyOptions)
        {
            m_httpClientConnectionProxyOptions = httpClientConnectionProxyOptions;
            return *this;
        }
        SecureTunnelConfigBuilder &SecureTunnelConfigBuilder::WithOnConnectionComplete(
            OnConnectionComplete onConnectionComplete)
        {
            m_OnConnectionComplete = onConnectionComplete;
            return *this;
        }
        SecureTunnelConfigBuilder &SecureTunnelConfigBuilder::WithOnConnectionShutdown(
            OnConnectionShutdown onConnectionShutdown)
        {
            m_OnConnectionShutdown = onConnectionShutdown;
            return *this;
        }
        SecureTunnelConfigBuilder &SecureTunnelConfigBuilder::WithOnSendDataComplete(
            OnSendDataComplete onSendDataComplete)
        {
            m_OnSendDataComplete = onSendDataComplete;
            return *this;
        }
        SecureTunnelConfigBuilder &SecureTunnelConfigBuilder::WithOnDataReceive(OnDataReceive onDataReceive)
        {
            m_OnDataReceive = onDataReceive;
            return *this;
        }
        SecureTunnelConfigBuilder &SecureTunnelConfigBuilder::WithOnStreamStart(OnStreamStart onStreamStart)
        {
            m_OnStreamStart = onStreamStart;
            return *this;
        }
        SecureTunnelConfigBuilder &SecureTunnelConfigBuilder::WithOnStreamReset(OnStreamReset onStreamReset)
        {
            m_OnStreamReset = onStreamReset;
            return *this;
        }
        SecureTunnelConfigBuilder &SecureTunnelConfigBuilder::WithOnSessionReset(OnSessionReset onSessionReset)
        {
            m_OnSessionReset = onSessionReset;
            return *this;
        }

        SecureTunnelConfig SecureTunnelConfigBuilder::Build() noexcept
        {
            // TODO Create the builder output
            if (m_lastError != 0)
            {
                return SecureTunnelConfig::CreateInvalid(m_lastError);
            }

            auto config = SecureTunnelConfig(
                m_allocator, m_clientBootstrap, m_socketOptions, m_accessToken, m_localProxyMode, m_endpointHost);

            config.m_rootCa = m_rootCa;
            config.m_httpClientConnectionProxyOptions = m_httpClientConnectionProxyOptions;

            config.m_OnConnectionComplete = m_OnConnectionComplete;
            config.m_OnConnectionShutdown = m_OnConnectionShutdown;
            config.m_OnSendDataComplete = m_OnSendDataComplete;
            config.m_OnDataReceive = m_OnDataReceive;
            config.m_OnStreamStart = m_OnStreamStart;
            config.m_OnStreamReset = m_OnStreamReset;
            config.m_OnSessionReset = m_OnSessionReset;

            return config;
        }

        SecureTunnel::SecureTunnel(SecureTunnelConfig secureTunnelConfig)
        {
            // Client callbacks
            m_OnConnectionComplete = secureTunnelConfig.m_OnConnectionComplete;
            m_OnConnectionShutdown = secureTunnelConfig.m_OnConnectionShutdown;
            m_OnSendDataComplete = secureTunnelConfig.m_OnSendDataComplete;
            m_OnDataReceive = secureTunnelConfig.m_OnDataReceive;
            m_OnStreamStart = secureTunnelConfig.m_OnStreamStart;
            m_OnStreamReset = secureTunnelConfig.m_OnStreamReset;
            m_OnSessionReset = secureTunnelConfig.m_OnSessionReset;

            m_socketOptions = secureTunnelConfig.m_socketOptions;
            m_accessToken = secureTunnelConfig.m_accessToken;
            m_endpointHost = secureTunnelConfig.m_endpointHost;
            m_rootCa = secureTunnelConfig.m_rootCa;

            // Initialize aws_secure_tunnel_options
            aws_secure_tunnel_options config;
            AWS_ZERO_STRUCT(config);

            config.allocator = secureTunnelConfig.m_allocator;
            config.bootstrap = secureTunnelConfig.m_clientBootstrap
                                   ? secureTunnelConfig.m_clientBootstrap->GetUnderlyingHandle()
                                   : nullptr;
            config.socket_options = &m_socketOptions.GetImpl();

            config.access_token = aws_byte_cursor_from_c_str(m_accessToken.c_str());
            config.local_proxy_mode = secureTunnelConfig.m_localProxyMode;
            config.endpoint_host = aws_byte_cursor_from_c_str(m_endpointHost.c_str());

            if (m_rootCa.length() > 0)
            {
                config.root_ca = m_rootCa.c_str();
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
            if (secureTunnelConfig.m_httpClientConnectionProxyOptions != NULL)
            {
                secureTunnelConfig.m_httpClientConnectionProxyOptions->InitializeRawProxyOptions(temp);
                config.http_proxy_options = &temp;
            }

            // Create the secure tunnel
            m_secure_tunnel = aws_secure_tunnel_new(&config);
        }

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
        {
            auto secureTunnelConfigBuilder = SecureTunnelConfigBuilder(
                allocator, clientBootstrap, socketOptions, accessToken, localProxyMode, endpointHost);
            secureTunnelConfigBuilder.WithRootCa(rootCa);
            secureTunnelConfigBuilder.WithOnConnectionComplete(onConnectionComplete);
            secureTunnelConfigBuilder.WithOnConnectionShutdown(onConnectionShutdown);
            secureTunnelConfigBuilder.WithOnSendDataComplete(onSendDataComplete);
            secureTunnelConfigBuilder.WithOnDataReceive(onDataReceive);
            secureTunnelConfigBuilder.WithOnStreamStart(onStreamStart);
            secureTunnelConfigBuilder.WithOnStreamReset(onStreamReset);
            secureTunnelConfigBuilder.WithOnSessionReset(onSessionReset);

            SecureTunnel(secureTunnelConfigBuilder.Build());
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

            m_socketOptions = other.m_socketOptions;
            m_accessToken = std::move(other.m_accessToken);
            m_endpointHost = std::move(other.m_endpointHost);
            m_rootCa = std::move(other.m_rootCa);

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

                m_socketOptions = other.m_socketOptions;
                m_accessToken = std::move(other.m_accessToken);
                m_endpointHost = std::move(other.m_endpointHost);
                m_rootCa = std::move(other.m_rootCa);

                m_secure_tunnel = other.m_secure_tunnel;

                other.m_secure_tunnel = nullptr;
            }

            return *this;
        }

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
            secureTunnel->m_OnConnectionComplete();
        }

        void SecureTunnel::s_OnConnectionShutdown(void *user_data)
        {
            auto *secureTunnel = static_cast<SecureTunnel *>(user_data);
            secureTunnel->m_OnConnectionShutdown();
        }

        void SecureTunnel::s_OnSendDataComplete(int error_code, void *user_data)
        {
            auto *secureTunnel = static_cast<SecureTunnel *>(user_data);
            secureTunnel->m_OnSendDataComplete(error_code);
        }

        void SecureTunnel::s_OnDataReceive(const struct aws_byte_buf *data, void *user_data)
        {
            auto *secureTunnel = static_cast<SecureTunnel *>(user_data);
            secureTunnel->m_OnDataReceive(*data);
        }

        void SecureTunnel::s_OnStreamStart(void *user_data)
        {
            auto *secureTunnel = static_cast<SecureTunnel *>(user_data);
            secureTunnel->m_OnStreamStart();
        }

        void SecureTunnel::s_OnStreamReset(void *user_data)
        {
            auto *secureTunnel = static_cast<SecureTunnel *>(user_data);
            secureTunnel->m_OnStreamReset();
        }

        void SecureTunnel::s_OnSessionReset(void *user_data)
        {
            auto *secureTunnel = static_cast<SecureTunnel *>(user_data);
            secureTunnel->m_OnSessionReset();
        }

    } // namespace Iotsecuretunneling
} // namespace Aws
