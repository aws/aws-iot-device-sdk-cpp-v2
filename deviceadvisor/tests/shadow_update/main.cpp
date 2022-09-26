/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/JsonObject.h>
#include <aws/crt/UUID.h>
#include <aws/iot/MqttClient.h>
#include <aws/iotshadow/IotShadowClient.h>
#include <aws/iotshadow/UpdateShadowRequest.h>
#include <aws/iotshadow/UpdateShadowSubscriptionRequest.h>

#include <condition_variable>

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
    if (!daEnv.init(TestType::SHADOW))
    {
        exit(-1);
    }

    /********************** Now Setup an Mqtt Client ******************/
    /*
     * Setup client configuration with the MqttClientConnectionConfigBuilder.
     */
    Aws::Iot::MqttClientConnectionConfigBuilder builder =
        Aws::Iot::MqttClientConnectionConfigBuilder(daEnv.certificatePath.c_str(), daEnv.keyPath.c_str());
    builder.WithEndpoint(daEnv.endpoint);
    auto clientConfig = builder.Build();
    if (!clientConfig)
    {
        exit(-1);
    }

    /*
     * Setup up mqttClients
     */
    Aws::Iot::MqttClient mqttClient;
    if (!mqttClient)
    {
        exit(-1);
    }

    /*
     * Now create a connection object.
     */
    auto connection = mqttClient.NewConnection(clientConfig);
    if (!connection)
    {
        exit(-1);
    }

    /*
     * Invoked when connection and disconnection has completed.
     */
    std::promise<bool> connectionCompletedPromise;
    std::promise<void> connectionClosedPromise;
    std::promise<void> shadowUpdatePromise;

    connection->OnConnectionCompleted = [&](Mqtt::MqttConnection &, int errorCode, Mqtt::ReturnCode returnCode, bool) {
        connectionCompletedPromise.set_value(errorCode == AWS_ERROR_SUCCESS && returnCode == AWS_MQTT_CONNECT_ACCEPTED);
    };
    connection->OnDisconnect = [&](Mqtt::MqttConnection &) { connectionClosedPromise.set_value(); };

    /*
     * Actually perform the connect dance.
     */
    if (!connection->Connect(
            clientId.c_str(), true /*cleanSession*/, 1000 /*keepAliveTimeSecs*/, 60000 /*pingTimeoutMs*/))
    {
        exit(-1);
    }

    if (connectionCompletedPromise.get_future().get())
    {
        Aws::Iotshadow::IotShadowClient shadowClient(connection);

        ShadowState shadowState;
        s_setShadowState(daEnv.shadowProperty, daEnv.shadowValue, shadowState);

        /*
         * The program will ignore the shadow updat error. If the update failed, device adivsor should report error.
         */
        auto publishCompleted = [&shadowUpdatePromise](int /*ioErr*/) { shadowUpdatePromise.set_value(); };

        UpdateShadowRequest updateShadowRequest;
        Aws::Crt::UUID uuid;
        updateShadowRequest.ClientToken = uuid.ToString();
        updateShadowRequest.ThingName = daEnv.thing_name;
        updateShadowRequest.State = shadowState;
        /*
         * Update the shadow property. We use AWS_MQTT_QOS_AT_MOST_ONCE since the device advisor will not send
         * back PUBACK. In case we busy waiting on the PUBACK, we used AWS_MQTT_QOS_AT_MOST_ONCE for now.
         */
        shadowClient.PublishUpdateShadow(updateShadowRequest, AWS_MQTT_QOS_AT_MOST_ONCE, publishCompleted);
        shadowUpdatePromise.get_future().wait();

        /* Disconnect */
        if (connection->Disconnect())
        {
            connectionClosedPromise.get_future().wait();
        }
    }
    else
    {
        exit(-1);
    }

    return 0;
}
