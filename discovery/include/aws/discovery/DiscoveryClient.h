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
