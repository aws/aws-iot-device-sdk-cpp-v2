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

void connection_setup(
    int argc,
    char *argv[],
    ApiHandle &apiHandle,
    Utils::cmdData &cmdData,
    Aws::Iot::MqttClientConnectionConfigBuilder &clientConfigBuilder)
{
    cmdData = Utils::parseSampleInputWebsocketConnect(argc, argv, &apiHandle);

    std::shared_ptr<Aws::Crt::Auth::ICredentialsProvider> provider = nullptr;
    Aws::Crt::Auth::CredentialsProviderChainDefaultConfig defaultConfig;
    provider = Aws::Crt::Auth::CredentialsProvider::CreateCredentialsProviderChainDefault(defaultConfig);
    if (!provider)
    {
        fprintf(stderr, "Failure to create credentials provider!\n");
        exit(-1);
    }
    Aws::Iot::WebsocketConfig config(cmdData.input_signingRegion, provider);
    clientConfigBuilder = Aws::Iot::MqttClientConnectionConfigBuilder(config);
    if (cmdData.input_ca != "")
    {
        clientConfigBuilder.WithCertificateAuthority(cmdData.input_ca.c_str());
    }
    if (cmdData.input_proxyHost != "")
    {
        Aws::Crt::Http::HttpClientConnectionProxyOptions proxyOptions;
        proxyOptions.HostName = cmdData.input_proxyHost;
        proxyOptions.Port = static_cast<uint32_t>(cmdData.input_proxyPort);
        proxyOptions.AuthType = Aws::Crt::Http::AwsHttpProxyAuthenticationType::None;
        clientConfigBuilder.WithHttpProxyOptions(proxyOptions);
    }
    if (cmdData.input_port != 0)
    {
        clientConfigBuilder.WithPortOverride(static_cast<uint32_t>(cmdData.input_port));
    }
    clientConfigBuilder.WithEndpoint(cmdData.input_endpoint);
}

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
    Utils::cmdData cmdData;
    Aws::Iot::MqttClient client;
    // Create the MQTT builder and populate it with data from cmdData.
    Aws::Iot::MqttClientConnectionConfigBuilder clientConfigBuilder;

    connection_setup(argc, argv, apiHandle, cmdData, clientConfigBuilder);

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
