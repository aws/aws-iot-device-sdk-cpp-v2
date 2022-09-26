/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/UUID.h>
#include <aws/iot/MqttClient.h>

#include <condition_variable>

#include "../utils/datest_utils.h"

using namespace Aws::Crt;
using namespace DATest_Utils;

int main()
{

    /************************ Setup the Lib ****************************/
    /*
     * Do the global initialization for the API.
     */
    ApiHandle apiHandle;
    String clientId(String("test-") + Aws::Crt::UUID().ToString());
    String messagePayload("Hello world!");

    /*********************** Parse Arguments ***************************/
    DeviceAdvisorEnvironment daVars;
    if (!daVars.init(TestType::SUB_PUB))
    {
        exit(1);
    }

    /********************** Now Setup an Mqtt Client ******************/
    /*
     * Setup client configuration with the MqttClientConnectionConfigBuilder.
     */
    Aws::Iot::MqttClientConnectionConfigBuilder builder =
        Aws::Iot::MqttClientConnectionConfigBuilder(daVars.certificatePath.c_str(), daVars.keyPath.c_str());
    builder.WithEndpoint(daVars.endpoint);
    auto clientConfig = builder.Build();
    if (!clientConfig)
    {
        exit(2);
    }

    /*
     * Setup up mqttClients
     */
    Aws::Iot::MqttClient mqttClient;
    if (!mqttClient)
    {
        exit(3);
    }

    /*
     * Now create a connection object.
     */
    auto connection = mqttClient.NewConnection(clientConfig);
    if (!connection)
    {
        exit(4);
    }

    /*
     * Invoked when connection and disconnection has completed.
     */
    std::promise<bool> connectionCompletedPromise;
    std::promise<void> connectionClosedPromise;
    std::promise<void> publishFinishedPromise;
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
        exit(5);
    }

    if (connectionCompletedPromise.get_future().get())
    {

        ByteBuf payload = ByteBufFromArray((const uint8_t *)messagePayload.data(), messagePayload.length());

        auto onPublishComplete = [&](Mqtt::MqttConnection &, uint16_t, int) { publishFinishedPromise.set_value(); };
        connection->Publish(daVars.topic.c_str(), AWS_MQTT_QOS_AT_MOST_ONCE, false, payload, onPublishComplete);
        fprintf(stderr, "waiting on publish..... ");
        publishFinishedPromise.get_future().wait();
        fprintf(stderr, "get on publish..... ");

        /* Disconnect */
        if (connection->Disconnect())
        {
            connectionClosedPromise.get_future().wait();
        }
    }
    else
    {
        exit(6);
    }

    return 0;
}
