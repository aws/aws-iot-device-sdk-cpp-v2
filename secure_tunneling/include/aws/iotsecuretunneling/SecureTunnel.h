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

namespace Aws
{
    namespace Iotsecuretunneling
    {
        class SecureTunnel;

        // Client callback type definitions
        using OnConnectionComplete = std::function<void(void)>;
        using OnConnectionShutdown = std::function<void(void)>;
        using OnSendDataComplete = std::function<void(int errorCode)>;
        using OnDataReceive = std::function<void(const Crt::ByteBuf &data)>;
        using OnStreamStart = std::function<void()>;
        using OnStreamReset = std::function<void(void)>;
        using OnSessionReset = std::function<void(void)>;

        class AWS_IOTSECURETUNNELING_API SecureTunnelBuilder final
        {
          public:
            /**
             * Constructor arguments are the minimum required to create a secure tunnel
             */
            SecureTunnelBuilder(
                Crt::Allocator *allocator,                        // Should out live this object
                Aws::Crt::Io::ClientBootstrap &clientBootstrap,   // Should out live this object
                const Aws::Crt::Io::SocketOptions &socketOptions, // Make a copy and save in this object
                const std::string &accessToken,                   // Make a copy and save in this object
                aws_secure_tunneling_local_proxy_mode localProxyMode,
                const std::string &endpointHost); // Make a copy and save in this object

            /**
             * Constructor arguments are the minimum required to create a secure tunnel
             */
            SecureTunnelBuilder(
                Crt::Allocator *allocator,                        // Should out live this object
                const Aws::Crt::Io::SocketOptions &socketOptions, // Make a copy and save in this object
                const std::string &accessToken,                   // Make a copy and save in this object
                aws_secure_tunneling_local_proxy_mode localProxyMode,
                const std::string &endpointHost); // Make a copy and save in this object

            /**
             * Optional members
             */
            SecureTunnelBuilder &WithRootCa(const std::string &rootCa);
            SecureTunnelBuilder &WithHttpClientConnectionProxyOptions(
                const Aws::Crt::Http::HttpClientConnectionProxyOptions &httpClientConnectionProxyOptions);
            SecureTunnelBuilder &WithOnConnectionComplete(OnConnectionComplete onConnectionComplete);
            SecureTunnelBuilder &WithOnConnectionShutdown(OnConnectionShutdown onConnectionShutdown);
            SecureTunnelBuilder &WithOnSendDataComplete(OnSendDataComplete onSendDataComplete);
            SecureTunnelBuilder &WithOnDataReceive(OnDataReceive onDataReceive);
            SecureTunnelBuilder &WithOnStreamStart(OnStreamStart onStreamStart);
            SecureTunnelBuilder &WithOnStreamReset(OnStreamReset onStreamReset);
            SecureTunnelBuilder &WithOnSessionReset(OnSessionReset onSessionReset);

            /**
             * Will return a shared pointer to a new SecureTunnel that countains a
             * new aws_secure_tunnel that is generated using the set members of SecureTunnelBuilder.
             * On failure, the shared_ptr will contain a nullptr, call Aws::Crt::LastErrorOrUnknown(); to get the reason
             * for failure.
             */
            std::shared_ptr<SecureTunnel> Build() noexcept;

          private:
            /**
             * Required members
             */
            Crt::Allocator *m_allocator;
            Aws::Crt::Io::ClientBootstrap *m_clientBootstrap;
            Aws::Crt::Io::SocketOptions m_socketOptions;
            std::string m_accessToken;
            aws_secure_tunneling_local_proxy_mode m_localProxyMode;
            std::string m_endpointHost;

            /**
             * Optional members
             */
            std::string m_rootCa;
            Crt::Optional<Crt::Http::HttpClientConnectionProxyOptions> m_httpClientConnectionProxyOptions;

            /**
             * Callbacks
             */
            OnConnectionComplete m_OnConnectionComplete;
            OnConnectionShutdown m_OnConnectionShutdown;
            OnSendDataComplete m_OnSendDataComplete;
            OnDataReceive m_OnDataReceive;
            OnStreamStart m_OnStreamStart;
            OnStreamReset m_OnStreamReset;
            OnSessionReset m_OnSessionReset;

            friend class SecureTunnel;
        };

        class AWS_IOTSECURETUNNELING_API SecureTunnel final
        {
          public:
            SecureTunnel(
                Crt::Allocator *allocator,                        // Should out live this object
                Aws::Crt::Io::ClientBootstrap *clientBootstrap,   // Should out live this object
                const Aws::Crt::Io::SocketOptions &socketOptions, // Make a copy and save in this object

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
                Crt::Allocator *allocator,                        // Should out live this object
                const Aws::Crt::Io::SocketOptions &socketOptions, // Make a copy and save in this object

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
            SecureTunnel(const SecureTunnel &) = delete;
            SecureTunnel(SecureTunnel &&) noexcept;

            virtual ~SecureTunnel();

            SecureTunnel &operator=(const SecureTunnel &) = delete;
            SecureTunnel &operator=(SecureTunnel &&) noexcept;

            bool IsValid();

            int Connect();

            int Close();

            int SendData(const Crt::ByteCursor &data);

            int SendStreamStart();

            int SendStreamReset();

            aws_secure_tunnel *GetUnderlyingHandle();

          private:
            /**
             * This constructor is used with SecureTunnelBuilder and should be modified when members are added or
             * removed.
             */
            SecureTunnel(
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
                OnSessionReset onSessionReset);

            // aws-c-iot callbacks
            static void s_OnConnectionComplete(void *user_data);
            static void s_OnConnectionShutdown(void *user_data);
            static void s_OnSendDataComplete(int error_code, void *user_data);
            static void s_OnDataReceive(const struct aws_byte_buf *data, void *user_data);
            static void s_OnStreamStart(void *user_data);
            static void s_OnStreamReset(void *user_data);
            static void s_OnSessionReset(void *user_data);

            // Client callbacks
            OnConnectionComplete m_OnConnectionComplete;
            OnConnectionShutdown m_OnConnectionShutdown;
            OnSendDataComplete m_OnSendDataComplete;
            OnDataReceive m_OnDataReceive;
            OnStreamStart m_OnStreamStart;
            OnStreamReset m_OnStreamReset;
            OnSessionReset m_OnSessionReset;
            aws_secure_tunnel *m_secure_tunnel;

            friend class SecureTunnelBuilder;
        };
    } // namespace Iotsecuretunneling
} // namespace Aws
