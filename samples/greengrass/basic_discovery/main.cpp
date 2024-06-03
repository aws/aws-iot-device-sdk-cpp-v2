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

#include "../../utils/CommandLineUtils.h"

using namespace Aws::Crt;
using namespace Aws::Discovery;

std::shared_ptr<Mqtt::MqttConnection> getMqttConnection(
    Aws::Iot::MqttClient &mqttClient,
    DiscoverResponse *response,
    Utils::cmdData &cmdData,
    std::promise<void> &shutdownCompletedPromise)
{
    std::shared_ptr<Mqtt::MqttConnection> connection;

    for (const auto &groupToUse : *response->GGGroups)
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
                Aws::Iot::MqttClientConnectionConfigBuilder(cmdData.input_cert.c_str(), cmdData.input_key.c_str())
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

            if (!connection->Connect(cmdData.input_thingName.c_str(), false))
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

int main(int argc, char *argv[])
{
    /************************ Setup ****************************/

    // Do the global initialization for the API.
    ApiHandle apiHandle;

    /**
     * cmdData is the arguments/input from the command line placed into a single struct for
     * use in this sample. This handles all of the command line parsing, validating, etc.
     * See the Utils/CommandLineUtils for more information.
     */
    Utils::cmdData cmdData = Utils::parseSampleInputGreengrassDiscovery(argc, argv, &apiHandle);

    // We're using Mutual TLS for MQTT, so we need to load our client certificates
    Io::TlsContextOptions tlsCtxOptions =
        Io::TlsContextOptions::InitClientWithMtls(cmdData.input_cert.c_str(), cmdData.input_key.c_str());

    if (!tlsCtxOptions)
    {
        fprintf(stderr, "TLS Context Options creation failed with error %s\n", ErrorDebugString(Aws::Crt::LastError()));
        exit(-1);
    }

    if (!cmdData.input_ca.empty())
    {
        tlsCtxOptions.OverrideDefaultTrustStore(nullptr, cmdData.input_ca.c_str());
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
    clientConfig.Region = cmdData.input_signingRegion;

    Aws::Crt::Http::HttpClientConnectionProxyOptions proxyOptions;
    if (cmdData.input_proxyHost.length() > 0 && cmdData.input_proxyPort != 0)
    {
        proxyOptions.HostName = cmdData.input_proxyHost;
        proxyOptions.Port = static_cast<uint32_t>(cmdData.input_proxyPort);
        clientConfig.ProxyOptions = proxyOptions;
    }

    /************************ Run the sample ****************************/

    fprintf(stdout, "Starting discovery\n");

    auto discoveryClient = DiscoveryClient::CreateClient(clientConfig);

    Aws::Iot::MqttClient mqttClient;
    std::shared_ptr<Mqtt::MqttConnection> connection(nullptr);

    std::promise<void> connectionFinishedPromise;
    std::promise<void> shutdownCompletedPromise;

    discoveryClient->Discover(
        cmdData.input_thingName, [&](DiscoverResponse *response, int error, int httpResponseCode) {
            fprintf(stdout, "Discovery completed with error code %d; http code %d\n", error, httpResponseCode);
            if (!error && response->GGGroups)
            {
                // Print the discovery response information and then exit. Does not use the discovery info.
                // (unless in CI, in which case just note it was successful and exit)
                if (cmdData.input_PrintDiscoverRespOnly)
                {
                    // Print the discovery response information and then exit (unless in CI, in which case just note it
                    // was successful)
                    if (!cmdData.input_isCI)
                    {
                        for (size_t a = 0; a < response->GGGroups->size(); a++)
                        {
                            auto tmpGroup = std::move(response->GGGroups->at(a));
                            fprintf(stdout, "Group ID: %s\n", tmpGroup.GGGroupId->c_str());
                            for (size_t b = 0; b < tmpGroup.Cores->size(); b++)
                            {
                                fprintf(stdout, "  Thing ARN: %s\n", tmpGroup.Cores->at(b).ThingArn->c_str());
                                for (size_t c = 0; c < tmpGroup.Cores->at(b).Connectivity->size(); c++)
                                {
                                    fprintf(
                                        stdout,
                                        "    Connectivity Host Address: %s\n",
                                        tmpGroup.Cores->at(b).Connectivity->at(c).HostAddress->c_str());
                                    fprintf(
                                        stdout,
                                        "    Connectivity Host Port: %d\n",
                                        tmpGroup.Cores->at(b).Connectivity->at(c).Port.value());
                                }
                            }
                        }
                    }
                    else
                    {
                        fprintf(
                            stdout,
                            "Received a greengrass discovery result! Not showing result in CI for possible data "
                            "sensitivity.\n");
                    }
                    exit(0);
                }

                connection = getMqttConnection(mqttClient, response, cmdData, shutdownCompletedPromise);
                if (connection)
                {
                    connectionFinishedPromise.set_value();
                }
                else
                {
                    fprintf(stderr, "All connection attempts failed\n");
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

    connectionFinishedPromise.get_future().wait();

    if (cmdData.input_mode == "both" || cmdData.input_mode == "subscribe")
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

        connection->Subscribe(cmdData.input_topic.c_str(), AWS_MQTT_QOS_AT_MOST_ONCE, onMessage, onSubAck);
    }

    bool first_input = true;
    while (true)
    {
        String input;
        if (cmdData.input_mode == "both" || cmdData.input_mode == "publish")
        {
            if (cmdData.input_message.empty())
            {
                fprintf(
                    stdout,
                    "Enter the message you want to publish to topic %s and press enter. Enter 'exit' to exit this "
                    "program.\n",
                    cmdData.input_topic.c_str());
                std::getline(std::cin, input);
                cmdData.input_message = input;
            }
            else if (!first_input)
            {
                fprintf(stdout, "Enter a new message or enter 'exit' or 'quit' to exit the program.\n");
                std::getline(std::cin, input);
                cmdData.input_message = input;
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

        if (cmdData.input_mode == "both" || cmdData.input_mode == "publish")
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
                cmdData.input_topic.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, false, payload, onPublishComplete);
        }
    }

    connection->Disconnect();
    {
        shutdownCompletedPromise.get_future().wait();
    }

    return 0;
}
