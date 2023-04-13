/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/StlAllocator.h>
#include <aws/crt/UUID.h>
#include <aws/crt/auth/Credentials.h>
#include <aws/crt/io/TlsOptions.h>
#include <aws/iot/MqttClient.h>

#include <algorithm>

#include <chrono>
#include <mutex>
#include <thread>

#include "../../utils/CommandLineUtils.h"

using namespace Aws::Crt;

/**
 * A struct to hold all of the data for each MQTT connection in the
 * sample, like the client ID, whether it is connected, etc.
 */
struct CycleClient
{
    std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> client;
    String shared_topic = "test/shared_topic";
    String client_id = "";
    String publish_payload = "Hello World!";
    bool is_connected = false;
    bool is_subscribed_to_topics = false;
};

// A vector to hold all the clients used in the sample
std::vector<CycleClient> clients_holder;

/**
 * A helper function to get the CycleClient struct associated with the passed-in MqttConnection.
 * This allows us to get the CycleClient data in the MQTT callbacks.
 *
 * @param clients The vector containing all the CycleClients
 * @param connection The MqttConnection to find the CycleClient for
 * @return The CycleClient associated with the MqttConnection or nullptr if none is found
 */
CycleClient *getClientFromConnection(std::vector<CycleClient> *clients, Aws::Crt::Mqtt::MqttConnection *connection)
{
    for (size_t i = 0; i < clients->size(); i++)
    {
        CycleClient *current_client = &clients->at(i);
        if (current_client->client.get() == connection)
        {
            return current_client;
        }
    }
    return nullptr;
}

/**
 * A helper function that sets up the passed-in empty CycleClient struct.
 *
 * @param empty_client The CycleClient struct to set up
 * @param index The index of the CycleClient in the vector containing all the CycleClients
 * @param cmd_utils The CommandLineUtils used in the sample - for creating a MQTT connection
 */
void createNewClient(CycleClient *empty_client, size_t index, Utils::cmdData *cmdData)
{

    // Create the MQTT builder and populate it with data from cmdData
    auto clientConfigBuilder =
        Aws::Iot::MqttClientConnectionConfigBuilder(cmdData->input_cert.c_str(), cmdData->input_key.c_str());
    clientConfigBuilder.WithEndpoint(cmdData->input_endpoint);
    if (cmdData->input_ca != "")
    {
        clientConfigBuilder.WithCertificateAuthority(cmdData->input_ca.c_str());
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

    empty_client->client = connection;
    empty_client->client_id = "test-" + Aws::Crt::UUID().ToString() + "-client-";
    empty_client->client_id.append(std::to_string(index).c_str());

    auto onConnectionCompleted =
        [&](Mqtt::MqttConnection &connection, int errorCode, Mqtt::ReturnCode returnCode, bool) {
            if (errorCode)
            {
                fprintf(stdout, "[Lifecycle] Connection failed with error %s\n", ErrorDebugString(errorCode));
                exit(-1);
            }
            else
            {
                fprintf(stdout, "[Lifecycle] Connection completed with return code %d\n", returnCode);

                CycleClient *current_client = getClientFromConnection(&clients_holder, &connection);
                if (current_client != nullptr)
                {
                    current_client->is_connected = true;
                }
                else
                {
                    fprintf(stdout, "[Lifecycle] Connection could not find client in vector! Quitting sample");
                    exit(-1);
                }
            }
        };
    auto onInterrupted = [&](Mqtt::MqttConnection &, int error) {
        fprintf(stdout, "[Lifecycle] Connection interrupted with error %s\n", ErrorDebugString(error));
    };
    auto onDisconnect = [&](Mqtt::MqttConnection &connection) {
        {
            fprintf(stdout, "[Lifecycle] Disconnect completed\n");

            CycleClient *current_client = getClientFromConnection(&clients_holder, &connection);
            if (current_client != nullptr)
            {
                current_client->is_connected = false;
            }
            else
            {
                fprintf(stdout, "[Lifecycle] Disconnection could not find client in vector! Quitting sample");
                exit(-1);
            }
        }
    };
    auto onResumed = [&](Mqtt::MqttConnection &, Mqtt::ReturnCode, bool) {
        fprintf(stdout, "[Lifecycle] Connection resumed\n");
    };

    // Assign callbacks
    empty_client->client->OnConnectionCompleted = std::move(onConnectionCompleted);
    empty_client->client->OnDisconnect = std::move(onDisconnect);
    empty_client->client->OnConnectionInterrupted = std::move(onInterrupted);
    empty_client->client->OnConnectionResumed = std::move(onResumed);
}

/**
 * All of the operations the MQTT clients can perform in the sample
 */
enum OPERATIONS
{
    OPERATION_NULL,
    OPERATION_START,
    OPERATION_STOP,
    OPERATION_SUBSCRIBE,
    OPERATION_UNSUBSCRIBE,
    OPERATION_PUBLISH_QOS0,
    OPERATION_PUBLISH_QOS1,
    OPERATION_PUBLISH_TO_SUBSCRIBED_TOPIC_QOS0,
    OPERATION_PUBLISH_TO_SUBSCRIBED_TOPIC_QOS1,
    OPERATION_PUBLISH_TO_SHARED_TOPIC_QOS0,
    OPERATION_PUBLISH_TO_SHARED_TOPIC_QOS1,
    OPERATION_LENGTH
};

/**
 * The null operation - does nothing.
 *
 * @param current_client The client to perform the operation on
 * @param index The index of the client passed
 */
void operationNull(CycleClient *current_client, int index)
{
    (void)current_client;
    // Do nothing!
    fprintf(stdout, "[OP] Null called for client %i\n", index);
}

/**
 * The start operation will connect the passed-in client, if it is not already connected.
 *
 * @param current_client The client to perform the operation on
 * @param index The index of the client passed
 */
void operationStart(CycleClient *current_client, int index)
{
    if (current_client->is_connected == true)
    {
        fprintf(stdout, "[OP] Start called for client %i but is already connected\n", index);
        return;
    }
    fprintf(stdout, "[OP] About to start client %i\n", index);

    if (!current_client->client->Connect(current_client->client_id.c_str(), true, 1000))
    {
        fprintf(
            stdout,
            "[OP] Start client %i failed to connect with error %s\n",
            index,
            ErrorDebugString(current_client->client->LastError()));
        current_client->is_connected = false;
        exit(-1);
    }

    fprintf(stdout, "[OP] Waiting a maximum of 1 minute for client %i to report it is connected\n", index);
    uint64_t wait_count = 0;
    while (current_client->is_connected == false)
    {
        wait_count += 1;
        if (wait_count >= 60)
        {
            fprintf(stdout, "[OP] Client %i did not report connected after 1 minute\n", index);
            exit(-1);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    fprintf(stdout, "[OP] Started client %i\n", index);
}

/**
 * The stop operation will disconnect the passed-in client, if it is not already disconnected.
 *
 * @param current_client The client to perform the operation on
 * @param index The index of the client passed
 */
void operationStop(CycleClient *current_client, int index)
{
    if (current_client->is_connected == false)
    {
        fprintf(stdout, "[OP] Stop called for client %i but is not connected\n", index);
        return;
    }
    fprintf(stdout, "[OP] About to stop client %i\n", index);

    if (current_client->client->Disconnect())
    {
        fprintf(stdout, "[OP] Waiting a maximum of 1 minute for client %i to report it is disconnected\n", index);
        uint64_t wait_count = 0;
        while (current_client->is_connected == true)
        {
            wait_count += 1;
            if (wait_count >= 60)
            {
                fprintf(stdout, "[OP] Client %i did not report connected after 1 minute\n", index);
                exit(-1);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        fprintf(stdout, "[OP] Stopped client %i\n", index);
    }
    else
    {
        fprintf(stdout, "[OP] Disconnect call returned false, client %i likely already stopped\n", index);
    }
}

/**
 * The subscribe operation will subscribe to the shared topic and the client Id
 * topic of the passed-in client, if not already subscribed.
 *
 * @param current_client The client to perform the operation on
 * @param index The index of the client passed
 */
void operationSubscribe(CycleClient *current_client, int index)
{
    if (current_client->is_connected == false)
    {
        operationStart(current_client, index);
        return;
    }
    if (current_client->is_subscribed_to_topics == true)
    {
        fprintf(stdout, "[OP] Client %i already subscribed\n", index);
        return;
    }
    fprintf(stdout, "[OP] About to subscribe client %i\n", index);

    auto onMessage = [&](Mqtt::MqttConnection &, const String &topic, const ByteBuf &byteBuf, bool, Mqtt::QOS, bool) {
        {
            fprintf(stdout, "[Lifecycle] Publish received on topic %s\n", topic.c_str());
            fprintf(stdout, "[Lifecycle] Message: ");
            fwrite(byteBuf.buffer, 1, byteBuf.len, stdout);
            fprintf(stdout, "\n");
        }
    };

    std::promise<void> subscribeFinishedPromise;
    auto onSubAck = [&](Mqtt::MqttConnection &, uint16_t packetId, const String &topic, Mqtt::QOS QoS, int errorCode) {
        if (errorCode)
        {
            fprintf(stderr, "[Lifecycle] Subscribe failed with error %s\n", aws_error_debug_str(errorCode));
            exit(-1);
        }
        else
        {
            if (!packetId || QoS == AWS_MQTT_QOS_FAILURE)
            {
                fprintf(stderr, "[Lifecycle] Subscribe rejected by the broker.");
                exit(-1);
            }
            else
            {
                fprintf(
                    stdout, "[Lifecycle] Subscribe on topic %s on packetId %d Succeeded\n", topic.c_str(), packetId);
                subscribeFinishedPromise.set_value();
            }
        }
    };

    current_client->client->Subscribe(
        current_client->client_id.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, onMessage, onSubAck);
    subscribeFinishedPromise.get_future().wait();

    subscribeFinishedPromise = std::promise<void>();
    current_client->client->Subscribe(
        current_client->shared_topic.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, onMessage, onSubAck);
    subscribeFinishedPromise.get_future().wait();

    current_client->is_subscribed_to_topics = true;
    fprintf(stdout, "[OP] Subscribed client %i\n", index);
}

/**
 * The unsubscribe operation will unsubscribe to the shared topic and the client Id
 * topic of the passed-in client, if not already unsubscribed.
 *
 * @param current_client The client to perform the operation on
 * @param index The index of the client passed
 */
void operationUnsubscribe(CycleClient *current_client, int index)
{
    if (current_client->is_connected == false)
    {
        operationStart(current_client, index);
        return;
    }
    if (current_client->is_subscribed_to_topics == false)
    {
        fprintf(stdout, "[OP] Client %i not subscribed\n", index);
        return;
    }
    fprintf(stdout, "[OP] About to unsubscribe client %i\n", index);

    std::promise<void> unsubscribeFinishedPromise;
    current_client->client->Unsubscribe(current_client->client_id.c_str(), [&](Mqtt::MqttConnection &, uint16_t, int) {
        unsubscribeFinishedPromise.set_value();
    });
    unsubscribeFinishedPromise.get_future().wait();

    unsubscribeFinishedPromise = std::promise<void>();
    current_client->client->Unsubscribe(
        current_client->shared_topic.c_str(),
        [&](Mqtt::MqttConnection &, uint16_t, int) { unsubscribeFinishedPromise.set_value(); });
    unsubscribeFinishedPromise.get_future().wait();

    current_client->is_subscribed_to_topics = false;

    fprintf(stdout, "[OP] Unsubscribed client %i\n", index);
}

/**
 * The publish operation will publish a payload to the passed-in topic using the passed-in QOS.
 *
 * @param current_client The client to perform the operation on
 * @param index The index of the client passed
 * @param QOS The QOS to use with the publish
 * @param topic The topic to publish the message to
 */
void operationPublish(CycleClient *current_client, int index, Aws::Crt::Mqtt::QOS QOS, String topic)
{
    if (current_client->is_connected == false)
    {
        operationStart(current_client, index);
        return;
    }

    fprintf(stdout, "[OP] About to publish client %i with QoS %i with topic %s\n", index, QOS, topic.c_str());

    ByteBuf payload = ByteBufFromArray(
        (const uint8_t *)current_client->publish_payload.data(), current_client->publish_payload.length());
    auto onPublishComplete = [](Mqtt::MqttConnection &, uint16_t, int) {};
    current_client->client->Publish(topic.c_str(), QOS, false, payload, onPublishComplete);

    fprintf(stdout, "[OP] Published client %i with QoS %i with topic %s\n", index, QOS, topic.c_str());
}

/**
 * The publish QOS 0 operation will publish a QOS 0 message to the "topic1" topic
 *
 * @param current_client The client to perform the operation on
 * @param index The index of the client passed
 */
void operationPublishQOS0(CycleClient *current_client, int index)
{
    operationPublish(current_client, index, AWS_MQTT_QOS_AT_MOST_ONCE, "topic1");
}

/**
 * The publish QOS 1 operation will publish a QOS 1 message to the "topic1" topic
 *
 * @param current_client The client to perform the operation on
 * @param index The index of the client passed
 */
void operationPublishQOS1(CycleClient *current_client, int index)
{
    operationPublish(current_client, index, AWS_MQTT_QOS_AT_LEAST_ONCE, "topic1");
}

/**
 * The publish to subscribed QOS 0 operation will publish a QOS 0 message to the
 * client Id topic.
 *
 * @param current_client The client to perform the operation on
 * @param index The index of the client passed
 */
void operationPublishToSubscribedTopicQOS0(CycleClient *current_client, int index)
{
    operationPublish(current_client, index, AWS_MQTT_QOS_AT_MOST_ONCE, current_client->client_id);
}

/**
 * The publish to subscribed QOS 1 operation will publish a QOS 1 message to the
 * client Id topic.
 *
 * @param current_client The client to perform the operation on
 * @param index The index of the client passed
 */
void operationPublishToSubscribedTopicQOS1(CycleClient *current_client, int index)
{
    operationPublish(current_client, index, AWS_MQTT_QOS_AT_LEAST_ONCE, current_client->client_id);
}

/**
 * The publish to shared QOS 0 operation will publish a QOS 0 message to the
 * shared topic that all clients can be subscribed to.
 *
 * @param current_client The client to perform the operation on
 * @param index The index of the client passed
 */
void operationPublishToSharedTopicQOS0(CycleClient *current_client, int index)
{
    operationPublish(current_client, index, AWS_MQTT_QOS_AT_MOST_ONCE, current_client->shared_topic);
}

/**
 * The publish to shared QOS 1 operation will publish a QOS 1 message to the
 * shared topic that all clients can be subscribed to.
 *
 * @param current_client The client to perform the operation on
 * @param index The index of the client passed
 */
void operationPublishToSharedTopicQOS1(CycleClient *current_client, int index)
{
    operationPublish(current_client, index, AWS_MQTT_QOS_AT_LEAST_ONCE, current_client->shared_topic);
}

/**
 * Performs the operation for the passed-in random index on the passed-in CycleClient.
 *
 * @param current_client The client to perform the operation on
 * @param index The index of the client passed
 * @param random_index The index of the operation to perform
 */
void performOperation(CycleClient *current_client, int index, int random_index)
{
    if (random_index == OPERATION_NULL)
    {
        operationNull(current_client, index);
    }
    else if (random_index == OPERATION_START)
    {
        operationStart(current_client, index);
    }
    else if (random_index == OPERATION_STOP)
    {
        operationStop(current_client, index);
    }
    else if (random_index == OPERATION_SUBSCRIBE)
    {
        operationSubscribe(current_client, index);
    }
    else if (random_index == OPERATION_UNSUBSCRIBE)
    {
        operationUnsubscribe(current_client, index);
    }
    else if (random_index == OPERATION_PUBLISH_QOS0)
    {
        operationPublishQOS0(current_client, index);
    }
    else if (random_index == OPERATION_PUBLISH_QOS1)
    {
        operationPublishQOS1(current_client, index);
    }
    else if (random_index == OPERATION_PUBLISH_TO_SUBSCRIBED_TOPIC_QOS0)
    {
        operationPublishToSubscribedTopicQOS0(current_client, index);
    }
    else if (random_index == OPERATION_PUBLISH_TO_SUBSCRIBED_TOPIC_QOS1)
    {
        operationPublishToSubscribedTopicQOS1(current_client, index);
    }
    else if (random_index == OPERATION_PUBLISH_TO_SHARED_TOPIC_QOS0)
    {
        operationPublishToSharedTopicQOS0(current_client, index);
    }
    else if (random_index == OPERATION_PUBLISH_TO_SHARED_TOPIC_QOS1)
    {
        operationPublishToSharedTopicQOS1(current_client, index);
    }
    else
    {
        fprintf(stdout, "[OP] Unknown operation for client %i! Calling null operation...\n", index);
        operationNull(current_client, index);
    }
}

/**
 * Iterates through the vector holding all of the clients and performs a random operation
 *
 * @param clients A vector containing all of the clients
 */
void performRandomOperation(std::vector<CycleClient> *clients)
{
    int random_index = rand() % OPERATION_LENGTH;
    for (size_t i = 0; i < clients->size(); i++)
    {
        performOperation(&clients->at(i), (int)i, random_index);
        random_index = rand() % OPERATION_LENGTH;
    }
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
    Utils::cmdData cmdData = Utils::parseSampleInputCyclePubSub(argc, argv, &apiHandle);

    /******************** Start the client cycle ***********************/

    fprintf(stdout, "\nCycle pub-sub sample start\n\n");
    // Make the clients
    for (size_t i = 0; i < cmdData.input_clients; i++)
    {
        clients_holder.push_back(CycleClient());
        createNewClient(&clients_holder[i], i, &cmdData);
        fprintf(stdout, "Created client %zu\n", i);
    }

    // Get the current time
    auto startTime = std::chrono::steady_clock::now();

    // Seed the random number generator with the current time
    srand((unsigned int)time(nullptr));

    // Start the loop
    bool done = false;
    auto nowTime = std::chrono::steady_clock::now();
    uint64_t time_difference = 0;

    while (!done)
    {
        nowTime = std::chrono::steady_clock::now();
        time_difference = (uint64_t)std::chrono::duration_cast<std::chrono::seconds>(nowTime - startTime).count();
        if (time_difference >= cmdData.input_seconds)
        {
            done = true;
        }
        performRandomOperation(&clients_holder);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 * cmdData.input_tps));
    }

    // Stop all the clients and clear the vector
    for (size_t i = 0; i < cmdData.input_clients; i++)
    {
        operationStop(&clients_holder.at(i), (int)i);
    }
    clients_holder.clear();

    fprintf(stdout, "\nCycle pub-sub sample complete\n\n");
    return 0;
}
