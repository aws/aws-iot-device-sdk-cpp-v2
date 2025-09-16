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
#include <thread>

using namespace Aws::Crt;
using namespace Aws::Discovery;

struct CmdArgs
{
    String endpoint;
    String cert;
    String key;
    String caFile;
    String thingName;
    String topic = "test/topic";
    String message;
    String mode = "both";
    String signingRegion = "us-east-1";
    String proxyHost;
    uint32_t proxyPort = 0;
    bool printDiscoverRespOnly = false;
};

static std::shared_ptr<Mqtt::MqttConnection> getMqttConnection(
    Aws::Iot::MqttClient &mqttClient,
    const Aws::Crt::Vector<GGGroup> &ggGroups,
    CmdArgs &cmdData,
    std::promise<void> &shutdownCompletedPromise)
{
    std::shared_ptr<Mqtt::MqttConnection> connection;

    for (const auto &groupToUse : ggGroups)
    {
        for (const auto &connectivityInfo : *groupToUse.Cores->at(0).Connectivity)
        {
            fprintf(
                stdout,
                "Connecting to group %s with thing arn %s, using endpoint %s:%d\n",
                groupToUse.GGGroupId->c_str(),
                groupToUse.Cores->at(0).ThingArn->c_str(),
                connectivityInfo.HostAddress->c_str(),
                (int)connectivityInfo.Port.value());

            connection = mqttClient.NewConnection(
                Aws::Iot::MqttClientConnectionConfigBuilder(cmdData.cert.c_str(), cmdData.key.c_str())
                    .WithCertificateAuthority(ByteCursorFromCString(groupToUse.CAs->at(0).c_str()))
                    .WithPortOverride(connectivityInfo.Port.value())
                    .WithEndpoint(connectivityInfo.HostAddress.value())
                    .WithTcpConnectTimeout(3000)
                    .Build());

            if (!connection)
            {
                fprintf(stderr, "Connection setup failed with error %s", ErrorDebugString(mqttClient.LastError()));
                continue;
            }

            std::promise<bool> connectPromise;

            connection->OnConnectionCompleted = [&connectPromise, connectivityInfo, groupToUse](
                                                    Mqtt::MqttConnection & /*connection*/,
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
                    connectPromise.set_value(true);
                }
                else
                {
                    fprintf(
                        stderr,
                        "Error connecting to group %s (thing %s) using connection to %s:%d\n",
                        groupToUse.GGGroupId->c_str(),
                        groupToUse.Cores->at(0).ThingArn->c_str(),
                        connectivityInfo.HostAddress->c_str(),
                        (int)connectivityInfo.Port.value());
                    fprintf(stderr, "Error: %s\n", aws_error_debug_str(errorCode));
                    connectPromise.set_value(false);
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

            if (!connection->Connect(cmdData.thingName.c_str(), false))
            {
                fprintf(stderr, "Connect failed with error %s\n", aws_error_debug_str(aws_last_error()));
                continue;
            }

            auto connectFuture = connectPromise.get_future();

            if (connectFuture.get())
            {
                return connection;
            }
        }
    }
    return nullptr;
}

static void printGreengrassResponse(const Aws::Crt::Vector<GGGroup> &ggGroups)
{
    for (const auto &ggGroup : ggGroups)
    {
        fprintf(stdout, "Group ID: %s\n", ggGroup.GGGroupId->c_str());
        for (const auto &ggCore : *ggGroup.Cores)
        {
            fprintf(stdout, "  Thing ARN: %s\n", ggCore.ThingArn->c_str());
            for (const auto &connectivity : *ggCore.Connectivity)
            {
                fprintf(stdout, "    Connectivity Host Address: %s\n", connectivity.HostAddress->c_str());
                fprintf(stdout, "    Connectivity Host Port: %u\n", connectivity.Port.value());
            }
        }
    }
}

void printHelp()
{
    printf("Greengrass Discovery Sample\n");
    printf("options:\n");
    printf("  --help        show this help message and exit\n");
    printf("required arguments:\n");
    printf("  --cert        Path to the certificate file\n");
    printf("  --key         Path to the private key file\n");
    printf("  --thing_name  Thing name\n");
    printf("optional arguments:\n");
    printf("  --client_id   Client ID (default: test-<uuid>)\n");
    printf("  --ca_file     Path to optional CA bundle (PEM)\n");
    printf("  --topic       Topic (default: test/topic)\n");
    printf("  --message     Message to publish\n");
    printf("  --mode        Mode: publish, subscribe, both (default: both)\n");
    printf("  --signing_region  Signing region (default: us-east-1)\n");
    printf("  --proxy_host  HTTP proxy host\n");
    printf("  --proxy_port  HTTP proxy port\n");
    printf("  --print_discover_resp_only  Print discovery response only\n");
}

CmdArgs parseArgs(int argc, char *argv[])
{
    CmdArgs args;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--help") == 0)
        {
            printHelp();
            exit(0);
        }
        else if (i < argc - 1)
        {
            if (strcmp(argv[i], "--cert") == 0)
            {
                args.cert = argv[++i];
            }
            else if (strcmp(argv[i], "--key") == 0)
            {
                args.key = argv[++i];
            }
            else if (strcmp(argv[i], "--thing_name") == 0)
            {
                args.thingName = argv[++i];
            }
            else if (strcmp(argv[i], "--ca_file") == 0)
            {
                args.caFile = argv[++i];
            }
            else if (strcmp(argv[i], "--topic") == 0)
            {
                args.topic = argv[++i];
            }
            else if (strcmp(argv[i], "--message") == 0)
            {
                args.message = argv[++i];
            }
            else if (strcmp(argv[i], "--mode") == 0)
            {
                args.mode = argv[++i];
            }
            else if (strcmp(argv[i], "--signing_region") == 0)
            {
                args.signingRegion = argv[++i];
            }
            else if (strcmp(argv[i], "--proxy_host") == 0)
            {
                args.proxyHost = argv[++i];
            }
            else if (strcmp(argv[i], "--proxy_port") == 0)
            {
                args.proxyPort = atoi(argv[++i]);
            }
            else
            {
                fprintf(stderr, "Unknown argument: %s\n", argv[i]);
                printHelp();
                exit(1);
            }
        }
        else if (strcmp(argv[i], "--print_discover_resp_only") == 0)
        {
            args.printDiscoverRespOnly = true;
        }
    }
    if (args.cert.empty() || args.key.empty() || args.thingName.empty())
    {
        fprintf(stderr, "Error: --cert, --key, and --thing_name are required\n");
        printHelp();
        exit(1);
    }
    return args;
}

int main(int argc, char *argv[])
{
    /************************ Setup ****************************/

    // Parse command line arguments
    CmdArgs cmdData = parseArgs(argc, argv);

    // Do the global initialization for the API.
    ApiHandle apiHandle;

    // We're using Mutual TLS for MQTT, so we need to load our client certificates
    Io::TlsContextOptions tlsCtxOptions =
        Io::TlsContextOptions::InitClientWithMtls(cmdData.cert.c_str(), cmdData.key.c_str());

    if (!tlsCtxOptions)
    {
        fprintf(stderr, "TLS Context Options creation failed with error %s\n", ErrorDebugString(Aws::Crt::LastError()));
        exit(-1);
    }

    if (!cmdData.caFile.empty())
    {
        tlsCtxOptions.OverrideDefaultTrustStore(nullptr, cmdData.caFile.c_str());
    }

    Io::TlsContext tlsCtx(tlsCtxOptions, Io::TlsMode::CLIENT);

    if (!tlsCtx)
    {
        fprintf(stderr, "Tls Context creation failed with error %s\n", ErrorDebugString(LastErrorOrUnknown()));
        exit(-1);
    }

    // Default Socket options to use. IPV4 will be ignored based on what DNS tells us.
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
    clientConfig.Region = cmdData.signingRegion;

    Aws::Crt::Http::HttpClientConnectionProxyOptions proxyOptions;
    if (cmdData.proxyHost.length() > 0 && cmdData.proxyPort != 0)
    {
        proxyOptions.HostName = cmdData.proxyHost;
        proxyOptions.Port = cmdData.proxyPort;
        clientConfig.ProxyOptions = proxyOptions;
    }

    /************************ Run the sample ****************************/

    fprintf(stdout, "Starting discovery\n");

    auto discoveryClient = DiscoveryClient::CreateClient(clientConfig);

    Aws::Iot::MqttClient mqttClient;
    std::shared_ptr<Mqtt::MqttConnection> connection(nullptr);

    DiscoverResponse discoverResponse;

    std::promise<bool> discoveryStatusPromise;
    std::promise<void> shutdownCompletedPromise;

    // Initiate gathering a list of discoverable Greengrass cores.
    // NOTE: This is an asynchronous operation, so it completes before the results are actually ready. You need to use
    // synchronization techniques to obtain its results. For simplicity, we use promise/future in this sample.
    discoveryClient->Discover(
        cmdData.thingName,
        [&discoverResponse, &discoveryStatusPromise](DiscoverResponse *response, int error, int httpResponseCode) {
            fprintf(stdout, "Discovery completed with error code %d; http code %d\n", error, httpResponseCode);

            if (!error && response->GGGroups.has_value())
            {
                discoverResponse = *response;
                discoveryStatusPromise.set_value(true);
            }
            else
            {
                fprintf(
                    stderr,
                    "Discover failed with error: %s, and http response code %d\n",
                    aws_error_debug_str(error),
                    httpResponseCode);
                discoveryStatusPromise.set_value(false);
            }
        });

    // Wait for the discovery process to return actual discovery results, or error if something went wrong.
    auto isDiscoverySucceeded = discoveryStatusPromise.get_future().get();
    if (!isDiscoverySucceeded)
    {
        return 1;
    }

    // Print the discovery response information and then exit. Does not use the discovery info.
    if (cmdData.printDiscoverRespOnly)
    {
        printGreengrassResponse(*discoverResponse.GGGroups);
        return 0;
    }

    // Try to establish a connection with one of the discovered Greengrass cores.
    connection = getMqttConnection(mqttClient, *discoverResponse.GGGroups, cmdData, shutdownCompletedPromise);
    if (!connection)
    {
        fprintf(stderr, "All connection attempts failed\n");
        exit(-1);
    }

    // Now, with the established connection to a Greengrass core, we can perform MQTT-related actions.
    if (cmdData.mode == "both" || cmdData.mode == "subscribe")
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

        connection->Subscribe(cmdData.topic.c_str(), AWS_MQTT_QOS_AT_MOST_ONCE, onMessage, onSubAck);
    }

    bool first_input = true;
    while (true)
    {
        String input;
        if (cmdData.mode == "both" || cmdData.mode == "publish")
        {
            if (cmdData.message.empty())
            {
                fprintf(
                    stdout,
                    "Enter the message you want to publish to topic %s and press enter. Enter 'exit' to exit this "
                    "program.\n",
                    cmdData.topic.c_str());
                std::getline(std::cin, input);
                cmdData.message = input;
            }
            else if (!first_input)
            {
                fprintf(stdout, "Enter a new message or enter 'exit' or 'quit' to exit the program.\n");
                std::getline(std::cin, input);
                cmdData.message = input;
            }
            first_input = false;
        }
        else
        {
            fprintf(stdout, "Enter 'exit' or 'quit' to exit the program.\n");
            std::getline(std::cin, input);
        }

        if (input == "exit" || input == "quit")
        {
            fprintf(stdout, "Exiting...\n");
            break;
        }

        if (cmdData.mode == "both" || cmdData.mode == "publish")
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
            connection->Publish(
                cmdData.topic.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, false, payload, onPublishComplete);
        }
    }

    connection->Disconnect();
    {
        shutdownCompletedPromise.get_future().wait();
    }

    return 0;
}
