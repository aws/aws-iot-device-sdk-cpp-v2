/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/io/HostResolver.h>

#include <aws/discovery/DiscoveryClient.h>

#include <aws/iot/MqttClient.h>

#include <algorithm>
#include <condition_variable>
#include <iostream>
#include <mutex>

#include "../../utils/CommandLineUtils.h"

using namespace Aws::Crt;
using namespace Aws::Discovery;

int main(int argc, char *argv[])
{
    /************************ Setup the Lib ****************************/
    /*
     * Do the global initialization for the API.
     */
    ApiHandle apiHandle;

    String proxyHost;
    uint16_t proxyPort = 0;
    String region("us-east-1");
    String certificatePath;
    String keyPath;
    String caFile;
    String thingName;
    String topic("test/topic");
    String mode("both");
    String message("Hello World");

    /*********************** Parse Arguments ***************************/
    Utils::CommandLineUtils cmdUtils = Utils::CommandLineUtils();
    cmdUtils.RegisterProgramName("basic-discovery");
    cmdUtils.AddCommonMQTTCommands();
    cmdUtils.AddCommonProxyCommands();
    cmdUtils.AddCommonTopicMessageCommands();
    cmdUtils.RemoveCommand("endpoint");
    cmdUtils.RegisterCommand(
        "region", "<str>", "The region for your Greengrass groups (optional, default='us-east-1').");
    cmdUtils.RegisterCommand("thing_name", "<str>", "The name of your IOT thing");
    cmdUtils.RegisterCommand(
        "mode", "<str>", "Mode options: 'both', 'publish', or 'subscribe' (optional, default='both').");
    const char **const_argv = (const char **)argv;
    cmdUtils.SendArguments(const_argv, const_argv + argc);

    if (cmdUtils.HasCommand("help"))
    {
        cmdUtils.PrintHelp();
        exit(-1);
    }
    certificatePath = cmdUtils.GetCommandRequired("cert");
    keyPath = cmdUtils.GetCommandRequired("key");
    thingName = cmdUtils.GetCommandRequired("thing_name");
    caFile = cmdUtils.GetCommandOrDefault("ca_file", caFile);
    region = cmdUtils.GetCommandOrDefault("region", region);
    topic = cmdUtils.GetCommandOrDefault("topic", topic);
    mode = cmdUtils.GetCommandOrDefault("mode", mode);
    message = cmdUtils.GetCommandOrDefault("message", message);
    proxyHost = cmdUtils.GetCommandOrDefault("proxy_host", proxyHost);
    if (cmdUtils.HasCommand("proxy_port"))
    {
        String portString = cmdUtils.GetCommand("proxy_port");
        proxyPort = static_cast<uint16_t>(atoi(portString.c_str()));
    }

    /*
     * We're using Mutual TLS for Mqtt, so we need to load our client certificates
     */
    Io::TlsContextOptions tlsCtxOptions =
        Io::TlsContextOptions::InitClientWithMtls(certificatePath.c_str(), keyPath.c_str());

    if (!tlsCtxOptions)
    {
        fprintf(stderr, "TLS Context Options creation failed with error %s\n", ErrorDebugString(Aws::Crt::LastError()));
        exit(-1);
    }

    if (!caFile.empty())
    {
        tlsCtxOptions.OverrideDefaultTrustStore(nullptr, caFile.c_str());
    }

    Io::TlsContext tlsCtx(tlsCtxOptions, Io::TlsMode::CLIENT);

    if (!tlsCtx)
    {
        fprintf(stderr, "Tls Context creation failed with error %s\n", ErrorDebugString(LastErrorOrUnknown()));
        exit(-1);
    }

    /*
     * Default Socket options to use. IPV4 will be ignored based on what DNS
     * tells us.
     */
    Io::SocketOptions socketOptions;
    socketOptions.SetConnectTimeoutMs(3000);

    /**
     * Create the default ClientBootstrap, which will create the default
     * EventLoopGroup (to process IO events) and HostResolver.
     */
    if (apiHandle.GetOrCreateStaticDefaultClientBootstrap()->LastError() != AWS_ERROR_SUCCESS)
    {
        fprintf(
            stderr,
            "ClientBootstrap failed with error %s\n",
            ErrorDebugString(apiHandle.GetOrCreateStaticDefaultClientBootstrap()->LastError()));
        exit(-1);
    }

    DiscoveryClientConfig clientConfig;
    clientConfig.SocketOptions = socketOptions;
    clientConfig.TlsContext = tlsCtx;
    clientConfig.Region = region;

    Aws::Crt::Http::HttpClientConnectionProxyOptions proxyOptions;
    if (proxyHost.length() > 0 && proxyPort != 0)
    {
        proxyOptions.HostName = proxyHost;
        proxyOptions.Port = proxyPort;
        clientConfig.ProxyOptions = proxyOptions; //
    }

    auto discoveryClient = DiscoveryClient::CreateClient(clientConfig);

    Aws::Iot::MqttClient mqttClient;
    std::shared_ptr<Mqtt::MqttConnection> connection(nullptr);

    std::promise<void> connectionFinishedPromise;
    std::promise<void> shutdownCompletedPromise;

    discoveryClient->Discover(thingName, [&](DiscoverResponse *response, int error, int httpResponseCode) {
        if (!error && response->GGGroups)
        {
            auto groupToUse = std::move(response->GGGroups->at(0));

            auto connectivityInfo = groupToUse.Cores->at(0).Connectivity->at(0);

            fprintf(
                stdout,
                "Connecting to group %s with thing arn %s, using endpoint %s:%d\n",
                groupToUse.GGGroupId->c_str(),
                groupToUse.Cores->at(0).ThingArn->c_str(),
                connectivityInfo.HostAddress->c_str(),
                (int)connectivityInfo.Port.value());

            connection = mqttClient.NewConnection(
                Aws::Iot::MqttClientConnectionConfigBuilder(certificatePath.c_str(), keyPath.c_str())
                    .WithCertificateAuthority(ByteCursorFromCString(groupToUse.CAs->at(0).c_str()))
                    .WithPortOverride(connectivityInfo.Port.value())
                    .WithEndpoint(connectivityInfo.HostAddress.value())
                    .Build());

            if (!connection)
            {
                fprintf(stderr, "Connection setup failed with error %s", ErrorDebugString(mqttClient.LastError()));
                exit(-1);
            }

            connection->OnConnectionCompleted = [&, connectivityInfo, groupToUse](
                                                    Mqtt::MqttConnection &conn,
                                                    int errorCode,
                                                    Mqtt::ReturnCode /*returnCode*/,
                                                    bool /*sessionPresent*/) {
                if (!errorCode)
                {
                    fprintf(
                        stdout,
                        "Connected to group %s, using connection to %s:%d\n",
                        groupToUse.GGGroupId->c_str(),
                        connectivityInfo.HostAddress->c_str(),
                        (int)connectivityInfo.Port.value());

                    if (mode == "both" || mode == "subscribe")
                    {
                        auto onMessage = [&](Mqtt::MqttConnection & /*connection*/,
                                             const String &receivedOnTopic,
                                             const ByteBuf &payload,
                                             bool /*dup*/,
                                             Mqtt::QOS /*qos*/,
                                             bool /*retain*/) {
                            fprintf(stdout, "Publish received on topic %s\n", receivedOnTopic.c_str());
                            fprintf(stdout, "Message: \n");
                            fwrite(payload.buffer, 1, payload.len, stdout);
                            fprintf(stdout, "\n");
                        };

                        auto onSubAck = [&](Mqtt::MqttConnection & /*connection*/,
                                            uint16_t /*packetId*/,
                                            const String &topic,
                                            Mqtt::QOS /*qos*/,
                                            int errorCode) {
                            if (!errorCode)
                            {
                                fprintf(stdout, "Successfully subscribed to %s\n", topic.c_str());
                                connectionFinishedPromise.set_value();
                            }
                            else
                            {
                                fprintf(
                                    stderr,
                                    "Failed to subscribe to %s with error %s. Exiting\n",
                                    topic.c_str(),
                                    aws_error_debug_str(errorCode));
                                exit(-1);
                            }
                        };

                        conn.Subscribe(topic.c_str(), AWS_MQTT_QOS_AT_MOST_ONCE, onMessage, onSubAck);
                    }
                    else
                    {
                        connectionFinishedPromise.set_value();
                    }
                }
                else
                {
                    fprintf(
                        stderr,
                        "Error connecting to group %s, using connection to %s:%d\n",
                        groupToUse.GGGroupId->c_str(),
                        connectivityInfo.HostAddress->c_str(),
                        (int)connectivityInfo.Port.value());
                    fprintf(stderr, "Error: %s\n", aws_error_debug_str(errorCode));
                    exit(-1);
                }
            };

            connection->OnConnectionInterrupted = [](Mqtt::MqttConnection &, int errorCode) {
                fprintf(stderr, "Connection interrupted with error %s\n", aws_error_debug_str(errorCode));
            };

            connection->OnConnectionResumed = [](Mqtt::MqttConnection & /*connection*/,
                                                 Mqtt::ReturnCode /*connectCode*/,
                                                 bool /*sessionPresent*/) { fprintf(stdout, "Connection resumed\n"); };

            connection->OnDisconnect = [&](Mqtt::MqttConnection & /*connection*/) {
                fprintf(stdout, "Connection disconnected. Shutting Down.....\n");
                shutdownCompletedPromise.set_value();
            };

            if (!connection->Connect(thingName.c_str(), false))
            {
                fprintf(stderr, "Connect failed with error %s\n", aws_error_debug_str(aws_last_error()));
                exit(-1);
            }
        }
        else
        {
            fprintf(
                stderr,
                "Discover failed with error: %s, and http response code %d\n",
                aws_error_debug_str(error),
                httpResponseCode);
            exit(-1);
        }
    });

    {
        connectionFinishedPromise.get_future().wait();
    }

    while (true)
    {
        String input;
        if (mode == "both" || mode == "publish")
        {
            fprintf(
                stdout,
                "Enter the message you want to publish to topic %s and press enter. Enter 'exit' to exit this "
                "program.\n",
                topic.c_str());
        }
        else
        {
            fprintf(stdout, "Enter exit or quit to exit the program.\n");
        }
        std::getline(std::cin, input);

        if (input == "exit" || input == "quit")
        {
            fprintf(stdout, "Exiting...");
            break;
        }

        if (mode == "both" || mode == "publish")
        {
            ByteBuf payload = ByteBufNewCopy(DefaultAllocator(), (const uint8_t *)input.data(), input.length());
            ByteBuf *payloadPtr = &payload;

            auto onPublishComplete = [payloadPtr](Mqtt::MqttConnection &, uint16_t packetId, int errorCode) {
                aws_byte_buf_clean_up(payloadPtr);

                if (packetId)
                {
                    fprintf(stdout, "Operation on packetId %d Succeeded\n", packetId);
                }
                else
                {
                    fprintf(stdout, "Operation failed with error %s\n", aws_error_debug_str(errorCode));
                }
            };
            connection->Publish(topic.c_str(), AWS_MQTT_QOS_AT_MOST_ONCE, false, payload, onPublishComplete);
        }
    }

    connection->Disconnect();
    {
        shutdownCompletedPromise.get_future().wait();
    }

    return 0;
}
