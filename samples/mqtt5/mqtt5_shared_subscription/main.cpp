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
 * MQTT5 support is currently in <b>developer preview</b>.  We encourage feedback at all times, but feedback during the
 * preview window is especially valuable in shaping the final product.  During the preview period we may make
 * backwards-incompatible changes to the public API, but in general, this is something we will try our best to avoid.
 */


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
    std::condition_variable receiveSignal;
    uint32_t receivedMessages;
    uint32_t expectedMessages;
    bool sharedSubscriptionSupportNotAvailable;

    /**
     * A helper function to print a message and then exit the sample.
     */
    void PrintMessageAndExit(String message, int exitCode)
    {
        fprintf(stdout, "[%s]: %s\n", this->name.c_str(), message.c_str());
        exit(exitCode);
    }

    /**
     * Creates a MQTT5 client using direct MQTT5 via mTLS with the passed input data.
     */
    static std::shared_ptr<sample_mqtt5_client> create_mqtt5_client(
        String input_endpoint,
        String input_cert,
        String input_key,
        String input_ca,
        String input_clientId,
        uint32_t input_count,
        String input_clientName)
    {
        std::shared_ptr<sample_mqtt5_client> result = std::make_shared<sample_mqtt5_client>();

        result->receivedMessages = 0;
        result->expectedMessages = input_count;
        result->name = input_clientName;
        result->sharedSubscriptionSupportNotAvailable = false;

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
        connectOptions->withClientId(input_clientId);
        builder->withConnectOptions(connectOptions);

        builder->withClientConnectionSuccessCallback(
            [result](Mqtt5::Mqtt5Client &, const Mqtt5::OnConnectionSuccessEventData &eventData) {
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

        builder->withClientConnectionFailureCallback(
            [result](Mqtt5::Mqtt5Client &, const Mqtt5::OnConnectionFailureEventData &eventData) {
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

        builder->withClientStoppedCallback([result](Mqtt5::Mqtt5Client &, const Mqtt5::OnStoppedEventData &) {
            fprintf(stdout, "[%s]: Stopped.\n", result->name.c_str());
            result->stoppedPromise.set_value();
        });

        builder->withClientDisconnectionCallback(
            [result](Mqtt5::Mqtt5Client &, const Mqtt5::OnDisconnectionEventData &eventData) {
                fprintf(
                    stdout,
                    "[%s]: Disconnection with reason: %s.\n",
                    result->name.c_str(),
                    aws_error_debug_str(eventData.errorCode));
                if (eventData.disconnectPacket != nullptr)
                {
                    fprintf(stdout, "\tReason code: %u\n", (uint32_t)eventData.disconnectPacket->getReasonCode());
                    if (eventData.disconnectPacket->getReasonCode() ==
                        Mqtt5::DisconnectReasonCode::AWS_MQTT5_DRC_SHARED_SUBSCRIPTIONS_NOT_SUPPORTED)
                    {
                        result->sharedSubscriptionSupportNotAvailable = true;
                    }
                }
            });

        builder->withPublishReceivedCallback([result](
                                                 Mqtt5::Mqtt5Client & /*client*/,
                                                 const Mqtt5::PublishReceivedEventData &eventData) {
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
            result->receivedMessages += 1;
            if (result->receivedMessages > result->expectedMessages)
            {
                result->receiveSignal.notify_all();
            }
        });

        result->client = builder->Build();
        delete builder;
        return result;
    }
};

int main(int argc, char *argv[])
{
    /************************ Setup the Lib ****************************/
    /*
     * Do the global initialization for the API.
     */
    ApiHandle apiHandle;

    /*********************** Setup Arguments ***************************/
    Utils::CommandLineUtils cmdUtils = Utils::CommandLineUtils();
    cmdUtils.RegisterProgramName("mqtt5_shared_subscription");
    cmdUtils.AddCommonMQTTCommands();
    cmdUtils.RegisterCommand("key", "<path>", "Path to your key in PEM format.");
    cmdUtils.RegisterCommand("cert", "<path>", "Path to your client certificate in PEM format.");
    cmdUtils.AddCommonProxyCommands();
    cmdUtils.AddCommonTopicMessageCommands();
    cmdUtils.RegisterCommand("client_id", "<str>", "Client id to use (optional, default='test-*')");
    cmdUtils.RegisterCommand("count", "<int>", "The number of messages to send (optional, default='10')");
    cmdUtils.RegisterCommand(
        "group_identifier",
        "<str>",
        "The group identifier to use in the shared subscription (optional, default='cpp-sample')");
    cmdUtils.RegisterCommand(
        "is_ci", "<str>", "If present the sample will run in CI mode (will publish to shadow automatically).");
    cmdUtils.AddLoggingCommands();
    const char **const_argv = (const char **)argv;
    cmdUtils.SendArguments(const_argv, const_argv + argc);
    cmdUtils.StartLoggingBasedOnCommand(&apiHandle);

    /*********************** Pull data from arguments ***************************/

    String input_endpoint = cmdUtils.GetCommandRequired("endpoint");
    String input_cert = cmdUtils.GetCommandRequired("cert");
    String input_key = cmdUtils.GetCommandRequired("key");
    String input_ca = cmdUtils.GetCommandOrDefault("ca_file", "");
    String input_clientId = cmdUtils.GetCommandOrDefault("client_id", String("test-") + Aws::Crt::UUID().ToString());
    uint32_t input_count = 10;
    String input_topic = cmdUtils.GetCommandOrDefault("topic", "test/topic");
    String input_message = cmdUtils.GetCommandOrDefault("message", "Hello World! ");
    String input_groupIdentifier = cmdUtils.GetCommandOrDefault("group_identifier", "cpp-sample");
    bool input_isCI = cmdUtils.HasCommand("is_ci");

    if (cmdUtils.HasCommand("count"))
    {
        int count = atoi(cmdUtils.GetCommand("count").c_str());
        if (count > 0)
        {
            input_count = count;
        }
    }

    // If this is CI, append a UUID to the topic
    if (input_isCI)
    {
        input_topic = input_topic + Aws::Crt::UUID().ToString();
    }

    // Construct the shared topic
    String input_sharedTopic = String("$share/") + input_groupIdentifier + String("/") + input_topic;

    // Make sure the message count is even
    if (input_count % 2 != 0)
    {
        fprintf(stdout, "'--count' is an odd number. '--count' must be even or zero for this sample.");
        exit(-1);
    }

    /*********************** Create the MQTT5 clients: one publisher and two subscribers ***************************/

    std::shared_ptr<sample_mqtt5_client> publisher = sample_mqtt5_client::create_mqtt5_client(
        input_endpoint,
        input_cert,
        input_key,
        input_ca,
        input_clientId + String("1"),
        input_count / 2,
        String("Publisher"));
    std::shared_ptr<sample_mqtt5_client> subscriberOne = sample_mqtt5_client::create_mqtt5_client(
        input_endpoint,
        input_cert,
        input_key,
        input_ca,
        input_clientId + String("2"),
        input_count / 2,
        String("Subscriber One"));
    std::shared_ptr<sample_mqtt5_client> subscriberTwo = sample_mqtt5_client::create_mqtt5_client(
        input_endpoint,
        input_cert,
        input_key,
        input_ca,
        input_clientId + String("3"),
        input_count / 2,
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
    }
    else
    {
        publisher->PrintMessageAndExit("Could not start", -1);
        exit(-1);
    }

    if (subscriberOne->client->Start())
    {
        if (subscriberOne->connectionPromise.get_future().get() == false)
        {
            subscriberOne->PrintMessageAndExit("Connection was unsuccessful", -1);
        }
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
    }
    else
    {
        subscriberTwo->PrintMessageAndExit("Could not start", -1);
    }

    /*********************** Subscribe the two subscriber MQTT5 clients ***************************/

    std::promise<Mqtt5::SubAckReasonCode> subscribeSuccess;
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
    sub1.withNoLocal(false);
    std::shared_ptr<Mqtt5::SubscribePacket> subPacket = std::make_shared<Mqtt5::SubscribePacket>();
    subPacket->withSubscription(std::move(sub1));

    if (subscriberOne->client->Subscribe(subPacket, onSubAck))
    {
        // Wait just a little bit to see if the client was disconnected due to missing Shared Subscription support.
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        if (subscriberOne->sharedSubscriptionSupportNotAvailable == true)
        {
            if (input_isCI == true)
            {
                // TMP: If this fails subscribing in CI, just exit the sample gracefully
                subscriberOne->PrintMessageAndExit("Shared Subscriptions not supported", 0);
            }
            subscriberOne->PrintMessageAndExit("Shared Subscriptions not supported", -1);
        }
        Mqtt5::SubAckReasonCode result = subscribeSuccess.get_future().get();
        if (result >= Mqtt5::SubAckReasonCode::AWS_MQTT5_SARC_UNSPECIFIED_ERROR)
        {
            subscriberOne->PrintMessageAndExit("Failed to subscribe", -1);
        }
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
        if (subscriberTwo->sharedSubscriptionSupportNotAvailable == true)
        {
            if (input_isCI == true)
            {
                // TMP: If this fails subscribing in CI, just exit the sample gracefully
                subscriberTwo->PrintMessageAndExit("Shared Subscriptions not supported", 0);
            }
            subscriberTwo->PrintMessageAndExit("Shared Subscriptions not supported", -1);
        }
        Mqtt5::SubAckReasonCode result = subscribeSuccess.get_future().get();
        if (result >= Mqtt5::SubAckReasonCode::AWS_MQTT5_SARC_UNSPECIFIED_ERROR)
        {
            subscriberTwo->PrintMessageAndExit("Failed to subscribe", -1);
        }
    }
    else
    {
        subscriberTwo->PrintMessageAndExit("Failed to subscribe", -1);
    }

    /*********************** Publish ***************************/

    auto onPublishComplete = [publisher](
                                 std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> client,
                                 int,
                                 std::shared_ptr<Aws::Crt::Mqtt5::PublishResult> result) {
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

    uint32_t publishedCount = 0;
    while (publishedCount < input_count)
    {
        String message = input_message + std::to_string(publishedCount + 1).c_str();
        ByteCursor payload = ByteCursorFromString(message);
        std::shared_ptr<Mqtt5::PublishPacket> publish =
            std::make_shared<Mqtt5::PublishPacket>(input_topic, payload, Mqtt5::QOS::AWS_MQTT5_QOS_AT_LEAST_ONCE);
        if (publisher->client->Publish(publish, onPublishComplete))
        {
            ++publishedCount;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    /*********************** Make sure all the messages got to the subscribers ***************************/

    std::unique_lock<std::mutex> receivedLockOne(subscriberOne->receiveMutex);
    subscriberOne->receiveSignal.wait(receivedLockOne, [&, subscriberOne] {
        return subscriberOne->receivedMessages >= subscriberOne->expectedMessages;
    });
    std::unique_lock<std::mutex> receivedLockTwo(subscriberTwo->receiveMutex);
    subscriberTwo->receiveSignal.wait(receivedLockTwo, [&, subscriberTwo] {
        return subscriberTwo->receivedMessages >= subscriberTwo->expectedMessages;
    });

    /*********************** Unsubscribe the subscribers ***************************/

    std::promise<void> unsubscribeFinishedPromise;
    std::shared_ptr<Mqtt5::UnsubscribePacket> unsub = std::make_shared<Mqtt5::UnsubscribePacket>();
    unsub->withTopicFilter(input_sharedTopic);
    if (!subscriberOne->client->Unsubscribe(
            unsub, [&](std::shared_ptr<Mqtt5::Mqtt5Client>, int, std::shared_ptr<Mqtt5::UnSubAckPacket>) {
                unsubscribeFinishedPromise.set_value();
            }))
    {
        subscriberOne->PrintMessageAndExit("Unsubscribe failed", -1);
    }
    unsubscribeFinishedPromise.get_future().wait();

    unsubscribeFinishedPromise = std::promise<void>();
    if (!subscriberTwo->client->Unsubscribe(
            unsub, [&](std::shared_ptr<Mqtt5::Mqtt5Client>, int, std::shared_ptr<Mqtt5::UnSubAckPacket>) {
                unsubscribeFinishedPromise.set_value();
            }))
    {
        subscriberTwo->PrintMessageAndExit("Unsubscribe failed", -1);
    }
    unsubscribeFinishedPromise.get_future().wait();

    /*********************** Disconnect all the MQTT5 clients ***************************/

    if (!publisher->client->Stop())
    {
        publisher->PrintMessageAndExit("Failed to disconnect. Exiting...", -1);
    }
    publisher->stoppedPromise.get_future().wait();

    if (!subscriberOne->client->Stop())
    {
        subscriberOne->PrintMessageAndExit("Failed to disconnect. Exiting...", -1);
    }
    subscriberOne->stoppedPromise.get_future().wait();

    if (!subscriberTwo->client->Stop())
    {
        subscriberTwo->PrintMessageAndExit("Failed to disconnect. Exiting...", -1);
    }
    subscriberTwo->stoppedPromise.get_future().wait();

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
