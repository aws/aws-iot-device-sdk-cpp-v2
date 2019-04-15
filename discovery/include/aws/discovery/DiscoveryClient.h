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

* This file is generated
*/
#include <aws/discovery/DiscoverResponse.h>

#include <aws/crt/http/HttpConnectionManager.h>

namespace Aws
{
    namespace Discovery
    {
        using OnDiscoverResponse = std::function<void(DiscoverResponse *, int errorCode, int httpResponseCode)>;

        struct DiscoveryClientConfig
        {
            DiscoveryClientConfig() noexcept;
            Crt::Allocator *allocator;
            Crt::Io::ClientBootstrap *bootstrap;
            Crt::Io::TlsContext *tlsContext;
            Crt::Io::SocketOptions *socketOptions;
            Crt::String region;
            size_t maxConnections;
            // TODO: when supported add proxy configuration.
        };

        class DiscoveryClient final
        {
          public:
            DiscoveryClient(const DiscoveryClientConfig &) noexcept;

            bool Discover(const Crt::String &thingName, const OnDiscoverResponse &onDiscoverResponse) noexcept;

          private:
            std::shared_ptr<Crt::Http::HttpClientConnectionManager> m_connectionManager;
            Crt::String m_hostName;
            Crt::Allocator *m_allocator;
        };
    } // namespace Discovery
} // namespace Aws

