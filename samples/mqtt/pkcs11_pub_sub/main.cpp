/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/UUID.h>
#include <aws/crt/io/Pkcs11.h>
#include <aws/iot/MqttClient.h>

#include "../../utils/CommandLineUtils.h"

using namespace Aws::Crt;

int main(int argc, char *argv[])
{

    /************************ Setup the Lib ****************************/
    /*
     * Do the global initialization for the API.
     */
    ApiHandle apiHandle;

    // apiHandle.InitializeLogging(LogLevel::Error, stderr);

    /*********************** Parse Arguments ***************************/
    Utils::CommandLineUtils cmdUtils = Utils::CommandLineUtils();
    cmdUtils.RegisterProgramName("pkcs11_pub_sub");
    cmdUtils.AddCommonMQTTCommands();
    cmdUtils.AddCommonTopicMessageCommands();
    cmdUtils.RemoveCommand("key");
    cmdUtils.RegisterCommand("pkcs11_lib", "<path>", "Path to PKCS#11 library.");
    cmdUtils.RegisterCommand("pin", "<str>", "User PIN for logging into PKCS#11 token.");
    cmdUtils.RegisterCommand("token_label", "<str>", "Label of the PKCS#11 token to use (optional).");
    cmdUtils.RegisterCommand("slot_id", "<int>", "Slot ID containing PKCS#11 token to use (optional).");
    cmdUtils.RegisterCommand("key_label", "<str>", "Label of private key on the PKCS#11 token (optional).");
    cmdUtils.RegisterCommand("count", "<int>", "Number of messages to publish. (optional, default=10).");
    cmdUtils.RegisterCommand("client_id", "<str>", "Client id to use (optional, default='test-*').");
    cmdUtils.RegisterCommand("help", "", "Prints this message");
    const char **const_argv = (const char **)argv;
    cmdUtils.SendArguments(const_argv, const_argv + argc);

    if (cmdUtils.HasCommand("help"))
    {
        cmdUtils.PrintHelp();
        exit(-1);
    }
    String endpoint = cmdUtils.GetCommandRequired("endpoint");
    String certificatePath = cmdUtils.GetCommandRequired("cert");
    String pkcs11LibPath = cmdUtils.GetCommandRequired("pkcs11_lib");
    String pkcs11UserPin = cmdUtils.GetCommandRequired("pin");
    String pkcs11TokenLabel = cmdUtils.GetCommandOrDefault("token_label", "");
    String pkcs11SlotIdStr = cmdUtils.GetCommandOrDefault("slot_id", "");
    String pkcs11KeyLabel = cmdUtils.GetCommandOrDefault("key_label", "");
    String topic = cmdUtils.GetCommandOrDefault("topic", "test/topic");
    String messagePayload = cmdUtils.GetCommandOrDefault("message", "Hello world!");
    int messageCount = std::stoi(cmdUtils.GetCommandOrDefault("count", "10").c_str());
    String caFile = cmdUtils.GetCommandOrDefault("ca_file", "");
    String clientId = cmdUtils.GetCommandOrDefault("client_id", String("test-") + Aws::Crt::UUID().ToString());

    /********************** Now Setup an Mqtt Client ******************/
    if (apiHandle.GetOrCreateStaticDefaultClientBootstrap()->LastError() != AWS_ERROR_SUCCESS)
    {
        fprintf(
            stderr,
            "ClientBootstrap failed with error %s\n",
            ErrorDebugString(apiHandle.GetOrCreateStaticDefaultClientBootstrap()->LastError()));
        exit(-1);
    }

    std::shared_ptr<Io::Pkcs11Lib> pkcs11Lib = Io::Pkcs11Lib::Create(pkcs11LibPath);
    if (!pkcs11Lib)
    {
        fprintf(stderr, "Pkcs11Lib failed: %s\n", ErrorDebugString(Aws::Crt::LastError()));
        exit(-1);
    }

    Io::TlsContextPkcs11Options pkcs11Options(pkcs11Lib);
    pkcs11Options.SetCertificateFilePath(certificatePath);
    pkcs11Options.SetUserPin(pkcs11UserPin);

    if (!pkcs11TokenLabel.empty())
    {
        pkcs11Options.SetTokenLabel(pkcs11TokenLabel);
    }

    if (!pkcs11SlotIdStr.empty())
    {
        uint64_t slotId = std::stoull(pkcs11SlotIdStr.c_str());
        pkcs11Options.SetSlotId(slotId);
    }

    if (!pkcs11KeyLabel.empty())
    {
        pkcs11Options.SetPrivateKeyObjectLabel(pkcs11KeyLabel);
    }

    /*
     * Note that that remainder of this code is identical to the other "MQTT pub sub" samples.
     */

    Aws::Iot::MqttClientConnectionConfigBuilder builder(pkcs11Options);
    if (!builder)
    {
        fprintf(stderr, "MqttClientConnectionConfigBuilder failed: %s\n", ErrorDebugString(Aws::Crt::LastError()));
        exit(-1);
    }

    if (!caFile.empty())
    {
        builder.WithCertificateAuthority(caFile.c_str());
    }

    builder.WithEndpoint(endpoint);

    auto clientConfig = builder.Build();
    if (!clientConfig)
    {
        fprintf(
            stderr,
            "Client Configuration initialization failed with error %s\n",
            ErrorDebugString(clientConfig.LastError()));
        exit(-1);
    }

    Aws::Iot::MqttClient mqttClient;
    if (!mqttClient)
    {
        fprintf(stderr, "MQTT Client Creation failed with error %s\n", ErrorDebugString(mqttClient.LastError()));
        exit(-1);
    }

    /*
     * Now create a connection object. Note: This type is move only
     * and its underlying memory is managed by the client.
     */
    auto connection = mqttClient.NewConnection(clientConfig);

    if (!connection)
    {
        fprintf(stderr, "MQTT Connection Creation failed with error %s\n", ErrorDebugString(mqttClient.LastError()));
        exit(-1);
    }

    /*
     * In a real world application you probably don't want to enforce synchronous behavior
     * but this is a sample console application, so we'll just do that with a condition variable.
     */
    std::promise<bool> connectionCompletedPromise;
    std::promise<void> connectionClosedPromise;

    /*
     * This will execute when an mqtt connect has completed or failed.
     */
    auto onConnectionCompleted = [&](Mqtt::MqttConnection &, int errorCode, Mqtt::ReturnCode returnCode, bool) {
        if (errorCode)
        {
            fprintf(stdout, "Connection failed with error %s\n", ErrorDebugString(errorCode));
            connectionCompletedPromise.set_value(false);
        }
        else
        {
            if (returnCode != AWS_MQTT_CONNECT_ACCEPTED)
            {
                fprintf(stdout, "Connection failed with mqtt return code %d\n", (int)returnCode);
                connectionCompletedPromise.set_value(false);
            }
            else
            {
                fprintf(stdout, "Connection completed successfully.");
                connectionCompletedPromise.set_value(true);
            }
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

    /*
     * Actually perform the connect dance.
     */
    fprintf(stdout, "Connecting...\n");
    if (!connection->Connect(clientId.c_str(), false /*cleanSession*/, 1000 /*keepAliveTimeSecs*/))
    {
        fprintf(stderr, "Failed to kick off MQTT async Connect(): %s\n", ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    // wait for the OnConnectionCompleted callback to fire, which sets connectionCompletedPromise...
    if (connectionCompletedPromise.get_future().get() == false)
    {
        fprintf(stderr, "Connection failed\n");
        exit(-1);
    }

    std::mutex receiveMutex;
    std::condition_variable receiveSignal;
    int receivedCount = 0;

    /*
     * This is invoked upon the receipt of a Publish on a subscribed topic.
     */
    auto onMessage = [&](Mqtt::MqttConnection &,
                         const String &topic,
                         const ByteBuf &byteBuf,
                         bool /*dup*/,
                         Mqtt::QOS /*qos*/,
                         bool /*retain*/) {
        {
            std::lock_guard<std::mutex> lock(receiveMutex);
            ++receivedCount;
            fprintf(stdout, "Publish #%d received on topic %s\n", receivedCount, topic.c_str());
            fprintf(stdout, "Message: ");
            fwrite(byteBuf.buffer, 1, byteBuf.len, stdout);
            fprintf(stdout, "\n");
        }

        receiveSignal.notify_all();
    };

    /*
     * Subscribe for incoming publish messages on topic.
     */
    std::promise<void> subscribeFinishedPromise;
    auto onSubAck = [&](Mqtt::MqttConnection &, uint16_t packetId, const String &topic, Mqtt::QOS QoS, int errorCode) {
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

    connection->Subscribe(topic.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, onMessage, onSubAck);
    subscribeFinishedPromise.get_future().wait();

    int publishedCount = 0;
    while (publishedCount < messageCount)
    {
        ByteBuf payload = ByteBufFromArray((const uint8_t *)messagePayload.data(), messagePayload.length());

        auto onPublishComplete = [](Mqtt::MqttConnection &, uint16_t, int) {};
        connection->Publish(topic.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, false, payload, onPublishComplete);
        ++publishedCount;

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
    connection->Unsubscribe(
        topic.c_str(), [&](Mqtt::MqttConnection &, uint16_t, int) { unsubscribeFinishedPromise.set_value(); });
    unsubscribeFinishedPromise.get_future().wait();

    /* Disconnect */
    if (connection->Disconnect())
    {
        connectionClosedPromise.get_future().wait();
    }

    return 0;
}
