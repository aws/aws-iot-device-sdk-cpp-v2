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

using namespace Aws::Crt;
using namespace Aws::Discovery;

static void s_printHelp()
{
    fprintf(stdout, "Usage:\n");
    fprintf(
        stdout,
        "basic-discovery --region <optional: region> --cert <path to cert>"
        " --key <path to key> --ca_file <optional: path to custom ca>"
        " --thing_name <thing name> --topic <optional: topic> "
        " --mode <optional: both|publish|subscribe> --message <optional: message to publish>"
        " --proxy-host <optional: proxy host name> --proxy-port <optional: proxy port>\n\n");
    fprintf(stdout, "region: the region for your green grass groups, default us-east-1\n");
    fprintf(stdout, "cert: path to your client certificate in PEM format\n");
    fprintf(stdout, "key: path to your key in PEM format\n");
    fprintf(stdout, "ca_file: ca file to use in verifying TLS connections.\n");
    fprintf(stdout, "\tIt's the path to a CA file in PEM format\n");
    fprintf(stdout, "thing_name: the name of your IOT thing\n");
    fprintf(stdout, "topic: targeted topic. Default is test/topic\n");
    fprintf(stdout, "mode: default both\n");
    fprintf(stdout, "message: message to publish. default 'Hello World'\n");
    fprintf(stdout, "proxy-host: proxy host to use for discovery call. Default is to not use a proxy.\n");
    fprintf(stdout, "proxy-port: proxy port to use for discovery call.\n");
}

bool s_cmdOptionExists(char **begin, char **end, const String &option)
{
    return std::find(begin, end, option) != end;
}

char *s_getCmdOption(char **begin, char **end, const String &option)
{
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

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
    if (!(s_cmdOptionExists(argv, argv + argc, "--cert") && s_cmdOptionExists(argv, argv + argc, "--key") &&
          s_cmdOptionExists(argv, argv + argc, "--thing_name")))
    {
        s_printHelp();
        return 0;
    }

    certificatePath = s_getCmdOption(argv, argv + argc, "--cert");
    keyPath = s_getCmdOption(argv, argv + argc, "--key");
    thingName = s_getCmdOption(argv, argv + argc, "--thing_name");

    if (s_cmdOptionExists(argv, argv + argc, "--ca_file"))
    {
        caFile = s_getCmdOption(argv, argv + argc, "--ca_file");
    }

    if (s_cmdOptionExists(argv, argv + argc, "--region"))
    {
        region = s_getCmdOption(argv, argv + argc, "--region");
    }

    if (s_cmdOptionExists(argv, argv + argc, "--topic"))
    {
        topic = s_getCmdOption(argv, argv + argc, "--topic");
    }

    if (s_cmdOptionExists(argv, argv + argc, "--mode"))
    {
        mode = s_getCmdOption(argv, argv + argc, "--mode");
    }

    if (s_cmdOptionExists(argv, argv + argc, "--message"))
    {
        message = s_getCmdOption(argv, argv + argc, "--message");
    }

    if (s_cmdOptionExists(argv, argv + argc, "--proxy-host"))
    {
        proxyHost = s_getCmdOption(argv, argv + argc, "--proxy-host");
    }

    if (s_cmdOptionExists(argv, argv + argc, "--proxy-port"))
    {
        String portString = s_getCmdOption(argv, argv + argc, "--proxy-port");
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
