/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/StlAllocator.h>
#include <aws/crt/auth/Credentials.h>
#include <aws/crt/http/HttpProxyStrategy.h>
#include <aws/crt/io/TlsOptions.h>

#include <aws/iot/MqttClient.h>

#include <algorithm>
#include <aws/crt/UUID.h>
#include <condition_variable>
#include <iostream>
#include <mutex>

using namespace Aws::Crt;

static void s_printHelp()
{
    fprintf(stdout, "Usage:\n");
    fprintf(
        stdout,
        "basic-pub-sub --endpoint <endpoint> --cert <path to cert>"
        " --key <path to key> --topic <topic> --ca_file <optional: path to custom ca>"
        " --use_websocket --signing_region <region> --proxy_host <host> --proxy_port <port>"
        " --proxy_username <username> --proxy_password <password>"
        " --x509 --x509_role_alias <role_alias> --x509_endpoint <endpoint> --x509_thing <thing_name>"
        " --x509_cert <path to cert> --x509_key <path to key> --x509_rootca <path to root ca>\n\n");
    fprintf(stdout, "endpoint: the endpoint of the mqtt server not including a port\n");
    fprintf(
        stdout,
        "cert: path to your client certificate in PEM format. If this is not set you must specify use_websocket\n");
    fprintf(stdout, "key: path to your key in PEM format. If this is not set you must specify use_websocket\n");
    fprintf(stdout, "topic: topic to publish, subscribe to. (optional)\n");
    fprintf(stdout, "client_id: client id to use (optional)\n");
    fprintf(
        stdout,
        "ca_file: Optional, if the mqtt server uses a certificate that's not already"
        " in your trust store, set this.\n");
    fprintf(stdout, "\tIt's the path to a CA file in PEM format\n");
    fprintf(stdout, "use_websocket: if specified, uses a websocket over https (optional)\n");
    fprintf(
        stdout,
        "signing_region: used for websocket signer it should only be specific if websockets are used. (required for "
        "websockets)\n");
    fprintf(stdout, "proxy_host: if you want to use a proxy with websockets, specify the host here (optional).\n");
    fprintf(
        stdout, "proxy_port: defaults to 8080 is proxy_host is set. Set this to any value you'd like (optional).\n");
    fprintf(stdout, "proxy_username: (basic authentication) username to use when connecting to proxy (optional).\n");
    fprintf(stdout, "proxy_username: (basic authentication) password to use when connecting to proxy (optional).\n");
    fprintf(stdout, "  x509: Use the x509 credentials provider while using websockets (optional)\n");
    fprintf(stdout, "  x509_role_alias: Role alias to use with the x509 credentials provider (required for x509)\n");
    fprintf(stdout, "  x509_endpoint: Endpoint to fetch x509 credentials from (required for x509)\n");
    fprintf(stdout, "  x509_thing: Thing name to fetch x509 credentials on behalf of (required for x509)\n");
    fprintf(
        stdout,
        "  x509_cert: Path to the IoT thing certificate used in fetching x509 credentials (required for x509)\n");
    fprintf(
        stdout,
        "  x509_key: Path to the IoT thing private key used in fetching x509 credentials (required for x509)\n");
    fprintf(
        stdout,
        "  x509_rootca: Path to the root certificate used in fetching x509 credentials (required for x509)\n\n");
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
    apiHandle.InitializeLogging(Aws::Crt::LogLevel::Debug, "/tmp/log.txt");

    String endpoint;
    String certificatePath;
    String keyPath;
    String caFile;
    String topic("test/topic");
    String clientId(String("test-") + Aws::Crt::UUID().ToString());
    String signingRegion;
    String proxyHost;
    uint16_t proxyPort(8080);
    String proxyUsername;
    String proxyPassword;
    String proxyUsertoken;

    String x509Endpoint;
    String x509ThingName;
    String x509RoleAlias;
    String x509CertificatePath;
    String x509KeyPath;
    String x509RootCAFile;

    bool useWebSocket = false;
    bool useX509 = false;

    /*********************** Parse Arguments ***************************/
    if (!s_cmdOptionExists(argv, argv + argc, "--endpoint"))
    {
        s_printHelp();
        return 1;
    }

    endpoint = s_getCmdOption(argv, argv + argc, "--endpoint");

    if (s_cmdOptionExists(argv, argv + argc, "--key"))
    {
        keyPath = s_getCmdOption(argv, argv + argc, "--key");
    }

    if (s_cmdOptionExists(argv, argv + argc, "--cert"))
    {
        certificatePath = s_getCmdOption(argv, argv + argc, "--cert");
    }

    if (keyPath.empty() != certificatePath.empty())
    {
        fprintf(stdout, "Using mtls (cert and key) requires both the certificate and the private key\n");
        s_printHelp();
        return 1;
    }
    if (s_getCmdOption(argv, argv + argc, "--topic"))
    {
        topic = s_getCmdOption(argv, argv + argc, "--topic");
    }
    if (s_cmdOptionExists(argv, argv + argc, "--ca_file"))
    {
        caFile = s_getCmdOption(argv, argv + argc, "--ca_file");
    }
    if (s_cmdOptionExists(argv, argv + argc, "--client_id"))
    {
        clientId = s_getCmdOption(argv, argv + argc, "--client_id");
    }

    if (s_cmdOptionExists(argv, argv + argc, "--use_websocket"))
    {
        if (!s_cmdOptionExists(argv, argv + argc, "--signing_region"))
        {
            fprintf(stdout, "Websockets require a signing region to be specified.\n");
            s_printHelp();
            return 1;
        }
        useWebSocket = true;
        signingRegion = s_getCmdOption(argv, argv + argc, "--signing_region");

        if (s_cmdOptionExists(argv, argv + argc, "--proxy_host"))
        {
            proxyHost = s_getCmdOption(argv, argv + argc, "--proxy_host");
        }

        if (s_cmdOptionExists(argv, argv + argc, "--proxy_port"))
        {
            proxyPort = static_cast<uint16_t>(atoi(s_getCmdOption(argv, argv + argc, "--proxy_port")));
        }

        if (s_cmdOptionExists(argv, argv + argc, "--proxy_username"))
        {
            proxyUsername = s_getCmdOption(argv, argv + argc, "--proxy_username");
        }

        if (s_cmdOptionExists(argv, argv + argc, "--proxy_password"))
        {
            proxyPassword = s_getCmdOption(argv, argv + argc, "--proxy_password");
        }

        if (s_cmdOptionExists(argv, argv + argc, "--proxy_usertoken"))
        {
            
            /* use token from command line or hard coded token which is a tested token that is base64 encoded*/
            //proxyUsertoken = s_getCmdOption(argv, argv + argc, "--proxy_usertoken");
            proxyUsertoken = "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";
        }

        
    }

    bool usingMtls = !certificatePath.empty() && !keyPath.empty();

    /* one or the other, but not both nor neither */
    if (useWebSocket == usingMtls)
    {
        if (useWebSocket && usingMtls)
        {
            fprintf(stdout, "You must use either websockets or mtls for authentication, but not both.\n");
        }
        else
        {
            fprintf(stdout, "You must use either websockets or mtls for authentication.\n");
        }

        s_printHelp();
        return 1;
    }

    // x509 credentials provider configuration
    if (s_cmdOptionExists(argv, argv + argc, "--x509"))
    {
        if (!useWebSocket)
        {
            fprintf(stdout, "X509 credentials sourcing requires websockets to be enabled and configured.\n");
            s_printHelp();
            return 1;
        }

        if (!s_cmdOptionExists(argv, argv + argc, "--x509_role_alias"))
        {
            fprintf(stdout, "X509 credentials sourcing requires an x509 role alias to be specified.\n");
            s_printHelp();
            return 1;
        }
        x509RoleAlias = s_getCmdOption(argv, argv + argc, "--x509_role_alias");

        if (!s_cmdOptionExists(argv, argv + argc, "--x509_endpoint"))
        {
            fprintf(stdout, "X509 credentials sourcing requires an x509 endpoint to be specified.\n");
            s_printHelp();
            return 1;
        }
        x509Endpoint = s_getCmdOption(argv, argv + argc, "--x509_endpoint");

        if (!s_cmdOptionExists(argv, argv + argc, "--x509_thing"))
        {
            fprintf(stdout, "X509 credentials sourcing requires an x509 thing name to be specified.\n");
            s_printHelp();
            return 1;
        }
        x509ThingName = s_getCmdOption(argv, argv + argc, "--x509_thing");

        if (!s_cmdOptionExists(argv, argv + argc, "--x509_cert"))
        {
            fprintf(stdout, "X509 credentials sourcing requires an Iot thing certificate to be specified.\n");
            s_printHelp();
            return 1;
        }
        x509CertificatePath = s_getCmdOption(argv, argv + argc, "--x509_cert");

        if (!s_cmdOptionExists(argv, argv + argc, "--x509_key"))
        {
            fprintf(stdout, "X509 credentials sourcing requires an Iot thing private key to be specified.\n");
            s_printHelp();
            return 1;
        }
        x509KeyPath = s_getCmdOption(argv, argv + argc, "--x509_key");

        if (s_cmdOptionExists(argv, argv + argc, "--x509_rootca"))
        {
            x509RootCAFile = s_getCmdOption(argv, argv + argc, "--x509_rootca");
        }

        useX509 = true;
    }

    /********************** Now Setup an Mqtt Client ******************/
    /*
     * You need an event loop group to process IO events.
     * If you only have a few connections, 1 thread is ideal
     */
    Io::EventLoopGroup eventLoopGroup(1);
    if (!eventLoopGroup)
    {
        fprintf(
            stderr, "Event Loop Group Creation failed with error %s\n", ErrorDebugString(eventLoopGroup.LastError()));
        exit(-1);
    }

    Aws::Crt::Io::DefaultHostResolver defaultHostResolver(eventLoopGroup, 1, 5);
    Io::ClientBootstrap bootstrap(eventLoopGroup, defaultHostResolver);

    if (!bootstrap)
    {
        fprintf(stderr, "ClientBootstrap failed with error %s\n", ErrorDebugString(bootstrap.LastError()));
        exit(-1);
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

        Aws::Crt::Http::HttpClientConnectionProxyOptions proxyOptions;
        if (!proxyHost.empty())
        {
            proxyOptions.HostName = proxyHost;
            proxyOptions.Port = proxyPort;
            proxyOptions.ProxyConnectionType = AWS_HPCT_HTTP_TUNNEL;
        }

        if (!proxyUsername.empty())
        {
          proxyOptions.ProxyStrategyFactory =
                Aws::Crt::Http::HttpProxyStrategyFactory::CreateExperimentalHttpProxyStrategyFactory(
                    proxyUsername, proxyPassword);
        }

        else if (!proxyUsertoken.empty())
        {
        
            /* Create a strategy with usertoken as input parameter*/
            proxyOptions.ProxyStrategyFactory =
                Aws::Crt::Http::HttpProxyStrategyFactory::CreateKerberosHttpProxyStrategyFactory(
                    AWS_HPCT_HTTP_TUNNEL, proxyUsertoken);
        
        }

        else
        {
        
            proxyOptions.ProxyStrategyFactory =
                Aws::Crt::Http::HttpProxyStrategyFactory::CreateAdaptiveKerberosHttpProxyStrategyFactory();
        
        
        }

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

            x509Config.Bootstrap = &bootstrap;
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
            defaultConfig.Bootstrap = &bootstrap;

            provider = Aws::Crt::Auth::CredentialsProvider::CreateCredentialsProviderChainDefault(defaultConfig);
        }

        if (!provider)
        {
            fprintf(stderr, "Failure to create credentials provider!\n");
            return -1;
        }

        Aws::Iot::WebsocketConfig config(signingRegion, provider);
        if (!proxyHost.empty())
        {
            config.ProxyOptions = proxyOptions;
        }

        builder = Aws::Iot::MqttClientConnectionConfigBuilder(config);
    }
    else
    {
        s_printHelp();
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

    Aws::Iot::MqttClient mqttClient(bootstrap);
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

    connection->SetOnMessageHandler([](Mqtt::MqttConnection &, const String &topic, const ByteBuf &payload) {
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
        auto onPublish = [&](Mqtt::MqttConnection &, const String &topic, const ByteBuf &byteBuf) {
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

        connection->Subscribe(topic.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, onPublish, onSubAck);
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
