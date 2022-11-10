/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include "CommandLineUtils.h"
#include <aws/crt/Api.h>
#include <aws/crt/Types.h>
#include <aws/crt/auth/Credentials.h>
#include <aws/crt/io/Pkcs11.h>
#include <iostream>

namespace Utils
{
    CommandLineUtils::CommandLineUtils()
    {
        // Automatically register the help command
        RegisterCommand(m_cmd_help, "", "Prints this message");
    }

    void CommandLineUtils::RegisterProgramName(Aws::Crt::String newProgramName)
    {
        m_programName = std::move(newProgramName);
    }

    void CommandLineUtils::RegisterCommand(CommandLineOption option)
    {
        if (m_registeredCommands.count(option.m_commandName))
        {
            fprintf(stdout, "Cannot register command: %s: Command already registered!", option.m_commandName.c_str());
            return;
        }
        m_registeredCommands.insert({option.m_commandName, option});
    }

    void CommandLineUtils::RegisterCommand(
        Aws::Crt::String commandName,
        Aws::Crt::String exampleInput,
        Aws::Crt::String helpOutput)
    {
        RegisterCommand(CommandLineOption(commandName, exampleInput, helpOutput));
    }

    void CommandLineUtils::RemoveCommand(Aws::Crt::String commandName)
    {
        if (m_registeredCommands.count(commandName))
        {
            m_registeredCommands.erase(commandName);
        }
    }

    void CommandLineUtils::UpdateCommandHelp(Aws::Crt::String commandName, Aws::Crt::String newCommandHelp)
    {
        if (m_registeredCommands.count(commandName))
        {
            m_registeredCommands.at(commandName).m_helpOutput = std::move(newCommandHelp);
        }
    }

    void CommandLineUtils::SendArguments(const char **argv, const char **argc)
    {
        if (m_beginPosition != nullptr || m_endPosition != nullptr)
        {
            fprintf(stdout, "Arguments already sent!");
            return;
        }
        m_beginPosition = argv;
        m_endPosition = argc;

        // Automatically check and print the help message if the help command is present
        if (HasCommand(m_cmd_help))
        {
            PrintHelp();
            exit(-1);
        }
    }

    bool CommandLineUtils::HasCommand(Aws::Crt::String command)
    {
        return std::find(m_beginPosition, m_endPosition, "--" + command) != m_endPosition;
    }

    Aws::Crt::String CommandLineUtils::GetCommand(Aws::Crt::String command)
    {
        const char **itr = std::find(m_beginPosition, m_endPosition, "--" + command);
        if (itr != m_endPosition && ++itr != m_endPosition)
        {
            return Aws::Crt::String(*itr);
        }
        return "";
    }

    Aws::Crt::String CommandLineUtils::GetCommandOrDefault(Aws::Crt::String command, Aws::Crt::String commandDefault)
    {
        if (HasCommand(command))
        {
            return Aws::Crt::String(GetCommand(command));
        }
        return commandDefault;
    }

    Aws::Crt::String CommandLineUtils::GetCommandRequired(
        Aws::Crt::String command,
        Aws::Crt::String optionalAdditionalMessage)
    {
        if (HasCommand(command))
        {
            return GetCommand(command);
        }
        PrintHelp();
        fprintf(stderr, "Missing required argument: --%s\n", command.c_str());
        if (optionalAdditionalMessage != "")
        {
            fprintf(stderr, "%s\n", optionalAdditionalMessage.c_str());
        }
        exit(-1);
    }

    void CommandLineUtils::PrintHelp()
    {
        fprintf(stdout, "Usage:\n");
        fprintf(stdout, "%s", m_programName.c_str());

        for (auto const &pair : m_registeredCommands)
        {
            fprintf(stdout, " --%s %s", pair.first.c_str(), pair.second.m_exampleInput.c_str());
        }

        fprintf(stdout, "\n\n");

        for (auto const &pair : m_registeredCommands)
        {
            fprintf(stdout, "* %s:\t\t%s\n", pair.first.c_str(), pair.second.m_helpOutput.c_str());
        }

        fprintf(stdout, "\n");
    }

    void CommandLineUtils::AddCommonMQTTCommands()
    {
        RegisterCommand(m_cmd_endpoint, "<str>", "The endpoint of the mqtt server not including a port.");
        RegisterCommand(
            m_cmd_ca_file, "<path>", "Path to AmazonRootCA1.pem (optional, system trust store used by default).");
    }

    void CommandLineUtils::AddCommonProxyCommands()
    {
        RegisterCommand(m_cmd_proxy_host, "<str>", "Host name of the proxy server to connect through (optional)");
        RegisterCommand(
            m_cmd_proxy_port, "<int>", "Port of the proxy server to connect through (optional, default='8080'");
    }

    void CommandLineUtils::AddCommonX509Commands()
    {
        RegisterCommand(
            m_cmd_x509_role, "<str>", "Role alias to use with the x509 credentials provider (required for x509)");
        RegisterCommand(m_cmd_x509_endpoint, "<str>", "Endpoint to fetch x509 credentials from (required for x509)");
        RegisterCommand(
            m_cmd_x509_thing_name, "<str>", "Thing name to fetch x509 credentials on behalf of (required for x509)");
        RegisterCommand(
            m_cmd_x509_cert_file,
            "<path>",
            "Path to the IoT thing certificate used in fetching x509 credentials (required for x509)");
        RegisterCommand(
            m_cmd_x509_key_file,
            "<path>",
            "Path to the IoT thing private key used in fetching x509 credentials (required for x509)");
        RegisterCommand(
            m_cmd_x509_ca_file,
            "<path>",
            "Path to the root certificate used in fetching x509 credentials (required for x509)");
    }

    void CommandLineUtils::AddCommonTopicMessageCommands()
    {
        RegisterCommand(
            m_cmd_message, "<str>", "The message to send in the payload (optional, default='Hello world!')");
        RegisterCommand(m_cmd_topic, "<str>", "Topic to publish, subscribe to. (optional, default='test/topic')");
    }

    void CommandLineUtils::AddCommonCustomAuthorizerCommands()
    {
        RegisterCommand(
            m_cmd_custom_auth_username,
            "<str>",
            "The name to send when connecting through the custom authorizer (optional)");
        RegisterCommand(
            m_cmd_custom_auth_authorizer_name,
            "<str>",
            "The name of the custom authorizer to connect to (optional but required for everything but custom "
            "domains)");
        RegisterCommand(
            m_cmd_custom_auth_authorizer_signature,
            "<str>",
            "The signature to send when connecting through a custom authorizer (optional)");
        RegisterCommand(
            m_cmd_custom_auth_password,
            "<str>",
            "The password to send when connecting through a custom authorizer (optional)");
    }

    void CommandLineUtils::AddLoggingCommands()
    {
        RegisterCommand(
            m_cmd_verbosity,
            "<log level>",
            "The logging level to use. Choices are 'Trace', 'Debug', 'Info', 'Warn', 'Error', 'Fatal', and 'None'. "
            "(optional, default='none')");
        RegisterCommand(
            m_cmd_log_file,
            "<str>",
            "File to write logs to. If not provided, logs will be written to stdout. "
            "(optional, default='none')");
    }

    void CommandLineUtils::StartLoggingBasedOnCommand(Aws::Crt::ApiHandle *apiHandle)
    {
        // Process logging command
        if (HasCommand("verbosity"))
        {
            Aws::Crt::LogLevel logLevel = Aws::Crt::LogLevel::None;
            Aws::Crt::String verbosity = GetCommand(m_cmd_verbosity);
            if (verbosity == "Fatal")
            {
                logLevel = Aws::Crt::LogLevel::Fatal;
            }
            else if (verbosity == "Error")
            {
                logLevel = Aws::Crt::LogLevel::Error;
            }
            else if (verbosity == "Warn")
            {
                logLevel = Aws::Crt::LogLevel::Warn;
            }
            else if (verbosity == "Info")
            {
                logLevel = Aws::Crt::LogLevel::Info;
            }
            else if (verbosity == "Debug")
            {
                logLevel = Aws::Crt::LogLevel::Debug;
            }
            else if (verbosity == "Trace")
            {
                logLevel = Aws::Crt::LogLevel::Trace;
            }
            else
            {
                logLevel = Aws::Crt::LogLevel::None;
            }

            if (HasCommand("log_file"))
            {
                apiHandle->InitializeLogging(logLevel, GetCommand(m_cmd_log_file).c_str());
            }
            else
            {
                apiHandle->InitializeLogging(logLevel, stderr);
            }
        }
    }

    std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> CommandLineUtils::BuildPKCS11MQTTConnection(
        Aws::Iot::MqttClient *client)
    {
        std::shared_ptr<Aws::Crt::Io::Pkcs11Lib> pkcs11Lib =
            Aws::Crt::Io::Pkcs11Lib::Create(GetCommandRequired(m_cmd_pkcs11_lib));
        if (!pkcs11Lib)
        {
            fprintf(stderr, "Pkcs11Lib failed: %s\n", Aws::Crt::ErrorDebugString(Aws::Crt::LastError()));
            exit(-1);
        }

        Aws::Crt::Io::TlsContextPkcs11Options pkcs11Options(pkcs11Lib);
        pkcs11Options.SetCertificateFilePath(GetCommandRequired(m_cmd_cert_file));
        pkcs11Options.SetUserPin(GetCommandRequired(m_cmd_pkcs11_pin));

        if (HasCommand(m_cmd_pkcs11_token))
        {
            pkcs11Options.SetTokenLabel(GetCommand(m_cmd_pkcs11_token));
        }

        if (HasCommand(m_cmd_pkcs11_slot))
        {
            uint64_t slotId = std::stoull(GetCommand(m_cmd_pkcs11_slot).c_str());
            pkcs11Options.SetSlotId(slotId);
        }

        if (HasCommand(m_cmd_pkcs11_key))
        {
            pkcs11Options.SetPrivateKeyObjectLabel(GetCommand(m_cmd_pkcs11_key));
        }

        Aws::Iot::MqttClientConnectionConfigBuilder clientConfigBuilder(pkcs11Options);
        if (!clientConfigBuilder)
        {
            fprintf(
                stderr,
                "MqttClientConnectionConfigBuilder failed: %s\n",
                Aws::Crt::ErrorDebugString(Aws::Crt::LastError()));
            exit(-1);
        }

        if (HasCommand(m_cmd_ca_file))
        {
            clientConfigBuilder.WithCertificateAuthority(GetCommand(m_cmd_ca_file).c_str());
        }
        if (HasCommand(m_cmd_port_override))
        {
            int tmp_port = atoi(GetCommand(m_cmd_port_override).c_str());
            if (tmp_port > 0 && tmp_port < UINT16_MAX)
            {
                clientConfigBuilder.WithPortOverride(static_cast<uint16_t>(tmp_port));
            }
        }

        clientConfigBuilder.WithEndpoint(GetCommandRequired(m_cmd_endpoint));
        return GetClientConnectionForMQTTConnection(client, &clientConfigBuilder);
    }

    std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> CommandLineUtils::BuildWebsocketX509MQTTConnection(
        Aws::Iot::MqttClient *client)
    {
        Aws::Crt::Io::TlsContext x509TlsCtx;
        Aws::Iot::MqttClientConnectionConfigBuilder clientConfigBuilder;

        std::shared_ptr<Aws::Crt::Auth::ICredentialsProvider> provider = nullptr;

        Aws::Crt::Io::TlsContextOptions tlsCtxOptions = Aws::Crt::Io::TlsContextOptions::InitClientWithMtls(
            GetCommandRequired(m_cmd_x509_cert_file).c_str(), GetCommandRequired(m_cmd_x509_key_file).c_str());
        if (!tlsCtxOptions)
        {
            fprintf(
                stderr,
                "Unable to initialize tls context options, error: %s!\n",
                Aws::Crt::ErrorDebugString(tlsCtxOptions.LastError()));
            exit(-1);
        }

        if (HasCommand(m_cmd_x509_ca_file))
        {
            tlsCtxOptions.OverrideDefaultTrustStore(nullptr, GetCommand(m_cmd_x509_ca_file).c_str());
        }

        x509TlsCtx = Aws::Crt::Io::TlsContext(tlsCtxOptions, Aws::Crt::Io::TlsMode::CLIENT);
        if (!x509TlsCtx)
        {
            fprintf(
                stderr,
                "Unable to create tls context, error: %s!\n",
                Aws::Crt::ErrorDebugString(x509TlsCtx.GetInitializationError()));
            exit(-1);
        }

        Aws::Crt::Auth::CredentialsProviderX509Config x509Config;
        x509Config.TlsOptions = x509TlsCtx.NewConnectionOptions();
        if (!x509Config.TlsOptions)
        {
            fprintf(
                stderr,
                "Unable to create tls options from tls context, error: %s!\n",
                Aws::Crt::ErrorDebugString(x509Config.TlsOptions.LastError()));
            exit(-1);
        }

        x509Config.Endpoint = GetCommandRequired(m_cmd_x509_endpoint);
        x509Config.RoleAlias = GetCommandRequired(m_cmd_x509_role);
        x509Config.ThingName = GetCommandRequired(m_cmd_x509_thing_name);

        Aws::Crt::Http::HttpClientConnectionProxyOptions proxyOptions;
        if (HasCommand(m_cmd_proxy_host))
        {
            proxyOptions = GetProxyOptionsForMQTTConnection();
            x509Config.ProxyOptions = proxyOptions;
        }

        provider = Aws::Crt::Auth::CredentialsProvider::CreateCredentialsProviderX509(x509Config);
        if (!provider)
        {
            fprintf(stderr, "Failure to create credentials provider!\n");
            exit(-1);
        }

        Aws::Iot::WebsocketConfig config(GetCommandRequired(m_cmd_signing_region), provider);
        clientConfigBuilder = Aws::Iot::MqttClientConnectionConfigBuilder(config);

        if (HasCommand(m_cmd_ca_file))
        {
            clientConfigBuilder.WithCertificateAuthority(GetCommand(m_cmd_ca_file).c_str());
        }
        if (HasCommand(m_cmd_proxy_host))
        {
            clientConfigBuilder.WithHttpProxyOptions(proxyOptions);
        }
        if (HasCommand(m_cmd_port_override))
        {
            int tmp_port = atoi(GetCommand(m_cmd_port_override).c_str());
            if (tmp_port > 0 && tmp_port < UINT16_MAX)
            {
                clientConfigBuilder.WithPortOverride(static_cast<uint16_t>(tmp_port));
            }
        }

        clientConfigBuilder.WithEndpoint(GetCommandRequired(m_cmd_endpoint));
        return GetClientConnectionForMQTTConnection(client, &clientConfigBuilder);
    }
    std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> CommandLineUtils::BuildWebsocketMQTTConnection(
        Aws::Iot::MqttClient *client)
    {
        Aws::Crt::Io::TlsContext x509TlsCtx;
        Aws::Iot::MqttClientConnectionConfigBuilder clientConfigBuilder;

        std::shared_ptr<Aws::Crt::Auth::ICredentialsProvider> provider = nullptr;

        Aws::Crt::Auth::CredentialsProviderChainDefaultConfig defaultConfig;
        provider = Aws::Crt::Auth::CredentialsProvider::CreateCredentialsProviderChainDefault(defaultConfig);

        if (!provider)
        {
            fprintf(stderr, "Failure to create credentials provider!\n");
            exit(-1);
        }

        Aws::Iot::WebsocketConfig config(GetCommandRequired(m_cmd_signing_region), provider);
        clientConfigBuilder = Aws::Iot::MqttClientConnectionConfigBuilder(config);

        if (HasCommand(m_cmd_ca_file))
        {
            clientConfigBuilder.WithCertificateAuthority(GetCommand(m_cmd_ca_file).c_str());
        }
        if (HasCommand(m_cmd_proxy_host))
        {
            clientConfigBuilder.WithHttpProxyOptions(GetProxyOptionsForMQTTConnection());
        }
        if (HasCommand(m_cmd_port_override))
        {
            int tmp_port = atoi(GetCommand(m_cmd_port_override).c_str());
            if (tmp_port > 0 && tmp_port < UINT16_MAX)
            {
                clientConfigBuilder.WithPortOverride(static_cast<uint16_t>(tmp_port));
            }
        }

        clientConfigBuilder.WithEndpoint(GetCommandRequired(m_cmd_endpoint));
        return GetClientConnectionForMQTTConnection(client, &clientConfigBuilder);
    }
    std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> CommandLineUtils::BuildDirectMQTTConnection(
        Aws::Iot::MqttClient *client)
    {
        Aws::Crt::String certificatePath = GetCommandRequired(m_cmd_cert_file);
        Aws::Crt::String keyPath = GetCommandRequired(m_cmd_key_file);
        Aws::Crt::String endpoint = GetCommandRequired(m_cmd_endpoint);

        auto clientConfigBuilder =
            Aws::Iot::MqttClientConnectionConfigBuilder(certificatePath.c_str(), keyPath.c_str());
        clientConfigBuilder.WithEndpoint(endpoint);

        if (HasCommand(m_cmd_ca_file))
        {
            clientConfigBuilder.WithCertificateAuthority(GetCommand(m_cmd_ca_file).c_str());
        }
        if (HasCommand(m_cmd_proxy_host))
        {
            clientConfigBuilder.WithHttpProxyOptions(GetProxyOptionsForMQTTConnection());
        }
        if (HasCommand(m_cmd_port_override))
        {
            int tmp_port = atoi(GetCommand(m_cmd_port_override).c_str());
            if (tmp_port > 0 && tmp_port < UINT16_MAX)
            {
                clientConfigBuilder.WithPortOverride(static_cast<uint16_t>(tmp_port));
            }
        }

        return GetClientConnectionForMQTTConnection(client, &clientConfigBuilder);
    }

    std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> CommandLineUtils::BuildDirectMQTTConnectionWithCustomAuthorizer(
        Aws::Iot::MqttClient *client)
    {
        Aws::Crt::String auth_username = GetCommandOrDefault(m_cmd_custom_auth_username, "");
        Aws::Crt::String auth_authorizer_name = GetCommandOrDefault(m_cmd_custom_auth_authorizer_name, "");
        Aws::Crt::String auth_authorizer_signature = GetCommandOrDefault(m_cmd_custom_auth_authorizer_signature, "");
        Aws::Crt::String auth_password = GetCommandOrDefault(m_cmd_custom_auth_password, "");

        Aws::Crt::String endpoint = GetCommandRequired(m_cmd_endpoint);

        auto clientConfigBuilder = Aws::Iot::MqttClientConnectionConfigBuilder::NewDefaultBuilder();
        clientConfigBuilder.WithEndpoint(endpoint);
        clientConfigBuilder.WithCustomAuthorizer(
            auth_username, auth_authorizer_name, auth_authorizer_signature, auth_password);

        return GetClientConnectionForMQTTConnection(client, &clientConfigBuilder);
    }

    std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> CommandLineUtils::BuildMQTTConnection()
    {
        if (!m_internal_client)
        {
            m_internal_client = Aws::Iot::MqttClient();
            if (!m_internal_client)
            {
                fprintf(
                    stderr,
                    "MQTT Client Creation failed with error %s\n",
                    Aws::Crt::ErrorDebugString(m_internal_client.LastError()));
                exit(-1);
            }
        }

        if (HasCommand(m_cmd_pkcs11_lib))
        {
            return BuildPKCS11MQTTConnection(&m_internal_client);
        }
        else if (HasCommand(m_cmd_signing_region))
        {
            if (HasCommand(m_cmd_x509_endpoint))
            {
                return BuildWebsocketX509MQTTConnection(&m_internal_client);
            }
            else
            {
                return BuildWebsocketMQTTConnection(&m_internal_client);
            }
        }
        else if (HasCommand(m_cmd_custom_auth_authorizer_name))
        {
            return BuildDirectMQTTConnectionWithCustomAuthorizer(&m_internal_client);
        }
        else
        {
            return BuildDirectMQTTConnection(&m_internal_client);
        }
    }

    Aws::Crt::Http::HttpClientConnectionProxyOptions CommandLineUtils::GetProxyOptionsForMQTTConnection()
    {
        Aws::Crt::Http::HttpClientConnectionProxyOptions proxyOptions;
        proxyOptions.HostName = GetCommand(m_cmd_proxy_host);
        int ProxyPort = atoi(GetCommandOrDefault(m_cmd_proxy_port, "8080").c_str());
        if (ProxyPort > 0 && ProxyPort < UINT16_MAX)
        {
            proxyOptions.Port = static_cast<uint16_t>(ProxyPort);
        }
        else
        {
            proxyOptions.Port = 8080;
        }
        proxyOptions.AuthType = Aws::Crt::Http::AwsHttpProxyAuthenticationType::None;
        return proxyOptions;
    }

    std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> CommandLineUtils::GetClientConnectionForMQTTConnection(
        Aws::Iot::MqttClient *client,
        Aws::Iot::MqttClientConnectionConfigBuilder *clientConfigBuilder)
    {
        auto clientConfig = clientConfigBuilder->Build();
        if (!clientConfig)
        {
            fprintf(
                stderr,
                "Client Configuration initialization failed with error %s\n",
                Aws::Crt::ErrorDebugString(clientConfig.LastError()));
            exit(-1);
        }

        auto connection = client->NewConnection(clientConfig);
        if (!*connection)
        {
            fprintf(
                stderr,
                "MQTT Connection Creation failed with error %s\n",
                Aws::Crt::ErrorDebugString(connection->LastError()));
            exit(-1);
        }
        return connection;
    }

    void CommandLineUtils::SampleConnectAndDisconnect(
        std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> connection,
        Aws::Crt::String clientId)
    {
        /*
         * In a real world application you probably don't want to enforce synchronous behavior
         * but this is a sample console application, so we'll just do that with a condition variable.
         */
        std::promise<bool> connectionCompletedPromise;
        std::promise<void> connectionClosedPromise;

        /*
         * This will execute when an mqtt connect has completed or failed.
         */
        auto onConnectionCompleted =
            [&](Aws::Crt::Mqtt::MqttConnection &, int errorCode, Aws::Crt::Mqtt::ReturnCode returnCode, bool) {
                if (errorCode)
                {
                    fprintf(stdout, "Connection failed with error %s\n", Aws::Crt::ErrorDebugString(errorCode));
                    connectionCompletedPromise.set_value(false);
                }
                else
                {
                    fprintf(stdout, "Connection completed with return code %d\n", returnCode);
                    connectionCompletedPromise.set_value(true);
                }
            };

        auto onInterrupted = [&](Aws::Crt::Mqtt::MqttConnection &, int error) {
            fprintf(stdout, "Connection interrupted with error %s\n", Aws::Crt::ErrorDebugString(error));
        };
        auto onResumed = [&](Aws::Crt::Mqtt::MqttConnection &, Aws::Crt::Mqtt::ReturnCode, bool) {
            fprintf(stdout, "Connection resumed\n");
        };

        /*
         * Invoked when a disconnect message has completed.
         */
        auto onDisconnect = [&](Aws::Crt::Mqtt::MqttConnection &) {
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
            fprintf(
                stderr, "MQTT Connection failed with error %s\n", Aws::Crt::ErrorDebugString(connection->LastError()));
            exit(-1);
        }

        // wait for the OnConnectionCompleted callback to fire, which sets connectionCompletedPromise...
        if (connectionCompletedPromise.get_future().get() == false)
        {
            fprintf(stderr, "Connection failed\n");
            exit(-1);
        }

        /* Disconnect */
        if (connection->Disconnect())
        {
            connectionClosedPromise.get_future().wait();
        }
    }

} // namespace Utils
