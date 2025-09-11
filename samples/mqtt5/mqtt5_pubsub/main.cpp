/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/UUID.h>
#include <aws/crt/mqtt/Mqtt5Packets.h>
#include <aws/iot/Mqtt5Client.h>

#include <algorithm>
#include <cstring>
#include <thread>

using namespace Aws::Crt;

/* --------------------------------- ARGUMENT PARSING ----------------------------------------- */
struct CmdArgs {
    String endpoint;
    String cert;
    String key;
    String clientId;
    String topic = "test/topic";
    String message = "Hello from mqtt5 sample";
    uint32_t count = 5;
};

void printHelp() {
    printf("MQTT5 X509 Sample (mTLS)\n");
    printf("options:\n");
    printf("  --help        show this help message and exit\n");
    printf("required arguments:\n");
    printf("  --endpoint    IoT endpoint hostname (default: None)\n");
    printf(
        "  --cert        Path to the certificate file to use during mTLS connection establishment (default: None)\n");
    printf(
        "  --key         Path to the private key file to use during mTLS connection establishment (default: None)\n");
    printf("optional arguments:\n");
    printf("  --client-id   Client ID (default: mqtt5-sample-<uuid>)\n");
    printf("  --ca_file     Path to optional CA bundle (PEM) (default: None)\n");
    printf("  --topic       Topic (default: test/topic)\n");
    printf("  --message     Message payload (default: Hello from mqtt5 sample)\n");
    printf("  --count       Messages to publish (0 = infinite) (default: 5)\n");
}

CmdArgs parseArgs(int argc, char *argv[]) {
    CmdArgs args;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            printHelp();
            exit(0);
        }
        else if (i < argc - 1) {
            if (strcmp(argv[i], "--endpoint") == 0) {
                args.endpoint = argv[++i];
            }
            else if (strcmp(argv[i], "--cert") == 0) {
                args.cert = argv[++i];
            }
            else if (strcmp(argv[i], "--key") == 0) {
                args.key = argv[++i];
            }
            else if (strcmp(argv[i], "--client_id") == 0) {
                args.clientId = argv[++i];
            }
            else if (strcmp(argv[i], "--topic") == 0) {
                args.topic = argv[++i];
            }
            else if (strcmp(argv[i], "--message") == 0) {
                args.message = argv[++i];
            }
            else if (strcmp(argv[i], "--count") == 0) {
                args.count = atoi(argv[++i]);
            }
            else {
                fprintf(stderr, "Unknown argument: %s\n", argv[i]);
                printHelp();
                exit(1);
            }
        }
    }
    if (args.endpoint.empty() || args.cert.empty() || args.key.empty()) {
        fprintf(stderr, "Error: --endpoint, --cert, and --key are required\n");
        printHelp();
        exit(1);
    }
    if (args.clientId.empty()) args.clientId = String("test-") + UUID().ToString();
    return args;
}
/* --------------------------------- ARGUMENT PARSING END ----------------------------------------- */

int main(int argc, char *argv[])
{
    // Parse command line arguments
    CmdArgs cmdData = parseArgs(argc, argv);

    /************************ Setup ****************************/

    // Do the global initialization for the API.
    ApiHandle apiHandle;

    // Variables needed for the sample
    std::mutex receiveMutex;
    std::condition_variable receiveSignal;
    uint32_t receivedCount = 0;

    // Create the MQTT5 builder and populate it with data from cmdData.
    auto builder = std::unique_ptr<Aws::Iot::Mqtt5ClientBuilder>(
        Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(
            cmdData.endpoint, cmdData.cert.c_str(), cmdData.key.c_str()));

    // Check if the builder setup correctly.
    if (builder == nullptr)
    {
        printf(
            "Failed to setup mqtt5 client builder with error code %d: %s", LastError(), ErrorDebugString(LastError()));
        return -1;
    }

    // Setup connection options
    std::shared_ptr<Mqtt5::ConnectPacket> connectOptions =
        Aws::Crt::MakeShared<Mqtt5::ConnectPacket>(Aws::Crt::DefaultAllocatorImplementation());
    connectOptions->WithClientId(cmdData.clientId);
    builder->WithConnectOptions(connectOptions);

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
                        fprintf(stdout, "MQTT5 Client Subscription failed with server error code: %d\n", reasonCode);
                        if (suback->getReasonString().has_value())
                        {
                            fprintf(stdout, "\tError reason string: %s\n", suback->getReasonString()->c_str());
                        }
                        subscribeSuccess.set_value(false);
                        return;
                    }
                }
            }
            subscribeSuccess.set_value(true);
        };

        Mqtt5::Subscription sub1(cmdData.topic, Mqtt5::QOS::AWS_MQTT5_QOS_AT_LEAST_ONCE);
        sub1.WithNoLocal(false);
        std::shared_ptr<Mqtt5::SubscribePacket> subPacket =
            Aws::Crt::MakeShared<Mqtt5::SubscribePacket>(Aws::Crt::DefaultAllocatorImplementation());
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
                            fprintf(stdout, "PubACK reason code: %d\n", puback->getReasonCode());
                            if (puback->getReasonString().has_value())
                            {
                                fprintf(stdout, "\nError reason string: %s\n", puback->getReasonString()->c_str());
                            }
                        }
                    }
                };

                uint32_t publishedCount = 0;
                while (publishedCount < cmdData.count)
                {
                    // Add \" to 'JSON-ify' the message
                    String message = "\"" + cmdData.message + std::to_string(publishedCount + 1).c_str() + "\"";
                    ByteCursor payload = ByteCursorFromString(message);

                    std::shared_ptr<Mqtt5::PublishPacket> publish = Aws::Crt::MakeShared<Mqtt5::PublishPacket>(
                        Aws::Crt::DefaultAllocatorImplementation(),
                        cmdData.topic,
                        payload,
                        Mqtt5::QOS::AWS_MQTT5_QOS_AT_LEAST_ONCE);
                    if (client->Publish(publish, onPublishComplete))
                    {
                        ++publishedCount;
                    }

                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }

                {
                    std::unique_lock<std::mutex> receivedLock(receiveMutex);
                    receiveSignal.wait(receivedLock, [&] { return receivedCount >= cmdData.count; });
                }

                // Unsubscribe from the topic.
                std::promise<void> unsubscribeFinishedPromise;
                std::shared_ptr<Mqtt5::UnsubscribePacket> unsub =
                    Aws::Crt::MakeShared<Mqtt5::UnsubscribePacket>(Aws::Crt::DefaultAllocatorImplementation());
                unsub->WithTopicFilter(cmdData.topic);
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
