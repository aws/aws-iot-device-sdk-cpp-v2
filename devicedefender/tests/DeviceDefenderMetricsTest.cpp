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

int global_metric_number_func(double *output, void *data)
{
    *output = 1.2;
    return AWS_OP_SUCCESS;
};

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
        // Add the custom metrics
        aws_iotdevice_defender_get_number_fn *local_metric_number_func = [](double *output, void *data) {
            *output = 10;
            return AWS_OP_SUCCESS;
        };
        task->RegisterCustomMetricNumber(aws_byte_cursor_from_c_str("CustomNumberOne"), local_metric_number_func);
        task->RegisterCustomMetricNumber(aws_byte_cursor_from_c_str("CustomNumberTwo"), &global_metric_number_func);

        aws_iotdevice_defender_get_number_list_fn *local_metric_number_list_func = [](aws_array_list *output, void *data) {
            double list_num_01 = 101;
            double list_num_02 = 102;
            double list_num_03 = 103;
            aws_array_list_push_back(output, &list_num_01);
            aws_array_list_push_back(output, &list_num_02);
            aws_array_list_push_back(output, &list_num_03);
            return AWS_OP_SUCCESS;
        };
        task->RegisterCustomMetricNumberList(aws_byte_cursor_from_c_str("CustomNumberList"), local_metric_number_list_func);

        aws_iotdevice_defender_get_string_list_fn *local_metric_str_list_func = [](aws_array_list *output, void *data) {
            aws_string *list_str_01 = aws_string_new_from_c_str(Aws::Crt::DefaultAllocator(), "One Fish");
            aws_array_list_push_back(output, &list_str_01);
            aws_string *list_str_02 = aws_string_new_from_c_str(Aws::Crt::DefaultAllocator(), "Two Fish");
            aws_array_list_push_back(output, &list_str_02);
            aws_string *list_str_03 = aws_string_new_from_c_str(Aws::Crt::DefaultAllocator(), "Red Fish");
            aws_array_list_push_back(output, &list_str_03);
            aws_string *list_str_04 = aws_string_new_from_c_str(Aws::Crt::DefaultAllocator(), "Blue Fish");
            aws_array_list_push_back(output, &list_str_04);
            return AWS_OP_SUCCESS;
        };
        task->RegisterCustomMetricStringList(aws_byte_cursor_from_c_str("CustomStringList"), local_metric_str_list_func);

        aws_iotdevice_defender_get_string_list_fn *local_metric_ip_list_func = [](aws_array_list *output, void *data) {
            aws_string *list_str_01 = aws_string_new_from_c_str(Aws::Crt::DefaultAllocator(), "192.0.2.0");
            aws_array_list_push_back(output, &list_str_01);
            aws_string *list_str_02 = aws_string_new_from_c_str(Aws::Crt::DefaultAllocator(), "198.51.100.0");
            aws_array_list_push_back(output, &list_str_02);
            aws_string *list_str_03 = aws_string_new_from_c_str(Aws::Crt::DefaultAllocator(), "203.0.113.0");
            aws_array_list_push_back(output, &list_str_03);
            aws_string *list_str_04 = aws_string_new_from_c_str(Aws::Crt::DefaultAllocator(), "233.252.0.0");
            aws_array_list_push_back(output, &list_str_04);
            return AWS_OP_SUCCESS;
        };
        task->RegisterCustomMetricIpAddressList(aws_byte_cursor_from_c_str("CustomIPList"), local_metric_ip_list_func);

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

static int s_TestDeviceDefenderCustomMetricFail(Aws::Crt::Allocator *allocator, void *ctx)
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

        // Add the error custom metric
        aws_iotdevice_defender_get_number_fn *number_metric_func = [](double *output, void *data) {
            *output = 10;
            return AWS_OP_ERR;
        };
        task->RegisterCustomMetricNumber(aws_byte_cursor_from_c_str("CustomNumberOne"), number_metric_func);

        ASSERT_INT_EQUALS((int)Aws::Iotdevicedefenderv1::ReportTaskStatus::Ready, (int)task->GetStatus());

        task->StartTask();
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
AWS_TEST_CASE(DeviceDefenderCustomMetricFail, s_TestDeviceDefenderCustomMetricFail)
