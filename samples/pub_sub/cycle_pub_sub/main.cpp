/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/StlAllocator.h>
#include <aws/crt/auth/Credentials.h>
#include <aws/crt/io/TlsOptions.h>

#include <aws/iot/MqttClient.h>

#include <algorithm>
#include <aws/crt/UUID.h>
#include <chrono>
#include <mutex>

#include "../../utils/CommandLineUtils.h"

using namespace Aws::Crt;

struct CycleClient
{
    std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> client;
    // TODO - add a way to tell if we are connected or not via promises or similar. Right now is kind of unsafe but it
    // works
    String shared_topic = "test/shared_topic";
    String client_id = "";
    String publish_payload = "Hello World!";
    bool is_connected = false;
    bool subscribed_to_topics = false;
};

void createNewClient(CycleClient *empty_client, size_t index, Utils::CommandLineUtils *cmd_utils)
{
    empty_client->client = cmd_utils->BuildMQTTConnection();
    empty_client->client_id = "test-" + Aws::Crt::UUID().ToString() + "-client-";
    empty_client->client_id.append(std::to_string(index).c_str());

    auto onConnectionCompleted = [&](Mqtt::MqttConnection &, int errorCode, Mqtt::ReturnCode returnCode, bool) {
        if (errorCode)
        {
            fprintf(stdout, "[Lifecycle] Connection failed with error %s\n", ErrorDebugString(errorCode));
            exit(-1);
        }
        else
        {
            fprintf(stdout, "[Lifecycle] Connection completed with return code %d\n", returnCode);
        }
    };
    auto onInterrupted = [&](Mqtt::MqttConnection &, int error) {
        fprintf(stdout, "[Lifecycle] Connection interrupted with error %s\n", ErrorDebugString(error));
    };
    auto onDisconnect = [&](Mqtt::MqttConnection &) {
        {
            fprintf(stdout, "[Lifecycle] Disconnect completed\n");
        }
    };
    auto onResumed = [&](Mqtt::MqttConnection &, Mqtt::ReturnCode, bool) {
        fprintf(stdout, "[Lifecycle] Connection resumed\n");
    };

    empty_client->client->OnConnectionCompleted = std::move(onConnectionCompleted);
    empty_client->client->OnDisconnect = std::move(onDisconnect);
    empty_client->client->OnConnectionInterrupted = std::move(onInterrupted);
    empty_client->client->OnConnectionResumed = std::move(onResumed);
}

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

void operationNull(CycleClient *current_client, int index)
{
    (void)current_client;
    // Do nothing!
    fprintf(stdout, "[OP] Null called for client %i\n", index);
}

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
    current_client->is_connected = true;
    fprintf(stdout, "[OP] Started client %i\n", index);
}

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
        fprintf(stdout, "[OP] Stopped client %i\n", index);
        current_client->is_connected = false;
    }
    else
    {
        fprintf(stdout, "[OP] Disconnect call returned false, client %i likely already stopped\n", index);
    }
}

void operationSubscribe(CycleClient *current_client, int index)
{
    if (current_client->is_connected == false)
    {
        operationStart(current_client, index);
        return;
    }
    if (current_client->subscribed_to_topics == true)
    {
        fprintf(stdout, "[OP] Client %i already subscribed\n", index);
        return;
    }
    fprintf(stdout, "[OP] About to subscribe client %i\n", index);

    auto onMessage = [&](Mqtt::MqttConnection &,
                         const String &topic,
                         const ByteBuf &byteBuf,
                         bool /*dup*/,
                         Mqtt::QOS /*qos*/,
                         bool /*retain*/) {
        {
            fprintf(stdout, "[Lifecycle] Publish received on topic %s\n", topic.c_str());
            fprintf(stdout, "[Lifecycle] Message: ");
            fwrite(byteBuf.buffer, 1, byteBuf.len, stdout);
            fprintf(stdout, "\n");
        }
    };

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
            }
        }
    };

    current_client->client->Subscribe(
        current_client->client_id.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, onMessage, onSubAck);
    current_client->client->Subscribe(
        current_client->shared_topic.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, onMessage, onSubAck);
    // Not sure how to wait for subscribe, so... don't I guess...

    current_client->subscribed_to_topics = true;
    fprintf(stdout, "[OP] Subscribed client %i\n", index);
}

void operationUnsubscribe(CycleClient *current_client, int index)
{
    if (current_client->is_connected == false)
    {
        operationStart(current_client, index);
        return;
    }
    if (current_client->subscribed_to_topics == false)
    {
        fprintf(stdout, "[OP] Client %i not subscribed\n", index);
        return;
    }
    fprintf(stdout, "[OP] About to unsubscribe client %i\n", index);

    std::promise<void> unsubscribeFinishedPromise_One;
    current_client->client->Unsubscribe(current_client->client_id.c_str(), [&](Mqtt::MqttConnection &, uint16_t, int) {
        unsubscribeFinishedPromise_One.set_value();
    });
    unsubscribeFinishedPromise_One.get_future().wait();

    std::promise<void> unsubscribeFinishedPromise_Two;
    current_client->client->Unsubscribe(
        current_client->shared_topic.c_str(),
        [&](Mqtt::MqttConnection &, uint16_t, int) { unsubscribeFinishedPromise_Two.set_value(); });
    unsubscribeFinishedPromise_Two.get_future().wait();

    current_client->subscribed_to_topics = false;

    fprintf(stdout, "[OP] Unsubscribed client %i\n", index);
}

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

void operationPublishQOS0(CycleClient *current_client, int index)
{
    operationPublish(current_client, index, AWS_MQTT_QOS_AT_MOST_ONCE, "topic1");
}

void operationPublishQOS1(CycleClient *current_client, int index)
{
    operationPublish(current_client, index, AWS_MQTT_QOS_AT_LEAST_ONCE, "topic1");
}

void operationPublishToSubscribedTopicQOS0(CycleClient *current_client, int index)
{
    operationPublish(current_client, index, AWS_MQTT_QOS_AT_MOST_ONCE, current_client->client_id);
}

void operationPublishToSubscribedTopicQOS1(CycleClient *current_client, int index)
{
    operationPublish(current_client, index, AWS_MQTT_QOS_AT_LEAST_ONCE, current_client->client_id);
}

void operationPublishToSharedTopicQOS0(CycleClient *current_client, int index)
{
    operationPublish(current_client, index, AWS_MQTT_QOS_AT_MOST_ONCE, current_client->shared_topic);
}

void operationPublishToSharedTopicQOS1(CycleClient *current_client, int index)
{
    operationPublish(current_client, index, AWS_MQTT_QOS_AT_LEAST_ONCE, current_client->shared_topic);
}

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

void performRandomOperation(std::vector<CycleClient> *clients_holder, size_t length)
{
    int random_index = rand() % OPERATION_LENGTH;
    for (size_t i = 0; i < length; i++)
    {
        performOperation(&clients_holder->at(i), (int)i, random_index);
        random_index = rand() % OPERATION_LENGTH;
    }
}

int main(int argc, char *argv[])
{

    /************************ Setup the Lib ****************************/
    /*
     * Do the global initialization for the API.
     */
    ApiHandle apiHandle;

    uint32_t config_clients = 3;
    uint32_t config_tps = 12;
    // uint64_t config_seconds = 600;
    uint64_t config_seconds = 120;

    /*********************** Parse Arguments ***************************/
    Utils::CommandLineUtils cmdUtils = Utils::CommandLineUtils();
    cmdUtils.RegisterProgramName("cycle_pub_sub");
    cmdUtils.AddCommonMQTTCommands();
    cmdUtils.RegisterCommand("key", "<path>", "Path to your key in PEM format.");
    cmdUtils.RegisterCommand("cert", "<path>", "Path to your client certificate in PEM format.");

    cmdUtils.RegisterCommand("clients", "<int>", "The number of clients/connections to make (optional, default='3'");
    cmdUtils.RegisterCommand(
        "tps", "<int>", "The number of seconds to wait after performing an operation (optional, default=12)");
    cmdUtils.RegisterCommand(
        "seconds", "<int>", "The number of seconds to run the sample for (optional, default='600')");

    cmdUtils.AddLoggingCommands();
    const char **const_argv = (const char **)argv;
    cmdUtils.SendArguments(const_argv, const_argv + argc);
    cmdUtils.StartLoggingBasedOnCommand(&apiHandle);

    if (cmdUtils.HasCommand("clients"))
    {
        int tmp = atoi(cmdUtils.GetCommand("clients").c_str());
        if (tmp > 0)
        {
            config_clients = tmp;
        }
    }
    if (cmdUtils.HasCommand("tps"))
    {
        int tmp = atoi(cmdUtils.GetCommand("tps").c_str());
        if (tmp > 0)
        {
            config_tps = tmp;
        }
    }
    if (cmdUtils.HasCommand("seconds"))
    {
        int tmp = atoi(cmdUtils.GetCommand("seconds").c_str());
        if (tmp > 0)
        {
            config_seconds = tmp;
        }
    }

    fprintf(stdout, "\nCycle pub-sub sample start\n\n");

    /******************** Start the client cycle ***********************/

    // Make the clients
    std::vector<CycleClient> clients_holder;
    for (size_t i = 0; i < config_clients; i++)
    {
        clients_holder.push_back(CycleClient());
        createNewClient(&clients_holder[i], i, &cmdUtils);
        fprintf(stdout, "Created client %zu\n", i);
    }

    // Get the current time
    auto startTime = std::chrono::steady_clock::now();

    // Start the loop
    bool done = false;
    auto nowTime = std::chrono::steady_clock::now();
    uint64_t time_difference = 0;

    while (!done)
    {
        nowTime = std::chrono::steady_clock::now();
        time_difference = (uint64_t)std::chrono::duration_cast<std::chrono::seconds>(nowTime - startTime).count();
        if (time_difference >= config_seconds)
        {
            done = true;
        }
        performRandomOperation(&clients_holder, config_clients);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000 * config_tps));
    }

    /*************************** Clean up ******************************/

    // Stop all the clients
    for (size_t i = 0; i < config_clients; i++)
    {
        operationStop(&clients_holder.at(i), (int)i);
    }
    // Wait a second
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    fprintf(stdout, "\nCycle pub-sub sample complete\n\n");
    return 0;
}
