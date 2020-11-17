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

            const std::string &accessToken,
            aws_secure_tunneling_local_proxy_mode localProxyMode,
            const std::string &endpointHost,

            OnConnectionComplete onConnectionComplete,
            OnSendDataComplete onSendDataComplete,
            OnDataReceive onDataReceive,
            OnStreamStart onStreamStart,
            OnStreamReset onStreamReset,
            OnSessionReset onSessionReset)
        {
            // Client callbacks
            m_OnConnectionComplete = onConnectionComplete;
            m_OnSendDataComplete = onSendDataComplete;
            m_OnDataReceive = onDataReceive;
            m_OnStreamStart = onStreamStart;
            m_OnStreamReset = onStreamReset;
            m_OnSessionReset = onSessionReset;

            m_socketOptions = socketOptions;
            m_accessToken = accessToken;
            m_endpointHost = endpointHost;

            // Initialize aws_secure_tunneling_connection_config
            aws_secure_tunneling_connection_config config;
            AWS_ZERO_STRUCT(config);

            config.allocator = allocator;
            config.bootstrap = clientBootstrap ? clientBootstrap->GetUnderlyingHandle() : nullptr;
            config.socket_options = &m_socketOptions.GetImpl();

            config.access_token = aws_byte_cursor_from_c_str(m_accessToken.c_str());
            config.local_proxy_mode = localProxyMode;
            config.endpoint_host = aws_byte_cursor_from_c_str(m_endpointHost.c_str());

            config.on_connection_complete = s_OnConnectionComplete;
            config.on_send_data_complete = s_OnSendDataComplete;
            config.on_data_receive = s_OnDataReceive;
            config.on_stream_start = s_OnStreamStart;
            config.on_stream_reset = s_OnStreamReset;
            config.on_session_reset = s_OnSessionReset;

            config.user_data = this;

            // Create the secure tunnel
            m_secure_tunnel = aws_secure_tunnel_new(&config);
        }

        SecureTunnel::SecureTunnel(SecureTunnel &&from) noexcept
        {
            m_secure_tunnel = from.m_secure_tunnel;
            from.m_secure_tunnel = nullptr;
        }

        SecureTunnel::~SecureTunnel()
        {
            if (m_secure_tunnel)
            {
                aws_secure_tunnel_release(m_secure_tunnel);
            }
        }

        SecureTunnel &SecureTunnel::operator=(SecureTunnel &&rhs) noexcept
        {
            if (this != &rhs)
            {
                this->~SecureTunnel();

                m_secure_tunnel = rhs.m_secure_tunnel;
                rhs.m_secure_tunnel = nullptr;
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
            auto *secureTunnel = reinterpret_cast<SecureTunnel *>(user_data);
            secureTunnel->m_OnConnectionComplete();
        }

        void SecureTunnel::s_OnSendDataComplete(int error_code, void *user_data)
        {
            auto *secureTunnel = reinterpret_cast<SecureTunnel *>(user_data);
            secureTunnel->m_OnSendDataComplete(error_code);
        }

        void SecureTunnel::s_OnDataReceive(const struct aws_byte_buf *data, void *user_data)
        {
            auto *secureTunnel = reinterpret_cast<SecureTunnel *>(user_data);
            secureTunnel->m_OnDataReceive(*data);
        }

        void SecureTunnel::s_OnStreamStart(void *user_data)
        {
            auto *secureTunnel = reinterpret_cast<SecureTunnel *>(user_data);
            secureTunnel->m_OnStreamStart();
        }

        void SecureTunnel::s_OnStreamReset(void *user_data)
        {
            auto *secureTunnel = reinterpret_cast<SecureTunnel *>(user_data);
            secureTunnel->m_OnStreamReset();
        }

        void SecureTunnel::s_OnSessionReset(void *user_data)
        {
            auto *secureTunnel = reinterpret_cast<SecureTunnel *>(user_data);
            secureTunnel->m_OnSessionReset();
        }

    } // namespace Iotsecuretunneling
} // namespace Aws
