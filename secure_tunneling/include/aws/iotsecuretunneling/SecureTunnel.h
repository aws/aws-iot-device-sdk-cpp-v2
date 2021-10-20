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
        // Client callback type definitions
        using OnConnectionComplete = std::function<void(void)>;
        using OnConnectionShutdown = std::function<void(void)>;
        using OnSendDataComplete = std::function<void(int errorCode)>;
        using OnDataReceive = std::function<void(const Crt::ByteBuf &data)>;
        using OnStreamStart = std::function<void()>;
        using OnStreamReset = std::function<void(void)>;
        using OnSessionReset = std::function<void(void)>;

        class AWS_IOTSECURETUNNELING_API SecureTunnelConfig final
        {
          public:
            static SecureTunnelConfig CreateInvalid(int lastError) noexcept;

            SecureTunnelConfig(
                Crt::Allocator *allocator,                        // Should out live this object
                Aws::Crt::Io::ClientBootstrap *clientBootstrap,   // Should out live this object
                const Aws::Crt::Io::SocketOptions &socketOptions, // Make a copy and save in this object
                Aws::Crt::Http::HttpClientConnectionProxyOptions
                    *httpClientConnectionProxyOptions, // Make a copy and save in this object

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

            int LastError() const noexcept { return m_lastError; }

          private:
            SecureTunnelConfig(int lastError) noexcept;

            Crt::Allocator *m_allocator;
            Aws::Crt::Io::ClientBootstrap *m_clientBootstrap;
            Aws::Crt::Io::SocketOptions m_socketOptions;
            Aws::Crt::Http::HttpClientConnectionProxyOptions *m_httpClientConnectionProxyOptions;
            std::string m_accessToken;
            aws_secure_tunneling_local_proxy_mode m_localProxyMode;
            std::string m_endpointHost;
            std::string m_rootCa;

            OnConnectionComplete m_onConnectionComplete;
            OnConnectionShutdown m_onConnectionShutdown;
            OnSendDataComplete m_onSendDataComplete;
            OnDataReceive m_onDataReceive;
            OnStreamStart m_onStreamStart;
            OnStreamReset m_onStreamReset;
            OnSessionReset m_onSessionReset;

            int m_lastError;

            friend class SecureTunnelConfigBuilder;
        };
        /**
         * Represents configuration parameters for building a SecureTunnelConfig object. You can use
         * a single instance of this class PER SecureTunnelConfig you want to generate. If you generate
         * a config for a different endpoint or port etc... you need a new instance of this class.
         */
        class AWS_IOTSECURETUNNELING_API SecureTunnelConfigBuilder final
        {
          public:
            SecureTunnelConfigBuilder();

            /**
             * Builds a secure tunnel configuration object from the set options.
             */
            SecureTunnelConfigBuilder Build() noexcept;
            /**
             * @return true if the instance is in a valid state, false otherwise.
             */
            explicit operator bool() const noexcept { return m_lastError == 0; }
            /**
             * @return the value of the last aws error encountered by operations on this instance.
             */
            int LastError() const noexcept { return m_lastError ? m_lastError : AWS_ERROR_UNKNOWN; }

          private:
            Crt::Allocator *m_allocator;
            Aws::Crt::Io::ClientBootstrap *m_clientBootstrap;
            Aws::Crt::Io::SocketOptions m_socketOptions;
            Aws::Crt::Http::HttpClientConnectionProxyOptions *m_httpClientConnectionProxyOptions;
            std::string m_accessToken;
            aws_secure_tunneling_local_proxy_mode m_localProxyMode;
            std::string m_endpointHost;
            std::string m_rootCa;

            OnConnectionComplete m_onConnectionComplete;
            OnConnectionShutdown m_onConnectionShutdown;
            OnSendDataComplete m_onSendDataComplete;
            OnDataReceive m_onDataReceive;
            OnStreamStart m_onStreamStart;
            OnStreamReset m_onStreamReset;
            OnSessionReset m_onSessionReset;

            int m_lastError;
        };

        class AWS_IOTSECURETUNNELING_API SecureTunnel final
        {
          public:
            SecureTunnel(
                // Parameters map to aws_secure_tunneling_connection_config                           for builder
                Crt::Allocator *allocator,                        // Should out live this object          req
                Aws::Crt::Io::ClientBootstrap *clientBootstrap,   // Should out live this object          req
                const Aws::Crt::Io::SocketOptions &socketOptions, // Make a copy and save in this object  req
                Aws::Crt::Http::HttpClientConnectionProxyOptions
                    *httpClientConnectionProxyOptions, // Make a copy and save in this object         not req

                const std::string &accessToken, // Make a copy and save in this object                    req
                aws_secure_tunneling_local_proxy_mode localProxyMode, //                                  req
                const std::string &endpointHost, // Make a copy and save in this object                   req
                const std::string &rootCa,       // Make a copy and save in this object               not req

                OnConnectionComplete onConnectionComplete, // callbacks not req
                OnConnectionShutdown onConnectionShutdown,
                OnSendDataComplete onSendDataComplete,
                OnDataReceive onDataReceive,
                OnStreamStart onStreamStart,
                OnStreamReset onStreamReset,
                OnSessionReset onSessionReset);
            SecureTunnel(
                // Parameters map to aws_secure_tunneling_connection_config
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

            Aws::Crt::Io::SocketOptions m_socketOptions;
            std::string m_accessToken;
            std::string m_endpointHost;
            std::string m_rootCa;

            aws_secure_tunnel *m_secure_tunnel;
        };
    } // namespace Iotsecuretunneling
} // namespace Aws
