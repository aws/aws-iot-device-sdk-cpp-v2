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
    apiHandle.InitializeLogging(Aws::Crt::LogLevel::Debug, stdout);
    String clientId(String("test-") + Aws::Crt::UUID().ToString());

    /*********************** Parse Arguments ***************************/
    DeviceAdvisorEnvironment daVars;
    if (!daVars.init(TestType::SUB_PUB))
    {
        exit(-1);
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
    std::promise<void> subscriptionCompletedPromise;
    std::promise<void> connectionClosedPromise;

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
        /*
         * This is invoked upon the receipt of a Publish on a subscribed topic.
         */
        std::promise<void> subscribeFinishedPromise;

        auto onMessage = [&](Mqtt::MqttConnection &,
                             const String & /*topic*/,
                             const ByteBuf & /*byteBuf*/,
                             bool /*dup*/,
                             Mqtt::QOS /*qos*/,
                             bool /*retain*/) { subscriptionCompletedPromise.set_value(); };

        auto onSubAck = [&](Mqtt::MqttConnection &,
                            uint16_t /*packetId*/,
                            const String & /*topic*/,
                            Mqtt::QOS /*QoS*/,
                            int /*errorCode*/) { subscriptionCompletedPromise.set_value(); };

        connection->Subscribe(daVars.topic.c_str(), AWS_MQTT_QOS_AT_MOST_ONCE, onMessage, onSubAck);
        subscriptionCompletedPromise.get_future().wait();

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
