/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/UUID.h>
#include <aws/crt/mqtt/Mqtt5Packets.h>
#include <aws/iot/Mqtt5Client.h>

#include <thread>

#include "../../utils/CommandLineUtils.h"

using namespace Aws::Crt;

int main(int argc, char *argv[])
{

    /************************ Setup the Lib ****************************/
    /*
     * Do the global initialization for the API.
     */
    ApiHandle apiHandle;
    uint32_t messageCount = 10;

    std::mutex receiveMutex;
    std::condition_variable receiveSignal;
    uint32_t receivedCount = 0;

    /*********************** Parse Arguments ***************************/
    Utils::CommandLineUtils cmdUtils = Utils::CommandLineUtils();
    cmdUtils.RegisterProgramName("mqtt5-pubsub");
    cmdUtils.AddCommonMQTTCommands();
    cmdUtils.RegisterCommand("key", "<path>", "Path to your key in PEM format.");
    cmdUtils.RegisterCommand("cert", "<path>", "Path to your client certificate in PEM format.");
    cmdUtils.AddCommonProxyCommands();
    cmdUtils.AddCommonTopicMessageCommands();
    cmdUtils.RegisterCommand("client_id", "<str>", "Client id to use (optional, default='test-*')");
    cmdUtils.RegisterCommand("port_override", "<int>", "The port override to use when connecting (optional)");
    cmdUtils.RegisterCommand("count", "<int>", "The number of messages to send (optional, default='10')");
    cmdUtils.AddLoggingCommands();
    const char **const_argv = (const char **)argv;
    cmdUtils.SendArguments(const_argv, const_argv + argc);
    cmdUtils.StartLoggingBasedOnCommand(&apiHandle);

    String topic = cmdUtils.GetCommandOrDefault("topic", "test/topic");
    String clientId = cmdUtils.GetCommandOrDefault("client_id", String("test-") + Aws::Crt::UUID().ToString());

    String messagePayload = cmdUtils.GetCommandOrDefault("message", "Hello world!");
    if (cmdUtils.HasCommand("count"))
    {
        int count = atoi(cmdUtils.GetCommand("count").c_str());
        if (count > 0)
        {
            messageCount = count;
        }
    }

    // Create a Client using Mqtt5ClientBuilder
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(
        cmdUtils.GetCommand("endpoint"), cmdUtils.GetCommand("cert").c_str(), cmdUtils.GetCommand("key").c_str());

    if (builder == nullptr)
    {
        printf("Failed to setup mqtt5 client builder.");
        return -1;
    }

    // Setup connection options
    std::shared_ptr<Mqtt5::ConnectPacket> connectOptions = std::make_shared<Mqtt5::ConnectPacket>();
    connectOptions->withClientId(clientId);
    builder->withConnectOptions(connectOptions);

    std::promise<bool> connectionPromise;
    std::promise<void> stoppedPromise;
    std::promise<void> disconnectPromise;
    std::promise<bool> subscribeSuccess;

    // Setup lifecycle callbacks
    builder->withClientConnectionSuccessCallback(
        [&connectionPromise](Mqtt5::Mqtt5Client &, const Mqtt5::OnConnectionSuccessEventData &eventData) {
            fprintf(
                stdout,
                "Mqtt5 Client connection succeed, clientid: %s.\n",
                eventData.negotiatedSettings->getClientId().c_str());
            connectionPromise.set_value(true);
        });

    builder->withClientConnectionFailureCallback(
        [&connectionPromise](Mqtt5::Mqtt5Client &, const Mqtt5::OnConnectionFailureEventData &eventData) {
            fprintf(
                stdout, "Mqtt5 Client connection failed with error: %s.\n", aws_error_debug_str(eventData.errorCode));
            connectionPromise.set_value(false);
        });

    builder->withClientStoppedCallback([&stoppedPromise](Mqtt5::Mqtt5Client &, const Mqtt5::OnStoppedEventData &) {
        fprintf(stdout, "Mqtt5 Client stopped.\n");
        stoppedPromise.set_value();
    });

    builder->withClientAttemptingConnectCallback([](Mqtt5::Mqtt5Client &, const Mqtt5::OnAttemptingConnectEventData &) {
        fprintf(stdout, "Mqtt5 Client attempting connection...\n");
    });

    builder->withClientDisconnectionCallback(
        [&disconnectPromise](Mqtt5::Mqtt5Client &, const Mqtt5::OnDisconnectionEventData &eventData) {
            fprintf(stdout, "Mqtt5 Client disconnection with reason: %s.\n", aws_error_debug_str(eventData.errorCode));
            disconnectPromise.set_value();
        });

    /*
     * This is invoked upon the receipt of a Publish on a subscribed topic.
     */
    builder->withPublishReceivedCallback(
        [&receiveMutex, &receivedCount, &receiveSignal](
            Mqtt5::Mqtt5Client & /*client*/, const Mqtt5::PublishReceivedEventData &eventData) {
            if (eventData.publishPacket == nullptr)
                return;

            std::lock_guard<std::mutex> lock(receiveMutex);
            ++receivedCount;
            fprintf(stdout, "Publish received on topic %s:", eventData.publishPacket->getTopic().c_str());
            fwrite(eventData.publishPacket->getPayload().ptr, 1, eventData.publishPacket->getPayload().len, stdout);
            fprintf(stdout, "\n");

            for (Mqtt5::UserProperty prop : eventData.publishPacket->getUserProperties())
            {
                fprintf(stdout, "\twith UserProperty:(%s,%s)\n", prop.getName().c_str(), prop.getValue().c_str());
            }
            receiveSignal.notify_all();
        });

    // Create Mqtt5Client
    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> client = builder->Build();

    // Clean up the builder
    delete builder;

    if (client == nullptr)
    {
        fprintf(stdout, "Client creation failed.\n");
        return -1;
    }

    // Start mqtt5 connection session
    if (client->Start())
    {
        if (connectionPromise.get_future().get() == false)
        {
            return -1;
        }

        auto onSubAck =
            [&subscribeSuccess](
                std::shared_ptr<Mqtt5::Mqtt5Client>, int error_code, std::shared_ptr<Mqtt5::SubAckPacket> suback) {
                if (error_code != 0)
                {
                    fprintf(
                        stdout,
                        "MQTT5 Client Subscription failed with error code: (%d)%s\n",
                        error_code,
                        aws_error_debug_str(error_code));
                    subscribeSuccess.set_value(false);
                }
                if (suback != nullptr)
                {
                    for (Mqtt5::SubAckReasonCode reasonCode : suback->getReasonCodes())
                    {
                        if (reasonCode > Mqtt5::SubAckReasonCode::AWS_MQTT5_SARC_UNSPECIFIED_ERROR)
                        {
                            fprintf(
                                stdout,
                                "MQTT5 Client Subscription failed with server error code: (%d)%s\n",
                                reasonCode,
                                suback->getReasonString()->c_str());
                            subscribeSuccess.set_value(false);
                            return;
                        }
                    }
                }
                subscribeSuccess.set_value(true);
            };

        Mqtt5::Subscription sub1(topic, Mqtt5::QOS::AWS_MQTT5_QOS_AT_LEAST_ONCE);
        sub1.withNoLocal(false);
        std::shared_ptr<Mqtt5::SubscribePacket> subPacket = std::make_shared<Mqtt5::SubscribePacket>();
        subPacket->withSubscription(std::move(sub1));

        if (client->Subscribe(subPacket, onSubAck))
        {
            // Waiting for subscription completed.
            if (subscribeSuccess.get_future().get() == true)
            {
                fprintf(stdout, "Subscription Success.\n");

                // Setup publish completion callback. The callback will get triggered when the pulbish completes (when
                // the client received the PubAck from the server).
                auto onPublishComplete = [](std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> client,
                                            int,
                                            std::shared_ptr<Aws::Crt::Mqtt5::PublishResult> result) {
                    if (!result->wasSuccessful())
                    {
                        fprintf(stdout, "Publish failed with error_code: %d", result->getErrorCode());
                    }
                    else if (result != nullptr)
                    {
                        std::shared_ptr<Mqtt5::PubAckPacket> puback =
                            std::dynamic_pointer_cast<Mqtt5::PubAckPacket>(result->getAck());
                        if (puback->getReasonCode() == 0)
                        {
                            fprintf(stdout, "Publish Succeed.\n");
                        }
                        else
                        {
                            fprintf(
                                stdout,
                                "PubACK reason code: %d : %s\n",
                                puback->getReasonCode(),
                                puback->getReasonString()->c_str());
                        }
                    };
                };

                uint32_t publishedCount = 0;
                while (publishedCount < messageCount)
                {
                    String message = messagePayload + std::to_string(publishedCount).c_str();
                    ByteCursor payload = ByteCursorFromString(message);

                    std::shared_ptr<Mqtt5::PublishPacket> publish =
                        std::make_shared<Mqtt5::PublishPacket>(topic, payload, Mqtt5::QOS::AWS_MQTT5_QOS_AT_LEAST_ONCE);
                    if (client->Publish(publish, onPublishComplete))
                    {
                        ++publishedCount;
                    }

                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }

                {
                    std::unique_lock<std::mutex> receivedLock(receiveMutex);
                    receiveSignal.wait(receivedLock, [&] { return receivedCount >= messageCount; });
                }

                /*
                 * Unsubscribe from the topic.
                 */
                std::promise<void> unsubscribeFinishedPromise;
                std::shared_ptr<Mqtt5::UnsubscribePacket> unsub = std::make_shared<Mqtt5::UnsubscribePacket>();
                unsub->withTopicFilter(topic);
                if (!client->Unsubscribe(
                        unsub, [&](std::shared_ptr<Mqtt5::Mqtt5Client>, int, std::shared_ptr<Mqtt5::UnSubAckPacket>) {
                            unsubscribeFinishedPromise.set_value();
                        }))
                {
                    fprintf(stdout, "Unsubscription failed.\n");
                    exit(-1);
                }
                unsubscribeFinishedPromise.get_future().wait();
            }
            else
            {
                fprintf(stdout, "Subscription failed.\n");
            }
        }
        else
        {
            fprintf(stdout, "Subscribe operation failed on client.\n");
        }

        /* Disconnect */
        if (!client->Stop())
        {
            fprintf(stdout, "Failed to disconnect from the mqtt connection. Exiting..\n");
            return -1;
        }
        stoppedPromise.get_future().wait();
    }

    return 0;
}
