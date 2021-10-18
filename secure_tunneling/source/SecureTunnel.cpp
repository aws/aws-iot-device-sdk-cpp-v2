/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/iotsecuretunneling/SecureTunnel.h>

namespace Aws
{
    namespace Iotsecuretunneling
    {
        SecureTunnel::SecureTunnel(
            Crt::Allocator *allocator,
            Aws::Crt::Io::ClientBootstrap *clientBootstrap,
            const Aws::Crt::Io::SocketOptions &socketOptions,
            Aws::Crt::Http::HttpClientConnectionProxyOptions *httpClientConnectionProxyOptions,

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
            // Client callbacks
            m_OnConnectionComplete = onConnectionComplete;
            m_OnConnectionShutdown = onConnectionShutdown;
            m_OnSendDataComplete = onSendDataComplete;
            m_OnDataReceive = onDataReceive;
            m_OnStreamStart = onStreamStart;
            m_OnStreamReset = onStreamReset;
            m_OnSessionReset = onSessionReset;

            m_socketOptions = socketOptions;
            m_accessToken = accessToken;
            m_endpointHost = endpointHost;
            m_rootCa = rootCa;

            // Initialize aws_secure_tunneling_connection_config
            aws_secure_tunnel_options
                config; // aws_secure_tunneling_connection_config changed to aws_secure_tunnel_options
            AWS_ZERO_STRUCT(config);

            config.allocator = allocator;
            config.bootstrap = clientBootstrap ? clientBootstrap->GetUnderlyingHandle() : nullptr;
            config.socket_options = &m_socketOptions.GetImpl();

            config.access_token = aws_byte_cursor_from_c_str(m_accessToken.c_str());
            config.local_proxy_mode = localProxyMode;
            config.endpoint_host = aws_byte_cursor_from_c_str(m_endpointHost.c_str());
            config.root_ca = m_rootCa.c_str();

            config.on_connection_complete = s_OnConnectionComplete;
            config.on_connection_shutdown = s_OnConnectionShutdown;
            config.on_send_data_complete = s_OnSendDataComplete;
            config.on_data_receive = s_OnDataReceive;
            config.on_stream_start = s_OnStreamStart;
            config.on_stream_reset = s_OnStreamReset;
            config.on_session_reset = s_OnSessionReset;

            config.user_data = this;

            if (httpClientConnectionProxyOptions != NULL)
            {
                aws_http_proxy_options temp;
                httpClientConnectionProxyOptions->InitializeRawProxyOptions(temp);
                config.http_proxy_options = &temp;
            }

            m_aws_secure_tunnel_options_storage = aws_secure_tunnel_options_storage_new(&config);

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
            : SecureTunnel(
                  allocator,
                  clientBootstrap,
                  socketOptions,
                  nullptr,
                  accessToken,
                  localProxyMode,
                  endpointHost,
                  rootCa,
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
                aws_secure_tunnel_options_storage_destroy(m_aws_secure_tunnel_options_storage);
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
