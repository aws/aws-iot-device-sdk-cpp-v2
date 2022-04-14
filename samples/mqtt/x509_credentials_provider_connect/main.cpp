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

    /*********************** Parse Arguments ***************************/
    Utils::CommandLineUtils cmdUtils = Utils::CommandLineUtils();
    cmdUtils.RegisterProgramName("x509-credentials-provider-connect");
    cmdUtils.AddCommonMQTTCommands();
    cmdUtils.AddCommonProxyCommands();
    cmdUtils.AddCommonX509Commands();
    cmdUtils.RegisterCommand("signing_region", "<str>", "Used for websocket signer");
    cmdUtils.RegisterCommand("client_id", "<str>", "Client id to use (optional, default='test-*')");
    cmdUtils.RegisterCommand("count", "<int>", "The number of messages to send (optional, default='10')");
    const char **const_argv = (const char **)argv;
    cmdUtils.SendArguments(const_argv, const_argv + argc);

    // Make a MQTT client and create a connection using websockets and x509
    // Note: The data for the connection is gotten from cmdUtils
    // (see BuildWebsocketX509MQTTConnection for implementation)
    Aws::Iot::MqttClient mqttClient;
    std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> connection = cmdUtils.BuildWebsocketX509MQTTConnection(&mqttClient);

    // Get the client ID to send with the connection
    String clientId = cmdUtils.GetCommandOrDefault("client_id", String("test-") + Aws::Crt::UUID().ToString());

    // Connect and then disconnect using the connection we created
    // (see SampleConnectAndDisconnect for implementation)
    cmdUtils.SampleConnectAndDisconnect(connection, clientId);
    return 0;
}
