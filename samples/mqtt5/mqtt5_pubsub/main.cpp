/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/UUID.h>
#include <aws/iot/Mqtt5Client.h>
#include <aws/crt/mqtt/Mqtt5Packets.h>

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
    String testTopic = "test/topic1";

    std::mutex receiveMutex;
    std::condition_variable receiveSignal;
    uint32_t receivedCount = 0;

    /*********************** Parse Arguments ***************************/
    Utils::CommandLineUtils cmdUtils = Utils::CommandLineUtils();
    cmdUtils.RegisterProgramName("mqtt5_pubsub");
    cmdUtils.AddCommonMQTTCommands();
    cmdUtils.RegisterCommand("key", "<path>", "Path to your key in PEM format.");
    cmdUtils.RegisterCommand("cert", "<path>", "Path to your client certificate in PEM format.");
    cmdUtils.AddCommonProxyCommands();
    cmdUtils.RegisterCommand("client_id", "<str>", "Client id to use (optional, default='test-*')");
    cmdUtils.AddLoggingCommands();
    const char **const_argv = (const char **)argv;
    cmdUtils.SendArguments(const_argv, const_argv + argc);
    cmdUtils.StartLoggingBasedOnCommand(&apiHandle);

    // Create a Client using Mqtt5ClientBuilder
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(
        cmdUtils.GetCommand("endpoint"), cmdUtils.GetCommand("cert").c_str(), cmdUtils.GetCommand("key").c_str());

    Aws::Iot::Mqtt5CustomAuthConfig authconfig;
    authconfig.WithAuthrizaerName("SdkTestingAuthorizerNoSigning2")
        .WithUsername("Derp")
        .WithPassword(ByteCursorFromCString("test"));

    // Setup connection options
    std::shared_ptr<Aws::Crt::Mqtt5::ConnectPacket> connectOptions = std::make_shared<Aws::Crt::Mqtt5::ConnectPacket>();
    // Get the client ID to send with the connection
    String clientId = cmdUtils.GetCommandOrDefault("client_id", String("test-") + Aws::Crt::UUID().ToString());
    connectOptions->withClientId(clientId);
    builder->withConnectOptions(connectOptions);
    builder->WithCustomAuthorizer(authconfig);

    std::promise<bool> connectionPromise;
    std::promise<void> stoppedPromise;
    std::promise<void> disconnectPromise;

    std::promise<bool> subscribeSuccess;

    // Setup lifecycle callbacks
    builder->withClientConnectionSuccessCallback([&connectionPromise](
                                                     Mqtt5::Mqtt5Client&,
                                                     std::shared_ptr<Aws::Crt::Mqtt5::ConnAckPacket>,
                                                     std::shared_ptr<Aws::Crt::Mqtt5::NegotiatedSettings> settings) {
        fprintf(stdout, "Mqtt5 Client connection succeed, clientid: %s.\n", settings->getClientId().c_str());
        connectionPromise.set_value(true);
    });

    builder->withClientConnectionFailureCallback(
        [&connectionPromise](Mqtt5::Mqtt5Client &, int error_code, std::shared_ptr<Aws::Crt::Mqtt5::ConnAckPacket>)
        {
            fprintf(stdout, "Mqtt5 Client connection failed with error: %s.\n", aws_error_debug_str(error_code));
            connectionPromise.set_value(false);
        });

    builder->withClientStoppedCallback(
        [&stoppedPromise](Mqtt5::Mqtt5Client &)
        {
        fprintf(stdout, "Mqtt5 Client stopped.\n");
        stoppedPromise.set_value();
    });

    builder->withClientAttemptingConnectCallback([](Mqtt5::Mqtt5Client&)
                                                 { fprintf(stdout, "Mqtt5 Client attempting connection...\n"); });

    builder->withClientDisconnectionCallback([&disconnectPromise](
                                                 Mqtt5::Mqtt5Client &,
                                                 int errorCode,
                                                 std::shared_ptr<Aws::Crt::Mqtt5::DisconnectPacket> packet_disconnect) {
        if (errorCode == 0)
        {
            fprintf(stdout, "Mqtt5 Client disconnected.\n");
        }
        else
        {
            fprintf(stdout, "Mqtt5 Client connection failed with error: %s.\n", aws_error_debug_str(errorCode));
        }
        disconnectPromise.set_value();
    });

    builder->withPublishReceivedCallback(
        [&receiveMutex, &receivedCount, &receiveSignal](
            std::shared_ptr<Mqtt5::Mqtt5Client> /*client*/, std::shared_ptr<Mqtt5::PublishPacket> publish) {
            if (publish == nullptr)
                return;

            std::lock_guard<std::mutex> lock(receiveMutex);
            ++receivedCount;
            fprintf(stdout, "Publish received on topic %s:", publish->getTopic().c_str());
            fwrite(publish->getPayload().ptr, 1, publish->getPayload().len, stdout);
            fprintf(stdout, "\n");

            for (Mqtt5::UserProperty prop : publish->getUserProperties())
            {
                fprintf(stdout, "\twith UserProperty:(%s,%s)\n", prop.getName().c_str(), prop.getValue().c_str());
            }
            receiveSignal.notify_all();
        });

    // Create Mqtt5Client
    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> client = builder->Build();

    if (client == nullptr)
    {
        fprintf(stdout, "Client creation failed.\n");
        return -1;
    }

    if (client->Start())
    {
        if (connectionPromise.get_future().get() == false)
        {
            return -1;
        }

        Mqtt5::Subscription sub1(testTopic, Mqtt5::QOS::AWS_MQTT5_QOS_AT_LEAST_ONCE);
        sub1.withNoLocal(false);
        std::shared_ptr<Mqtt5::SubscribePacket> subPacket = std::make_shared<Mqtt5::SubscribePacket>();
        subPacket->withSubscription(std::move(sub1));
        if (client->Subscribe(
                subPacket,
                [&subscribeSuccess](
                    std::shared_ptr<Mqtt5::Mqtt5Client>, int error_code, std::shared_ptr<Mqtt5::SubAckPacket>) {
                    if (error_code == 0)
                    {
                        subscribeSuccess.set_value(true);
                    }
                    else
                    {
                        subscribeSuccess.set_value(false);
                    }
                }))
        {
            if (subscribeSuccess.get_future().get() == true)
            {

                fprintf(stdout, "Subscription Success.\n");

                uint32_t publishedCount = 0;
                while (publishedCount < messageCount)
                {
                    String message = String("Mqtt5 Publish Message: ") + std::to_string(publishedCount).c_str();
                    ByteCursor payload = ByteCursorFromString(message);

                    std::shared_ptr<Mqtt5::PublishPacket> publish = std::make_shared<Mqtt5::PublishPacket>(
                        testTopic, payload, Mqtt5::QOS::AWS_MQTT5_QOS_AT_LEAST_ONCE);
                    if (client->Publish(publish))
                        ++publishedCount;

                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }

                std::unique_lock<std::mutex> receivedLock(receiveMutex);
                receiveSignal.wait(receivedLock, [&] { return receivedCount >= messageCount; });
            }
            else
            {
                fprintf(stdout, "Subscription Failed.\n");
            }
        }
        else
        {
            fprintf(stdout, "Subscription Failed.\n");
        }

        if (!client->Stop())
        {
            fprintf(stdout, "Failed to disconnect from the mqtt connection. Exiting..\n");
            return -1;
        }
        disconnectPromise.get_future().wait();
    }

    return 0;
}
