/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/StlAllocator.h>
#include <aws/crt/auth/Credentials.h>

#include <aws/iot/MqttClient.h>

#include <algorithm>
#include <aws/crt/UUID.h>
#include <chrono>
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
    // uint16_t proxyPort(8080);

    /*********************** Parse Arguments ***************************/
    Utils::CommandLineUtils cmdUtils = Utils::CommandLineUtils();
    cmdUtils.RegisterProgramName("websocket-connect");
    cmdUtils.AddCommonMQTTCommands();
    cmdUtils.AddCommonProxyCommands();
    cmdUtils.RegisterCommand("signing_region", "<str>", "The signing region used for the websocket signer");
    cmdUtils.RegisterCommand("client_id", "<str>", "Client id to use (optional, default='test-*')");
    cmdUtils.AddLoggingCommands();
    const char **const_argv = (const char **)argv;
    cmdUtils.SendArguments(const_argv, const_argv + argc);
    cmdUtils.StartLoggingBasedOnCommand(&apiHandle);

    // Make a MQTT client and create a connection using websockets
    // Note: The data for the connection is gotten from cmdUtils
    // (see BuildWebsocketMQTTConnection for implementation)
    Aws::Iot::MqttClient mqttClient;
    std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> connection = cmdUtils.BuildWebsocketMQTTConnection(&mqttClient);

    // Get the client ID to send with the connection
    String clientId = cmdUtils.GetCommandOrDefault("client_id", String("test-") + Aws::Crt::UUID().ToString());

    // Connect and then disconnect using the connection we created
    // (see SampleConnectAndDisconnect for implementation)
    cmdUtils.SampleConnectAndDisconnect(connection, clientId);
    return 0;
}
