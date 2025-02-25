/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/iot/MqttClient.h>
#include <aws/iotsecuretunneling/IotSecureTunnelingClient.h>
#include <aws/iotsecuretunneling/SecureTunnel.h>
#include <aws/iotsecuretunneling/SecureTunnelingNotifyResponse.h>
#include <aws/iotsecuretunneling/SubscribeToTunnelsNotifyRequest.h>

#include <chrono>
#include <thread>

#include "../../utils/CommandLineUtils.h"

using namespace std;
using namespace Aws::Crt;
using namespace Aws::Crt::Mqtt;
using namespace Aws::Iotsecuretunneling;

int main(int argc, char *argv[])
{
    /************************ Setup ****************************/

    struct aws_allocator *allocator = aws_default_allocator();
    // Do the global initialization for the API.
    ApiHandle apiHandle;
    aws_iotdevice_library_init(allocator);
    std::shared_ptr<SecureTunnel> secureTunnel;

    /**
     * cmdData is the arguments/input from the command line placed into a single struct for
     * use in this sample. This handles all of the command line parsing, validating, etc.
     * See the Utils/CommandLineUtils for more information.
     */
    Utils::cmdData cmdData = Utils::parseSampleInputSecureTunnelNotification(argc, argv, &apiHandle);

    // Create the MQTT builder and populate it with data from cmdData.
    auto clientConfigBuilder =
        Aws::Iot::MqttClientConnectionConfigBuilder(cmdData.input_cert.c_str(), cmdData.input_key.c_str());
    clientConfigBuilder.WithEndpoint(cmdData.input_endpoint);
    if (cmdData.input_ca != "")
    {
        clientConfigBuilder.WithCertificateAuthority(cmdData.input_ca.c_str());
    }

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
    Aws::Iot::MqttClient client = Aws::Iot::MqttClient();
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

    // Invoked when a disconnect message has completed.
    auto onDisconnect = [&](Aws::Crt::Mqtt::MqttConnection &) {
        fprintf(stdout, "Disconnect completed\n");
        connectionClosedPromise.set_value();
    };

    // Assign callbacks
    connection->OnConnectionCompleted = std::move(onConnectionCompleted);
    connection->OnDisconnect = std::move(onDisconnect);

    /************************ Run the sample ****************************/

    // Connect
    fprintf(stdout, "Connecting...\n");
    if (!connection->Connect(cmdData.input_clientId.c_str(), true, 0))
    {
        fprintf(stderr, "MQTT Connection failed with error %s\n", ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    auto onSubscribeToTunnelsNotifyResponse = [&](Aws::Iotsecuretunneling::SecureTunnelingNotifyResponse *response,
                                                  int ioErr) -> void {
        if (ioErr == 0)
        {
            fprintf(stdout, "Received MQTT Tunnel Notification\n");

            fprintf(
                stdout,
                "Recv:\n\tToken:%s\n\tMode:%s\n\tRegion:%s\n",
                response->ClientAccessToken->c_str(),
                response->ClientMode->c_str(),
                response->Region->c_str());

            Aws::Crt::String region = response->Region->c_str();
            Aws::Crt::String endpoint = "data.tunneling.iot." + region + ".amazonaws.com";

            size_t nServices = response->Services->size();
            if (nServices <= 0)
            {
                fprintf(stdout, "\tNo Service Ids requested\n");
            }
            else
            {
                for (size_t i = 0; i < nServices; ++i)
                {
                    std::string service = response->Services->at(i).c_str();
                    fprintf(stdout, "\tService Id %zu=%s\n", (i + 1), service.c_str());
                }
            }

            SecureTunnelBuilder builder = SecureTunnelBuilder(
                allocator,
                response->ClientAccessToken->c_str(),
                AWS_SECURE_TUNNELING_DESTINATION_MODE,
                endpoint.c_str());

            builder.WithOnConnectionSuccess(
                [&](SecureTunnel *secureTunnel, const ConnectionSuccessEventData &eventData) {
                    (void)secureTunnel;
                    (void)eventData;
                    fprintf(stdout, "Secure Tunnel connected to Secure Tunnel Service\n");
                });
            secureTunnel = builder.Build();

            if (!secureTunnel)
            {
                fprintf(stderr, "Secure Tunnel Creation failed: %s\n", ErrorDebugString(LastError()));
                exit(-1);
            }
            secureTunnel->Start();
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
        request.ThingName = cmdData.input_thingName;

        IotSecureTunnelingClient secureClient(connection);
        secureClient.SubscribeToTunnelsNotify(
            request, AWS_MQTT_QOS_AT_LEAST_ONCE, onSubscribeToTunnelsNotifyResponse, OnSubscribeComplete);
    }

    while (1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        continue;
    }
}
