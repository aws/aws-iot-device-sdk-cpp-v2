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
    /*
     * Do the global initialization for the API.
     */
    ApiHandle apiHandle;

    /*********************** Parse Arguments ***************************/
    Utils::CommandLineUtils cmdUtils = Utils::CommandLineUtils();
    cmdUtils.RegisterProgramName("pkcs11-connect");
    cmdUtils.AddCommonMQTTCommands();
    cmdUtils.RegisterCommand("cert", "<path>", "Path to your client certificate in PEM format.");
    cmdUtils.RegisterCommand("pkcs11_lib", "<path>", "Path to PKCS#11 library.");
    cmdUtils.RegisterCommand("pin", "<str>", "User PIN for logging into PKCS#11 token.");
    cmdUtils.RegisterCommand("token_label", "<str>", "Label of the PKCS#11 token to use (optional).");
    cmdUtils.RegisterCommand("slot_id", "<int>", "Slot ID containing PKCS#11 token to use (optional).");
    cmdUtils.RegisterCommand("key_label", "<str>", "Label of private key on the PKCS#11 token (optional).");
    cmdUtils.RegisterCommand("client_id", "<str>", "Client id to use (optional, default='test-*').");
    cmdUtils.AddLoggingCommands();
    const char **const_argv = (const char **)argv;
    cmdUtils.SendArguments(const_argv, const_argv + argc);
    cmdUtils.StartLoggingBasedOnCommand(&apiHandle);

    // Make a MQTT client and create a connection using a PKCS11
    // Note: The data for the connection is gotten from cmdUtils
    // (see BuildPKCS11MQTTConnection for implementation)
    Aws::Iot::MqttClient mqttClient;
    std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> connection = cmdUtils.BuildPKCS11MQTTConnection(&mqttClient);

    // Get the client ID to send with the connection
    String clientId = cmdUtils.GetCommandOrDefault("client_id", String("test-") + Aws::Crt::UUID().ToString());

    // Connect and then disconnect using the connection we created
    // (see SampleConnectAndDisconnect for implementation)
    cmdUtils.SampleConnectAndDisconnect(connection, clientId);
    return 0;
}
