#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/crt/Api.h>
#include <aws/crt/Types.h>
#include <aws/iot/MqttClient.h>

namespace Utils
{
    /**
     * A struct to hold the command line options that can be passed to the program from the terminal/console.
     */
    struct CommandLineOption
    {
        Aws::Crt::String m_commandName;
        Aws::Crt::String m_exampleInput;
        Aws::Crt::String m_helpOutput;

        CommandLineOption(
            Aws::Crt::String inputName,
            Aws::Crt::String inputExampleInput,
            Aws::Crt::String inputHelp = "")
        {
            m_commandName = std::move(inputName);
            m_exampleInput = std::move(inputExampleInput);
            m_helpOutput = std::move(inputHelp);
        }
    };

    /**
     * A helper class that makes it easier to register, find, and parse commands passed to the program from the
     * terminal/console.
     */
    class CommandLineUtils
    {
      public:
        CommandLineUtils();

        /**
         * Changes the program name to the name given. The program name is shown when calling help and showing all the
         * commands.
         * @param newProgramName The program name to show when executing PrintHelp
         */
        void RegisterProgramName(Aws::Crt::String newProgramName);

        /**
         * Adds a new command to the utility. Used to show command data when printing all commands.
         * @param newCommand The command struct containing the new command/argument data
         */
        void RegisterCommand(CommandLineOption newCommand);
        /**
         * Adds a new command to the utility. Used to show command data when printing all commands.
         * @param commandName The name of the command
         * @param exampleInput Example input for the command (example "<endpoint>")
         * @param helpOutput The message to show with the command when printing all commands via help
         */
        void RegisterCommand(
            Aws::Crt::String commandName,
            Aws::Crt::String exampleInput,
            Aws::Crt::String helpOutput = "");

        /**
         * Removes the command if it has already been registered
         * @param commandName
         */
        void RemoveCommand(Aws::Crt::String commandName);

        /**
         * Updates the help text of a registered command. If the given command does not exist, nothing happens
         * @param commandName The name of the command
         * @param newCommandHelp
         */
        void UpdateCommandHelp(Aws::Crt::String commandName, Aws::Crt::String newCommandHelp);

        /**
         * Called to give the class a copy of the begin and end character pointers that contain the arguments from the
         * terminal/console
         * @param argv The beginning of terminal/console input
         * @param argc The end of terminal/console input
         */
        void SendArguments(const char **argv, const char **argc);

        /**
         * Returns true if the command was inputted into the terminal/console
         *
         * You must call SendArguments and pass terminal/console input first in order for the function to work.
         *
         * @param CommandName The name of the command you are looking for
         * @return true If the command is found
         * @return false If the command is not found
         */
        bool HasCommand(Aws::Crt::String CommandName);

        /**
         * Gets the value of the command passed into the console/terminal. This function assumes the command exists
         * and was passed into the program through the console/terminal.
         *
         * You must call SendArguments and pass console/terminal input first in order for the function to work.
         *
         * @param CommandName The name of the command you want to get the value of
         * @return Aws::Crt::String The value passed into the program at the command name
         */
        Aws::Crt::String GetCommand(Aws::Crt::String CommandName);

        /**
         * Gets the value of the command passed into the console/terminal if it exists, otherwise it returns
         * whatever value is appsed into CommandDefault
         *
         * You must call SendArguments and pass console/terminal input first in order for the function to work.
         *
         * @param CommandName The name of the command you want to get the value of
         * @param CommandDefault The value to assign if the command does not exist
         * @return Aws::Crt::String The value passed into the program at the command name
         */
        Aws::Crt::String GetCommandOrDefault(Aws::Crt::String CommandName, Aws::Crt::String CommandDefault);

        /**
         * Gets the value of the command passed into the console/terminal if it exists. If it does not exist,
         * the program will exit with an error message.
         *
         * @param CommandName The name of the command you want to get the value of
         * @return Aws::Crt::String The value passed into the program at the command name
         */
        Aws::Crt::String GetCommandRequired(Aws::Crt::String CommandName);

        /**
         * Gets the value of the command passed into the console/terminal if it exists. If it does not exist,
         * the program will exit with an error message.
         *
         * @param CommandName The name of the command you want to get the value of
         * @param CommandNameAlt The alternate name of the command you want to get the value of
         * @return Aws::Crt::String The value passed into the program at the command name
         */
        Aws::Crt::String GetCommandRequired(Aws::Crt::String CommandName, Aws::Crt::String CommandNameAlt);

        /**
         * Prints to the console/terminal all of the commands and their descriptions.
         */
        void PrintHelp();

        /**
         * A helper function that adds endpoint and ca_file commands
         */
        void AddCommonMQTTCommands();

        /**
         * A helper function that adds proxy_port, proxy_username, proxy_password, and proxy_host commands.
         */
        void AddCommonProxyCommands();

        /**
         * A helper function that adds x509, x509_role_alias, x509_endpoint, x509_thing, x509_cert,
         * x509_key, and x509_ca_file commands
         */
        void AddCommonX509Commands();

        /**
         * A helper function that adds key and cert commands
         */
        void AddCommonKeyCertCommands();

        /**
         * A helper function that adds topic and message commands
         */
        void AddCommonTopicMessageCommands();

        /**
         * A helper function that adds custom_auth_username, custom_auth_authorizer_name,
         * custom_auth_authorizer_signature and custom_auth_password commands
         */
        void AddCommonCustomAuthorizerCommands();

        /**
         * A helper function that adds the cognito_identity command
         */
        void AddCognitoCommands();

        /**
         * A helper function that adds the verbosity command for controlling logging in the samples
         */
        void AddLoggingCommands();

        /**
         * Starts logging based on the result of the verbosity command
         */
        void StartLoggingBasedOnCommand(Aws::Crt::ApiHandle *apiHandle);

      private:
        Aws::Crt::String m_programName = "Application";
        const char **m_beginPosition = nullptr;
        const char **m_endPosition = nullptr;
        Aws::Crt::Map<Aws::Crt::String, CommandLineOption> m_registeredCommands;

        Aws::Crt::Http::HttpClientConnectionProxyOptions GetProxyOptionsForMQTTConnection();
        std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> GetClientConnectionForMQTTConnection(
            Aws::Iot::MqttClient *client,
            Aws::Iot::MqttClientConnectionConfigBuilder *clientConfigBuilder);
    };

    struct cmdData
    {
        // General use
        Aws::Crt::String input_endpoint;
        Aws::Crt::String input_cert;
        Aws::Crt::String input_key;
        Aws::Crt::String input_ca;
        Aws::Crt::String input_clientId;
        uint64_t input_port;
        bool input_isCI;
        // Proxy
        Aws::Crt::String input_proxyHost;
        uint64_t input_proxyPort;
        // PubSub
        Aws::Crt::String input_topic;
        Aws::Crt::String input_message;
        uint64_t input_count;
        // Websockets
        Aws::Crt::String input_signingRegion;

        Aws::Crt::String input_accessKeyId;
        Aws::Crt::String input_secretAccessKey;
        Aws::Crt::String input_sessionToken;
        // Cognito
        Aws::Crt::String input_cognitoIdentity;
        Aws::Crt::String input_cognitoEndpoint;
        // Custom auth
        Aws::Crt::String input_customAuthUsername;
        Aws::Crt::String input_customAuthorizerName;
        Aws::Crt::String input_customAuthorizerSignature;
        Aws::Crt::String input_customAuthPassword;
        Aws::Crt::String input_customTokenKeyName;
        Aws::Crt::String input_customTokenValue;
        // Fleet provisioning
        Aws::Crt::String input_templateName;
        Aws::Crt::String input_templateParameters;
        Aws::Crt::String input_csrPath;
        // Services (Shadow, Jobs, Greengrass, etc)
        Aws::Crt::String input_thingName;
        Aws::Crt::String input_mode;
        // Java Keystore
        Aws::Crt::String input_keystore;
        Aws::Crt::String input_keystorePassword;
        Aws::Crt::String input_keystoreFormat;
        Aws::Crt::String input_certificateAlias;
        Aws::Crt::String input_certificatePassword;
        // Shared Subscription
        Aws::Crt::String input_groupIdentifier;
        // PKCS#11
        Aws::Crt::String input_pkcs11LibPath;
        Aws::Crt::String input_pkcs11UserPin;
        Aws::Crt::String input_pkcs11TokenLabel;
        uint64_t input_pkcs11SlotId;
        Aws::Crt::String input_pkcs11KeyLabel;
        // X509
        Aws::Crt::String input_x509Endpoint;
        Aws::Crt::String input_x509Role;
        Aws::Crt::String input_x509ThingName;
        Aws::Crt::String input_x509Cert;
        Aws::Crt::String input_x509Key;
        Aws::Crt::String input_x509Ca;
        // Device Defender
        uint64_t input_reportTime;
        // Jobs
        Aws::Crt::String input_jobId;
        // Cycle PubSub
        uint64_t input_clients;
        uint64_t input_tps;
        uint64_t input_seconds;
        // Secure Tunnel
        Aws::Crt::String input_accessTokenFile;
        Aws::Crt::String input_accessToken;
        Aws::Crt::String input_localProxyModeSource;
        Aws::Crt::String input_clientTokenFile;
        Aws::Crt::String input_clientToken;
        Aws::Crt::String input_proxyUserName;
        Aws::Crt::String input_proxyPassword;
        // Shadow
        Aws::Crt::String input_shadowProperty;
        Aws::Crt::String input_shadowName;
        Aws::Crt::String input_shadowValue;
        // PKCS12
        Aws::Crt::String input_pkcs12File;
        Aws::Crt::String input_pkcs12Password;
        // Greengrass Discovery
        bool input_PrintDiscoverRespOnly;
        // MQTT protocol version
        uint64_t input_mqtt_version;
    };

    cmdData parseSampleInputDeviceDefender(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle);
    cmdData parseSampleInputGreengrassDiscovery(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle);
    cmdData parseSampleInputGreengrassIPC(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle);
    cmdData parseSampleInputFleetProvisioning(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle);
    cmdData parseSampleInputJobs(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle);
    cmdData parseSampleInputBasicConnect(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle);
    cmdData parseSampleInputCognitoConnect(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle);
    cmdData parseSampleInputCustomAuthorizerConnect(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle);
    cmdData parseSampleInputPKCS11Connect(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle);
    cmdData parseSampleInputWebsocketStaticCredentialsConnect(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle);
    cmdData parseSampleInputWebsocketConnect(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle);
    cmdData parseSampleInputWindowsCertificateConnect(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle);
    cmdData parseSampleInputX509Connect(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle);
    cmdData parseSampleInputPubSub(
        int argc,
        char *argv[],
        Aws::Crt::ApiHandle *api_handle,
        Aws::Crt::String programName);
    cmdData parseSampleInputSharedSubscription(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle);
    cmdData parseSampleInputCyclePubSub(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle);
    cmdData parseSampleInputSecureTunnel(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle);
    cmdData parseSampleInputSecureTunnelNotification(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle);
    cmdData parseSampleInputShadow(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle);
    cmdData parseSampleInputPKCS12Connect(int argc, char *argv[], Aws::Crt::ApiHandle *api_handle);

} // namespace Utils
