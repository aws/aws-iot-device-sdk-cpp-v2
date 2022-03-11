/**
* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
* SPDX-License-Identifier: Apache-2.0.
*/
#include <aws/crt/Api.h>
#include <aws/crt/StlAllocator.h>
#include <aws/crt/auth/Credentials.h>
#include <aws/crt/io/TlsOptions.h>

#include <aws/iot/MqttClient.h>

#include <aws/iotdevicecommon/IotDevice.h>
#include <aws/iotdevicedefender/DeviceDefender.h>

#include <algorithm>
#include <aws/crt/UUID.h>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>

#include <aws/crt/Api.h>

using namespace Aws::Crt;

static void s_printHelp()
{
   fprintf(stdout, "Usage:\n");
   fprintf(
       stdout,
       "basic-report --endpoint <endpoint> --cert <path to cert>"
       " --key <path to key> --ca_file <optional: path to custom ca>\n\n");
   fprintf(stdout, "endpoint: the endpoint of the mqtt server not including a port\n");
   fprintf(
       stdout,
       "cert: path to your client certificate in PEM format. If this is not set you must specify use_websocket\n");
   fprintf(stdout, "key: path to your key in PEM format. If this is not set you must specify use_websocket\n");
   fprintf(
       stdout,
       "ca_file: Optional, if the mqtt server uses a certificate that's not already"
       " in your trust store, set this.\n");
   fprintf(stdout, "\tIt's the path to a CA file in PEM format\n");
}

bool s_cmdOptionExists(char **begin, char **end, const String &option)
{
   return std::find(begin, end, option) != end;
}

char *s_getCmdOption(char **begin, char **end, const String &option)
{
   char **itr = std::find(begin, end, option);
   if (itr != end && ++itr != end)
   {
       return *itr;
   }
   return 0;
}

int main(int argc, char *argv[])
{

   /************************ Setup the Lib ****************************/
   /*
    * Do the global initialization for the API.
    */
   ApiHandle apiHandle;

   String endpoint;
   String certificatePath;
   String keyPath;
   String caFile;
   String topic("test/topic");
   String clientId(String("test-") + Aws::Crt::UUID().ToString());

   /*********************** Parse Arguments ***************************/
   if (!s_cmdOptionExists(argv, argv + argc, "--endpoint"))
   {
       s_printHelp();
       return 1;
   }

   endpoint = s_getCmdOption(argv, argv + argc, "--endpoint");

   if (s_cmdOptionExists(argv, argv + argc, "--key"))
   {
       keyPath = s_getCmdOption(argv, argv + argc, "--key");
   }

   if (s_cmdOptionExists(argv, argv + argc, "--cert"))
   {
       certificatePath = s_getCmdOption(argv, argv + argc, "--cert");
   }

   if (keyPath.empty() || certificatePath.empty())
   {
       fprintf(stdout, "Using mtls (cert and key) requires both the certificate and the private key\n");
       s_printHelp();
       return 1;
   }
   if (s_cmdOptionExists(argv, argv + argc, "--ca_file"))
   {
       caFile = s_getCmdOption(argv, argv + argc, "--ca_file");
   }

   /********************** Now Setup an Mqtt Client ******************/
   if (apiHandle.GetOrCreateStaticDefaultClientBootstrap()->LastError() != AWS_ERROR_SUCCESS)
   {
       fprintf(
           stderr,
           "ClientBootstrap failed with error %s\n",
           ErrorDebugString(apiHandle.GetOrCreateStaticDefaultClientBootstrap()->LastError()));
       exit(-1);
   }

   // ENABLE LOGGING
   //apiHandle.InitializeLogging(Aws::Crt::LogLevel::Trace, stdout);

   Aws::Crt::Io::TlsContext x509TlsCtx;
   Aws::Iot::MqttClientConnectionConfigBuilder builder;

   builder = Aws::Iot::MqttClientConnectionConfigBuilder(certificatePath.c_str(), keyPath.c_str());

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
   /*
    * Since no exceptions are used, always check the bool operator
    * when an error could have occurred.
    */
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
       fprintf(stderr, "MQTT Connection failed with error %s\n", ErrorDebugString(connection->LastError()));
       exit(-1);
   }

   if (connectionCompletedPromise.get_future().get())
   {
       std::mutex receiveMutex;
       std::condition_variable receiveSignal;
       uint32_t receivedCount = 0;

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

       // ======================================================================
       Aws::Crt::Allocator *allocator = Aws::Crt::DefaultAllocator();
       Aws::Crt::Io::EventLoopGroup *eventLoopGroup = Aws::Crt::ApiHandle::GetOrCreateStaticDefaultEventLoopGroup();
       const Aws::Crt::String thingName("TestThing");

       int result = -1;

       bool callbackSuccess = false;
       auto onCancelled = [&](void *a) -> void {
           auto *data = reinterpret_cast<bool *>(a);
           *data = true;
       };

       Aws::Iotdevicedefenderv1::ReportTaskBuilder taskBuilder(allocator, connection, *eventLoopGroup, thingName);
       taskBuilder.WithTaskPeriodSeconds((uint32_t)1UL)
           .WithNetworkConnectionSamplePeriodSeconds((uint32_t)1UL)
           .WithTaskCancelledHandler(onCancelled)
           .WithTaskCancellationUserData(&callbackSuccess);

       std::shared_ptr<Aws::Iotdevicedefenderv1::ReportTask> task = taskBuilder.Build();

       // ================
       // Add the custom metric
       aws_iotdevice_defender_get_number_fn *local_metric_func = [](int64_t *output, void* data) {
           *output = 10;
           fprintf(stdout, "\nGet custom metric number called!!!\n");
           return AWS_OP_SUCCESS;
       };
       task->RegisterCustomMetricNumber(aws_byte_cursor_from_c_str("CustomNumber_One"), local_metric_func);

       /*
       ASSERT_INT_EQUALS(
           AWS_OP_SUCCESS,
           task->RegisterCustomMetricNumber(aws_byte_cursor_from_c_str("CustomNumber_Two"), &global_metric_func));
        */
       // ================

       result = task->StartTask();
       fprintf(stdout, "\n\nDevice Defender Result: %d\n\n", result);
       if ((int)Aws::Iotdevicedefenderv1::ReportTaskStatus::Running == (int)task->GetStatus()) {
           fprintf(stdout, "\n\nDevice Defender Task is running\n\n");
       } else {
           fprintf(stdout, "\n\nDevice Defender Task is NOT running\n\n");
       }
       // ======================================================================

       /*
        * Subscribe for incoming publish messages on topic.
        */
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

           // FOR TESTING
           //task->StopTask();
       };

       connection->Subscribe(topic.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, onMessage, onSubAck);
       subscribeFinishedPromise.get_future().wait();

       uint32_t publishedCount = 0;
       while (publishedCount < 2)
       {
           String messagePayload = "test";
           ByteBuf payload = ByteBufFromArray((const uint8_t *)messagePayload.data(), messagePayload.length());

           auto onPublishComplete = [](Mqtt::MqttConnection &, uint16_t, int) {};
           connection->Publish(topic.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, false, payload, onPublishComplete);
           ++publishedCount;

           //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
           std::this_thread::sleep_for(std::chrono::milliseconds(2000));
       }

       {
           std::unique_lock<std::mutex> receivedLock(receiveMutex);
           receiveSignal.wait(receivedLock, [&] { return receivedCount >= 2; });
       }

       /*
        * Unsubscribe from the topic.
        */
       std::promise<void> unsubscribeFinishedPromise;
       connection->Unsubscribe(
           topic.c_str(), [&](Mqtt::MqttConnection &, uint16_t, int) { unsubscribeFinishedPromise.set_value(); });
       unsubscribeFinishedPromise.get_future().wait();

       // FOR TESTING
       task->StopTask();

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
