/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/StlAllocator.h>

#include <aws/crt/http/HttpRequestResponse.h>
#include <aws/crt/mqtt/MqttClient.h>

#include <algorithm>
#include <aws/crt/UUID.h>
#include <condition_variable>
#include <cstdint>
#include <iostream>
#include <mutex>

#include "../../utils/CommandLineUtils.h"

using namespace Aws::Crt;

int main(int argc, char *argv[])
{

    /************************ Setup the Lib ****************************/
    /*
     * Do the global initialization for the API.
     */
    ApiHandle apiHandle;
    uint16_t proxyPort(8080);
    // Valid protocol names are documented on page:
    // https://docs.aws.amazon.com/iot/latest/developerguide/protocols.html
    // Use "mqtt" for Custom Authentication
    String protocolName("x-amzn-mqtt-ca"); // X.509 client certificate auth
    Vector<String> authParams;
    bool useWebSocket = false;

    /*********************** Parse Arguments ***************************/
    Utils::CommandLineUtils cmdUtils = Utils::CommandLineUtils();
    cmdUtils.RegisterProgramName("raw-connect");
    cmdUtils.AddCommonMQTTCommands();
    cmdUtils.RegisterCommand("key", "<path>", "Path to your key in PEM format.");
    cmdUtils.RegisterCommand("cert", "<path>", "Path to your client certificate in PEM format.");
    cmdUtils.AddCommonProxyCommands();
    cmdUtils.RegisterCommand("client_id", "<str>", "Client id to use (optional, default='test-*').");
    cmdUtils.RegisterCommand("user_name", "<str>", "User name to send with mqtt connect.");
    cmdUtils.RegisterCommand("password", "<str>", "Password to send with mqtt connect.");
    cmdUtils.RegisterCommand(
        "protocol_name", "<str>", "The X.509 client certificate auth (optional, default='x-amzn-mqtt-ca').");
    cmdUtils.RegisterCommand(
        "auth_params",
        "<comma delimited list>",
        "Comma delimited list of auth parameters. For websockets these will be set as headers (optional).");
    const char **const_argv = (const char **)argv;
    cmdUtils.SendArguments(const_argv, const_argv + argc);

    String endpoint = cmdUtils.GetCommandRequired("endpoint");
    String keyPath = cmdUtils.GetCommandOrDefault("key", "");
    String certificatePath = cmdUtils.GetCommandOrDefault("cert", "");
    String caFile = cmdUtils.GetCommandOrDefault("ca_file", "");
    String clientId = cmdUtils.GetCommandOrDefault("client_id", String("test-") + Aws::Crt::UUID().ToString());
    if (cmdUtils.HasCommand("use_websocket"))
    {
        protocolName = "http/1.1";
        useWebSocket = true;
    }
    String proxyHost = cmdUtils.GetCommandOrDefault("proxy_host", "");
    if (cmdUtils.HasCommand("proxy_port"))
    {
        int port = atoi(cmdUtils.GetCommand("proxy_port").c_str());
        if (port > 0 && port <= UINT16_MAX)
        {
            proxyPort = static_cast<uint16_t>(port);
        }
    }
    String userName = cmdUtils.GetCommandOrDefault("user_name", "");
    String password = cmdUtils.GetCommandOrDefault("password", "");

    protocolName = cmdUtils.GetCommandOrDefault("protocol_name", protocolName);
    if (cmdUtils.HasCommand("auth_params"))
    {
        String params = cmdUtils.GetCommand("auth_params");
        ByteCursor commaDelimitedParams = ByteCursorFromCString(params.c_str());
        ByteCursor split;
        AWS_ZERO_STRUCT(split);
        while (aws_byte_cursor_next_split(&commaDelimitedParams, ',', &split))
        {
            authParams.push_back(String((char *)split.ptr, split.len));
        }
    }

    /********************** Now Setup an Mqtt Client ******************/
    if (apiHandle.GetOrCreateStaticDefaultClientBootstrap()->LastError() != AWS_ERROR_SUCCESS)
    {
        fprintf(
            stderr,
            "ClientBootstrap failed with error %s\n",
            ErrorDebugString(apiHandle.GetOrCreateStaticDefaultClientBootstrap()->LastError()));
        exit(-1);
    }

    Mqtt::MqttClient client;
    Io::TlsContextOptions ctxOptions = Io::TlsContextOptions::InitDefaultClient();

    if (!certificatePath.empty() && !keyPath.empty())
    {
        ctxOptions = Io::TlsContextOptions::InitClientWithMtls(certificatePath.c_str(), keyPath.c_str());
    }

    if (!caFile.empty())
    {
        ctxOptions.OverrideDefaultTrustStore(nullptr, caFile.c_str());
    }

    uint16_t port = useWebSocket ? 443 : 8883;
    if (Io::TlsContextOptions::IsAlpnSupported())
    {
        port = 443;
        ctxOptions.SetAlpnList(protocolName.c_str());
    }

    Io::SocketOptions socketOptions;
    socketOptions.SetConnectTimeoutMs(3000);

    Io::TlsContext tlsContext(ctxOptions, Io::TlsMode::CLIENT);

    auto connection = client.NewConnection(endpoint.c_str(), port, socketOptions, tlsContext, useWebSocket);

    if (!connection)
    {
        fprintf(stderr, "MQTT Connection Creation failed with error %s\n", ErrorDebugString(client.LastError()));
        exit(-1);
    }

    if (!authParams.empty())
    {
        if (useWebSocket)
        {
            /* set authorizer headers on the outgoing websocket upgrade request. */
            connection->WebsocketInterceptor = [&](std::shared_ptr<Http::HttpRequest> req,
                                                   const Mqtt::OnWebSocketHandshakeInterceptComplete &onComplete) {
                for (auto &param : authParams)
                {
                    Http::HttpHeader header;
                    AWS_ZERO_STRUCT(header);
                    ByteCursor paramPair = ByteCursorFromCString(param.c_str());
                    ByteCursor nameOrValue;
                    AWS_ZERO_STRUCT(nameOrValue);
                    if (!aws_byte_cursor_next_split(&paramPair, '=', &nameOrValue))
                    {
                        cmdUtils.PrintHelp();
                        exit(-1);
                    }
                    header.name = nameOrValue;
                    if (!aws_byte_cursor_next_split(&paramPair, '=', &nameOrValue))
                    {
                        cmdUtils.PrintHelp();
                        exit(-1);
                    }
                    header.value = nameOrValue;
                    req->AddHeader(header);
                }
                onComplete(req, AWS_OP_SUCCESS);
            };
        } /* set authorizer parameters on username if we're using raw mqtt */
        else if (!userName.empty())
        {
            userName += "?";
            size_t count = 0;
            for (auto &param : authParams)
            {
                userName += param;
                count++;
                if (count < authParams.size())
                {
                    userName += "&";
                }
            }
        }
    }

    if (!userName.empty())
    {
        connection->SetLogin(userName.c_str(), password.c_str());
    }

    if (!proxyHost.empty())
    {
        Http::HttpClientConnectionProxyOptions proxyOptions;
        proxyOptions.HostName = proxyHost;
        proxyOptions.Port = proxyPort;
        connection->SetHttpProxyOptions(proxyOptions);
    }

    /*
     * In a real world application you probably don't want to enforce synchronous behavior
     * but this is a sample console application, so we'll just do that with a condition variable.
     */
    std::promise<bool> connectionCompletedPromise;
    std::promise<void> connectionClosedPromise;

    /*
     * This will execute when an mqtt connect has completed or failed.
     */
    auto onConnectionCompleted = [&](Mqtt::MqttConnection &, int errorCode, Mqtt::ReturnCode returnCode, bool) {
        if (errorCode)
        {
            fprintf(stdout, "Connection failed with error %s\n", ErrorDebugString(errorCode));
            connectionCompletedPromise.set_value(false);
        }
        else
        {
            if (returnCode != AWS_MQTT_CONNECT_ACCEPTED)
            {
                fprintf(stdout, "Connection failed with mqtt return code %d\n", (int)returnCode);
                connectionCompletedPromise.set_value(false);
            }
            else
            {
                fprintf(stdout, "Connection completed successfully.\n");
                connectionCompletedPromise.set_value(true);
            }
        }
    };

    auto onInterrupted = [&](Mqtt::MqttConnection &, int error) {
        fprintf(stdout, "Connection interrupted with error %s\n", ErrorDebugString(error));
    };
    auto onResumed = [&](Mqtt::MqttConnection &, Mqtt::ReturnCode, bool) { fprintf(stdout, "Connection resumed\n"); };

    /*
     * Invoked when a disconnect message has completed.
     */
    auto onDisconnect = [&](Mqtt::MqttConnection &) {
        fprintf(stdout, "Disconnect completed\n");
        connectionClosedPromise.set_value();
    };

    connection->OnConnectionCompleted = std::move(onConnectionCompleted);
    connection->OnDisconnect = std::move(onDisconnect);
    connection->OnConnectionInterrupted = std::move(onInterrupted);
    connection->OnConnectionResumed = std::move(onResumed);

    /*
     * Actually perform the connect dance.
     */
    fprintf(stdout, "Connecting...\n");
    if (!connection->Connect(clientId.c_str(), false /*cleanSession*/, 1000 /*keepAliveTimeSecs*/))
    {
        fprintf(stderr, "MQTT Connection failed with error %s\n", ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    if (connectionCompletedPromise.get_future().get())
    {
        /* Wait for half a second */
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        /* Disconnect */
        if (connection->Disconnect())
        {
            connectionClosedPromise.get_future().wait();
        }
    }
    else
    {
        exit(-1);
    }

    return 0;
}
