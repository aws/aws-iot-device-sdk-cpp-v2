#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/crt/io/Bootstrap.h>
#include <aws/crt/io/SocketOptions.h>
#include <aws/iotdevice/secure_tunneling.h>
#include <aws/iotsecuretunneling/Exports.h>

int s_SecureTunnelingCallbackTest(Aws::Crt::Allocator *allocator, void *ctx);

namespace Aws
{
    namespace Iotsecuretunneling
    {
        class AWS_IOTSECURETUNNELING_API DeviceApiHandle final
        {
          public:
            DeviceApiHandle(Crt::Allocator *allocator) noexcept;
            ~DeviceApiHandle();
            DeviceApiHandle(const DeviceApiHandle &) = delete;
            DeviceApiHandle(DeviceApiHandle &&) = delete;
            DeviceApiHandle &operator=(const DeviceApiHandle &) = delete;
            DeviceApiHandle &operator=(DeviceApiHandle &&) = delete;
        };

        // Client callback type definitions
        using OnConnectionComplete = std::function<void(void)>;
        using OnSendDataComplete = std::function<void(int errorCode)>;
        using OnDataReceive = std::function<void(const Crt::ByteBuf &data)>;
        using OnStreamStart = std::function<void()>;
        using OnStreamReset = std::function<void(void)>;
        using OnSessionReset = std::function<void(void)>;

        class AWS_IOTSECURETUNNELING_API SecureTunnel final
        {
          public:
            SecureTunnel(
                // Parameters map to aws_secure_tunneling_connection_config
                Crt::Allocator *allocator,
                Aws::Crt::Io::ClientBootstrap *clientBootstrap,
                Aws::Crt::Io::SocketOptions *socketOptions,

                std::string accessToken,
                aws_secure_tunneling_local_proxy_mode localProxyMode,
                std::string endpointHost,

                OnConnectionComplete onConnectionComplete,
                OnSendDataComplete onSendDataComplete,
                OnDataReceive onDataReceive,
                OnStreamStart onStreamStart,
                OnStreamReset onStreamReset,
                OnSessionReset onSessionReset);
            SecureTunnel(const SecureTunnel &) = delete;
            SecureTunnel(SecureTunnel &&) noexcept;

            virtual ~SecureTunnel();

            SecureTunnel &operator=(const SecureTunnel &) = delete;
            SecureTunnel &operator=(SecureTunnel &&) noexcept;

            int Connect();

            int Close();

            int SendData(const Crt::ByteCursor &data);

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
            OnConnectionComplete m_OnConnectionComplete;
            OnSendDataComplete m_OnSendDataComplete;
            OnDataReceive m_OnDataReceive;
            OnStreamStart m_OnStreamStart;
            OnStreamReset m_OnStreamReset;
            OnSessionReset m_OnSessionReset;

            aws_secure_tunnel *m_secure_tunnel;

            friend int ::s_SecureTunnelingCallbackTest(Aws::Crt::Allocator *allocator, void *ctx);
        };
    } // namespace Iotsecuretunneling
} // namespace Aws
