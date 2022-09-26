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
    cmdUtils.RegisterProgramName("basic-connect");
    cmdUtils.AddCommonMQTTCommands();
    cmdUtils.RegisterCommand("key", "<path>", "Path to your key in PEM format.");
    cmdUtils.RegisterCommand("cert", "<path>", "Path to your client certificate in PEM format.");
    cmdUtils.AddCommonProxyCommands();
    cmdUtils.RegisterCommand("client_id", "<str>", "Client id to use (optional, default='test-*')");
    cmdUtils.AddLoggingCommands();
    const char **const_argv = (const char **)argv;
    cmdUtils.SendArguments(const_argv, const_argv + argc);
    cmdUtils.StartLoggingBasedOnCommand(&apiHandle);

    /**
     * Make a MQTT client and create a connection using a certificate and key
     * Note: The data for the connection is gotten from cmdUtils
     * (see BuildDirectMQTTConnection for implementation)
     */
    Aws::Iot::MqttClient client = Aws::Iot::MqttClient();
    std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> connection = cmdUtils.BuildDirectMQTTConnection(&client);

    /* Get the client ID to send with the connection */
    String clientId = cmdUtils.GetCommandOrDefault("client_id", String("test-") + Aws::Crt::UUID().ToString());

    /**
     * Connect and then disconnect using the connection we created
     * (see SampleConnectAndDisconnect for implementation)
     */
    cmdUtils.SampleConnectAndDisconnect(connection, clientId);
    return 0;
}
