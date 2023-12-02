/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/JsonObject.h>
#include <aws/crt/UUID.h>
#include <aws/crt/io/HostResolver.h>

#include <aws/crt/mqtt/Mqtt5Packets.h>
#include <aws/iot/Mqtt5Client.h>
#include <aws/iot/MqttClient.h>

#include <aws/iotshadow/ErrorResponse.h>
#include <aws/iotshadow/IotShadowClient.h>
#include <aws/iotshadow/ShadowDeltaUpdatedEvent.h>
#include <aws/iotshadow/ShadowDeltaUpdatedSubscriptionRequest.h>
#include <aws/iotshadow/UpdateNamedShadowRequest.h>
#include <aws/iotshadow/UpdateShadowRequest.h>
#include <aws/iotshadow/UpdateShadowResponse.h>
#include <aws/iotshadow/UpdateShadowSubscriptionRequest.h>

#include <aws/iotshadow/GetShadowRequest.h>
#include <aws/iotshadow/GetShadowResponse.h>
#include <aws/iotshadow/GetShadowSubscriptionRequest.h>
#include <aws/iotshadow/UpdateShadowSubscriptionRequest.h>

#include <algorithm>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

#include "../../../samples/utils/CommandLineUtils.h"

using namespace Aws::Crt;
using namespace Aws::Iotshadow;

void changeShadowValue(Aws::Crt::String thingName, String property, String value);
void changeNamedShadowValue(String thingName, String property, String value, String shadowName);

IotShadowClient *shadowClient = nullptr;
int main(int argc, char *argv[])
{
    printf("Starting shadow update test\n");
    /************************ Setup ****************************/
    ApiHandle apiHandle;

    /**
     * cmdData is the arguments/input from the command line placed into a single struct for
     * use in this sample. This handles all of the command line parsing, validating, etc.
     * See the Utils/CommandLineUtils for more information.
     */
    Utils::cmdData cmdData = Utils::parseSampleInputShadow(argc, argv, &apiHandle);

    /**
     * In a real world application you probably don't want to enforce synchronous behavior
     * but this is a sample console application, so we'll just do that with a condition variable.
     */
    std::promise<bool> connectionCompletedPromise;
    std::promise<void> connectionClosedPromise;
    std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> connection;

    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(
        cmdData.input_endpoint, cmdData.input_cert.c_str(), cmdData.input_key.c_str());

    // Check if the builder setup correctly.
    if (builder == nullptr)
    {
        printf(
            "Failed to setup mqtt5 client builder with error code %d: %s", LastError(), ErrorDebugString(LastError()));
        return -1;
    }
    // Create Mqtt5Client
    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> client5;
    if (cmdData.input_mqtt_version == 5UL)
    {
        // Create the MQTT5 builder and populate it with data from cmdData.
        // Setup connection options
        std::shared_ptr<Mqtt5::ConnectPacket> connectOptions = std::make_shared<Mqtt5::ConnectPacket>();
        connectOptions->WithClientId(cmdData.input_clientId);
        builder->WithConnectOptions(connectOptions);
        if (cmdData.input_port != 0)
        {
            builder->WithPort(static_cast<uint16_t>(cmdData.input_port));
        }
        // Setup lifecycle callbacks
        builder->WithClientConnectionSuccessCallback(
            [&connectionCompletedPromise](const Mqtt5::OnConnectionSuccessEventData &eventData) {
                fprintf(
                    stdout,
                    "Mqtt5 Client connection succeed, clientid: %s.\n",
                    eventData.negotiatedSettings->getClientId().c_str());
                connectionCompletedPromise.set_value(true);
            });
        builder->WithClientConnectionFailureCallback([&connectionCompletedPromise](
                                                         const Mqtt5::OnConnectionFailureEventData &eventData) {
            fprintf(
                stdout, "Mqtt5 Client connection failed with error: %s.\n", aws_error_debug_str(eventData.errorCode));
            connectionCompletedPromise.set_value(false);
        });
        builder->WithClientStoppedCallback([&connectionClosedPromise](const Mqtt5::OnStoppedEventData &) {
            fprintf(stdout, "Mqtt5 Client stopped.\n");
            connectionClosedPromise.set_value();
        });

        client5 = builder->Build();
        if (client5 == nullptr)
        {
            fprintf(
                stdout,
                "Failed to Init Mqtt5Client with error code %d: %s.\n",
                LastError(),
                ErrorDebugString(LastError()));
            return -1;
        }
        fprintf(stdout, "Connecting...\n");
        if (!client5->Start())
        {
            fprintf(stderr, "MQTT5 Connection failed to start");
            exit(-1);
        }
        shadowClient = new IotShadowClient(client5);
    }
    else if (cmdData.input_mqtt_version == 3UL)
    {
        Aws::Iot::MqttClientConnectionConfigBuilder clientConfigBuilder;
        // Create the MQTT builder and populate it with data from cmdData.
        clientConfigBuilder =
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

        Aws::Iot::MqttClient client3 = Aws::Iot::MqttClient();
        connection = client3.NewConnection(clientConfig);
        if (!*connection)
        {
            fprintf(
                stderr,
                "MQTT Connection Creation failed with error %s\n",
                Aws::Crt::ErrorDebugString(connection->LastError()));
            exit(-1);
        }

        // Invoked when a MQTT connect has completed or failed
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

        // Invoked when a disconnect has been completed
        auto onDisconnect = [&](Mqtt::MqttConnection & /*conn*/) {
            {
                fprintf(stdout, "Disconnect completed\n");
                connectionClosedPromise.set_value();
            }
        };

        connection->OnConnectionCompleted = std::move(onConnectionCompleted);
        connection->OnDisconnect = std::move(onDisconnect);

        fprintf(stdout, "Connecting...\n");
        if (!connection->Connect(cmdData.input_clientId.c_str(), true, 0))
        {
            fprintf(stderr, "MQTT Connection failed with error %s\n", ErrorDebugString(connection->LastError()));
            exit(-1);
        }
        shadowClient = new IotShadowClient(connection);
    }
    else
    {
        fprintf(stderr, "MQTT Version not supported\n");
        exit(-1);
    }

    delete builder;

    /************************ Run the sample ****************************/
    if (connectionCompletedPromise.get_future().get())
    {
        if (cmdData.input_shadowName.empty())
        {
            changeShadowValue(cmdData.input_thingName, cmdData.input_shadowProperty, cmdData.input_shadowValue);
        }
        else
        {
            changeNamedShadowValue(
                cmdData.input_thingName,
                cmdData.input_shadowProperty,
                cmdData.input_shadowValue,
                cmdData.input_shadowName);
        }
    }
    /************************ sample ends ****************************/
    /* Closing down */
    // Wait just a little bit to let the console print
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    if (cmdData.input_mqtt_version == 5UL)
    {
        // Disconnect
        if (client5->Stop() == true)
        {
            fprintf(stderr, "waiting on future\n");
            connectionClosedPromise.get_future().wait();
            fprintf(stderr, "future passed\n");
        }
    }
    else
    { // mqtt3

        // Disconnect
        if (connection->Disconnect() == true)
        {
            connectionClosedPromise.get_future().wait();
        }
    }
    delete shadowClient;
    fprintf(stderr, "returning\n");
    return 0;
}

void changeShadowValue(Aws::Crt::String thingName, String property, String value)
{
    printf("change shadow value ....\n");
    JsonObject desired;
    JsonObject reported;
    ShadowState state;
    desired.WithObject(property, value);
    reported.WithObject(property, value);

    state.Desired = desired;
    state.Reported = reported;

    UpdateShadowRequest request;
    request.ThingName = thingName;
    request.State = state;

    std::promise<void> shadowCompletedPromise;
    auto publishCompleted = [thingName, value, &shadowCompletedPromise](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Failed to update %s shadow state: error %s\n", thingName.c_str(), ErrorDebugString(ioErr));
            exit(-1);
        }
        fprintf(stdout, "Successfully updated shadow state for %s, to %s\n", thingName.c_str(), value.c_str());
        shadowCompletedPromise.set_value();
    };
    shadowClient->PublishUpdateShadow(request, AWS_MQTT_QOS_AT_LEAST_ONCE, publishCompleted);
    shadowCompletedPromise.get_future().get();
}

void changeNamedShadowValue(String thingName, String property, String value, String shadowName)
{
    printf("change named shadow value ....\n");
    JsonObject desired;
    JsonObject reported;
    ShadowState state;
    desired.WithObject(property, value);
    reported.WithObject(property, value);

    state.Desired = desired;
    state.Reported = reported;

    UpdateNamedShadowRequest request;
    request.ThingName = thingName;
    request.State = state;
    request.ShadowName = shadowName;

    std::promise<void> shadowCompletedPromise;
    auto publishCompleted = [thingName, value, &shadowCompletedPromise](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Failed to update %s shadow state: error %s\n", thingName.c_str(), ErrorDebugString(ioErr));
            exit(-1);
        }

        fprintf(stdout, "Successfully updated shadow state for %s, to %s\n", thingName.c_str(), value.c_str());
        shadowCompletedPromise.set_value();
    };
    shadowClient->PublishUpdateNamedShadow(request, AWS_MQTT_QOS_AT_LEAST_ONCE, publishCompleted);
    shadowCompletedPromise.get_future().get();
}
