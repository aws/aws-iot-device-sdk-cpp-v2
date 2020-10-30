#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/crt/io/Bootstrap.h>
#include <aws/crt/io/SocketOptions.h>
#include <aws/iotdevice/secure_tunneling.h>

int s_SecureTunnelingCallbackTest(Aws::Crt::Allocator *allocator, void *ctx);

namespace Aws
{
    namespace Crt
    {
        namespace Iot
        {
            // Client callback type definitions
            using OnSecureTunnelingConnectionComplete = std::function<void(void)>;
            using OnSecureTunnelingSendDataComplete = std::function<void(int errorCode)>;
            using OnSecureTunnelingDataReceive = std::function<void(const ByteBuf &data)>;
            using OnSecureTunnelingStreamStart = std::function<void()>;
            using OnSecureTunnelingStreamReset = std::function<void(void)>;
            using OnSecureTunnelingSessionReset = std::function<void(void)>;

            class SecureTunnel
            {
              public:
                SecureTunnel(
                    // Parameters map to aws_secure_tunneling_connection_config
                    Allocator *allocator,
                    Aws::Crt::Io::ClientBootstrap *clientBootstrap,
                    Aws::Crt::Io::SocketOptions *socketOptions,

                    std::string accessToken,
                    aws_secure_tunneling_local_proxy_mode localProxyMode,
                    std::string endpointHost,

                    OnSecureTunnelingConnectionComplete onConnectionComplete,
                    OnSecureTunnelingSendDataComplete onSendDataComplete,
                    OnSecureTunnelingDataReceive onDataReceive,
                    OnSecureTunnelingStreamStart onStreamStart,
                    OnSecureTunnelingStreamReset onStreamReset,
                    OnSecureTunnelingSessionReset onSessionReset);

                virtual ~SecureTunnel();

                int Connect();

                int Close();

                int SendData(const ByteCursor &data);

                int SendStreamStart();

                int SendStreamReset();

                aws_secure_tunnel *GetUnderlyingHandle();

              private:
                // aws-c-iot callbacks
                static void s_OnConnectionComplete(void *user_data);
                static void s_OnSendDataComplete(int error_code, void *user_data);
                static void s_OnDataReceive(const struct aws_byte_buf *data, void *user_data);
                static void s_OnStreamStart(void *user_data);
                static void s_OnStreamReset(void *user_data);
                static void s_OnSessionReset(void *user_data);

                // Client callbacks
                OnSecureTunnelingConnectionComplete m_OnConnectionComplete;
                OnSecureTunnelingSendDataComplete m_OnSendDataComplete;
                OnSecureTunnelingDataReceive m_OnDataReceive;
                OnSecureTunnelingStreamStart m_OnStreamStart;
                OnSecureTunnelingStreamReset m_OnStreamReset;
                OnSecureTunnelingSessionReset m_OnSessionReset;

                aws_secure_tunnel *m_secure_tunnel;

                friend int ::s_SecureTunnelingCallbackTest(Aws::Crt::Allocator *allocator, void *ctx);
            };
        } // namespace Iot
    }     // namespace Crt
} // namespace Aws
