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
        Aws::Crt::String GetCommandRequired(
            Aws::Crt::String CommandName,
            Aws::Crt::String OptionalAdditionalMessage = "");

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
         * A helper function that adds topic and message commands
         */
        void AddCommonTopicMessageCommands();

        /**
         * A helper function that adds custom_auth_username, custom_auth_authorizer_name,
         * custom_auth_authorizer_signature and custom_auth_password commands
         */
        void AddCommonCustomAuthorizerCommands();

        /**
         * A helper function that adds the verbosity command for controlling logging in the samples
         */
        void AddLoggingCommands();

        /**
         * Starts logging based on the result of the verbosity command
         */
        void StartLoggingBasedOnCommand(Aws::Crt::ApiHandle *apiHandle);

        /**
         * A helper function that builds and returns a PKCS11 direct MQTT connection.
         *
         * Will get the required data from the CommandLineUtils from "pkcs111_lib", "pin", "token_label",
         * "slot_id", and "key_label" commands. See mqtt/pkcs11_connect for example setup.
         * @param client The client to use to make the connection.
         * @return The created direct PKCS11 MQTT connection.
         */
        std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> BuildPKCS11MQTTConnection(Aws::Iot::MqttClient *client);

        /**
         * A helper function that builds and returns a websocket x509 MQTT connection.
         *
         * Will get the required data from the CommandLineUtils from arguments defined in the
         * AddCommonX509Commands function. See mqtt/x509_connect for example setup.
         * @param client The client to use to make the connection.
         * @return The created websocket x509 MQTT connection.
         */
        std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> BuildWebsocketX509MQTTConnection(Aws::Iot::MqttClient *client);

        /**
         * A helper function that builds and returns a websocket MQTT connection.
         *
         * Will get the required data from the CommandLineUtils from the "signing_region" command.
         * See mqtt/websocket_connect for example setup.
         * @param client The client to use to make the connection
         * @return The created websocket MQTT connection
         */
        std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> BuildWebsocketMQTTConnection(Aws::Iot::MqttClient *client);

        /**
         * A helper function that builds and returns a direct MQTT connection using a key and certificate.
         * @param client The client to use to make the connection
         *
         * Will get the required data from the CommandLineUtils from arguments defined in the
         * AddCommonMQTTCommands function, "cert" command, and "key" command. See mqtt/basic_connect for example.
         * @return The created direct MQTT connection
         */
        std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> BuildDirectMQTTConnection(Aws::Iot::MqttClient *client);

        /**
         * A helper function that builds and returns a direct MQTT connection that will connect through a
         * CustomAuthorizer.
         * @param client The client to use to make the connection
         *
         * Will get the required data from the CommandLineUtils from arguments defined in the
         * AddCommonCustomAuthorizerCommands. See mqtt/custom_authorizer_connect for example.
         * @return The created direct MQTT connection
         */
        std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> BuildDirectMQTTConnectionWithCustomAuthorizer(
            Aws::Iot::MqttClient *client);

        /**
         * A helper function that builds and returns a MQTT connection automatically based
         * on the commands passed into CommandLineUtils. Will make a direct MQTT connection, PKCS11 MQTT connection,
         * a websocket connection, or a x509 connection via websockets.
         * @return The automatically created connection
         */
        std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> BuildMQTTConnection();

        /**
         * A helper function that uses a MQTT connection to connect, and then disconnect from AWS servers. This is used
         * in all the connect samples to show how to make a connection.
         * @param connection The MqttConnection to use when making a connection
         * @param clientId The client ID to send with the connection
         */
        void SampleConnectAndDisconnect(
            std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> connection,
            Aws::Crt::String clientId);

      private:
        Aws::Crt::String m_programName = "Application";
        const char **m_beginPosition = nullptr;
        const char **m_endPosition = nullptr;
        Aws::Crt::Map<Aws::Crt::String, CommandLineOption> m_registeredCommands;

        Aws::Iot::MqttClient m_internal_client;
        Aws::Crt::Http::HttpClientConnectionProxyOptions GetProxyOptionsForMQTTConnection();
        std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> GetClientConnectionForMQTTConnection(
            Aws::Iot::MqttClient *client,
            Aws::Iot::MqttClientConnectionConfigBuilder *clientConfigBuilder);

        /** Constants for commonly used/needed commands */
        const Aws::Crt::String m_cmd_endpoint = "endpoint";
        const Aws::Crt::String m_cmd_ca_file = "ca_file";
        const Aws::Crt::String m_cmd_cert_file = "cert";
        const Aws::Crt::String m_cmd_key_file = "key";
        const Aws::Crt::String m_cmd_proxy_host = "proxy_host";
        const Aws::Crt::String m_cmd_proxy_port = "proxy_port";
        const Aws::Crt::String m_cmd_signing_region = "signing_region";
        const Aws::Crt::String m_cmd_x509_endpoint = "x509_endpoint";
        const Aws::Crt::String m_cmd_x509_role = "x509_role_alias";
        const Aws::Crt::String m_cmd_x509_thing_name = "x509_thing_name";
        const Aws::Crt::String m_cmd_x509_cert_file = "x509_cert";
        const Aws::Crt::String m_cmd_x509_key_file = "x509_key";
        const Aws::Crt::String m_cmd_x509_ca_file = "x509_ca_file";
        const Aws::Crt::String m_cmd_pkcs11_lib = "pkcs11_lib";
        const Aws::Crt::String m_cmd_pkcs11_cert = "cert";
        const Aws::Crt::String m_cmd_pkcs11_pin = "pin";
        const Aws::Crt::String m_cmd_pkcs11_token = "token_label";
        const Aws::Crt::String m_cmd_pkcs11_slot = "slot_id";
        const Aws::Crt::String m_cmd_pkcs11_key = "key_label";
        const Aws::Crt::String m_cmd_message = "message";
        const Aws::Crt::String m_cmd_topic = "topic";
        const Aws::Crt::String m_cmd_port_override = "port_override";
        const Aws::Crt::String m_cmd_help = "help";
        const Aws::Crt::String m_cmd_custom_auth_username = "custom_auth_username";
        const Aws::Crt::String m_cmd_custom_auth_authorizer_name = "custom_auth_authorizer_name";
        const Aws::Crt::String m_cmd_custom_auth_authorizer_signature = "custom_auth_authorizer_signature";
        const Aws::Crt::String m_cmd_custom_auth_password = "custom_auth_password";
        const Aws::Crt::String m_cmd_verbosity = "verbosity";
        const Aws::Crt::String m_cmd_log_file = "log_file";
    };
} // namespace Utils
