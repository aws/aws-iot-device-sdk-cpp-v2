/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/UUID.h>
#include <aws/iot/MqttClient.h>

#include "../../utils/CommandLineUtils.h"

using namespace Aws::Crt;

int main(int argc, char *argv[])
{

    /************************ Setup the Lib ****************************/
    /*
     * Do the global initialization for the API.
     */
    ApiHandle apiHandle;

    apiHandle.InitializeLogging(LogLevel::Error, stderr);

    /*********************** Parse Arguments ***************************/
    Utils::CommandLineUtils cmdUtils = Utils::CommandLineUtils();
    cmdUtils.RegisterProgramName("windows-cert-connect");
    cmdUtils.AddCommonMQTTCommands();
    cmdUtils.RegisterCommand(
        "cert",
        "<str>",
        "Your client certificate in the Windows certificate store. e.g. "
        "'CurrentUser\\MY\\6ac133ac58f0a88b83e9c794eba156a98da39b4c'");
    cmdUtils.RegisterCommand("client_id", "<str>", "Client id to use (optional, default='test-*').");
    cmdUtils.AddLoggingCommands();
    const char **const_argv = (const char **)argv;
    cmdUtils.SendArguments(const_argv, const_argv + argc);
    cmdUtils.StartLoggingBasedOnCommand(&apiHandle);

    String endpoint = cmdUtils.GetCommandRequired("endpoint");
    String windowsCertStorePath = cmdUtils.GetCommandRequired("cert");
    String caFile = cmdUtils.GetCommandOrDefault("ca_file", "");
    String clientId = cmdUtils.GetCommandOrDefault("client_id", String("test-") + Aws::Crt::UUID().ToString());

    /********************** Now Setup an Mqtt Client ******************/
    if (apiHandle.GetOrCreateStaticDefaultClientBootstrap()->LastError() != AWS_ERROR_SUCCESS)
    {
        fprintf(
            stderr,
            "ClientBootstrap failed with error %s\n",
            ErrorDebugString(apiHandle.GetOrCreateStaticDefaultClientBootstrap()->LastError()));
        exit(-1);
    }

    Aws::Iot::MqttClientConnectionConfigBuilder builder(windowsCertStorePath.c_str());
    if (!builder)
    {
        fprintf(stderr, "MqttClientConnectionConfigBuilder failed: %s\n", ErrorDebugString(Aws::Crt::LastError()));
        exit(-1);
    }

    /*
     * Note that that remainder of this code is identical to how the other connect samples setup their connection.
     */

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

    Aws::Iot::MqttClient mqttClient;
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

    // Connect and then disconnect using the connection we created
    // (see SampleConnectAndDisconnect for implementation)
    cmdUtils.SampleConnectAndDisconnect(connection, clientId);

    return 0;
}
