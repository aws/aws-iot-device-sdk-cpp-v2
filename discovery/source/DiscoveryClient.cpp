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
#include <aws/crt/http/HttpRequestResponse.h>
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

            Crt::Http::HttpClientConnectionOptions connectionOptions;
            connectionOptions.SetSocketOptions(*clientConfig.socketOptions);
            connectionOptions.SetBootstrap(clientConfig.bootstrap);
            connectionOptions.SetTlsOptions(tlsConnectionOptions);
            connectionOptions.SetInitialWindowSize(SIZE_MAX);
            connectionOptions.SetHostName(Crt::String((const char *)serverName.ptr, serverName.len));
            connectionOptions.SetPort(port);

            Crt::Http::HttpClientConnectionManagerOptions connectionManagerOptions;
            connectionManagerOptions.SetConnectionOptions(connectionOptions);
            connectionManagerOptions.SetMaxConnections(clientConfig.maxConnections);


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
            auto callbackContext = Crt::MakeShared<ClientCallbackContext>(m_allocator);
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
                        auto request = Aws::Crt::MakeShared<Crt::Http::HttpRequest>(m_allocator, m_allocator);

                        Crt::StringStream ss;
                        ss << "/greengrass/discover/thing/" << thingName;
                        Crt::String uriStr = ss.str();
                        request->SetMethod(Crt::ByteCursorFromCString("GET"));
                        request->SetPath(Crt::ByteCursorFromCString(uriStr.c_str()));

                        Crt::Http::HttpHeader hostNameHeader;
                        hostNameHeader.name = Crt::ByteCursorFromCString("host");
                        hostNameHeader.value = Crt::ByteCursorFromCString(m_hostName.c_str());

                        request->AddHeader(hostNameHeader);

                        Crt::Http::HttpRequestOptions requestOptions;
                        requestOptions.request = request.get();
                        requestOptions.onIncomingHeaders =
                            [](Crt::Http::HttpStream &, const Crt::Http::HttpHeader *, std::size_t) {};
                        requestOptions.onIncomingHeadersBlockDone =
                            [callbackContext](Crt::Http::HttpStream &stream, bool) {
                                callbackContext->responseCode = stream.GetResponseStatusCode();
                            };
                        requestOptions.onIncomingBody =
                            [callbackContext](Crt::Http::HttpStream &, const Crt::ByteCursor &data) {
                                callbackContext->ss.write(reinterpret_cast<const char *>(data.ptr), data.len);
                            };
                        requestOptions.onStreamComplete = [request, connection, callbackContext, onDiscoverResponse](
                                                              Crt::Http::HttpStream &stream, int errorCode) {
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
                            onDiscoverResponse(nullptr, aws_last_error(), 0);
                        }

                        return;
                    }

                    onDiscoverResponse(nullptr, errorCode, 0);
                });

            if (!res)
            {
                return false;
            }

            return true;
        }
    } // namespace Discovery
} // namespace Aws
