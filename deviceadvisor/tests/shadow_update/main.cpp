/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/JsonObject.h>
#include <aws/crt/UUID.h>
#include <aws/crt/io/HostResolver.h>

#include <aws/iot/MqttClient.h>

#include <aws/iotshadow/ErrorResponse.h>
#include <aws/iotshadow/IotShadowClient.h>
#include <aws/iotshadow/ShadowDeltaUpdatedEvent.h>
#include <aws/iotshadow/ShadowDeltaUpdatedSubscriptionRequest.h>
#include <aws/iotshadow/UpdateShadowRequest.h>
#include <aws/iotshadow/UpdateShadowResponse.h>
#include <aws/iotshadow/UpdateShadowSubscriptionRequest.h>

#include <algorithm>
#include <condition_variable>
#include <iostream>
#include <mutex>

#include "../utils/datest_utils.h"

using namespace Aws::Crt;
using namespace Aws::Iotshadow;
using namespace DATest_Utils;

static void s_setShadowState(const String &shadowProperty, const String &value, ShadowState &state)
{
    fprintf(stdout, "Changing local shadow value to %s.\n", value.c_str());

    JsonObject desired;
    JsonObject reported;

    if (value == "null")
    {
        JsonObject nullObject;
        nullObject.AsNull();
        desired.WithObject(shadowProperty, nullObject);
        reported.WithObject(shadowProperty, nullObject);
    }
    else if (value == "clear_shadow")
    {
        desired.AsNull();
        reported.AsNull();
    }
    else
    {
        desired.WithString(shadowProperty, value);
        reported.WithString(shadowProperty, value);
    }
    state.Desired = desired;
    state.Reported = reported;
}

int main()
{
    /************************ Setup the Lib ****************************/
    /*
     * Do the global initialization for the API.
     */
    ApiHandle apiHandle;

    String endpoint;
    String certificatePath;
    String keyPath;
    String thingName;
    String clientId(String("test-") + Aws::Crt::UUID().ToString());
    String shadowPropertyName;
    String shadowValueSet;

    /*********************** Parse Arguments ***************************/
    s_getenv(ENV_ENDPONT, endpoint);
    s_getenv(ENV_CERTI, certificatePath);
    s_getenv(ENV_KEY, keyPath);
    s_getenv(ENV_THING_NAME, thingName);
    s_getenv(ENV_SHADOW_PROPERTY, shadowPropertyName);
    s_getenv(ENV_SHADOW_VALUE_SET, shadowValueSet);

    /********************** Now Setup an Mqtt Client ******************/
    /*
     * You need an event loop group to process IO events.
     * If you only have a few connections, 1 thread is ideal
     */
    Io::EventLoopGroup eventLoopGroup(1);
    if (!eventLoopGroup)
    {
        fprintf(
            stderr, "Event Loop Group Creation failed with error %s\n", ErrorDebugString(eventLoopGroup.LastError()));
        exit(-1);
    }

    Io::DefaultHostResolver hostResolver(eventLoopGroup, 2, 30);
    Io::ClientBootstrap bootstrap(eventLoopGroup, hostResolver);

    if (!bootstrap)
    {
        fprintf(stderr, "ClientBootstrap failed with error %s\n", ErrorDebugString(bootstrap.LastError()));
        exit(-1);
    }

    auto clientConfigBuilder = Aws::Iot::MqttClientConnectionConfigBuilder(certificatePath.c_str(), keyPath.c_str());
    clientConfigBuilder.WithEndpoint(endpoint);
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
    Aws::Iot::MqttClient mqttClient(bootstrap);

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
    std::promise<void> shadowUpdatePromise;

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

    if (connectionCompletedPromise.get_future().get())
    {
        Aws::Iotshadow::IotShadowClient shadowClient(connection);
        ShadowState shadowState;
        s_setShadowState(shadowPropertyName, shadowValueSet, shadowState);
        auto publishCompleted = [thingName, shadowValueSet, &shadowUpdatePromise](int ioErr) {
            shadowUpdatePromise.set_value();
            if (ioErr != AWS_OP_SUCCESS)
            {
                fprintf(
                    stderr, "failed to update %s shadow state: error %s\n", thingName.c_str(), ErrorDebugString(ioErr));
                return;
            }

            fprintf(
                stdout, "Successfully updated shadow state for %s, to %s\n", thingName.c_str(), shadowValueSet.c_str());
        };

        UpdateShadowRequest updateShadowRequest;
        Aws::Crt::UUID uuid;
        updateShadowRequest.ClientToken = uuid.ToString();
        updateShadowRequest.ThingName = thingName;
        updateShadowRequest.State = shadowState;
        // Update the property
        fprintf(stdout, "Update Desired state of %s to %s\n", shadowPropertyName.c_str(), shadowValueSet.c_str());
        shadowClient.PublishUpdateShadow(updateShadowRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, std::move(publishCompleted));

        shadowUpdatePromise.get_future().wait();
    }

    /* Disconnect */
    if (connection->Disconnect())
    {
        connectionClosedPromise.get_future().wait();
    }

    return 0;
}