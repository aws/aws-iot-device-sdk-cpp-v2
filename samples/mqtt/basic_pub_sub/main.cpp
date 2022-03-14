/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/StlAllocator.h>
#include <aws/crt/auth/Credentials.h>
#include <aws/crt/io/TlsOptions.h>

#include <aws/iot/MqttClient.h>

#include <algorithm>
#include <aws/crt/UUID.h>
#include <chrono>
#include <condition_variable>
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
    String signingRegion;
    String proxyHost;
    uint16_t proxyPort(8080);

    String x509Endpoint;
    String x509ThingName;
    String x509RoleAlias;
    String x509CertificatePath;
    String x509KeyPath;
    String x509RootCAFile;

    bool useWebSocket = false;
    bool useX509 = false;

    uint32_t messageCount = 10;
    String messagePayload("Hello world!");

    /*********************** Parse Arguments ***************************/
    Utils::CommandLineUtils cmdUtils = Utils::CommandLineUtils();
    cmdUtils.RegisterProgramName("basic_pub_sub");
    cmdUtils.AddCommonMQTTCommands();
    cmdUtils.AddCommonProxyCommands();
    cmdUtils.AddCommonTopicMessageCommands();
    cmdUtils.AddCommonX509Commands();
    cmdUtils.AddCommonWebsocketCommands();
    cmdUtils.UpdateCommandHelp(
        "key", "Path to your key in PEM format. If this is not set you must specify use_websocket");
    cmdUtils.UpdateCommandHelp(
        "cert", "Path to your client certificate in PEM format. If this is not set you must specify use_websocket");
    cmdUtils.RegisterCommand(
        "client_id",
        "<str>"
        "Client id to use (optional, default='test-*')");
    cmdUtils.RegisterCommand("count", "<int>", "The number of messages to send (optional, default='10')");
    cmdUtils.RegisterCommand("help", "", "Prints this message");
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
    if (keyPath.empty() != certificatePath.empty())
    {
        cmdUtils.PrintHelp();
        fprintf(stdout, "Using mtls (cert and key) requires both the certificate and private key\n");
        return 1;
    }
    topic = cmdUtils.GetCommandOrDefault("topic", topic);
    caFile = cmdUtils.GetCommandOrDefault("ca_file", caFile);
    clientId = cmdUtils.GetCommandOrDefault("client_id", clientId);
    if (cmdUtils.HasCommand("use_websocket"))
    {
        useWebSocket = true;
        signingRegion =
            cmdUtils.GetCommandRequired("signing_region", "Websockets require a signing region to be specified.");
    }
    proxyHost = cmdUtils.GetCommandOrDefault("proxy_host", proxyHost);
    if (cmdUtils.HasCommand("prox_port"))
    {
        int port = atoi(cmdUtils.GetCommand("proxy_port").c_str());
        if (port > 0 && port < UINT16_MAX)
        {
            proxyPort = static_cast<uint16_t>(port);
        }
    }
    bool usingMtls = !certificatePath.empty() && !keyPath.empty();

    /* one or the other, but not both nor neither */
    if (useWebSocket == usingMtls)
    {
        cmdUtils.PrintHelp();
        if (useWebSocket && usingMtls)
        {
            fprintf(stdout, "You must use either websockets or mtls for authentication, but not both.\n");
        }
        else
        {
            fprintf(stdout, "You must use either websockets or mtls for authentication.\n");
        }
        return 1;
    }

    // x509 credentials provider configuration
    if (cmdUtils.HasCommand("x509"))
    {
        if (!useWebSocket)
        {
            cmdUtils.PrintHelp();
            fprintf(stdout, "X509 credentials sourcing requires websockets to be enabled and configured.\n");
            return 1;
        }
        x509RoleAlias = cmdUtils.GetCommandRequired(
            "x509_role_alias", "X509 credentials sourcing requires an x509 role alias to be specified.");
        x509Endpoint = cmdUtils.GetCommandRequired(
            "x509_endpoint", "X509 credentials sourcing requires an x509 endpoint to be specified.");
        x509ThingName = cmdUtils.GetCommandRequired(
            "x509_thing", "X509 credentials sourcing requires an x509 thing name to be specified.");
        x509CertificatePath = cmdUtils.GetCommandRequired(
            "x509_cert", "X509 credentials sourcing requires an IoT certificate to be specified.");
        x509KeyPath = cmdUtils.GetCommandRequired(
            "x509_key", "X509 credentials sourcing requires an IoT thing private key to be specified.");
        x509RootCAFile = cmdUtils.GetCommandOrDefault("x509_ca_file", x509RootCAFile);
    }

    messagePayload = cmdUtils.GetCommandOrDefault("message", messagePayload);
    if (cmdUtils.HasCommand("count"))
    {
        int count = atoi(cmdUtils.GetCommand("count").c_str());
        if (count > 0)
        {
            messageCount = count;
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

    Aws::Crt::Http::HttpClientConnectionProxyOptions proxyOptions;
    if (!proxyHost.empty())
    {
        proxyOptions.HostName = proxyHost;
        proxyOptions.Port = proxyPort;
        proxyOptions.AuthType = Aws::Crt::Http::AwsHttpProxyAuthenticationType::None;
    }

    Aws::Crt::Io::TlsContext x509TlsCtx;
    Aws::Iot::MqttClientConnectionConfigBuilder builder;

    if (!certificatePath.empty() && !keyPath.empty())
    {
        builder = Aws::Iot::MqttClientConnectionConfigBuilder(certificatePath.c_str(), keyPath.c_str());
    }
    else if (useWebSocket)
    {
        std::shared_ptr<Aws::Crt::Auth::ICredentialsProvider> provider = nullptr;
        if (useX509)
        {
            Aws::Crt::Io::TlsContextOptions tlsCtxOptions =
                Aws::Crt::Io::TlsContextOptions::InitClientWithMtls(x509CertificatePath.c_str(), x509KeyPath.c_str());
            if (!tlsCtxOptions)
            {
                fprintf(
                    stderr,
                    "Unable to initialize tls context options, error: %s!\n",
                    ErrorDebugString(tlsCtxOptions.LastError()));
                return -1;
            }

            if (!x509RootCAFile.empty())
            {
                tlsCtxOptions.OverrideDefaultTrustStore(nullptr, x509RootCAFile.c_str());
            }

            x509TlsCtx = Aws::Crt::Io::TlsContext(tlsCtxOptions, Aws::Crt::Io::TlsMode::CLIENT);
            if (!x509TlsCtx)
            {
                fprintf(
                    stderr,
                    "Unable to create tls context, error: %s!\n",
                    ErrorDebugString(x509TlsCtx.GetInitializationError()));
                return -1;
            }

            Aws::Crt::Auth::CredentialsProviderX509Config x509Config;

            x509Config.TlsOptions = x509TlsCtx.NewConnectionOptions();
            if (!x509Config.TlsOptions)
            {
                fprintf(
                    stderr,
                    "Unable to create tls options from tls context, error: %s!\n",
                    ErrorDebugString(x509Config.TlsOptions.LastError()));
                return -1;
            }

            x509Config.Endpoint = x509Endpoint;
            x509Config.RoleAlias = x509RoleAlias;
            x509Config.ThingName = x509ThingName;

            if (!proxyHost.empty())
            {
                x509Config.ProxyOptions = proxyOptions;
            }

            provider = Aws::Crt::Auth::CredentialsProvider::CreateCredentialsProviderX509(x509Config);
        }
        else
        {
            Aws::Crt::Auth::CredentialsProviderChainDefaultConfig defaultConfig;
            provider = Aws::Crt::Auth::CredentialsProvider::CreateCredentialsProviderChainDefault(defaultConfig);
        }

        if (!provider)
        {
            fprintf(stderr, "Failure to create credentials provider!\n");
            return -1;
        }

        Aws::Iot::WebsocketConfig config(signingRegion, provider);
        builder = Aws::Iot::MqttClientConnectionConfigBuilder(config);
    }
    else
    {
        cmdUtils.PrintHelp();
    }

    if (!proxyHost.empty())
    {
        builder.WithHttpProxyOptions(proxyOptions);
    }

    if (!caFile.empty())
    {
        builder.WithCertificateAuthority(caFile.c_str());
    }

    builder.WithEndpoint(endpoint);

    auto clientConfig = builder.Build();

    if (!clientConfig)
    {
        fprintf(
            stderr,
            "Client Configuration initialization failed with error %s\n",
            ErrorDebugString(clientConfig.LastError()));
        exit(-1);
    }

    Aws::Iot::MqttClient mqttClient;
    /*
     * Since no exceptions are used, always check the bool operator
     * when an error could have occurred.
     */
    if (!mqttClient)
    {
        fprintf(stderr, "MQTT Client Creation failed with error %s\n", ErrorDebugString(mqttClient.LastError()));
        exit(-1);
    }

    /*
     * Now create a connection object. Note: This type is move only
     * and its underlying memory is managed by the client.
     */
    auto connection = mqttClient.NewConnection(clientConfig);

    if (!connection)
    {
        fprintf(stderr, "MQTT Connection Creation failed with error %s\n", ErrorDebugString(mqttClient.LastError()));
        exit(-1);
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
        std::mutex receiveMutex;
        std::condition_variable receiveSignal;
        uint32_t receivedCount = 0;

        /*
         * This is invoked upon the receipt of a Publish on a subscribed topic.
         */
        auto onMessage = [&](Mqtt::MqttConnection &,
                             const String &topic,
                             const ByteBuf &byteBuf,
                             bool /*dup*/,
                             Mqtt::QOS /*qos*/,
                             bool /*retain*/) {
            {
                std::lock_guard<std::mutex> lock(receiveMutex);
                ++receivedCount;
                fprintf(stdout, "Publish #%d received on topic %s\n", receivedCount, topic.c_str());
                fprintf(stdout, "Message: ");
                fwrite(byteBuf.buffer, 1, byteBuf.len, stdout);
                fprintf(stdout, "\n");
            }

            receiveSignal.notify_all();
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

        uint32_t publishedCount = 0;
        while (publishedCount < messageCount)
        {
            ByteBuf payload = ByteBufFromArray((const uint8_t *)messagePayload.data(), messagePayload.length());

            auto onPublishComplete = [](Mqtt::MqttConnection &, uint16_t, int) {};
            connection->Publish(topic.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, false, payload, onPublishComplete);
            ++publishedCount;

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        {
            std::unique_lock<std::mutex> receivedLock(receiveMutex);
            receiveSignal.wait(receivedLock, [&] { return receivedCount >= messageCount; });
        }

        /*
         * Unsubscribe from the topic.
         */
        std::promise<void> unsubscribeFinishedPromise;
        connection->Unsubscribe(
            topic.c_str(), [&](Mqtt::MqttConnection &, uint16_t, int) { unsubscribeFinishedPromise.set_value(); });
        unsubscribeFinishedPromise.get_future().wait();

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
