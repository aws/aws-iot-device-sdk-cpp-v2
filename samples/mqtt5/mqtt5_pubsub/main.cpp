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

    /************************ Setup ****************************/

    // Do the global initialization for the API.
    ApiHandle apiHandle;

    // Variables needed for the sample
    std::mutex receiveMutex;
    std::condition_variable receiveSignal;
    uint32_t receivedCount = 0;

    /**
     * cmdData is the arguments/input from the command line placed into a single struct for
     * use in this sample. This handles all of the command line parsing, validating, etc.
     * See the Utils/CommandLineUtils for more information.
     */
    Utils::cmdData cmdData = Utils::parseSampleInputPubSub(argc, argv, &apiHandle, "mqtt5-pubsub");

    // Create the MQTT5 builder and populate it with data from cmdData.
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(
        cmdData.input_endpoint, cmdData.input_cert.c_str(), cmdData.input_key.c_str());

    // Check if the builder setup correctly.
    if (builder == nullptr)
    {
        printf(
            "Failed to setup mqtt5 client builder with error code %d: %s", LastError(), ErrorDebugString(LastError()));
        return -1;
    }

    // Setup connection options
    std::shared_ptr<Mqtt5::ConnectPacket> connectOptions = std::make_shared<Mqtt5::ConnectPacket>();
    connectOptions->WithClientId(cmdData.input_clientId);
    builder->WithConnectOptions(connectOptions);
    if (cmdData.input_port != 0)
    {
        builder->WithPort(static_cast<uint32_t>(cmdData.input_port));
    }

    std::promise<bool> connectionPromise;
    std::promise<void> stoppedPromise;
    std::promise<void> disconnectPromise;
    std::promise<bool> subscribeSuccess;

    // Setup lifecycle callbacks
    builder->WithClientConnectionSuccessCallback(
        [&connectionPromise](const Mqtt5::OnConnectionSuccessEventData &eventData) {
            fprintf(
                stdout,
                "Mqtt5 Client connection succeed, clientid: %s.\n",
                eventData.negotiatedSettings->getClientId().c_str());
            connectionPromise.set_value(true);
        });
    builder->WithClientConnectionFailureCallback([&connectionPromise](
                                                     const Mqtt5::OnConnectionFailureEventData &eventData) {
        fprintf(stdout, "Mqtt5 Client connection failed with error: %s.\n", aws_error_debug_str(eventData.errorCode));
        connectionPromise.set_value(false);
    });
    builder->WithClientStoppedCallback([&stoppedPromise](const Mqtt5::OnStoppedEventData &) {
        fprintf(stdout, "Mqtt5 Client stopped.\n");
        stoppedPromise.set_value();
    });
    builder->WithClientAttemptingConnectCallback([](const Mqtt5::OnAttemptingConnectEventData &) {
        fprintf(stdout, "Mqtt5 Client attempting connection...\n");
    });
    builder->WithClientDisconnectionCallback([&disconnectPromise](const Mqtt5::OnDisconnectionEventData &eventData) {
        fprintf(stdout, "Mqtt5 Client disconnection with reason: %s.\n", aws_error_debug_str(eventData.errorCode));
        disconnectPromise.set_value();
    });

    // This is invoked upon the receipt of a Publish on a subscribed topic.
    builder->WithPublishReceivedCallback(
        [&receiveMutex, &receivedCount, &receiveSignal](const Mqtt5::PublishReceivedEventData &eventData) {
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
        fprintf(
            stdout, "Failed to Init Mqtt5Client with error code %d: %s", LastError(), ErrorDebugString(LastError()));
        return -1;
    }

    /************************ Run the sample ****************************/

    // Start mqtt5 connection session
    if (client->Start())
    {
        if (connectionPromise.get_future().get() == false)
        {
            return -1;
        }

        auto onSubAck = [&subscribeSuccess](int error_code, std::shared_ptr<Mqtt5::SubAckPacket> suback) {
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

        Mqtt5::Subscription sub1(cmdData.input_topic, Mqtt5::QOS::AWS_MQTT5_QOS_AT_LEAST_ONCE);
        sub1.WithNoLocal(false);
        std::shared_ptr<Mqtt5::SubscribePacket> subPacket = std::make_shared<Mqtt5::SubscribePacket>();
        subPacket->WithSubscription(std::move(sub1));

        if (client->Subscribe(subPacket, onSubAck))
        {
            // Waiting for subscription completed.
            if (subscribeSuccess.get_future().get() == true)
            {
                fprintf(stdout, "Subscription Success.\n");

                /**
                 * Setup publish completion callback. The callback will get triggered when the publish completes (when
                 * the client received the PubAck from the server).
                 */
                auto onPublishComplete = [](int, std::shared_ptr<Aws::Crt::Mqtt5::PublishResult> result) {
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
                while (publishedCount < cmdData.input_count)
                {
                    // Add \" to 'JSON-ify' the message
                    String message = "\"" + cmdData.input_message + std::to_string(publishedCount + 1).c_str() + "\"";
                    ByteCursor payload = ByteCursorFromString(message);

                    std::shared_ptr<Mqtt5::PublishPacket> publish = std::make_shared<Mqtt5::PublishPacket>(
                        cmdData.input_topic, payload, Mqtt5::QOS::AWS_MQTT5_QOS_AT_LEAST_ONCE);
                    if (client->Publish(publish, onPublishComplete))
                    {
                        ++publishedCount;
                    }

                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }

                {
                    std::unique_lock<std::mutex> receivedLock(receiveMutex);
                    receiveSignal.wait(receivedLock, [&] { return receivedCount >= cmdData.input_count; });
                }

                // Unsubscribe from the topic.
                std::promise<void> unsubscribeFinishedPromise;
                std::shared_ptr<Mqtt5::UnsubscribePacket> unsub = std::make_shared<Mqtt5::UnsubscribePacket>();
                unsub->WithTopicFilter(cmdData.input_topic);
                if (!client->Unsubscribe(unsub, [&](int, std::shared_ptr<Mqtt5::UnSubAckPacket>) {
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

        // Disconnect
        if (!client->Stop())
        {
            fprintf(stdout, "Failed to disconnect from the mqtt connection. Exiting..\n");
            return -1;
        }
        stoppedPromise.get_future().wait();
    }
    return 0;
}
