/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/UUID.h>

#include "../../utils/CommandLineUtils.h"

using namespace Aws::Crt;

int main(int argc, char *argv[])
{

    /************************ Setup the Lib ****************************/
    /**
     * Do the global initialization for the API.
     */
    ApiHandle apiHandle;

    /*********************** Parse Arguments ***************************/
    Utils::CommandLineUtils cmdUtils = Utils::CommandLineUtils();
    cmdUtils.RegisterProgramName("custom_authorizer_connect");
    cmdUtils.AddCommonMQTTCommands();
    cmdUtils.AddCommonCustomAuthorizerCommands();
    cmdUtils.RegisterCommand("client_id", "<str>", "Client id to use (optional, default='test-*')");
    cmdUtils.RemoveCommand("ca_file");
    cmdUtils.AddLoggingCommands();
    const char **const_argv = (const char **)argv;
    cmdUtils.SendArguments(const_argv, const_argv + argc);
    cmdUtils.StartLoggingBasedOnCommand(&apiHandle);

    /**
     * Make a MQTT client and create a connection through a custom authorizer
     * Note: The data for the connection is gotten from cmdUtils
     * (see BuildDirectMQTTConnectionWithCustomAuthorizer for implementation)
     */
    Aws::Iot::MqttClient client = Aws::Iot::MqttClient();
    std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> connection =
        cmdUtils.BuildDirectMQTTConnectionWithCustomAuthorizer(&client);

    /* Get the client ID to send with the connection */
    String clientId = cmdUtils.GetCommandOrDefault("client_id", String("test-") + Aws::Crt::UUID().ToString());

    /**
     * Connect and then disconnect using the connection we created
     * (see SampleConnectAndDisconnect for implementation)
     */
    cmdUtils.SampleConnectAndDisconnect(connection, clientId);
    return 0;
}
