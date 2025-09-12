/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/UUID.h>
#include <aws/crt/mqtt/Mqtt5Packets.h>
#include <aws/iot/Mqtt5Client.h>

#include <thread>

using namespace Aws::Crt;

/* --------------------------------- ARGUMENT PARSING ----------------------------------------- */
struct CmdArgs
{
    String endpoint;
    String cert;
    String key;
    String clientId;
    String caFile;
    String topic = "test/topic";
    String message = "Hello from mqtt5 sample";
    uint32_t count = 5;
};

void printHelp()
{
    printf("MQTT5 X509 Sample (mTLS)\n");
    printf("options:\n");
    printf("  --help        show this help message and exit\n");
    printf("required arguments:\n");
    printf("  --endpoint    IoT endpoint hostname\n");
    printf(
        "  --cert        Path to the certificate file to use during mTLS connection establishment (default: None)\n");
    printf(
        "  --key         Path to the private key file to use during mTLS connection establishment (default: None)\n");
    printf("optional arguments:\n");
    printf("  --client-id   Client ID (default: mqtt5-sample-<uuid>)\n");
    printf("  --ca_file     Path to optional CA bundle (PEM)\n");
    printf("  --topic       Topic (default: test/topic)\n");
    printf("  --message     Message payload (default: Hello from mqtt5 sample)\n");
    printf("  --count       Messages to publish (0 = infinite) (default: 5)\n");
}

CmdArgs parseArgs(int argc, char *argv[])
{
    CmdArgs args;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--help") == 0)
        {
            printHelp();
            exit(0);
        }
        else if (i < argc - 1)
        {
            if (strcmp(argv[i], "--endpoint") == 0)
            {
                args.endpoint = argv[++i];
            }
            else if (strcmp(argv[i], "--cert") == 0)
            {
                args.cert = argv[++i];
            }
            else if (strcmp(argv[i], "--key") == 0)
            {
                args.key = argv[++i];
            }
            else if (strcmp(argv[i], "--ca_file") == 0)
            {
                args.caFile = argv[++i];
            }
            else if (strcmp(argv[i], "--client_id") == 0)
            {
                args.clientId = argv[++i];
            }
            else if (strcmp(argv[i], "--topic") == 0)
            {
                args.topic = argv[++i];
            }
            else if (strcmp(argv[i], "--message") == 0)
            {
                args.message = argv[++i];
            }
            else if (strcmp(argv[i], "--count") == 0)
            {
                args.count = atoi(argv[++i]);
            }
            else
            {
                fprintf(stderr, "Unknown argument: %s\n", argv[i]);
                printHelp();
                exit(1);
            }
        }
    }
    if (args.endpoint.empty() || args.cert.empty() || args.key.empty())
    {
        fprintf(stderr, "Error: --endpoint, --cert, and --key are required\n");
        printHelp();
        exit(1);
    }
    if (args.clientId.empty())
        args.clientId = String("test-") + UUID().ToString();
    return args;
}
/* --------------------------------- ARGUMENT PARSING END ----------------------------------------- */

int main(int argc, char *argv[])
{
    // Parse command line arguments
    CmdArgs cmdData = parseArgs(argc, argv);

    // Variables needed for the sample
    std::mutex receiveMutex;
    std::condition_variable receiveSignal;
    uint32_t receivedCount = 0;
    std::promise<bool> connectionPromise;
    std::promise<void> stoppedPromise;
    std::promise<void> disconnectPromise;
    std::promise<bool> subscribeSuccess;
    std::promise<void> unsubscribeFinishedPromise;
    int exitCode = 0;

    /* Do the global initialization for the API. */
    ApiHandle apiHandle;

    /**
     * Create MQTT5 client builder using mutual TLS via X509 Certificate and Private Key,
     * The builder will be used to create the final client
     */
    auto builder = std::unique_ptr<Aws::Iot::Mqtt5ClientBuilder>(
        Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(
            cmdData.endpoint, cmdData.cert.c_str(), cmdData.key.c_str()));

    // Check if the builder setup correctly.
    if (builder == nullptr)
    {
        printf(
            "Failed to setup Mqtt5 client builder with error code %d: %s", LastError(), ErrorDebugString(LastError()));
        exit(1);
    }

    // Setup CA file if provided
    if (!cmdData.caFile.empty())
    {
        builder->WithCertificateAuthority(cmdData.caFile.c_str());
    }

    // Setup connection options
    std::shared_ptr<Mqtt5::ConnectPacket> connectOptions =
        Aws::Crt::MakeShared<Mqtt5::ConnectPacket>(Aws::Crt::DefaultAllocatorImplementation());
    connectOptions->WithClientId(cmdData.clientId);
    builder->WithConnectOptions(connectOptions);

    /* Setup lifecycle callbacks */
    // Callback when any publish is received
    builder->WithPublishReceivedCallback(
        [&receiveMutex, &receivedCount, &receiveSignal](const Mqtt5::PublishReceivedEventData &eventData)
        {
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

    // Callback for the lifecycle event the client Stopped
    builder->WithClientStoppedCallback(
        [&stoppedPromise](const Mqtt5::OnStoppedEventData &)
        {
            fprintf(stdout, "Mqtt5 Client stopped.\n");
            stoppedPromise.set_value();
        });

    // Callback for lifecycle event Attempting Connect
    builder->WithClientAttemptingConnectCallback([](const Mqtt5::OnAttemptingConnectEventData &)
                                                 { fprintf(stdout, "Mqtt5 Client attempting connection...\n"); });

    // Callback for the lifecycle event Connection Success
    builder->WithClientConnectionSuccessCallback(
        [&connectionPromise](const Mqtt5::OnConnectionSuccessEventData &eventData)
        {
            fprintf(
                stdout,
                "Mqtt5 Client connection succeed, clientid: %s.\n",
                eventData.negotiatedSettings->getClientId().c_str());
            connectionPromise.set_value(true);
        });

    // Callback for the lifecycle event Connection Failure
    builder->WithClientConnectionFailureCallback(
        [&connectionPromise](const Mqtt5::OnConnectionFailureEventData &eventData)
        {
            fprintf(
                stdout, "Mqtt5 Client connection failed with error: %s.\n", aws_error_debug_str(eventData.errorCode));
            connectionPromise.set_value(false);
        });

    // Callback for the lifecycle event Connection get disconnected
    builder->WithClientDisconnectionCallback(
        [&disconnectPromise](const Mqtt5::OnDisconnectionEventData &eventData)
        {
            fprintf(stdout, "Mqtt5 Client disconnection with reason: %s.\n", aws_error_debug_str(eventData.errorCode));
            disconnectPromise.set_value();
        });

    /* Create Mqtt5Client from the builder */
    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> client = builder->Build();

    if (client == nullptr)
    {
        fprintf(
            stdout, "Failed to init Mqtt5Client with error code %d: %s", LastError(), ErrorDebugString(LastError()));
        exit(1);
    }

    /**
     * Start the client, instructing the client to desire a connected state. The client will try to
     * establish a connection with the provided settings. If the client is disconnected while in this
     * state it will attempt to reconnect automatically.
     */
    fprintf(stdout, "=== Mqtt5 Client starting === \n");
    if (!client->Start())
    {
        fprintf(
            stdout, "Failed to start Mqtt5Client with error code %d: %s", LastError(), ErrorDebugString(LastError()));
        exit(1);
    }

    // We await the `ClientConnectionSuccessCallback` callback to be invoked.
    if (connectionPromise.get_future().get())
    {

        /**
         * Subscribe to test topic
         */
        // Setup the callback that will be triggered on receiveing SUBACK from the server
        fprintf(stdout, "=== Mqtt5 Client subscribing to topic %s === \n", cmdData.topic.c_str());
        auto onSubAck = [&subscribeSuccess](int error_code, std::shared_ptr<Mqtt5::SubAckPacket> suback)
        {
            if (error_code != 0)
            {
                fprintf(
                    stdout,
                    "Mqtt5 Client Subscription failed with error code: (%d)%s\n",
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
                        fprintf(stdout, "Mqtt5 Client Subscription failed with server error code: %d\n", reasonCode);
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

        // Create a subscription object, and add it to a subscribe packet
        Mqtt5::Subscription sub1(cmdData.topic, Mqtt5::QOS::AWS_MQTT5_QOS_AT_LEAST_ONCE);
        sub1.WithNoLocal(false);
        std::shared_ptr<Mqtt5::SubscribePacket> subPacket =
            Aws::Crt::MakeShared<Mqtt5::SubscribePacket>(Aws::Crt::DefaultAllocatorImplementation());
        subPacket->WithSubscription(std::move(sub1));

        // Subscribe & wait for the subscription to complete
        if (client->Subscribe(subPacket, onSubAck) && 
            subscribeSuccess.get_future().get())
        {

            /**
             * Publish to the topics
             */

            // Setup publish completion callback. The callback will get triggered when the publish completes (when
            // the client received the PubAck from the server).
            auto onPublishComplete = [](int, std::shared_ptr<Aws::Crt::Mqtt5::PublishResult> result)
            {
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
                        fprintf(stdout, "PubAck reason code: %d\n", puback->getReasonCode());
                        if (puback->getReasonString().has_value())
                        {
                            fprintf(stdout, "\nError reason string: %s\n", puback->getReasonString()->c_str());
                        }
                    }
                }
            };

            if (cmdData.count == 0)
            {
                fprintf(stdout, "=== Mqtt5 Client sending messages until program killed ===\n");
            }
            else
            {
                fprintf(stdout, "=== Mqtt5 Client sending %d message(s) ===\n", cmdData.count);
            }

            uint32_t publishedCount = 0;
            while (publishedCount < cmdData.count || cmdData.count == 0)
            {
                // Add \" to 'JSON-ify' the message
                String message = "\"" + cmdData.message + std::to_string(publishedCount + 1).c_str() + "\"";
                ByteCursor payload = ByteCursorFromString(message);

                // Create a publish packet
                std::shared_ptr<Mqtt5::PublishPacket> publish = Aws::Crt::MakeShared<Mqtt5::PublishPacket>(
                    Aws::Crt::DefaultAllocatorImplementation(),
                    cmdData.topic,
                    payload,
                    Mqtt5::QOS::AWS_MQTT5_QOS_AT_LEAST_ONCE);
                // Publish
                if (client->Publish(publish, onPublishComplete))
                {
                    ++publishedCount;
                }

                // Sleep between publishes to avoid flooding the server
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }

            // Wait to receive all the messages we sent.
            {
                std::unique_lock<std::mutex> receivedLock(receiveMutex);
                receiveSignal.wait(receivedLock, [&] { return receivedCount >= cmdData.count; });
            }

            /**
             * Unsubscribe from the topic.
             */
            fprintf(stdout, "=== Mqtt5 Client unsubscribing from topic %s === \n", cmdData.topic.c_str());
            // Setup the callback that will be triggered on receiveing UNSUBACK from the server
            auto onUnSubAck = [&unsubscribeFinishedPromise](int /*error_code*/, std::shared_ptr<Mqtt5::UnSubAckPacket> /*suback*/)
            { unsubscribeFinishedPromise.set_value(); };

            // Create an unsubscribe packet
            std::shared_ptr<Mqtt5::UnsubscribePacket> unsub =
                Aws::Crt::MakeShared<Mqtt5::UnsubscribePacket>(Aws::Crt::DefaultAllocatorImplementation());
            unsub->WithTopicFilter(cmdData.topic);

            // Unsubscribe
            if (client->Unsubscribe(unsub, onUnSubAck)){
                // Wait for unsubscription to finish
                unsubscribeFinishedPromise.get_future().wait();
            } else{
                fprintf(stdout, "Mqtt5 Client failed to unsubscribe. Exiting...\n");
                exitCode = 1;
            }
        }else{
            fprintf(stdout, "Mqtt5 Client failed to subscribe. Exiting...\n");
            exitCode = 1;
        }

        fprintf(stdout, "=== Mqtt5 Client stopping the client === \n");
        // Disconnect
        if (!client->Stop())
        {
            fprintf(stdout, "Failed to disconnect from the Mqtt connection. Exiting...\n");
            exit(1);
        }
        stoppedPromise.get_future().wait();

    }else{
        fprintf(stdout, "Mqtt5 Client connection failed.");
        exitCode = 1;
    }
    
    exit(exitCode);
}
