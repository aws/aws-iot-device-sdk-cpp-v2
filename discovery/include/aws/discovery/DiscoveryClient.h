#pragma once
/* Copyright 2010-2019 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */
#include <aws/discovery/DiscoverResponse.h>

#include <aws/crt/http/HttpConnectionManager.h>

namespace Aws
{
    namespace Discovery
    {
        using OnDiscoverResponse = std::function<void(DiscoverResponse *, int errorCode, int httpResponseCode)>;

        class DiscoveryClientConfig
        {
          public:
            explicit DiscoveryClientConfig(Crt::Allocator *allocator = Crt::DefaultAllocator()) noexcept;
            DiscoveryClientConfig(const DiscoveryClientConfig &rhs) = default;
            DiscoveryClientConfig(DiscoveryClientConfig &&rhs) = default;

            DiscoveryClientConfig &operator=(const DiscoveryClientConfig &rhs) = default;
            DiscoveryClientConfig &operator=(DiscoveryClientConfig &&rhs) = default;

            ~DiscoveryClientConfig() = default;

            /**
             * Sets the client bootstrap to use for setting up and tearing down connections.
             * This value must be set.
             */
            void SetBootstrap(Crt::Io::ClientBootstrap *bootstrap) noexcept { m_bootstrap = bootstrap; }

            /**
             * Gets the client bootstrap to use for setting up and tearing down connections.
             */
            Crt::Io::ClientBootstrap *GetBootstrap() const noexcept { return m_bootstrap; }

            /**
             * Sets the TLS options for all http connections made by this client
             */
            void SetTlsContext(const Crt::Io::TlsContext &context) noexcept { m_tlsContext = context; }

            /**
             * Gets the TLS options for all http connections made by this client
             */
            const Crt::Io::TlsContext *GetTlsContext() const noexcept
            {
                return m_tlsContext.has_value() ? &m_tlsContext.value() : nullptr;
            }

            /**
             * Sets the socket options of the connections made by the client.
             * This value must be set.
             */
            void SetSocketOptions(const Crt::Io::SocketOptions &options) noexcept { m_socketOptions = options; }

            /**
             * Gets the socket options of the connections made by the client
             */
            const Crt::Io::SocketOptions &GetSocketOptions() const noexcept { return m_socketOptions; }

            /**
             * Sets the value of the Aws region to connect to.
             * This value must be set.
             */
            void SetRegion(const Crt::String &region) noexcept { m_region = region; }

            /**
             * Gets the value of the Aws region to connect to.
             */
            const Crt::String &GetRegion() const noexcept { return m_region; }

            /**
             * Sets the maximum number of concurrent connections allowed
             */
            void SetMaxConnections(size_t maxConnections) noexcept { m_maxConnections = maxConnections; }

            /**
             * Gets the maximum number of concurrent connections allowed
             */
            size_t GetMaxConnections() const noexcept { return m_maxConnections; }

            /**
             * Sets the proxy options for all http connections made by this client
             */
            void SetProxyOptions(const Crt::Http::HttpClientConnectionProxyOptions &options) noexcept
            {
                m_proxyOptions = options;
            }

            /**
             * Gets the proxy options for all http connections made by this client
             */
            const Crt::Http::HttpClientConnectionProxyOptions *GetProxyOptions() const noexcept
            {
                return m_proxyOptions.has_value() ? &m_proxyOptions.value() : nullptr;
            }

          private:
            Crt::Allocator *m_allocator;
            Crt::Io::ClientBootstrap *m_bootstrap;
            Crt::Optional<Crt::Io::TlsContext> m_tlsContext;
            Crt::Io::SocketOptions m_socketOptions;
            Crt::String m_region;
            size_t m_maxConnections;
            Crt::Optional<Crt::Http::HttpClientConnectionProxyOptions> m_proxyOptions;
        };

        class DiscoveryClient final
        {
          public:
            bool Discover(const Crt::String &thingName, const OnDiscoverResponse &onDiscoverResponse) noexcept;

            static std::shared_ptr<DiscoveryClient> CreateClient(
                const DiscoveryClientConfig &config,
                Crt::Allocator *allocator = Crt::DefaultAllocator());

          private:
            DiscoveryClient(const DiscoveryClientConfig &config, Crt::Allocator *allocator) noexcept;

            std::shared_ptr<Crt::Http::HttpClientConnectionManager> m_connectionManager;
            Crt::String m_hostName;
            Crt::Allocator *m_allocator;
        };
    } // namespace Discovery
} // namespace Aws
