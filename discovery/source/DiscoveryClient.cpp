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
#include <aws/discovery/DiscoveryClient.h>

#include <aws/crt/Api.h>
#include <aws/crt/Types.h>
#include <aws/crt/http/HttpRequestResponse.h>
#include <aws/crt/io/TlsOptions.h>
#include <aws/crt/io/Uri.h>

namespace Aws
{
    namespace Discovery
    {
        DiscoveryClientConfig::DiscoveryClientConfig() noexcept
            : m_bootstrap(nullptr), m_tlsContext(), m_socketOptions(), m_region(),
              m_maxConnections(2), m_proxyOptions()
        {
        }

        DiscoveryClient::DiscoveryClient(
            const Aws::Discovery::DiscoveryClientConfig &clientConfig,
            Crt::Allocator *allocator) noexcept
        {
            AWS_ASSERT(clientConfig.GetTlsContext());
            AWS_ASSERT(clientConfig.GetBootstrap());

            m_allocator = allocator;

            Crt::StringStream ss;
            ss << "greengrass-ats.iot." << clientConfig.GetRegion() << ".amazonaws.com";

            Crt::Io::TlsConnectionOptions tlsConnectionOptions = clientConfig.GetTlsContext()->NewConnectionOptions();
            uint16_t port = 443;

            if (Crt::Io::TlsContextOptions::IsAlpnSupported())
            {
                tlsConnectionOptions.SetAlpnList("x-amzn-http-ca");
            }
            else
            {
                port = 8883;
            }

            m_hostName = ss.str();
            Crt::ByteCursor serverName = Crt::ByteCursorFromCString(m_hostName.c_str());
            tlsConnectionOptions.SetServerName(serverName);

            Crt::Http::HttpClientConnectionOptions connectionOptions;
            connectionOptions.SetSocketOptions(clientConfig.GetSocketOptions());
            connectionOptions.SetBootstrap(clientConfig.GetBootstrap());
            connectionOptions.SetTlsOptions(tlsConnectionOptions);
            connectionOptions.SetInitialWindowSize(SIZE_MAX);
            connectionOptions.SetHostName(Crt::String((const char *)serverName.ptr, serverName.len));
            connectionOptions.SetPort(port);
            if (clientConfig.GetProxyOptions())
            {
                connectionOptions.SetProxyOptions(*clientConfig.GetProxyOptions());
            }

            Crt::Http::HttpClientConnectionManagerOptions connectionManagerOptions;
            connectionManagerOptions.SetConnectionOptions(connectionOptions);
            connectionManagerOptions.SetMaxConnections(clientConfig.GetMaxConnections());

            m_connectionManager =
                Crt::Http::HttpClientConnectionManager::NewClientConnectionManager(connectionManagerOptions, allocator);
        }

        std::shared_ptr<DiscoveryClient> DiscoveryClient::CreateClient(
            const Aws::Discovery::DiscoveryClientConfig &clientConfig,
            Crt::Allocator *allocator)
        {
            auto *toSeat = static_cast<DiscoveryClient *>(aws_mem_acquire(allocator, sizeof(DiscoveryClient)));
            if (toSeat)
            {
                toSeat = new (toSeat) DiscoveryClient(clientConfig, allocator);
                return std::shared_ptr<DiscoveryClient>(
                    toSeat, [allocator](DiscoveryClient *client) { Crt::Delete(client, allocator); });
            }

            return nullptr;
        }

        struct ClientCallbackContext
        {
            Crt::StringStream ss;
            int responseCode;
        };

        bool DiscoveryClient::Discover(
            const Crt::String &thingName,
            const OnDiscoverResponse &onDiscoverResponse) noexcept
        {
            auto callbackContext = Crt::MakeShared<ClientCallbackContext>(m_allocator);
            if (!callbackContext)
            {
                return false;
            }

            callbackContext->responseCode = 0;

            bool res = m_connectionManager->AcquireConnection(
                [this, callbackContext, thingName, onDiscoverResponse](
                    std::shared_ptr<Crt::Http::HttpClientConnection> connection, int errorCode) {
                    if (errorCode)
                    {
                        onDiscoverResponse(nullptr, errorCode, 0);
                        return;
                    }

                    auto request = Aws::Crt::MakeShared<Crt::Http::HttpRequest>(m_allocator, m_allocator);
                    if (request == nullptr)
                    {
                        onDiscoverResponse(nullptr, Crt::LastErrorOrUnknown(), 0);
                        return;
                    }

                    Crt::StringStream ss;
                    ss << "/greengrass/discover/thing/" << thingName;
                    Crt::String uriStr = ss.str();
                    if (!request->SetMethod(Crt::ByteCursorFromCString("GET")))
                    {
                        onDiscoverResponse(nullptr, Crt::LastErrorOrUnknown(), 0);
                        return;
                    }

                    if (!request->SetPath(Crt::ByteCursorFromCString(uriStr.c_str())))
                    {
                        onDiscoverResponse(nullptr, Crt::LastErrorOrUnknown(), 0);
                        return;
                    }

                    Crt::Http::HttpHeader hostNameHeader;
                    hostNameHeader.name = Crt::ByteCursorFromCString("host");
                    hostNameHeader.value = Crt::ByteCursorFromCString(m_hostName.c_str());

                    if (!request->AddHeader(hostNameHeader))
                    {
                        onDiscoverResponse(nullptr, Crt::LastErrorOrUnknown(), 0);
                        return;
                    }

                    Crt::Http::HttpRequestOptions requestOptions;
                    requestOptions.request = request.get();
                    requestOptions.onIncomingHeaders =
                        [](Crt::Http::HttpStream &, const Crt::Http::HttpHeader *, std::size_t) {};
                    requestOptions.onIncomingHeadersBlockDone = [callbackContext](Crt::Http::HttpStream &stream, bool) {
                        callbackContext->responseCode = stream.GetResponseStatusCode();
                    };
                    requestOptions.onIncomingBody =
                        [callbackContext](Crt::Http::HttpStream &, const Crt::ByteCursor &data) {
                            callbackContext->ss.write(reinterpret_cast<const char *>(data.ptr), data.len);
                        };
                    requestOptions.onStreamComplete = [request, connection, callbackContext, onDiscoverResponse](
                                                          Crt::Http::HttpStream &, int errorCode) {
                        if (!errorCode && callbackContext->responseCode == 200)
                        {
                            Crt::JsonObject jsonObject(callbackContext->ss.str());
                            DiscoverResponse response(jsonObject.View());
                            onDiscoverResponse(&response, AWS_ERROR_SUCCESS, callbackContext->responseCode);
                        }
                        else
                        {
                            if (!errorCode)
                            {
                                errorCode = AWS_ERROR_UNKNOWN;
                            }
                            onDiscoverResponse(nullptr, errorCode, callbackContext->responseCode);
                        }
                    };

                    if (!connection->NewClientStream(requestOptions))
                    {
                        onDiscoverResponse(nullptr, Crt::LastErrorOrUnknown(), 0);
                    }
                });

            if (!res)
            {
                return false;
            }

            return true;
        }
    } // namespace Discovery
} // namespace Aws
