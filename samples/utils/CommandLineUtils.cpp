/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include "CommandLineUtils.h"
#include <aws/crt/Api.h>
#include <aws/crt/Types.h>
#include <aws/crt/UUID.h>
#include <aws/crt/auth/Credentials.h>
#include <aws/crt/io/Pkcs11.h>
#include <fstream>
#include <iostream>

namespace Utils
{
    // The command names for the samples
    static const char *m_cmd_mqtt_version = "mqtt_version";
    static const char *m_cmd_endpoint = "endpoint";
    static const char *m_cmd_ca_file = "ca_file";
    static const char *m_cmd_cert_file = "cert";
    static const char *m_cmd_key_file = "key";
    static const char *m_cmd_proxy_host = "proxy_host";
    static const char *m_cmd_proxy_port = "proxy_port";
    static const char *m_cmd_signing_region = "signing_region";
    static const char *m_cmd_access_key_id = "access_key_id";
    static const char *m_cmd_secret_access_key = "secret_access_key";
    static const char *m_cmd_session_token = "session_token";
    static const char *m_cmd_x509_endpoint = "x509_endpoint";
    static const char *m_cmd_x509_role = "x509_role_alias";
    static const char *m_cmd_x509_thing_name = "x509_thing_name";
    static const char *m_cmd_x509_cert_file = "x509_cert";
    static const char *m_cmd_x509_key_file = "x509_key";
    static const char *m_cmd_x509_ca_file = "x509_ca_file";
    static const char *m_cmd_pkcs11_lib = "pkcs11_lib";
    static const char *m_cmd_pkcs11_pin = "pin";
    static const char *m_cmd_pkcs11_token = "token_label";
    static const char *m_cmd_pkcs11_slot = "slot_id";
    static const char *m_cmd_pkcs11_key = "key_label";
    static const char *m_cmd_message = "message";
    static const char *m_cmd_topic = "topic";
    static const char *m_cmd_port_override = "port_override";
    static const char *m_cmd_help = "help";
    static const char *m_cmd_custom_auth_username = "custom_auth_username";
    static const char *m_cmd_custom_auth_authorizer_name = "custom_auth_authorizer_name";
    static const char *m_cmd_custom_auth_authorizer_signature = "custom_auth_authorizer_signature";
    static const char *m_cmd_custom_auth_password = "custom_auth_password";
    static const char *m_cmd_custom_auth_token_name = "custom_auth_token_name";
    static const char *m_cmd_custom_token_value = "custom_auth_token_value";
    static const char *m_cmd_verbosity = "verbosity";
    static const char *m_cmd_log_file = "log_file";
    static const char *m_cmd_cognito_identity = "cognito_identity";
    static const char *m_cmd_mode = "mode";
    static const char *m_cmd_client_id = "client_id";
    static const char *m_cmd_thing_name = "thing_name";
    static const char *m_cmd_count = "count";
    static const char *m_cmd_report_time = "report_time";
    static const char *m_cmd_template_name = "template_name";
    static const char *m_cmd_template_parameters = "template_parameters";
    static const char *m_cmd_template_csr = "csr";
    static const char *m_cmd_job_id = "job_id";
    static const char *m_cmd_group_identifier = "group_identifier";
    static const char *m_cmd_is_ci = "is_ci";
    static const char *m_cmd_clients = "clients";
    static const char *m_cmd_tps = "tps";
    static const char *m_cmd_seconds = "seconds";
    static const char *m_cmd_access_token_file = "access_token_file";
    static const char *m_cmd_access_token = "access_token";
    static const char *m_cmd_local_proxy_mode_source = "local_proxy_mode_source";
    static const char *m_cmd_client_token_file = "client_token_file";
    static const char *m_cmd_client_token = "client_token";
    static const char *m_cmd_proxy_user_name = "proxy_user_name";
    static const char *m_cmd_proxy_password = "proxy_password";
    static const char *m_cmd_shadow_property = "shadow_property";
    static const char *m_cmd_shadow_name = "shadow_name";
    static const char *m_cmd_shadow_value = "shadow_value";
    static const char *m_cmd_region = "region";
    static const char *m_cmd_pkcs12_file = "pkcs12_file";
    static const char *m_cmd_pkcs12_password = "pkcs12_password";
    static const char *m_cmd_print_discover_resp_only = "print_discover_resp_only";

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

    Aws::Crt::String CommandLineUtils::GetCommandRequired(Aws::Crt::String command)
    {
        if (HasCommand(command))
        {
            return GetCommand(command);
        }
        PrintHelp();
        fprintf(stderr, "Missing required argument: --%s\n", command.c_str());
        exit(-1);
    }

    Aws::Crt::String CommandLineUtils::GetCommandRequired(Aws::Crt::String command, Aws::Crt::String commandAlt)
    {
        if (HasCommand(commandAlt))
        {
            return GetCommand(commandAlt);
        }
        return GetCommandRequired(command);
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
        RegisterCommand(m_cmd_is_ci, "<str>", "If present the sample will run in CI mode.");
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

    void CommandLineUtils::AddCommonKeyCertCommands()
    {
        RegisterCommand(m_cmd_key_file, "<path>", "Path to your key in PEM format.");
        RegisterCommand(m_cmd_cert_file, "<path>", "Path to your client certificate in PEM format.");
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

    void CommandLineUtils::AddCognitoCommands()
    {
        RegisterCommand(m_cmd_cognito_identity, "<str>", "The Cognito identity ID to use to connect via Cognito");
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

    cmdData parseSampleInputDeviceDefender(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle)
    {
        CommandLineUtils cmdUtils = CommandLineUtils();
        cmdUtils.RegisterProgramName("basic-report");
        cmdUtils.AddCommonMQTTCommands();
        cmdUtils.RegisterCommand(m_cmd_client_id, "<str>", "Client id to use (optional, default='test-*')");
        cmdUtils.RegisterCommand(
            m_cmd_thing_name, "<str>", "The name of your IOT thing (optional, default='TestThing').");
        cmdUtils.RegisterCommand(
            m_cmd_report_time,
            "<int>",
            "The frequency to send Device Defender reports in seconds (optional, default='60')");
        cmdUtils.RegisterCommand(m_cmd_count, "<int>", "The number of reports to send (optional, default='10')");
        cmdUtils.AddLoggingCommands();
        const char **const_argv = (const char **)argv;
        cmdUtils.SendArguments(const_argv, const_argv + argc);
        cmdUtils.StartLoggingBasedOnCommand(api_handle);

        if (cmdUtils.HasCommand(m_cmd_help))
        {
            cmdUtils.PrintHelp();
            exit(-1);
        }

        cmdData returnData = cmdData();

        returnData.input_endpoint = cmdUtils.GetCommandRequired(m_cmd_endpoint);
        returnData.input_cert = cmdUtils.GetCommandRequired(m_cmd_cert_file);
        returnData.input_key = cmdUtils.GetCommandRequired(m_cmd_key_file);
        returnData.input_clientId =
            cmdUtils.GetCommandOrDefault(m_cmd_client_id, Aws::Crt::String("test-") + Aws::Crt::UUID().ToString());

        if (cmdUtils.HasCommand(m_cmd_ca_file))
        {
            returnData.input_ca = cmdUtils.GetCommand(m_cmd_ca_file);
        }
        if (cmdUtils.HasCommand(m_cmd_proxy_host))
        {
            returnData.input_proxyHost = cmdUtils.GetCommandRequired(m_cmd_proxy_host);
            returnData.input_proxyPort = atoi(cmdUtils.GetCommandOrDefault(m_cmd_proxy_port, "8080").c_str());
        }
        if (cmdUtils.HasCommand(m_cmd_port_override))
        {
            returnData.input_port = atoi(cmdUtils.GetCommandRequired(m_cmd_port_override).c_str());
        }

        returnData.input_thingName = cmdUtils.GetCommandOrDefault(m_cmd_thing_name, "TestThing");

        if (cmdUtils.HasCommand(m_cmd_report_time))
        {
            returnData.input_reportTime = atoi(cmdUtils.GetCommand(m_cmd_report_time).c_str());
        }
        if (cmdUtils.HasCommand(m_cmd_count))
        {
            returnData.input_count = atoi(cmdUtils.GetCommand(m_cmd_count).c_str());
        }

        return returnData;
    }

    static void s_addLoggingSendArgumentsStartLogging(
        int argc,
        char *argv[],
        Aws::Crt::ApiHandle *api_handle,
        CommandLineUtils *cmdUtils)
    {
        cmdUtils->AddLoggingCommands();
        const char **const_argv = (const char **)argv;
        cmdUtils->SendArguments(const_argv, const_argv + argc);
        cmdUtils->StartLoggingBasedOnCommand(api_handle);
        if (cmdUtils->HasCommand(m_cmd_help))
        {
            cmdUtils->PrintHelp();
            exit(-1);
        }
    }

    static void s_parseCommonMQTTCommands(CommandLineUtils *cmdUtils, cmdData *cmdData)
    {
        cmdData->input_endpoint = cmdUtils->GetCommandRequired(m_cmd_endpoint);
        if (cmdUtils->HasCommand(m_cmd_ca_file))
        {
            cmdData->input_ca = cmdUtils->GetCommand(m_cmd_ca_file);
        }
        cmdData->input_isCI = cmdUtils->HasCommand(m_cmd_is_ci);
        cmdData->input_mqtt_version = atoi(cmdUtils->GetCommandOrDefault(m_cmd_mqtt_version, "3").c_str());
    }

    static void s_populateTopic(CommandLineUtils *cmdUtils, cmdData *cmdData)
    {
        cmdData->input_topic = cmdUtils->GetCommandOrDefault(m_cmd_topic, "test/topic");
        if (cmdUtils->HasCommand(m_cmd_is_ci))
        {
            cmdData->input_topic += Aws::Crt::String("/") + Aws::Crt::UUID().ToString();
        }
    }

    cmdData parseSampleInputGreengrassDiscovery(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle)
    {
        CommandLineUtils cmdUtils = CommandLineUtils();
        cmdUtils.RegisterProgramName("basic-discovery");
        cmdUtils.AddCommonMQTTCommands();
        cmdUtils.AddCommonKeyCertCommands();
        cmdUtils.AddCommonProxyCommands();
        cmdUtils.AddCommonTopicMessageCommands();
        cmdUtils.RemoveCommand(m_cmd_endpoint);
        cmdUtils.RegisterCommand(m_cmd_region, "<str>", "The region for your Greengrass groups.");
        cmdUtils.RegisterCommand(m_cmd_thing_name, "<str>", "The name of your IOT thing");
        cmdUtils.RegisterCommand(
            m_cmd_mode, "<str>", "Mode options: 'both', 'publish', or 'subscribe' (optional, default='both').");
        cmdUtils.UpdateCommandHelp(
            m_cmd_message,
            "The message to send. If no message is provided, you will be prompted to input one (optional, default='')");
        cmdUtils.RegisterCommand(
            m_cmd_print_discover_resp_only,
            "",
            "If present, the sample will only print the discovery result (optional, default='False')");
        s_addLoggingSendArgumentsStartLogging(argc, argv, api_handle, &cmdUtils);

        cmdData returnData = cmdData();
        returnData.input_cert = cmdUtils.GetCommandRequired(m_cmd_cert_file);
        returnData.input_key = cmdUtils.GetCommandRequired(m_cmd_key_file);
        returnData.input_thingName = cmdUtils.GetCommandRequired(m_cmd_thing_name);
        if (cmdUtils.HasCommand(m_cmd_ca_file))
        {
            returnData.input_ca = cmdUtils.GetCommand(m_cmd_ca_file);
        }
        returnData.input_signingRegion = cmdUtils.GetCommandRequired(m_cmd_region, m_cmd_signing_region);
        s_populateTopic(&cmdUtils, &returnData);
        returnData.input_message = cmdUtils.GetCommandOrDefault(m_cmd_message, "");
        returnData.input_mode = cmdUtils.GetCommandOrDefault(m_cmd_mode, "both");
        if (cmdUtils.HasCommand(m_cmd_proxy_host))
        {
            returnData.input_proxyHost = cmdUtils.GetCommandRequired(m_cmd_proxy_host);
            returnData.input_proxyPort = atoi(cmdUtils.GetCommandOrDefault(m_cmd_proxy_port, "8080").c_str());
        }
        if (cmdUtils.HasCommand(m_cmd_print_discover_resp_only))
        {
            returnData.input_PrintDiscoverRespOnly = true;
        }

        return returnData;
    }

    cmdData parseSampleInputGreengrassIPC(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle)
    {
        CommandLineUtils cmdUtils = CommandLineUtils();
        cmdUtils.RegisterProgramName("greengrass-ipc");
        cmdUtils.AddCommonTopicMessageCommands();
        s_addLoggingSendArgumentsStartLogging(argc, argv, api_handle, &cmdUtils);

        cmdData returnData = cmdData();
        s_populateTopic(&cmdUtils, &returnData);
        returnData.input_message = cmdUtils.GetCommandOrDefault(m_cmd_message, "Hello World");
        return returnData;
    }

    cmdData parseSampleInputFleetProvisioning(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle)
    {
        CommandLineUtils cmdUtils = CommandLineUtils();
        cmdUtils.RegisterProgramName("fleet-provisioning");
        cmdUtils.AddCommonMQTTCommands();
        cmdUtils.AddCommonKeyCertCommands();
        cmdUtils.RegisterCommand(m_cmd_client_id, "<str>", "Client id to use (optional, default='test-*')");
        cmdUtils.RegisterCommand(m_cmd_template_name, "<str>", "The name of your provisioning template");
        cmdUtils.RegisterCommand(m_cmd_template_parameters, "<json>", "Template parameters json");
        cmdUtils.RegisterCommand(m_cmd_template_csr, "<path>", "Path to CSR in PEM format (optional)");
        s_addLoggingSendArgumentsStartLogging(argc, argv, api_handle, &cmdUtils);

        cmdData returnData = cmdData();
        s_parseCommonMQTTCommands(&cmdUtils, &returnData);
        returnData.input_cert = cmdUtils.GetCommandRequired(m_cmd_cert_file);
        returnData.input_key = cmdUtils.GetCommandRequired(m_cmd_key_file);
        returnData.input_clientId =
            cmdUtils.GetCommandOrDefault(m_cmd_client_id, Aws::Crt::String("test-") + Aws::Crt::UUID().ToString());
        returnData.input_templateName = cmdUtils.GetCommandRequired(m_cmd_template_name);
        returnData.input_templateParameters = cmdUtils.GetCommandRequired(m_cmd_template_parameters);
        if (cmdUtils.HasCommand(m_cmd_template_csr))
        {
            returnData.input_csrPath = cmdUtils.GetCommand(m_cmd_template_csr);
        }
        return returnData;
    }

    cmdData parseSampleInputJobs(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle)
    {
        CommandLineUtils cmdUtils = CommandLineUtils();
        cmdUtils.RegisterProgramName("describe-job-execution");
        cmdUtils.AddCommonMQTTCommands();
        cmdUtils.AddCommonKeyCertCommands();
        cmdUtils.RegisterCommand(m_cmd_client_id, "<str>", "Client id to use (optional, default='test-*')");
        cmdUtils.RegisterCommand(m_cmd_thing_name, "<str>", "The name of your IOT thing");
        cmdUtils.RegisterCommand(m_cmd_job_id, "<str>", "The job id you want to describe.");
        s_addLoggingSendArgumentsStartLogging(argc, argv, api_handle, &cmdUtils);

        cmdData returnData = cmdData();
        s_parseCommonMQTTCommands(&cmdUtils, &returnData);
        returnData.input_cert = cmdUtils.GetCommandRequired(m_cmd_cert_file);
        returnData.input_key = cmdUtils.GetCommandRequired(m_cmd_key_file);
        returnData.input_clientId =
            cmdUtils.GetCommandOrDefault(m_cmd_client_id, Aws::Crt::String("test-") + Aws::Crt::UUID().ToString());
        returnData.input_thingName = cmdUtils.GetCommandRequired(m_cmd_thing_name);
        returnData.input_jobId = cmdUtils.GetCommandOrDefault(m_cmd_job_id, "1");
        return returnData;
    }

    cmdData parseSampleInputBasicConnect(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle)
    {
        CommandLineUtils cmdUtils = CommandLineUtils();
        cmdUtils.RegisterProgramName("basic-connect");
        cmdUtils.AddCommonMQTTCommands();
        cmdUtils.AddCommonProxyCommands();
        cmdUtils.AddCommonKeyCertCommands();
        cmdUtils.RegisterCommand(m_cmd_client_id, "<str>", "Client id to use (optional, default='test-*')");
        cmdUtils.RegisterCommand(m_cmd_port_override, "<int>", "The port override to use when connecting (optional)");
        s_addLoggingSendArgumentsStartLogging(argc, argv, api_handle, &cmdUtils);

        cmdData returnData = cmdData();
        s_parseCommonMQTTCommands(&cmdUtils, &returnData);
        returnData.input_cert = cmdUtils.GetCommandRequired(m_cmd_cert_file);
        returnData.input_key = cmdUtils.GetCommandRequired(m_cmd_key_file);
        returnData.input_clientId =
            cmdUtils.GetCommandOrDefault(m_cmd_client_id, Aws::Crt::String("test-") + Aws::Crt::UUID().ToString());
        if (cmdUtils.HasCommand(m_cmd_proxy_host))
        {
            returnData.input_proxyHost = cmdUtils.GetCommandRequired(m_cmd_proxy_host);
            returnData.input_proxyPort = atoi(cmdUtils.GetCommandOrDefault(m_cmd_proxy_port, "8080").c_str());
        }
        if (cmdUtils.HasCommand(m_cmd_port_override))
        {
            returnData.input_port = atoi(cmdUtils.GetCommandOrDefault(m_cmd_port_override, "0").c_str());
        }
        return returnData;
    }

    cmdData parseSampleInputCognitoConnect(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle)
    {
        CommandLineUtils cmdUtils = CommandLineUtils();
        cmdUtils.RegisterProgramName("cognito-connect");
        cmdUtils.AddCommonMQTTCommands();
        cmdUtils.AddCommonProxyCommands();
        cmdUtils.AddCognitoCommands();
        cmdUtils.RegisterCommand(m_cmd_signing_region, "<str>", "The signing region used for the websocket signer");
        cmdUtils.RegisterCommand(m_cmd_client_id, "<str>", "Client id to use (optional, default='test-*')");
        s_addLoggingSendArgumentsStartLogging(argc, argv, api_handle, &cmdUtils);

        cmdData returnData = cmdData();
        s_parseCommonMQTTCommands(&cmdUtils, &returnData);
        returnData.input_clientId =
            cmdUtils.GetCommandOrDefault(m_cmd_client_id, Aws::Crt::String("test-") + Aws::Crt::UUID().ToString());
        returnData.input_signingRegion = cmdUtils.GetCommandRequired(m_cmd_signing_region, m_cmd_region);
        returnData.input_cognitoEndpoint =
            "cognito-identity." + cmdUtils.GetCommandRequired(m_cmd_signing_region, m_cmd_region) + ".amazonaws.com";
        returnData.input_cognitoIdentity = cmdUtils.GetCommandRequired(m_cmd_cognito_identity);
        if (cmdUtils.HasCommand(m_cmd_proxy_host))
        {
            returnData.input_proxyHost = cmdUtils.GetCommandRequired(m_cmd_proxy_host);
            returnData.input_proxyPort = atoi(cmdUtils.GetCommandOrDefault(m_cmd_proxy_port, "8080").c_str());
        }
        return returnData;
    }

    cmdData parseSampleInputCustomAuthorizerConnect(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle)
    {
        CommandLineUtils cmdUtils = CommandLineUtils();
        cmdUtils.RegisterProgramName("custom-authorizer-connect");
        cmdUtils.AddCommonMQTTCommands();
        cmdUtils.AddCommonCustomAuthorizerCommands();
        cmdUtils.RegisterCommand(m_cmd_client_id, "<str>", "Client id to use (optional, default='test-*')");
        cmdUtils.RegisterCommand(m_cmd_custom_auth_password, "<str>", "Password (optional)");
        cmdUtils.RegisterCommand(m_cmd_custom_auth_token_name, "<str>", "Token name (optional)");
        cmdUtils.RegisterCommand(m_cmd_custom_token_value, "<str>", "Token value (optional)");
        cmdUtils.RemoveCommand("ca_file");
        s_addLoggingSendArgumentsStartLogging(argc, argv, api_handle, &cmdUtils);
        cmdData returnData = cmdData();
        s_parseCommonMQTTCommands(&cmdUtils, &returnData);
        returnData.input_clientId =
            cmdUtils.GetCommandOrDefault(m_cmd_client_id, Aws::Crt::String("test-") + Aws::Crt::UUID().ToString());
        returnData.input_customAuthUsername = cmdUtils.GetCommandOrDefault(m_cmd_custom_auth_username, "");
        returnData.input_customAuthorizerName = cmdUtils.GetCommandOrDefault(m_cmd_custom_auth_authorizer_name, "");
        returnData.input_customAuthorizerSignature =
            cmdUtils.GetCommandOrDefault(m_cmd_custom_auth_authorizer_signature, "");
        returnData.input_customAuthPassword = cmdUtils.GetCommandOrDefault(m_cmd_custom_auth_password, "");
        returnData.input_customTokenKeyName = cmdUtils.GetCommandOrDefault(m_cmd_custom_auth_token_name, "");
        returnData.input_customTokenValue = cmdUtils.GetCommandOrDefault(m_cmd_custom_token_value, "");

        return returnData;
    }

    cmdData parseSampleInputPKCS11Connect(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle)
    {

        CommandLineUtils cmdUtils = CommandLineUtils();
        cmdUtils.RegisterProgramName("pkcs11-connect");
        cmdUtils.AddCommonMQTTCommands();
        cmdUtils.RegisterCommand(m_cmd_cert_file, "<path>", "Path to your client certificate in PEM format.");
        cmdUtils.RegisterCommand(m_cmd_pkcs11_lib, "<path>", "Path to PKCS#11 library.");
        cmdUtils.RegisterCommand(m_cmd_pkcs11_pin, "<str>", "User PIN for logging into PKCS#11 token.");
        cmdUtils.RegisterCommand(m_cmd_pkcs11_token, "<str>", "Label of the PKCS#11 token to use (optional).");
        cmdUtils.RegisterCommand(m_cmd_pkcs11_slot, "<int>", "Slot ID containing PKCS#11 token to use (optional).");
        cmdUtils.RegisterCommand(m_cmd_pkcs11_key, "<str>", "Label of private key on the PKCS#11 token (optional).");
        cmdUtils.RegisterCommand(m_cmd_client_id, "<str>", "Client id to use (optional, default='test-*')");
        s_addLoggingSendArgumentsStartLogging(argc, argv, api_handle, &cmdUtils);

        cmdData returnData = cmdData();
        s_parseCommonMQTTCommands(&cmdUtils, &returnData);
        returnData.input_clientId =
            cmdUtils.GetCommandOrDefault(m_cmd_client_id, Aws::Crt::String("test-") + Aws::Crt::UUID().ToString());
        returnData.input_cert = cmdUtils.GetCommandRequired(m_cmd_cert_file);
        returnData.input_pkcs11LibPath = cmdUtils.GetCommandRequired(m_cmd_pkcs11_lib);
        returnData.input_pkcs11UserPin = cmdUtils.GetCommandRequired(m_cmd_pkcs11_pin);
        if (cmdUtils.HasCommand(m_cmd_pkcs11_token))
        {
            returnData.input_pkcs11TokenLabel = cmdUtils.GetCommand(m_cmd_pkcs11_token);
        }
        if (cmdUtils.HasCommand(m_cmd_pkcs11_slot))
        {
            returnData.input_pkcs11SlotId = std::stoull(cmdUtils.GetCommand(m_cmd_pkcs11_slot).c_str());
        }
        if (cmdUtils.HasCommand(m_cmd_pkcs11_key))
        {
            returnData.input_pkcs11KeyLabel = cmdUtils.GetCommand(m_cmd_pkcs11_key);
        }
        return returnData;
    }
    cmdData parseSampleInputWebsocketStaticCredentialsConnect(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle)
    {
        CommandLineUtils cmdUtils = CommandLineUtils();
        cmdUtils.RegisterProgramName("websocket-connect");
        cmdUtils.AddCommonMQTTCommands();
        cmdUtils.AddCommonProxyCommands();
        cmdUtils.RegisterCommand(m_cmd_signing_region, "<str>", "The signing region used for the websocket signer");
        cmdUtils.RegisterCommand(m_cmd_client_id, "<str>", "Client id to use (optional, default='test-*')");
        cmdUtils.RegisterCommand(m_cmd_port_override, "<int>", "The port override to use when connecting (optional)");
        cmdUtils.RegisterCommand(m_cmd_access_key_id, "<str>", "The Access key ID (optional)");
        cmdUtils.RegisterCommand(m_cmd_secret_access_key, "<str>", "The secret access key (optional)");
        cmdUtils.RegisterCommand(m_cmd_session_token, "<str>", "The session token (optional)");
        s_addLoggingSendArgumentsStartLogging(argc, argv, api_handle, &cmdUtils);

        cmdData returnData = cmdData();
        s_parseCommonMQTTCommands(&cmdUtils, &returnData);
        returnData.input_clientId =
            cmdUtils.GetCommandOrDefault(m_cmd_client_id, Aws::Crt::String("test-") + Aws::Crt::UUID().ToString());
        returnData.input_signingRegion = cmdUtils.GetCommandRequired(m_cmd_signing_region, m_cmd_region);
        if (cmdUtils.HasCommand(m_cmd_proxy_host))
        {
            returnData.input_proxyHost = cmdUtils.GetCommandRequired(m_cmd_proxy_host);
            returnData.input_proxyPort = atoi(cmdUtils.GetCommandOrDefault(m_cmd_proxy_port, "8080").c_str());
        }
        if (cmdUtils.HasCommand(m_cmd_port_override))
        {
            returnData.input_port = atoi(cmdUtils.GetCommandOrDefault(m_cmd_port_override, "0").c_str());
        }
        returnData.input_accessKeyId = cmdUtils.GetCommandRequired(m_cmd_access_key_id, "");
        returnData.input_secretAccessKey = cmdUtils.GetCommandRequired(m_cmd_secret_access_key, "");
        returnData.input_sessionToken = cmdUtils.GetCommandRequired(m_cmd_session_token, "");
        return returnData;
    }

    cmdData parseSampleInputWebsocketConnect(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle)
    {
        CommandLineUtils cmdUtils = CommandLineUtils();
        cmdUtils.RegisterProgramName("websocket-connect");
        cmdUtils.AddCommonMQTTCommands();
        cmdUtils.AddCommonProxyCommands();
        cmdUtils.RegisterCommand(m_cmd_signing_region, "<str>", "The signing region used for the websocket signer");
        cmdUtils.RegisterCommand(m_cmd_client_id, "<str>", "Client id to use (optional, default='test-*')");
        cmdUtils.RegisterCommand(m_cmd_port_override, "<int>", "The port override to use when connecting (optional)");
        cmdUtils.RegisterCommand(m_cmd_access_key_id, "<str>", "The Access key ID (optional)");
        cmdUtils.RegisterCommand(m_cmd_secret_access_key, "<str>", "The secret access key (optional)");
        cmdUtils.RegisterCommand(m_cmd_session_token, "<str>", "The session token (optional)");
        s_addLoggingSendArgumentsStartLogging(argc, argv, api_handle, &cmdUtils);

        cmdData returnData = cmdData();
        s_parseCommonMQTTCommands(&cmdUtils, &returnData);
        returnData.input_clientId =
            cmdUtils.GetCommandOrDefault(m_cmd_client_id, Aws::Crt::String("test-") + Aws::Crt::UUID().ToString());
        returnData.input_signingRegion = cmdUtils.GetCommandRequired(m_cmd_signing_region, m_cmd_region);
        if (cmdUtils.HasCommand(m_cmd_proxy_host))
        {
            returnData.input_proxyHost = cmdUtils.GetCommandRequired(m_cmd_proxy_host);
            returnData.input_proxyPort = atoi(cmdUtils.GetCommandOrDefault(m_cmd_proxy_port, "8080").c_str());
        }
        if (cmdUtils.HasCommand(m_cmd_port_override))
        {
            returnData.input_port = atoi(cmdUtils.GetCommandOrDefault(m_cmd_port_override, "0").c_str());
        }
        return returnData;
    }

    cmdData parseSampleInputWindowsCertificateConnect(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle)
    {
        CommandLineUtils cmdUtils = CommandLineUtils();
        cmdUtils.RegisterProgramName("windows-cert-connect");
        cmdUtils.AddCommonMQTTCommands();
        cmdUtils.RegisterCommand(
            m_cmd_cert_file,
            "<str>",
            "Your client certificate in the Windows certificate store. e.g. "
            "'CurrentUser\\MY\\6ac133ac58f0a88b83e9c794eba156a98da39b4c'");
        cmdUtils.RegisterCommand(m_cmd_client_id, "<str>", "Client id to use (optional, default='test-*').");
        cmdUtils.RegisterCommand(m_cmd_port_override, "<int>", "The port override to use when connecting (optional)");
        s_addLoggingSendArgumentsStartLogging(argc, argv, api_handle, &cmdUtils);

        cmdData returnData = cmdData();
        s_parseCommonMQTTCommands(&cmdUtils, &returnData);
        returnData.input_clientId =
            cmdUtils.GetCommandOrDefault(m_cmd_client_id, Aws::Crt::String("test-") + Aws::Crt::UUID().ToString());
        returnData.input_cert = cmdUtils.GetCommandRequired(m_cmd_cert_file);
        if (cmdUtils.HasCommand(m_cmd_port_override))
        {
            returnData.input_port = atoi(cmdUtils.GetCommandOrDefault(m_cmd_port_override, "0").c_str());
        }
        return returnData;
    }

    cmdData parseSampleInputX509Connect(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle)
    {
        CommandLineUtils cmdUtils = CommandLineUtils();
        cmdUtils.RegisterProgramName("x509-credentials-provider-connect");
        cmdUtils.AddCommonMQTTCommands();
        cmdUtils.AddCommonX509Commands();
        cmdUtils.RegisterCommand(m_cmd_signing_region, "<str>", "Used for websocket signer");
        cmdUtils.RegisterCommand(m_cmd_client_id, "<str>", "Client id to use (optional, default='test-*')");
        s_addLoggingSendArgumentsStartLogging(argc, argv, api_handle, &cmdUtils);

        cmdData returnData = cmdData();
        s_parseCommonMQTTCommands(&cmdUtils, &returnData);
        returnData.input_clientId =
            cmdUtils.GetCommandOrDefault(m_cmd_client_id, Aws::Crt::String("test-") + Aws::Crt::UUID().ToString());
        returnData.input_signingRegion = cmdUtils.GetCommandRequired(m_cmd_signing_region, m_cmd_region);
        if (cmdUtils.HasCommand(m_cmd_proxy_host))
        {
            returnData.input_proxyHost = cmdUtils.GetCommandRequired(m_cmd_proxy_host);
            returnData.input_proxyPort = atoi(cmdUtils.GetCommandOrDefault(m_cmd_proxy_port, "8080").c_str());
        }
        returnData.input_x509Endpoint = cmdUtils.GetCommandRequired(m_cmd_x509_endpoint);
        returnData.input_x509Role = cmdUtils.GetCommandRequired(m_cmd_x509_role);
        returnData.input_x509ThingName = cmdUtils.GetCommandRequired(m_cmd_x509_thing_name);
        returnData.input_x509Cert = cmdUtils.GetCommandRequired(m_cmd_x509_cert_file);
        returnData.input_x509Key = cmdUtils.GetCommandRequired(m_cmd_x509_key_file);
        if (cmdUtils.HasCommand(m_cmd_x509_ca_file))
        {
            returnData.input_x509Ca = cmdUtils.GetCommandRequired(m_cmd_x509_ca_file);
        }

        return returnData;
    }

    cmdData parseSampleInputPubSub(
        int argc,
        char *argv[],
        Aws::Crt::ApiHandle *api_handle,
        Aws::Crt::String programName)
    {
        CommandLineUtils cmdUtils = CommandLineUtils();
        cmdUtils.RegisterProgramName(programName);
        cmdUtils.AddCommonMQTTCommands();
        cmdUtils.AddCommonKeyCertCommands();
        cmdUtils.AddCommonProxyCommands();
        cmdUtils.AddCommonTopicMessageCommands();
        cmdUtils.RegisterCommand(m_cmd_client_id, "<str>", "Client id to use (optional, default='test-*')");
        cmdUtils.RegisterCommand(m_cmd_port_override, "<int>", "The port override to use when connecting (optional)");
        cmdUtils.RegisterCommand(m_cmd_count, "<int>", "The number of messages to send (optional, default='10')");
        s_addLoggingSendArgumentsStartLogging(argc, argv, api_handle, &cmdUtils);

        cmdData returnData = cmdData();
        s_parseCommonMQTTCommands(&cmdUtils, &returnData);
        returnData.input_cert = cmdUtils.GetCommandRequired(m_cmd_cert_file);
        returnData.input_key = cmdUtils.GetCommandRequired(m_cmd_key_file);
        returnData.input_clientId =
            cmdUtils.GetCommandOrDefault(m_cmd_client_id, Aws::Crt::String("test-") + Aws::Crt::UUID().ToString());
        if (cmdUtils.HasCommand(m_cmd_proxy_host))
        {
            returnData.input_proxyHost = cmdUtils.GetCommandRequired(m_cmd_proxy_host);
            returnData.input_proxyPort = atoi(cmdUtils.GetCommandOrDefault(m_cmd_proxy_port, "8080").c_str());
        }
        s_populateTopic(&cmdUtils, &returnData);
        returnData.input_message = cmdUtils.GetCommandOrDefault(m_cmd_message, "Hello World ");
        returnData.input_count = atoi(cmdUtils.GetCommandOrDefault(m_cmd_count, "10").c_str());
        if (cmdUtils.HasCommand(m_cmd_port_override))
        {
            returnData.input_port = atoi(cmdUtils.GetCommandOrDefault(m_cmd_port_override, "0").c_str());
        }

        return returnData;
    }

    cmdData parseSampleInputSharedSubscription(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle)
    {
        CommandLineUtils cmdUtils = CommandLineUtils();
        cmdUtils.RegisterProgramName("mqtt5-shared-subscription");
        cmdUtils.AddCommonMQTTCommands();
        cmdUtils.AddCommonKeyCertCommands();
        cmdUtils.AddCommonTopicMessageCommands();
        cmdUtils.RegisterCommand(m_cmd_client_id, "<str>", "Client id to use (optional, default='test-*')");
        cmdUtils.RegisterCommand(m_cmd_count, "<int>", "The number of messages to send (optional, default='10')");
        cmdUtils.RegisterCommand(
            m_cmd_group_identifier,
            "<str>",
            "The group identifier to use in the shared subscription (optional, default='cpp-sample')");
        s_addLoggingSendArgumentsStartLogging(argc, argv, api_handle, &cmdUtils);

        cmdData returnData = cmdData();
        s_parseCommonMQTTCommands(&cmdUtils, &returnData);
        returnData.input_cert = cmdUtils.GetCommandRequired(m_cmd_cert_file);
        returnData.input_key = cmdUtils.GetCommandRequired(m_cmd_key_file);
        returnData.input_clientId =
            cmdUtils.GetCommandOrDefault(m_cmd_client_id, Aws::Crt::String("test-") + Aws::Crt::UUID().ToString());
        s_populateTopic(&cmdUtils, &returnData);
        returnData.input_message = cmdUtils.GetCommandOrDefault(m_cmd_message, "Hello World ");
        returnData.input_count = atoi(cmdUtils.GetCommandOrDefault(m_cmd_count, "10").c_str());
        returnData.input_groupIdentifier = cmdUtils.GetCommandOrDefault(m_cmd_group_identifier, "cpp-sample");

        return returnData;
    }

    cmdData parseSampleInputCyclePubSub(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle)
    {
        CommandLineUtils cmdUtils = CommandLineUtils();
        cmdUtils.RegisterProgramName("cycle-pub-sub");
        cmdUtils.AddCommonMQTTCommands();
        cmdUtils.AddCommonKeyCertCommands();
        cmdUtils.RegisterCommand(
            m_cmd_clients, "<int>", "The number of clients/connections to make (optional, default='3'");
        cmdUtils.RegisterCommand(
            m_cmd_tps, "<int>", "The number of seconds to wait after performing an operation (optional, default=12)");
        cmdUtils.RegisterCommand(
            m_cmd_seconds, "<int>", "The number of seconds to run the sample for (optional, default='300')");
        s_addLoggingSendArgumentsStartLogging(argc, argv, api_handle, &cmdUtils);

        cmdData returnData = cmdData();
        s_parseCommonMQTTCommands(&cmdUtils, &returnData);
        returnData.input_cert = cmdUtils.GetCommandRequired(m_cmd_cert_file);
        returnData.input_key = cmdUtils.GetCommandRequired(m_cmd_key_file);
        returnData.input_clients = atoi(cmdUtils.GetCommandOrDefault(m_cmd_clients, "3").c_str());
        returnData.input_tps = atoi(cmdUtils.GetCommandOrDefault(m_cmd_tps, "12").c_str());
        returnData.input_seconds = atoi(cmdUtils.GetCommandOrDefault(m_cmd_seconds, "300").c_str());
        return returnData;
    }

    cmdData parseSampleInputSecureTunnel(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle)
    {
        CommandLineUtils cmdUtils = CommandLineUtils();
        cmdUtils.RegisterProgramName("secure-tunnel");
        cmdUtils.AddCommonProxyCommands();
        cmdUtils.RegisterCommand(m_cmd_signing_region, "<str>", "The region of your secure tunnel");
        cmdUtils.RegisterCommand(
            m_cmd_ca_file, "<path>", "Path to AmazonRootCA1.pem (optional, system trust store used by default).");
        cmdUtils.RegisterCommand(
            m_cmd_access_token_file,
            "<path>",
            "Path to the tunneling access token file (optional if --access_token used).");
        cmdUtils.RegisterCommand(
            m_cmd_access_token, "<str>", "Tunneling access token (optional if --access_token_file used).");
        cmdUtils.RegisterCommand(
            m_cmd_local_proxy_mode_source,
            "<str>",
            "Use to set local proxy mode to source (optional, default='destination').");
        cmdUtils.RegisterCommand(
            m_cmd_client_token_file, "<path>", "Path to the tunneling client token (optional if --client_token used).");
        cmdUtils.RegisterCommand(
            m_cmd_client_token, "<str>", "Tunneling client token (optional if --client_token_file used).");
        cmdUtils.RegisterCommand(m_cmd_message, "<str>", "Message to send (optional, default='Hello World!').");
        cmdUtils.RegisterCommand(
            m_cmd_proxy_user_name, "<str>", "User name passed if proxy server requires a user name (optional)");
        cmdUtils.RegisterCommand(
            m_cmd_proxy_password, "<str>", "Password passed if proxy server requires a password (optional)");
        cmdUtils.RegisterCommand(
            m_cmd_count, "<int>", "Number of messages to send before completing (optional, default='5')");
        s_addLoggingSendArgumentsStartLogging(argc, argv, api_handle, &cmdUtils);

        cmdData returnData = cmdData();
        returnData.input_signingRegion = cmdUtils.GetCommandRequired(m_cmd_signing_region, m_cmd_region);
        returnData.input_endpoint = "data.tunneling.iot." + returnData.input_signingRegion + ".amazonaws.com";
        returnData.input_accessTokenFile = cmdUtils.GetCommandOrDefault(m_cmd_access_token_file, "");
        returnData.input_accessToken = cmdUtils.GetCommandOrDefault(m_cmd_access_token, "");
        if (returnData.input_accessToken == "" && returnData.input_accessTokenFile != "")
        {
            Aws::Crt::String tempAccessToken;

            std::ifstream accessTokenFile(returnData.input_accessTokenFile.c_str());
            if (accessTokenFile.is_open())
            {
                getline(accessTokenFile, returnData.input_accessToken);
                accessTokenFile.close();
            }
            else
            {
                fprintf(stderr, "Failed to open access token file");
                exit(-1);
            }
        }

        returnData.input_clientTokenFile = cmdUtils.GetCommandOrDefault(m_cmd_client_token_file, "");
        returnData.input_clientToken = cmdUtils.GetCommandOrDefault(m_cmd_client_token, "");
        if (returnData.input_clientToken == "" && returnData.input_clientTokenFile != "")
        {
            Aws::Crt::String tempAccessToken;

            std::ifstream clientTokenFile(returnData.input_clientTokenFile.c_str());
            if (clientTokenFile.is_open())
            {
                getline(clientTokenFile, returnData.input_clientToken);
                clientTokenFile.close();
            }
            else
            {
                fprintf(stderr, "Failed to open client token file");
                exit(-1);
            }
        }

        returnData.input_localProxyModeSource =
            cmdUtils.GetCommandOrDefault(m_cmd_local_proxy_mode_source, "destination");
        returnData.input_message = cmdUtils.GetCommandOrDefault(m_cmd_message, "Hello World!");
        if (cmdUtils.HasCommand(m_cmd_proxy_host))
        {
            returnData.input_proxyHost = cmdUtils.GetCommandRequired(m_cmd_proxy_host);
            returnData.input_proxyPort = atoi(cmdUtils.GetCommandOrDefault(m_cmd_proxy_port, "8080").c_str());
        }
        returnData.input_proxyUserName = cmdUtils.GetCommandOrDefault(m_cmd_proxy_user_name, "");
        returnData.input_proxyPassword = cmdUtils.GetCommandOrDefault(m_cmd_proxy_password, "");
        returnData.input_count = atoi(cmdUtils.GetCommandOrDefault(m_cmd_count, "5").c_str());
        return returnData;
    }

    cmdData parseSampleInputSecureTunnelNotification(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle)
    {
        Utils::CommandLineUtils cmdUtils = Utils::CommandLineUtils();
        cmdUtils.RegisterProgramName("tunnel-notification");
        cmdUtils.AddCommonMQTTCommands();
        cmdUtils.AddCommonKeyCertCommands();
        cmdUtils.RegisterCommand(m_cmd_thing_name, "<str>", "The name of your IOT thing");
        cmdUtils.RegisterCommand(m_cmd_client_id, "<str>", "Client id to use (optional, default='test-*')");
        s_addLoggingSendArgumentsStartLogging(argc, argv, api_handle, &cmdUtils);

        cmdData returnData = cmdData();
        s_parseCommonMQTTCommands(&cmdUtils, &returnData);
        returnData.input_cert = cmdUtils.GetCommandRequired(m_cmd_cert_file);
        returnData.input_key = cmdUtils.GetCommandRequired(m_cmd_key_file);
        returnData.input_thingName = cmdUtils.GetCommandRequired(m_cmd_thing_name);
        returnData.input_clientId =
            cmdUtils.GetCommandOrDefault(m_cmd_client_id, Aws::Crt::String("test-") + Aws::Crt::UUID().ToString());
        return returnData;
    }

    cmdData parseSampleInputShadow(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle)
    {
        Utils::CommandLineUtils cmdUtils = Utils::CommandLineUtils();
        cmdUtils.RegisterProgramName("shadow_sync");
        cmdUtils.AddCommonMQTTCommands();
        cmdUtils.AddCommonKeyCertCommands();
        cmdUtils.RegisterCommand(m_cmd_thing_name, "<str>", "The name of your IOT thing");
        cmdUtils.RegisterCommand(
            m_cmd_shadow_property,
            "<str>",
            "The name of the shadow property you want to change (optional, default='color')");
        cmdUtils.RegisterCommand(m_cmd_client_id, "<str>", "Client id to use (optional, default='test-*')");
        s_addLoggingSendArgumentsStartLogging(argc, argv, api_handle, &cmdUtils);

        cmdData returnData = cmdData();
        s_parseCommonMQTTCommands(&cmdUtils, &returnData);
        returnData.input_cert = cmdUtils.GetCommandRequired(m_cmd_cert_file);
        returnData.input_key = cmdUtils.GetCommandRequired(m_cmd_key_file);
        returnData.input_thingName = cmdUtils.GetCommandRequired(m_cmd_thing_name);
        returnData.input_shadowProperty = cmdUtils.GetCommandOrDefault(m_cmd_shadow_property, "color");
        returnData.input_clientId =
            cmdUtils.GetCommandOrDefault(m_cmd_client_id, Aws::Crt::String("test-") + Aws::Crt::UUID().ToString());

        returnData.input_shadowName = cmdUtils.GetCommandOrDefault(m_cmd_shadow_name, "");
        returnData.input_shadowValue = cmdUtils.GetCommandOrDefault(m_cmd_shadow_value, "");
        return returnData;
    }

    cmdData parseSampleInputPKCS12Connect(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle)
    {
        CommandLineUtils cmdUtils = CommandLineUtils();
        cmdUtils.RegisterProgramName("pkcs12-connect");
        cmdUtils.AddCommonMQTTCommands();
        cmdUtils.RegisterCommand(m_cmd_pkcs12_file, "<path>", "Path to the PKCS#12 file.");
        cmdUtils.RegisterCommand(m_cmd_pkcs12_password, "<str>", "Password for the PKCS#12 file.");
        cmdUtils.RegisterCommand(m_cmd_client_id, "<str>", "Client id to use (optional, default='test-*')");
        s_addLoggingSendArgumentsStartLogging(argc, argv, api_handle, &cmdUtils);

        cmdData returnData = cmdData();
        s_parseCommonMQTTCommands(&cmdUtils, &returnData);
        returnData.input_clientId =
            cmdUtils.GetCommandOrDefault(m_cmd_client_id, Aws::Crt::String("test-") + Aws::Crt::UUID().ToString());
        returnData.input_pkcs12File = cmdUtils.GetCommandRequired(m_cmd_pkcs12_file);
        returnData.input_pkcs12Password = cmdUtils.GetCommandRequired(m_cmd_pkcs12_password);
        return returnData;
    }

} // namespace Utils
