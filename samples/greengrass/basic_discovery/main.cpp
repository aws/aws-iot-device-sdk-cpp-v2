/*
 * Copyright 2010-2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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
        " --key <path to key> --ca_file <path to custom ca>"
        " --thing_name <thing name> --topic <optional: topic> "
        " --mode <optional: both|publish|subscribe> --message <optional: message to publish>\n\n");
    fprintf(stdout, "region: the region for your green grass groups, default us-east-1\n");
    fprintf(stdout, "cert: path to your client certificate in PEM format\n");
    fprintf(stdout, "key: path to your key in PEM format\n");
    fprintf(stdout, "ca_file: ca file to use in verifying TLS connections.\n");
    fprintf(stdout, "\tIt's the path to a CA file in PEM format\n");
    fprintf(stdout, "thing_name: the name of your IOT thing\n");
    fprintf(stdout, "topic: targeted topic. Default is sdk/test/cpp-v2\n");
    fprintf(stdout, "mode: default both\n");
    fprintf(stdout, "message: message to publish. default 'Hello World'\n");
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
     * These make debug output via ErrorDebugString() work.
     */
    LoadErrorStrings();

    /*
     * Do the global initialization for the API.
     */
    ApiHandle apiHandle;

    String region("us-east-1");
    String certificatePath;
    String keyPath;
    String caFile;
    String thingName;
    String topic("sdk/test/cpp-v2");
    String mode("both");
    String message("Hello World");

    /*********************** Parse Arguments ***************************/
    if (!(s_cmdOptionExists(argv, argv + argc, "--cert") && s_cmdOptionExists(argv, argv + argc, "--key") &&
          s_cmdOptionExists(argv, argv + argc, "--thing_name") && s_cmdOptionExists(argv, argv + argc, "--ca_file")))
    {
        s_printHelp();
        return 0;
    }

    certificatePath = s_getCmdOption(argv, argv + argc, "--cert");
    keyPath = s_getCmdOption(argv, argv + argc, "--key");
    thingName = s_getCmdOption(argv, argv + argc, "--thing_name");
    caFile = s_getCmdOption(argv, argv + argc, "--ca_file");

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

    Io::EventLoopGroup eventLoopGroup(1);
    if (!eventLoopGroup)
    {
        fprintf(
            stderr, "Event Loop Group Creation failed with error %s\n", ErrorDebugString(eventLoopGroup.LastError()));
        exit(-1);
    }
    /*
     * We're using Mutual TLS for Mqtt, so we need to load our client certificates
     */
    Io::TlsContextOptions tlsCtxOptions =
        Io::TlsContextOptions::InitClientWithMtls(certificatePath.c_str(), keyPath.c_str());

    tlsCtxOptions.OverrideDefaultTrustStore(nullptr, caFile.c_str());
    Io::TlsContext tlsCtx(tlsCtxOptions, Io::TlsMode::CLIENT);

    if (!tlsCtx)
    {
        fprintf(stderr, "Tls Context creation failed with error %s\n", ErrorDebugString(tlsCtx.LastError()));
        exit(-1);
    }

    /*
     * Default Socket options to use. IPV4 will be ignored based on what DNS
     * tells us.
     */
    Io::SocketOptions socketOptions;
    socketOptions.connect_timeout_ms = 3000;
    socketOptions.domain = AWS_SOCKET_IPV4;
    socketOptions.type = AWS_SOCKET_STREAM;
    socketOptions.keep_alive_interval_sec = 0;
    socketOptions.keep_alive_timeout_sec = 0;
    socketOptions.keepalive = false;

    Io::DefaultHostResolver hostResolver(eventLoopGroup, 64, 30);
    Io::ClientBootstrap bootstrap(eventLoopGroup, hostResolver);

    if (!bootstrap)
    {
        fprintf(stderr, "ClientBootstrap failed with error %s\n", ErrorDebugString(bootstrap.LastError()));
        exit(-1);
    }

    DiscoveryClientConfig clientConfig;
    clientConfig.bootstrap = &bootstrap;
    clientConfig.socketOptions = &socketOptions;
    clientConfig.tlsContext = &tlsCtx;
    clientConfig.region = region;

    DiscoveryClient discoveryClient(clientConfig);

    GGGroup groupToUse;
    Aws::Iot::MqttClientConnectionConfigBuilder connectionConfigBuilder(certificatePath.c_str(), keyPath.c_str());
    Aws::Iot::MqttClientConnectionConfig connectionConfig;

    Aws::Iot::MqttClient mqttClient(bootstrap);
    std::shared_ptr<Mqtt::MqttConnection> connection(nullptr);

    std::mutex semaphoreLock;
    std::condition_variable semaphore;
    std::atomic<bool> connectionFinished(false);
    std::atomic<bool> shutdownCompleted(false);

    discoveryClient.Discover(thingName, [&](DiscoverResponse *response, int error, int httpResponseCode) {
        if (!error && response->GGGroups)
        {
            groupToUse = std::move(response->GGGroups->at(0));
            ByteCursor caCursor = ByteCursorFromArray(
                reinterpret_cast<const uint8_t *>(groupToUse.CAs->at(0).data()), groupToUse.CAs->at(0).length());
            connectionConfigBuilder.WithCertificateAuthority(caCursor);

            auto connectivityInfo = groupToUse.Cores->at(0).Connectivity->at(0);

            fprintf(
                stdout,
                "Connecting to group %s with thing arn %s, using endpoint %s:%d\n",
                groupToUse.GGGroupId->c_str(),
                groupToUse.Cores->at(0).ThingArn->c_str(),
                connectivityInfo.HostAddress->c_str(),
                (int)connectivityInfo.Port.value());

            connectionConfigBuilder.WithPortOverride(connectivityInfo.Port.value());
            connectionConfigBuilder.WithEndpoint(connectivityInfo.HostAddress.value());
            connectionConfig = connectionConfigBuilder.Build();
            connection = mqttClient.NewConnection(connectionConfig);

            connection->OnConnectionCompleted =
                [&, connectivityInfo](
                    Mqtt::MqttConnection &conn, int errorCode, Mqtt::ReturnCode returnCode, bool sessionPresent) {
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
                            auto onPublish = [&](Mqtt::MqttConnection &connection,
                                                 const String &receivedOnTopic,
                                                 const ByteBuf &payload) {
                                fprintf(stdout, "Publish received on topic %s\n", receivedOnTopic.c_str());
                                fprintf(stdout, "Message: \n");
                                fwrite(payload.buffer, 1, payload.len, stdout);
                                fprintf(stdout, "\n");
                            };

                            auto onSubAck = [&](Mqtt::MqttConnection &connection,
                                                uint16_t packetId,
                                                const String &topic,
                                                Mqtt::QOS qos,
                                                int errorCode) {
                                if (!errorCode)
                                {
                                    fprintf(stdout, "Successfully subscribed to %s\n", topic.c_str());
                                    connectionFinished = true;
                                    semaphore.notify_one();
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

                            conn.Subscribe(topic.c_str(), AWS_MQTT_QOS_AT_MOST_ONCE, onPublish, onSubAck);
                        }
                        else
                        {
                            connectionFinished = true;
                            semaphore.notify_one();
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

            connection->OnConnectionResumed = [](Mqtt::MqttConnection &,
                                                 Mqtt::ReturnCode connectCode,
                                                 bool sessionPresent) { fprintf(stdout, "Connection resumed\n"); };

            connection->OnDisconnect = [&](Mqtt::MqttConnection &connection) {
                fprintf(stdout, "Connection disconnected. Shutting Down.....\n");
                shutdownCompleted = true;
                semaphore.notify_one();
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
        std::unique_lock<std::mutex> lock(semaphoreLock);
        semaphore.wait(lock, [&]() { return connectionFinished.load(); });
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
        std::unique_lock<std::mutex> lock(semaphoreLock);
        semaphore.wait(lock, [&]() { return shutdownCompleted.load(); });
    }

    return 0;
}
