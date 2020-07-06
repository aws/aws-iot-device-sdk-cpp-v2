#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/discovery/DiscoverResponse.h>

#include <aws/crt/http/HttpConnectionManager.h>

namespace Aws
{
    namespace Discovery
    {
        using OnDiscoverResponse = std::function<void(DiscoverResponse *, int errorCode, int httpResponseCode)>;

        class AWS_DISCOVERY_API DiscoveryClientConfig
        {
          public:
            DiscoveryClientConfig() noexcept;
            DiscoveryClientConfig(const DiscoveryClientConfig &rhs) = default;
            DiscoveryClientConfig(DiscoveryClientConfig &&rhs) = default;

            DiscoveryClientConfig &operator=(const DiscoveryClientConfig &rhs) = default;
            DiscoveryClientConfig &operator=(DiscoveryClientConfig &&rhs) = default;

            ~DiscoveryClientConfig() = default;

            /**
             * The client bootstrap to use for setting up and tearing down connections.
             * Required.
             */
            Crt::Io::ClientBootstrap *Bootstrap;

            /**
             * The TLS options for all http connections made by this client.
             * Optional.
             */
            Crt::Optional<Crt::Io::TlsContext> TlsContext;

            /**
             * The socket options of the connections made by the client.
             * Required.
             */
            Crt::Io::SocketOptions SocketOptions;

            /**
             * The value of the Aws region to connect to.
             * Required.
             */
            Crt::String Region;

            /**
             * The maximum number of concurrent connections allowed
             */
            size_t MaxConnections;

            /**
             * The proxy options for all http connections made by this client.
             * Optional.
             */
            Crt::Optional<Crt::Http::HttpClientConnectionProxyOptions> ProxyOptions;
        };

        class AWS_DISCOVERY_API DiscoveryClient final
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
