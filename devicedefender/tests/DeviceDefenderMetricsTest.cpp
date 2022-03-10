/**
* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
* SPDX-License-Identifier: Apache-2.0.
*/
#include "aws/common/error.h"
#include <aws/crt/Api.h>

#include <aws/iotdevicecommon/IotDevice.h>
#include <aws/iotdevicedefender/DeviceDefender.h>
#include <aws/testing/aws_test_harness.h>
#include <utility>

static int s_TestDeviceDefenderCustomMetricSuccess(Aws::Crt::Allocator *allocator, void *ctx)
{
   (void)ctx;
   {
       Aws::Crt::ApiHandle apiHandle(allocator);
       Aws::Iotdevicecommon::DeviceApiHandle deviceApiHandle(allocator);
       Aws::Crt::Io::TlsContextOptions tlsCtxOptions = Aws::Crt::Io::TlsContextOptions::InitDefaultClient();
       Aws::Crt::Io::TlsContext tlsContext(tlsCtxOptions, Aws::Crt::Io::TlsMode::CLIENT, allocator);
       Aws::Crt::Io::SocketOptions socketOptions;
       socketOptions.SetConnectTimeoutMs(3000);
       Aws::Crt::Io::EventLoopGroup eventLoopGroup(0, allocator);
       Aws::Crt::Io::DefaultHostResolver defaultHostResolver(eventLoopGroup, 8, 30, allocator);
       Aws::Crt::Io::ClientBootstrap clientBootstrap(eventLoopGroup, defaultHostResolver, allocator);
       clientBootstrap.EnableBlockingShutdown();
       Aws::Crt::Mqtt::MqttClient mqttClient(clientBootstrap, allocator);
       Aws::Crt::Mqtt::MqttClient mqttClientMoved = std::move(mqttClient);

       auto mqttConnection = mqttClientMoved.NewConnection("www.example.com", 443, socketOptions, tlsContext);

       const Aws::Crt::String thingName("TestThing");
       bool callbackSuccess = false;

       std::mutex mutex;
       std::condition_variable cv;
       bool taskStopped = false;

       auto onCancelled = [&](void *a) -> void {
           auto *data = reinterpret_cast<bool *>(a);
           *data = true;
           taskStopped = true;
           cv.notify_one();
       };

       Aws::Iotdevicedefenderv1::ReportTaskBuilder taskBuilder(allocator, mqttConnection, eventLoopGroup, thingName);
       taskBuilder.WithTaskPeriodSeconds((uint32_t)1UL)
           .WithNetworkConnectionSamplePeriodSeconds((uint32_t)1UL)
           .WithTaskCancelledHandler(onCancelled)
           .WithTaskCancellationUserData(&callbackSuccess);

       std::shared_ptr<Aws::Iotdevicedefenderv1::ReportTask> task = taskBuilder.Build();

       // ================
       // Add the custom metric
       aws_iotdevice_defender_get_number_fn *metric_func = [](int64_t *output, void* data) {
           *output = 10;
           fprintf(stdout, "\nGet custom metric number called!!!\n");
           return AWS_OP_SUCCESS;
       };
       ASSERT_INT_EQUALS(
           AWS_OP_SUCCESS,
           task->RegisterCustomMetricNumber(aws_byte_cursor_from_c_str("CustomNumber"), metric_func));
        // ================

       ASSERT_INT_EQUALS((int)Aws::Iotdevicedefenderv1::ReportTaskStatus::Ready, (int)task->GetStatus());

       ASSERT_SUCCESS(task->StartTask());
       ASSERT_INT_EQUALS((int)Aws::Iotdevicedefenderv1::ReportTaskStatus::Running, (int)task->GetStatus());
       ASSERT_FAILS(task->StartTask());
       ASSERT_TRUE(aws_last_error() == AWS_ERROR_INVALID_STATE);
       task->StopTask();

       ASSERT_TRUE(task->GetStatus() == Aws::Iotdevicedefenderv1::ReportTaskStatus::Stopped);

       {
           std::unique_lock<std::mutex> lock(mutex);
           cv.wait(lock, [&]() { return taskStopped; });
       }

       ASSERT_TRUE(callbackSuccess);

       mqttConnection->Disconnect();
       ASSERT_TRUE(mqttConnection);

       ASSERT_FALSE(mqttClient);

       ASSERT_INT_EQUALS((int)Aws::Iotdevicedefenderv1::ReportTaskStatus::Stopped, (int)task->GetStatus());
   }

   return AWS_ERROR_SUCCESS;
}
AWS_TEST_CASE(DeviceDefenderCustomMetricSuccess, s_TestDeviceDefenderCustomMetricSuccess)
