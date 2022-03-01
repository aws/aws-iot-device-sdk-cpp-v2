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
    JsonObject desired;
    JsonObject reported;

    desired.WithString(shadowProperty, value);
    reported.WithString(shadowProperty, value);
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
    String clientId(String("test-") + Aws::Crt::UUID().ToString());

    /*********************** Parse Arguments ***************************/
    DeviceAdvisorEnvironment daEnv;
    daEnv.init(TestType::SHADOW);

    /********************** Now Setup an Mqtt Client ******************/
    /*
     * You need an event loop group to process IO events.
     * If you only have a few connections, 1 thread is ideal
     */
    Io::EventLoopGroup eventLoopGroup(1);
    if (!eventLoopGroup)
    {
        exit(-1);
    }

    Io::DefaultHostResolver hostResolver(eventLoopGroup, 2, 30);
    Io::ClientBootstrap bootstrap(eventLoopGroup, hostResolver);

    if (!bootstrap)
    {
        exit(-1);
    }

    auto clientConfigBuilder =
        Aws::Iot::MqttClientConnectionConfigBuilder(daEnv.certificatePath.c_str(), daEnv.keyPath.c_str());
    clientConfigBuilder.WithEndpoint(daEnv.endpoint);
    auto clientConfig = clientConfigBuilder.Build();

    if (!clientConfig)
    {
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
        exit(-1);
    }
    /*
     * Now create a connection object. Note: This type is move only
     * and its underlying memory is managed by the client.
     */
    auto connection = mqttClient.NewConnection(clientConfig);

    if (!*connection)
    {
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
    auto onConnectionCompleted = [&](Mqtt::MqttConnection &, int errorCode, Mqtt::ReturnCode /*returnCode*/, bool) {
        if (errorCode)
        {
            connectionCompletedPromise.set_value(false);
        }
        else
        {
            connectionCompletedPromise.set_value(true);
        }
    };

    /*
     * Invoked when a disconnect message has completed.
     */
    auto onDisconnect = [&](Mqtt::MqttConnection & /*conn*/) {
        {
            connectionClosedPromise.set_value();
        }
    };

    connection->OnConnectionCompleted = std::move(onConnectionCompleted);
    connection->OnDisconnect = std::move(onDisconnect);

    /*
     * Actually perform the connect dance.
     */
    if (!connection->Connect(clientId.c_str(), true, 0))
    {
        exit(-1);
    }

    if (connectionCompletedPromise.get_future().get())
    {
        Aws::Iotshadow::IotShadowClient shadowClient(connection);
        ShadowState shadowState;
        s_setShadowState(daEnv.shadowProperty, daEnv.shadowValue, shadowState);
        auto publishCompleted = [&shadowUpdatePromise](int /*ioErr*/) { shadowUpdatePromise.set_value(); };

        UpdateShadowRequest updateShadowRequest;
        Aws::Crt::UUID uuid;
        updateShadowRequest.ClientToken = uuid.ToString();
        updateShadowRequest.ThingName = daEnv.thing_name;
        updateShadowRequest.State = shadowState;
        // Update the property
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
