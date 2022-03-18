/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/crt/Api.h>
#include <aws/crt/UUID.h>
#include <aws/crt/io/HostResolver.h>

#include <aws/iot/MqttClient.h>

#include <aws/iotsecuretunneling/IotSecureTunnelingClient.h>
#include <aws/iotsecuretunneling/SecureTunnelingNotifyResponse.h>
#include <aws/iotsecuretunneling/SubscribeToTunnelsNotifyRequest.h>
#include <iostream>

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

#include "../../utils/CommandLineUtils.h"

using namespace std;
using namespace Aws::Iot;
using namespace Aws::Crt;
using namespace Aws::Crt::Mqtt;
using namespace Aws::Iotsecuretunneling;

int main(int argc, char *argv[])
{
    /************************ Setup the Lib ****************************/
    /*
     * Do the global initialization for the API.
     */
    ApiHandle apiHandle;

    String endpoint;
    String certificatePath;
    String keyPath;
    String caFile;
    String clientId(String("test-") + Aws::Crt::UUID().ToString());
    String thingName;

    /*********************** Parse Arguments ***************************/
    Utils::CommandLineUtils cmdUtils = Utils::CommandLineUtils();
    cmdUtils.RegisterProgramName("tunnel_notification");
    cmdUtils.AddCommonMQTTCommands();
    cmdUtils.RegisterCommand("thing_name", "<str>", "The name of your IOT thing");
    const char **const_argv = (const char **)argv;
    cmdUtils.SendArguments(const_argv, const_argv + argc);

    if (cmdUtils.HasCommand("help"))
    {
        cmdUtils.PrintHelp();
        exit(-1);
    }
    endpoint = cmdUtils.GetCommandRequired("endpoint");
    certificatePath = cmdUtils.GetCommandRequired("cert");
    keyPath = cmdUtils.GetCommandRequired("key");
    thingName = cmdUtils.GetCommandRequired("thing_name");
    caFile = cmdUtils.GetCommandOrDefault("ca_file", caFile);

    /********************** Now Setup an Mqtt Client ******************/
    if (apiHandle.GetOrCreateStaticDefaultClientBootstrap()->LastError() != AWS_ERROR_SUCCESS)
    {
        fprintf(
            stderr,
            "ClientBootstrap failed with error %s\n",
            ErrorDebugString(apiHandle.GetOrCreateStaticDefaultClientBootstrap()->LastError()));
        exit(-1);
    }

    auto clientConfigBuilder = Aws::Iot::MqttClientConnectionConfigBuilder(certificatePath.c_str(), keyPath.c_str());
    clientConfigBuilder.WithEndpoint(endpoint);
    if (!caFile.empty())
    {
        clientConfigBuilder.WithCertificateAuthority(caFile.c_str());
    }
    auto clientConfig = clientConfigBuilder.Build();

    if (!clientConfig)
    {
        fprintf(
            stderr,
            "Client Configuration initialization failed with error %s\n",
            ErrorDebugString(clientConfig.LastError()));
        exit(-1);
    }

    /*
     * Now Create a client. This can not throw.
     * An instance of a client must outlive its connections.
     * It is the users responsibility to make sure of this.
     */
    Aws::Iot::MqttClient mqttClient;

    /*
     * Since no exceptions are used, always check the bool operator
     * when an error could have occurred.
     */
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

    if (!*connection)
    {
        fprintf(stderr, "MQTT Connection Creation failed with error %s\n", ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    /*
     * In a real world application you probably don't want to enforce synchronous behavior
     * but this is a sample console application, so we'll just do that with a condition variable.
     */
    std::promise<bool> connectionCompletedPromise;
    std::promise<void> connectionClosedPromise;

    /*
     * This will execute when an mqtt connect has completed or failed.
     */
    auto onConnectionCompleted = [&](Mqtt::MqttConnection &, int errorCode, Mqtt::ReturnCode returnCode, bool) {
        if (errorCode)
        {
            fprintf(stdout, "Connection failed with error %s\n", ErrorDebugString(errorCode));
            connectionCompletedPromise.set_value(false);
        }
        else
        {
            fprintf(stdout, "Connection completed with return code %d\n", returnCode);
            connectionCompletedPromise.set_value(true);
        }
    };

    /*
     * Invoked when a disconnect message has completed.
     */
    auto onDisconnect = [&](Mqtt::MqttConnection & /*conn*/) {
        {
            fprintf(stdout, "Disconnect completed\n");
            connectionClosedPromise.set_value();
        }
    };

    connection->OnConnectionCompleted = std::move(onConnectionCompleted);
    connection->OnDisconnect = std::move(onDisconnect);

    /*
     * Actually perform the connect dance.
     */
    fprintf(stdout, "Connecting...\n");
    if (!connection->Connect(clientId.c_str(), true, 0))
    {
        fprintf(stderr, "MQTT Connection failed with error %s\n", ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    auto onSubscribeToTunnelsNotifyResponse = [&](Aws::Iotsecuretunneling::SecureTunnelingNotifyResponse *response,
                                                  int ioErr) -> void {
        if (ioErr == 0)
        {
            fprintf(stdout, "Received MQTT Tunnel Notification\n");

            std::string clientAccessToken = response->ClientAccessToken->c_str();
            std::string clientMode = response->ClientMode->c_str();
            std::string region = response->Region->c_str();

            fprintf(
                stdout,
                "Recv: Token:%s, Mode:%s, Region:%s\n",
                clientAccessToken.c_str(),
                clientMode.c_str(),
                region.c_str());

            size_t nServices = response->Services->size();
            if (nServices <= 0)
            {
                fprintf(stdout, "No service requested\n");
            }
            else
            {
                std::string service = response->Services->at(0).c_str();
                fprintf(stdout, "Requested service=%s\n", service.c_str());

                if (nServices > 1)
                {
                    fprintf(stdout, "Multiple services not supported yet\n");
                }
            }
        }
        else
        {
            fprintf(stderr, "MQTT Connection failed with error %d\n", ioErr);
        }
    };

    auto OnSubscribeComplete = [&](int ioErr) -> void {
        if (ioErr)
        {
            fprintf(stderr, "MQTT Connection failed with error %d\n", ioErr);
        }
    };

    if (connectionCompletedPromise.get_future().get())
    {
        SubscribeToTunnelsNotifyRequest request;
        request.ThingName = thingName;

        IotSecureTunnelingClient client(connection);
        client.SubscribeToTunnelsNotify(
            request, AWS_MQTT_QOS_AT_LEAST_ONCE, onSubscribeToTunnelsNotifyResponse, OnSubscribeComplete);
    }
    while (1)
    {
        std::this_thread::sleep_for(500ms);
        continue;
    }
}
