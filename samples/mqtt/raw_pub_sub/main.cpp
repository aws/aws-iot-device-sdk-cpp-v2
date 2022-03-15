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

    String endpoint;
    String certificatePath;
    String keyPath;
    String caFile;
    String topic("test/topic");
    String clientId(String("test-") + Aws::Crt::UUID().ToString());
    String proxyHost;
    uint16_t proxyPort(8080);
    String userName;
    String password;
    // Valid protocol names are documented on page:
    // https://docs.aws.amazon.com/iot/latest/developerguide/protocols.html
    // Use "mqtt" for Custom Authentication
    String protocolName("x-amzn-mqtt-ca"); // X.509 client certificate auth
    Vector<String> authParams;

    bool useWebSocket = false;

    /*********************** Parse Arguments ***************************/
    Utils::CommandLineUtils cmdUtils = Utils::CommandLineUtils();
    cmdUtils.RegisterProgramName("raw_pub_sub");
    cmdUtils.AddCommonMQTTCommands();
    cmdUtils.AddCommonProxyCommands();
    cmdUtils.AddCommonTopicMessageCommands();
    cmdUtils.AddCommonWebsocketCommands();
    cmdUtils.RemoveCommand("message");
    cmdUtils.RemoveCommand("region");
    cmdUtils.RegisterCommand("client_id", "<str>", "Client id to use (optional, default='test-*').");
    cmdUtils.RegisterCommand("user_name", "<str>", "User name to send with mqtt connect.");
    cmdUtils.RegisterCommand("password", "<str>", "Password to send with mqtt connect.");
    cmdUtils.RegisterCommand(
        "protocol_name", "<str>", "The X.509 client certificate auth (optional, default='x-amzn-mqtt-ca').");
    cmdUtils.RegisterCommand(
        "auth_params",
        "<comma delimited list>",
        "Comma delimited list of auth parameters. For websockets these will be set as headers (optional).");
    cmdUtils.RegisterCommand(Utils::CommandLineOption("help", "", "Prints this message"));
    const char **const_argv = (const char **)argv;
    cmdUtils.SendArguments(const_argv, const_argv + argc);

    if (cmdUtils.HasCommand("help"))
    {
        cmdUtils.PrintHelp();
        exit(-1);
    }
    endpoint = cmdUtils.GetCommandRequired("endpoint");
    keyPath = cmdUtils.GetCommandOrDefault("key", keyPath);
    certificatePath = cmdUtils.GetCommandOrDefault("cert", certificatePath);
    topic = cmdUtils.GetCommandOrDefault("topic", topic);
    caFile = cmdUtils.GetCommandOrDefault("ca_file", caFile);
    clientId = cmdUtils.GetCommandOrDefault("client_id", clientId);
    if (cmdUtils.HasCommand("use_websocket"))
    {
        protocolName = "http/1.1";
        useWebSocket = true;
    }
    proxyHost = cmdUtils.GetCommandOrDefault("proxy_host", proxyHost);
    if (cmdUtils.HasCommand("proxy_port"))
    {
        int port = atoi(cmdUtils.GetCommand("proxy_port").c_str());
        if (port > 0 && port <= UINT16_MAX)
        {
            proxyPort = static_cast<uint16_t>(port);
        }
    }
    userName = cmdUtils.GetCommandOrDefault("user_name", userName);
    password = cmdUtils.GetCommandOrDefault("password", password);
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
                fprintf(stdout, "Connection completed successfully.");
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
        {
            fprintf(stdout, "Disconnect completed\n");
            connectionClosedPromise.set_value();
        }
    };

    connection->OnConnectionCompleted = std::move(onConnectionCompleted);
    connection->OnDisconnect = std::move(onDisconnect);
    connection->OnConnectionInterrupted = std::move(onInterrupted);
    connection->OnConnectionResumed = std::move(onResumed);

    connection->SetOnMessageHandler([](Mqtt::MqttConnection &,
                                       const String &topic,
                                       const ByteBuf &payload,
                                       bool /*dup*/,
                                       Mqtt::QOS /*qos*/,
                                       bool /*retain*/) {
        fprintf(stdout, "Generic Publish received on topic %s, payload:\n", topic.c_str());
        fwrite(payload.buffer, 1, payload.len, stdout);
        fprintf(stdout, "\n");
    });

    /*
     * Actually perform the connect dance.
     * This will use default ping behavior of 1 hour and 3 second timeouts.
     * If you want different behavior, those arguments go into slots 3 & 4.
     */
    fprintf(stdout, "Connecting...\n");
    if (!connection->Connect(clientId.c_str(), false, 1000))
    {
        fprintf(stderr, "MQTT Connection failed with error %s\n", ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    if (connectionCompletedPromise.get_future().get())
    {
        /*
         * This is invoked upon the receipt of a Publish on a subscribed topic.
         */
        auto onMessage = [&](Mqtt::MqttConnection &,
                             const String &topic,
                             const ByteBuf &byteBuf,
                             bool /*dup*/,
                             Mqtt::QOS /*qos*/,
                             bool /*retain*/) {
            fprintf(stdout, "Publish received on topic %s\n", topic.c_str());
            fprintf(stdout, "\n Message:\n");
            fwrite(byteBuf.buffer, 1, byteBuf.len, stdout);
            fprintf(stdout, "\n");
        };

        /*
         * Subscribe for incoming publish messages on topic.
         */
        std::promise<void> subscribeFinishedPromise;
        auto onSubAck =
            [&](Mqtt::MqttConnection &, uint16_t packetId, const String &topic, Mqtt::QOS QoS, int errorCode) {
                if (errorCode)
                {
                    fprintf(stderr, "Subscribe failed with error %s\n", aws_error_debug_str(errorCode));
                    exit(-1);
                }
                else
                {
                    if (!packetId || QoS == AWS_MQTT_QOS_FAILURE)
                    {
                        fprintf(stderr, "Subscribe rejected by the broker.");
                        exit(-1);
                    }
                    else
                    {
                        fprintf(stdout, "Subscribe on topic %s on packetId %d Succeeded\n", topic.c_str(), packetId);
                    }
                }
                subscribeFinishedPromise.set_value();
            };

        connection->Subscribe(topic.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, onMessage, onSubAck);
        subscribeFinishedPromise.get_future().wait();

        while (true)
        {
            String input;
            fprintf(
                stdout,
                "Enter the message you want to publish to topic %s and press enter. Enter 'exit' to exit this "
                "program.\n",
                topic.c_str());
            std::getline(std::cin, input);

            if (input == "exit")
            {
                break;
            }

            ByteBuf payload = ByteBufFromArray((const uint8_t *)input.data(), input.length());

            auto onPublishComplete = [](Mqtt::MqttConnection &, uint16_t packetId, int errorCode) {
                if (packetId)
                {
                    fprintf(stdout, "Operation on packetId %d Succeeded\n", packetId);
                }
                else
                {
                    fprintf(stdout, "Operation failed with error %s\n", aws_error_debug_str(errorCode));
                }
            };
            connection->Publish(topic.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, false, payload, onPublishComplete);
        }

        /*
         * Unsubscribe from the topic.
         */
        std::promise<void> unsubscribeFinishedPromise;
        connection->Unsubscribe(
            topic.c_str(), [&](Mqtt::MqttConnection &, uint16_t, int) { unsubscribeFinishedPromise.set_value(); });
        unsubscribeFinishedPromise.get_future().wait();
    }

    /* Disconnect */
    if (connection->Disconnect())
    {
        connectionClosedPromise.get_future().wait();
    }

    return 0;
}
