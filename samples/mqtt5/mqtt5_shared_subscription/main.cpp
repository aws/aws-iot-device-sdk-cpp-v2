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

/**
 * For the purposes of this sample, we need to associate certain variables with a particular MQTT5 client
 * and to do so we use this class to hold all the data for a particular client used in the sample.
 */
class sample_mqtt5_client
{
  public:
    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> client;
    String name;
    std::promise<bool> connectionPromise;
    std::promise<void> stoppedPromise;
    std::mutex receiveMutex;

    // A helper function to print a message and then exit the sample.
    void PrintMessageAndExit(String message, int exitCode)
    {
        fprintf(stdout, "[%s]: %s\n", this->name.c_str(), message.c_str());
        exit(exitCode);
    }

    // Creates a MQTT5 client using direct MQTT5 via mTLS with the passed input data.
    static std::shared_ptr<sample_mqtt5_client> create_mqtt5_client(
        String input_endpoint,
        String input_cert,
        String input_key,
        String input_ca,
        String input_clientId,
        String input_clientName)
    {
        std::shared_ptr<sample_mqtt5_client> result = std::make_shared<sample_mqtt5_client>();
        result->name = input_clientName;

        Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(
            input_endpoint, input_cert.c_str(), input_key.c_str());
        if (builder == nullptr)
        {
            return nullptr;
        }

        if (input_ca != "")
        {
            builder->WithCertificateAuthority(input_ca.c_str());
        }
        std::shared_ptr<Mqtt5::ConnectPacket> connectOptions = std::make_shared<Mqtt5::ConnectPacket>();
        connectOptions->WithClientId(input_clientId);
        builder->WithConnectOptions(connectOptions);

        builder->WithClientConnectionSuccessCallback([result](const Mqtt5::OnConnectionSuccessEventData &eventData) {
            fprintf(
                stdout,
                "[%s]: Connection succeed, clientid: %s.\n",
                result->name.c_str(),
                eventData.negotiatedSettings->getClientId().c_str());

            try
            {
                result->connectionPromise.set_value(true);
            }
            catch (...)
            {
                fprintf(
                    stdout,
                    "[%s]: Exception ocurred trying to set connection promise (likely already set)\n",
                    result->name.c_str());
            }
        });

        builder->WithClientConnectionFailureCallback([result](const Mqtt5::OnConnectionFailureEventData &eventData) {
            fprintf(
                stdout,
                "[%s]: Connection failed with error: %s.\n",
                result->name.c_str(),
                aws_error_debug_str(eventData.errorCode));

            try
            {
                result->connectionPromise.set_value(false);
            }
            catch (...)
            {
                fprintf(
                    stdout,
                    "[%s]: Exception ocurred trying to set connection promise (likely already set)\n",
                    result->name.c_str());
            }
        });

        builder->WithClientStoppedCallback([result](const Mqtt5::OnStoppedEventData &) {
            fprintf(stdout, "[%s]: Stopped.\n", result->name.c_str());
            result->stoppedPromise.set_value();
        });

        builder->WithClientDisconnectionCallback([result](const Mqtt5::OnDisconnectionEventData &eventData) {
            fprintf(
                stdout,
                "[%s]: Disconnection with reason: %s.\n",
                result->name.c_str(),
                aws_error_debug_str(eventData.errorCode));
            if (eventData.disconnectPacket != nullptr)
            {
                fprintf(stdout, "\tReason code: %u\n", (uint32_t)eventData.disconnectPacket->getReasonCode());
            }
        });

        builder->WithPublishReceivedCallback([result](const Mqtt5::PublishReceivedEventData &eventData) {
            fprintf(stdout, "[%s]: Received a publish\n", result->name.c_str());

            std::lock_guard<std::mutex> lock(result->receiveMutex);
            if (eventData.publishPacket != nullptr)
            {
                fprintf(stdout, "\tPublish received on topic: %s\n", eventData.publishPacket->getTopic().c_str());
                fprintf(stdout, "\tMessage: ");
                fwrite(eventData.publishPacket->getPayload().ptr, 1, eventData.publishPacket->getPayload().len, stdout);
                fprintf(stdout, "\n");

                for (Mqtt5::UserProperty prop : eventData.publishPacket->getUserProperties())
                {
                    fprintf(stdout, "\t\twith UserProperty:(%s,%s)\n", prop.getName().c_str(), prop.getValue().c_str());
                }
            }
        });

        result->client = builder->Build();
        delete builder;
        return result;
    }
};

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
    Utils::cmdData cmdData = Utils::parseSampleInputSharedSubscription(argc, argv, &apiHandle);

    String input_sharedTopic = String("$share/") + cmdData.input_groupIdentifier + String("/") + cmdData.input_topic;

    // Create the MQTT5 clients: one publisher and two subscribers
    std::shared_ptr<sample_mqtt5_client> publisher = sample_mqtt5_client::create_mqtt5_client(
        cmdData.input_endpoint,
        cmdData.input_cert,
        cmdData.input_key,
        cmdData.input_ca,
        cmdData.input_clientId + String("1"),
        String("Publisher"));
    std::shared_ptr<sample_mqtt5_client> subscriberOne = sample_mqtt5_client::create_mqtt5_client(
        cmdData.input_endpoint,
        cmdData.input_cert,
        cmdData.input_key,
        cmdData.input_ca,
        cmdData.input_clientId + String("2"),
        String("Subscriber One"));
    std::shared_ptr<sample_mqtt5_client> subscriberTwo = sample_mqtt5_client::create_mqtt5_client(
        cmdData.input_endpoint,
        cmdData.input_cert,
        cmdData.input_key,
        cmdData.input_ca,
        cmdData.input_clientId + String("3"),
        String("Subscriber Two"));

    if (publisher == nullptr || subscriberOne == nullptr || subscriberTwo == nullptr)
    {
        fprintf(stdout, "Client creation failed.\n");
        exit(-1);
    }

    /*********************** Connect the MQTT5 clients ***************************/

    if (publisher->client->Start())
    {
        if (publisher->connectionPromise.get_future().get() == false)
        {
            publisher->PrintMessageAndExit("Connection was unsuccessful", -1);
        }
        fprintf(stdout, "[%s] Connected.\n", publisher->name.c_str());
    }
    else
    {
        publisher->PrintMessageAndExit("Could not start", -1);
    }

    if (subscriberOne->client->Start())
    {
        if (subscriberOne->connectionPromise.get_future().get() == false)
        {
            subscriberOne->PrintMessageAndExit("Connection was unsuccessful", -1);
        }
        fprintf(stdout, "[%s] Connected.\n", subscriberOne->name.c_str());
    }
    else
    {
        subscriberOne->PrintMessageAndExit("Could not start", -1);
    }

    if (subscriberTwo->client->Start())
    {
        if (subscriberTwo->connectionPromise.get_future().get() == false)
        {
            subscriberTwo->PrintMessageAndExit("Connection was unsuccessful", -1);
        }
        fprintf(stdout, "[%s] Connected.\n", subscriberTwo->name.c_str());
    }
    else
    {
        subscriberTwo->PrintMessageAndExit("Could not start", -1);
    }

    /*********************** Subscribe the two subscriber MQTT5 clients ***************************/

    std::promise<Mqtt5::SubAckReasonCode> subscribeSuccess;
    auto onSubAck = [&subscribeSuccess](int error_code, std::shared_ptr<Mqtt5::SubAckPacket> suback) {
        if (error_code != 0)
        {
            fprintf(
                stdout,
                "MQTT5 Client Subscription failed with error code: (%d)%s\n",
                error_code,
                aws_error_debug_str(error_code));
            subscribeSuccess.set_value(Mqtt5::SubAckReasonCode::AWS_MQTT5_SARC_UNSPECIFIED_ERROR);
        }
        if (suback != nullptr)
        {
            for (Mqtt5::SubAckReasonCode reasonCode : suback->getReasonCodes())
            {
                if (reasonCode >= Mqtt5::SubAckReasonCode::AWS_MQTT5_SARC_UNSPECIFIED_ERROR)
                {
                    fprintf(
                        stdout,
                        "MQTT5 Client Subscription failed with server error code: (%d)%s\n",
                        reasonCode,
                        suback->getReasonString()->c_str());
                    subscribeSuccess.set_value(reasonCode);
                    return;
                }
            }
        }
        subscribeSuccess.set_value(Mqtt5::SubAckReasonCode::AWS_MQTT5_SARC_GRANTED_QOS_1);
    };
    Mqtt5::Subscription sub1(input_sharedTopic, Mqtt5::QOS::AWS_MQTT5_QOS_AT_LEAST_ONCE);
    sub1.WithNoLocal(false);
    std::shared_ptr<Mqtt5::SubscribePacket> subPacket = std::make_shared<Mqtt5::SubscribePacket>();
    subPacket->WithSubscription(std::move(sub1));

    if (subscriberOne->client->Subscribe(subPacket, onSubAck))
    {
        // Wait just a little bit to see if the client was disconnected due to missing Shared Subscription support.
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        Mqtt5::SubAckReasonCode result = subscribeSuccess.get_future().get();
        if (result >= Mqtt5::SubAckReasonCode::AWS_MQTT5_SARC_UNSPECIFIED_ERROR)
        {
            subscriberOne->PrintMessageAndExit("Failed to subscribe", -1);
        }

        fprintf(
            stdout,
            "[%s] Subscribed to topic '%s' in shared subscription group '%s'. \n",
            subscriberOne->name.c_str(),
            cmdData.input_topic.c_str(),
            cmdData.input_groupIdentifier.c_str());
        fprintf(
            stdout, "[%s] Full subscribed topic is: '%s'.\n", subscriberOne->name.c_str(), input_sharedTopic.c_str());
    }
    else
    {
        subscriberOne->PrintMessageAndExit("Failed to subscribe", -1);
    }

    subscribeSuccess = std::promise<Mqtt5::SubAckReasonCode>();
    if (subscriberTwo->client->Subscribe(subPacket, onSubAck))
    {
        // Wait just a little bit to see if the client was disconnected due to missing Shared Subscription support.
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        Mqtt5::SubAckReasonCode result = subscribeSuccess.get_future().get();
        if (result >= Mqtt5::SubAckReasonCode::AWS_MQTT5_SARC_UNSPECIFIED_ERROR)
        {
            subscriberTwo->PrintMessageAndExit("Failed to subscribe", -1);
        }

        fprintf(
            stdout,
            "[%s] Subscribed to topic '%s' in shared subscription group '%s'.\n",
            subscriberTwo->name.c_str(),
            cmdData.input_topic.c_str(),
            cmdData.input_groupIdentifier.c_str());
        fprintf(
            stdout, "[%s] Full subscribed topic is: '%s'.\n", subscriberTwo->name.c_str(), input_sharedTopic.c_str());
    }
    else
    {
        subscriberTwo->PrintMessageAndExit("Failed to subscribe", -1);
    }

    /*********************** Publish ***************************/

    auto onPublishComplete = [publisher](int, std::shared_ptr<Aws::Crt::Mqtt5::PublishResult> result) {
        if (!result->wasSuccessful())
        {
            fprintf(stdout, "[%s] Publish failed with error_code: %d", publisher->name.c_str(), result->getErrorCode());
        }
        else if (result != nullptr)
        {
            std::shared_ptr<Mqtt5::PubAckPacket> puback =
                std::dynamic_pointer_cast<Mqtt5::PubAckPacket>(result->getAck());
            if (puback->getReasonCode() == 0)
            {
                fprintf(stdout, "[%s]: Publish Succeed.\n", publisher->name.c_str());
            }
            else
            {
                fprintf(
                    stdout,
                    "[%s] Publish failed. PubACK reason code: %d : %s\n",
                    publisher->name.c_str(),
                    puback->getReasonCode(),
                    puback->getReasonString()->c_str());
            }
        };
    };

    uint64_t publishedCount = 0;
    while (publishedCount < cmdData.input_count)
    {
        // Add \" to 'JSON-ify' the message
        String message = "\"" + cmdData.input_message + std::to_string(publishedCount + 1).c_str() + "\"";
        ByteCursor payload = ByteCursorFromString(message);
        std::shared_ptr<Mqtt5::PublishPacket> publish = std::make_shared<Mqtt5::PublishPacket>(
            cmdData.input_topic, payload, Mqtt5::QOS::AWS_MQTT5_QOS_AT_LEAST_ONCE);
        if (publisher->client->Publish(publish, onPublishComplete))
        {
            ++publishedCount;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    // Wait 5 seconds to let the last publish go out before unsubscribing
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    /*********************** Unsubscribe the subscribers ***************************/

    std::promise<void> unsubscribeFinishedPromise;
    std::shared_ptr<Mqtt5::UnsubscribePacket> unsub = std::make_shared<Mqtt5::UnsubscribePacket>();
    unsub->WithTopicFilter(input_sharedTopic);
    if (!subscriberOne->client->Unsubscribe(
            unsub, [&](int, std::shared_ptr<Mqtt5::UnSubAckPacket>) { unsubscribeFinishedPromise.set_value(); }))
    {
        subscriberOne->PrintMessageAndExit("Unsubscribe failed", -1);
    }
    unsubscribeFinishedPromise.get_future().wait();
    fprintf(
        stdout,
        "[%s] Unsubscribed to topic '%s' in shared subscription group '%s'.\n",
        subscriberOne->name.c_str(),
        cmdData.input_topic.c_str(),
        cmdData.input_groupIdentifier.c_str());
    fprintf(stdout, "[%s] Full unsubscribed topic is: '%s'.\n", subscriberOne->name.c_str(), input_sharedTopic.c_str());

    unsubscribeFinishedPromise = std::promise<void>();
    if (!subscriberTwo->client->Unsubscribe(
            unsub, [&](int, std::shared_ptr<Mqtt5::UnSubAckPacket>) { unsubscribeFinishedPromise.set_value(); }))
    {
        subscriberTwo->PrintMessageAndExit("Unsubscribe failed", -1);
    }
    unsubscribeFinishedPromise.get_future().wait();
    fprintf(
        stdout,
        "[%s] Unsubscribed to topic '%s' in shared subscription group '%s'.\n",
        subscriberTwo->name.c_str(),
        cmdData.input_topic.c_str(),
        cmdData.input_groupIdentifier.c_str());
    fprintf(stdout, "[%s] Full unsubscribed topic is: '%s'.\n", subscriberTwo->name.c_str(), input_sharedTopic.c_str());

    /*********************** Disconnect all the MQTT5 clients ***************************/

    if (!publisher->client->Stop())
    {
        publisher->PrintMessageAndExit("Failed to disconnect. Exiting...", -1);
    }
    publisher->stoppedPromise.get_future().wait();
    fprintf(stdout, "[%s] Fully stopped.\n", publisher->name.c_str());

    if (!subscriberOne->client->Stop())
    {
        subscriberOne->PrintMessageAndExit("Failed to disconnect. Exiting...", -1);
    }
    subscriberOne->stoppedPromise.get_future().wait();
    fprintf(stdout, "[%s] Fully stopped.\n", subscriberOne->name.c_str());

    if (!subscriberTwo->client->Stop())
    {
        subscriberTwo->PrintMessageAndExit("Failed to disconnect. Exiting...", -1);
    }
    subscriberTwo->stoppedPromise.get_future().wait();
    fprintf(stdout, "[%s] Fully stopped.\n", subscriberTwo->name.c_str());

    /*********************** Free the shared pointers (MQTT5 clients) ***************************/
    publisher->client = nullptr;
    subscriberOne->client = nullptr;
    subscriberTwo->client = nullptr;
    publisher = nullptr;
    subscriberOne = nullptr;
    subscriberTwo = nullptr;

    fprintf(stdout, "Complete!\n");
    exit(0);
}
