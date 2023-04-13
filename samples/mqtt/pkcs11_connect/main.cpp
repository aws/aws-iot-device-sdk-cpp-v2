/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/UUID.h>
#include <aws/crt/io/Pkcs11.h>

#include "../../utils/CommandLineUtils.h"

using namespace Aws::Crt;

int main(int argc, char *argv[])
{

    /************************ Setup ****************************/

    // Do the global initialization for the API.
    ApiHandle apiHandle;

    /**
     * cmdData is the arguments/input from the command line placed into a single struct for
     * use in this sample. This handles all of the command line parsing, validating, etc.
     * See the Utils/CommandLineUtils for more information.
     */
    Utils::cmdData cmdData = Utils::parseSampleInputPKCS11Connect(argc, argv, &apiHandle);

    // Create the MQTT builder and populate it with data from cmdData.
    Aws::Iot::MqttClient client;
    std::shared_ptr<Aws::Crt::Io::Pkcs11Lib> pkcs11Lib = Aws::Crt::Io::Pkcs11Lib::Create(cmdData.input_pkcs11LibPath);
    if (!pkcs11Lib)
    {
        fprintf(stderr, "Pkcs11Lib failed: %s\n", Aws::Crt::ErrorDebugString(Aws::Crt::LastError()));
        exit(-1);
    }
    Aws::Crt::Io::TlsContextPkcs11Options pkcs11Options(pkcs11Lib);
    pkcs11Options.SetCertificateFilePath(cmdData.input_cert);
    pkcs11Options.SetUserPin(cmdData.input_pkcs11UserPin);
    if (cmdData.input_pkcs11TokenLabel != "")
    {
        pkcs11Options.SetTokenLabel(cmdData.input_pkcs11TokenLabel);
    }
    if (cmdData.input_pkcs11SlotId != 0)
    {
        pkcs11Options.SetSlotId(cmdData.input_pkcs11SlotId);
    }
    if (cmdData.input_pkcs11KeyLabel != "")
    {
        pkcs11Options.SetPrivateKeyObjectLabel(cmdData.input_pkcs11KeyLabel);
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
    if (cmdData.input_ca != "")
    {
        clientConfigBuilder.WithCertificateAuthority(cmdData.input_ca.c_str());
    }
    clientConfigBuilder.WithEndpoint(cmdData.input_endpoint);

    // Create the MQTT connection from the MQTT builder
    auto clientConfig = clientConfigBuilder.Build();
    if (!clientConfig)
    {
        fprintf(
            stderr,
            "Client Configuration initialization failed with error %s\n",
            Aws::Crt::ErrorDebugString(clientConfig.LastError()));
        exit(-1);
    }
    auto connection = client.NewConnection(clientConfig);
    if (!*connection)
    {
        fprintf(
            stderr,
            "MQTT Connection Creation failed with error %s\n",
            Aws::Crt::ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    /**
     * In a real world application you probably don't want to enforce synchronous behavior
     * but this is a sample console application, so we'll just do that with a condition variable.
     */
    std::promise<bool> connectionCompletedPromise;
    std::promise<void> connectionClosedPromise;

    // Invoked when a MQTT connect has completed or failed
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

    // Invoked when a MQTT connection was interrupted/lost
    auto onInterrupted = [&](Aws::Crt::Mqtt::MqttConnection &, int error) {
        fprintf(stdout, "Connection interrupted with error %s\n", Aws::Crt::ErrorDebugString(error));
    };

    // Invoked when a MQTT connection was interrupted/lost, but then reconnected successfully
    auto onResumed = [&](Aws::Crt::Mqtt::MqttConnection &, Aws::Crt::Mqtt::ReturnCode, bool) {
        fprintf(stdout, "Connection resumed\n");
    };

    // Invoked when a disconnect message has completed.
    auto onDisconnect = [&](Aws::Crt::Mqtt::MqttConnection &) {
        fprintf(stdout, "Disconnect completed\n");
        connectionClosedPromise.set_value();
    };

    // Assign callbacks
    connection->OnConnectionCompleted = std::move(onConnectionCompleted);
    connection->OnDisconnect = std::move(onDisconnect);
    connection->OnConnectionInterrupted = std::move(onInterrupted);
    connection->OnConnectionResumed = std::move(onResumed);

    /************************ Run the sample ****************************/

    // Connect
    fprintf(stdout, "Connecting...\n");
    if (!connection->Connect(cmdData.input_clientId.c_str(), false /*cleanSession*/, 1000 /*keepAliveTimeSecs*/))
    {
        fprintf(stderr, "MQTT Connection failed with error %s\n", Aws::Crt::ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    // wait for the OnConnectionCompleted callback to fire, which sets connectionCompletedPromise...
    if (connectionCompletedPromise.get_future().get() == false)
    {
        fprintf(stderr, "Connection failed\n");
        exit(-1);
    }

    // Disconnect
    if (connection->Disconnect())
    {
        connectionClosedPromise.get_future().wait();
    }

    return 0;
}
