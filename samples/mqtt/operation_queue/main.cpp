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
#include <thread>

#include "MqttOperationQueue.h"
#include "../../utils/CommandLineUtils.h"

using namespace Aws::Crt;

int main(int argc, char *argv[])
{

    /************************ Setup the Lib ****************************/
    /*
     * Do the global initialization for the API.
     */
    ApiHandle apiHandle;
    uint32_t messageCount = 20;
    uint32_t queueLimit = 10;
    uint32_t queueMode = 0;

    /*********************** Parse Arguments ***************************/
    Utils::CommandLineUtils cmdUtils = Utils::CommandLineUtils();
    cmdUtils.RegisterProgramName("operation_queue");
    cmdUtils.AddCommonMQTTCommands();
    cmdUtils.RegisterCommand("key", "<path>", "Path to your key in PEM format.");
    cmdUtils.RegisterCommand("cert", "<path>", "Path to your client certificate in PEM format.");
    cmdUtils.AddCommonProxyCommands();
    cmdUtils.AddCommonTopicMessageCommands();
    cmdUtils.RegisterCommand("client_id", "<str>", "Client id to use (optional, default='test-*')");
    cmdUtils.RegisterCommand("count", "<int>", "The number of messages to send (optional, default='20')");
    cmdUtils.RegisterCommand("port_override", "<int>", "The port override to use when connecting (optional)");
    cmdUtils.RegisterCommand("queue_limit", "<int>", "The maximum number of operations for the queue (optional, default='10')");
    cmdUtils.RegisterCommand("queue_mode", "<int>", "The mode for the queue to use. (optional, default='0')"
                             "\n\t0 = Overflow removes from queue back and new messages are pushed to queue back"
                             "\n\t1 = Overflow removes from queue front and new messages are pushed to queue back"
                             "\n\t2 = Overflow removes from queue front and new messages are pushed to queue front"
                             "\n\t3 = Overflow removes from queue back and messages are pushed to queue front");
    cmdUtils.AddLoggingCommands();
    const char **const_argv = (const char **)argv;
    cmdUtils.SendArguments(const_argv, const_argv + argc);
    cmdUtils.StartLoggingBasedOnCommand(&apiHandle);

    String topic = cmdUtils.GetCommandOrDefault("topic", "test/topic");
    String clientId = cmdUtils.GetCommandOrDefault("client_id", String("test-") + Aws::Crt::UUID().ToString());

    String messagePayload = cmdUtils.GetCommandOrDefault("message", "Hello world: ");
    if (cmdUtils.HasCommand("count"))
    {
        int count = atoi(cmdUtils.GetCommand("count").c_str());
        if (count > 0)
        {
            messageCount = count;
        }
    }
    if (cmdUtils.HasCommand("queue_limit")) {
        int limit = atoi(cmdUtils.GetCommand("queue_limit").c_str());
        if (limit > 0) {
            queueLimit = limit;
        }
    }
    if (cmdUtils.HasCommand("queue_mode")) {
        int mode = atoi(cmdUtils.GetCommand("queue_mode").c_str());
        if (mode > 0) {
            queueMode = mode;
        }
    }

    /* Get a MQTT client connection from the command parser */
    auto connection = cmdUtils.BuildMQTTConnection();

    /*
     * In a real world application you probably don't want to enforce synchronous behavior
     * but this is a sample console application, so we'll just do that with a condition variable.
     */
    std::promise<bool> connectionCompletedPromise;
    std::promise<void> connectionClosedPromise;

    /*
     * This will execute when an MQTT connect has completed or failed.
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

    auto onInterrupted = [&](Mqtt::MqttConnection &, int error) {
        fprintf(stdout, "Connection interrupted with error %s\n", ErrorDebugString(error));
    };

    auto onResumed = [&](Mqtt::MqttConnection &, Mqtt::ReturnCode, bool) { fprintf(stdout, "Connection resumed\n"); };

    /*
     * Invoked when a disconnect message has completed.
     */
    auto onDisconnect = [&](Mqtt::MqttConnection &) {
        {
            fprintf(stdout, "Disconnect completed\n");
            connectionClosedPromise.set_value();
        }
    };

    connection->OnConnectionCompleted = std::move(onConnectionCompleted);
    connection->OnDisconnect = std::move(onDisconnect);
    connection->OnConnectionInterrupted = std::move(onInterrupted);
    connection->OnConnectionResumed = std::move(onResumed);

    std::promise<void> onQueueEmptyPromise;
    auto onQueueEmpty = [&]() {
        onQueueEmptyPromise.set_value();
    };

    /*
     * Setup the queue
     */
    MqttOperationQueue::MqttOperationQueueBuilder queueBuilder = MqttOperationQueue::MqttOperationQueueBuilder();
    queueBuilder.WithConnection(connection).WithQueueLimitSize(queueLimit).WithEnableLogging(true);
    queueBuilder.WithQueueEmptyCallback(std::move(onQueueEmpty));
    /*
     * The different queue insert/limit mode combos
     */
    if (queueMode == 0) {
        queueBuilder.WithQueueInsertBehavior(MqttOperationQueue::QueueInsertBehavior::INSERT_BACK)
                    .WithQueueLimitBehavior(MqttOperationQueue::QueueLimitBehavior::DROP_BACK);
    } else if (queueMode == 1) {
        queueBuilder.WithQueueInsertBehavior(MqttOperationQueue::QueueInsertBehavior::INSERT_BACK)
                    .WithQueueLimitBehavior(MqttOperationQueue::QueueLimitBehavior::DROP_FRONT);
    } else if (queueMode == 2) {
        queueBuilder.WithQueueInsertBehavior(MqttOperationQueue::QueueInsertBehavior::INSERT_FRONT)
                    .WithQueueLimitBehavior(MqttOperationQueue::QueueLimitBehavior::DROP_FRONT);
    } else if (queueMode == 3) {
        queueBuilder.WithQueueInsertBehavior(MqttOperationQueue::QueueInsertBehavior::INSERT_FRONT)
                    .WithQueueLimitBehavior(MqttOperationQueue::QueueLimitBehavior::DROP_BACK);
    }
    /* Create the queue from the configuration */
    std::shared_ptr<MqttOperationQueue::MqttOperationQueue> operationQueue = queueBuilder.Build();

    /*
     * Actually perform the connect dance.
     */
    fprintf(stdout, "Connecting...\n");
    if (!connection->Connect(clientId.c_str(), false /*cleanSession*/, 1000 /*keepAliveTimeSecs*/))
    {
        fprintf(stderr, "MQTT Connection failed with error %s\n", ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    if (connectionCompletedPromise.get_future().get())
    {
        std::mutex receiveMutex;
        std::condition_variable receiveSignal;
        uint32_t receivedCount = 0;

        /* Start the operation queue so it can process operations */
        operationQueue->Start();

        /* This is invoked upon the receipt of a Publish on a subscribed topic */
        auto onMessage = [&](Mqtt::MqttConnection &,
                             const String &topic,
                             const ByteBuf &byteBuf,
                             bool /*dup*/,
                             Mqtt::QOS /*qos*/,
                             bool /*retain*/) {
            {
                std::lock_guard<std::mutex> lock(receiveMutex);
                ++receivedCount;
                fprintf(stdout, "Publish #%d received on topic %s with message: ", receivedCount, topic.c_str());
                fwrite(byteBuf.buffer, 1, byteBuf.len, stdout);
                fprintf(stdout, "\n");
            }

            receiveSignal.notify_all();
        };

        /* Subscribe for incoming publish messages on topic.*/
        std::promise<void> subscribeFinishedPromise;
        auto onSubAck =
            [&](Mqtt::MqttConnection &, uint16_t packetId, const String &topic, Mqtt::QOS QoS, int errorCode) {
                if (errorCode)
                {
                    fprintf(stderr, "Subscribe failed with error %s\n", aws_error_debug_str(errorCode));
                    exit(-1);
                }
                else
                {
                    if (!packetId || QoS == AWS_MQTT_QOS_FAILURE)
                    {
                        fprintf(stderr, "Subscribe rejected by the broker.");
                        exit(-1);
                    }
                    else
                    {
                        fprintf(stdout, "Subscribe on topic %s on packetId %d Succeeded\n", topic.c_str(), packetId);
                    }
                }
                subscribeFinishedPromise.set_value();
            };
        operationQueue->Subscribe(topic.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, onMessage, onSubAck);
        /* Wait for the queue to be empty, meaning all operations in the queue were sent */
        onQueueEmptyPromise.get_future().wait();
        // Wait for the subscribe ACK from the server
        subscribeFinishedPromise.get_future().wait();

        /* Publish messageCount times. If messageCount is larger than the queue, only the queue amount of messages will be published */
        onQueueEmptyPromise = std::promise<void>();
        uint32_t publishedCount = 0;
        while (publishedCount < messageCount)
        {
            String sendPayload = messagePayload + std::to_string(publishedCount+1).c_str();
            ByteBuf payload = ByteBufFromArray((const uint8_t *)sendPayload.data(), sendPayload.length());

            auto onPublishComplete = [](Mqtt::MqttConnection &, uint16_t, int) {};
            operationQueue->Publish(topic.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, false, payload, onPublishComplete);
            ++publishedCount;
        }
        /* Wait for the queue to be empty, meaning all operations in the queue were sent */
        onQueueEmptyPromise.get_future().wait();
        // Wait for all the publishes sent to have gotten ACKs from the server
        {
            std::unique_lock<std::mutex> receivedLock(receiveMutex);
            receiveSignal.wait(receivedLock, [&] { return receivedCount >= queueLimit; });
        }

        /* Unsubscribe from the topic. */
        onQueueEmptyPromise = std::promise<void>();
        std::promise<void> unsubscribeFinishedPromise;
        operationQueue->Unsubscribe(
            topic.c_str(), [&](Mqtt::MqttConnection &, uint16_t, int) { unsubscribeFinishedPromise.set_value(); });
        /* Wait for the queue to be empty, meaning all operations in the queue were sent */
        onQueueEmptyPromise.get_future().wait();
        /* Wait for the unsubscribe ACK from the server */
        unsubscribeFinishedPromise.get_future().wait();

        /* Stop the queue so it no longer can process operations. This is necessary due to the queue using a thread */
        operationQueue->Stop();

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
