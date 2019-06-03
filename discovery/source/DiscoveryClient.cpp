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

#include <aws/crt/Types.h>
#include <aws/crt/io/TlsOptions.h>
#include <aws/crt/io/Uri.h>

namespace Aws
{
    namespace Discovery
    {
        DiscoveryClientConfig::DiscoveryClientConfig() noexcept
            : allocator(Crt::DefaultAllocator()), bootstrap(nullptr), tlsContext(nullptr), socketOptions(nullptr),
              region("us-east-1"), maxConnections(2)
        {
        }

        DiscoveryClient::DiscoveryClient(const Aws::Discovery::DiscoveryClientConfig &clientConfig) noexcept
        {
            AWS_ASSERT(clientConfig.tlsContext);
            AWS_ASSERT(clientConfig.bootstrap);
            AWS_ASSERT(clientConfig.socketOptions);

            m_allocator = clientConfig.allocator;

            Crt::StringStream ss;
            ss << "greengrass-ats.iot." << clientConfig.region << ".amazonaws.com";

            Crt::Io::TlsConnectionOptions tlsConnectionOptions = clientConfig.tlsContext->NewConnectionOptions();
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

            Crt::Http::HttpClientConnectionManagerOptions connectionManagerOptions;
            connectionManagerOptions.socketOptions = clientConfig.socketOptions;
            connectionManagerOptions.bootstrap = clientConfig.bootstrap;
            connectionManagerOptions.tlsConnectionOptions = &tlsConnectionOptions;
            connectionManagerOptions.maxConnections = clientConfig.maxConnections;
            connectionManagerOptions.initialWindowSize = SIZE_MAX;
            connectionManagerOptions.hostName = serverName;
            connectionManagerOptions.port = port;

            m_connectionManager = Crt::Http::HttpClientConnectionManager::NewClientConnectionManager(
                connectionManagerOptions, clientConfig.allocator);
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

            auto *callbackContext = Crt::New<ClientCallbackContext>(m_allocator);
            if (!callbackContext)
            {
                return false;
            }

            callbackContext->responseCode = 0;

            bool res = m_connectionManager->AcquireConnection(
                [this, callbackContext, thingName, onDiscoverResponse](
                    std::shared_ptr<Crt::Http::HttpClientConnection> connection, int errorCode) {
                    if (!errorCode)
                    {
                        Crt::StringStream ss;
                        ss << "/greengrass/discover/thing/" << thingName;
                        Crt::String uriStr = ss.str();
                        Crt::Http::HttpRequestOptions requestOptions;
                        requestOptions.uri = Crt::ByteCursorFromCString(uriStr.c_str());
                        requestOptions.method = Crt::ByteCursorFromCString("GET");

                        Crt::Http::HttpHeader hostNameHeader;
                        hostNameHeader.name = Crt::ByteCursorFromCString("host");
                        hostNameHeader.value = Crt::ByteCursorFromCString(m_hostName.c_str());

                        requestOptions.headerArray = &hostNameHeader;
                        requestOptions.headerArrayLength = 1;

                        requestOptions.onStreamOutgoingBody = nullptr;
                        requestOptions.onIncomingHeaders =
                            [](Crt::Http::HttpStream &, const Crt::Http::HttpHeader *, std::size_t) {};
                        requestOptions.onIncomingHeadersBlockDone =
                            [callbackContext](Crt::Http::HttpStream &stream, bool) {
                                callbackContext->responseCode = stream.GetResponseStatusCode();
                            };
                        requestOptions.onIncomingBody =
                            [callbackContext](Crt::Http::HttpStream &, const Crt::ByteCursor &data, std::size_t &) {
                                callbackContext->ss.write(reinterpret_cast<const char *>(data.ptr), data.len);
                            };
                        requestOptions.onStreamComplete = [this, connection, callbackContext, onDiscoverResponse](
                                                              Crt::Http::HttpStream &stream, int errorCode) {
                            if (!errorCode && callbackContext->responseCode == 200)
                            {
                                Crt::JsonObject jsonObject(callbackContext->ss.str());
                                DiscoverResponse response(jsonObject.View());
                                onDiscoverResponse(&response, AWS_ERROR_SUCCESS, callbackContext->responseCode);
                            }
                            else if (errorCode)
                            {
                                onDiscoverResponse(nullptr, errorCode, callbackContext->responseCode);
                            }
                            else
                            {
                                onDiscoverResponse(nullptr, errorCode, callbackContext->responseCode);
                            }

                            Crt::Delete(callbackContext, m_allocator);
                            m_connectionManager->ReleaseConnection(connection);
                        };

                        if (!connection->NewClientStream(requestOptions))
                        {
                            onDiscoverResponse(nullptr, aws_last_error(), 0);
                            Crt::Delete(callbackContext, m_allocator);
                            m_connectionManager->ReleaseConnection(connection);
                        }

                        return;
                    }

                    onDiscoverResponse(nullptr, errorCode, 0);
                    Crt::Delete(callbackContext, m_allocator);
                });

            if (!res)
            {
                Crt::Delete(callbackContext, m_allocator);
                return false;
            }

            return true;
        }
    } // namespace Discovery
} // namespace Aws
