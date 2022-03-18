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

int global_metric_number_func(double *output)
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
        std::function<int(double *)> local_metric_number_func = [](double *output) {
            *output = 10;
            return AWS_OP_SUCCESS;
        };
        task->RegisterCustomMetricNumber("CustomNumberOne", local_metric_number_func);
        std::function<int(double *)> global_metric_number_func_ref = global_metric_number_func;
        task->RegisterCustomMetricNumber("CustomNumberTwo", global_metric_number_func_ref);

        std::function<int(std::vector<double> *)> local_metric_number_list_func = [](std::vector<double> *output) {
            output->push_back(101);
            output->push_back(102);
            output->push_back(103);
            return AWS_OP_SUCCESS;
        };
        task->RegisterCustomMetricNumberList("CustomNumberList", local_metric_number_list_func);

        std::function<int(std::vector<std::string> *)> local_metric_str_list_func = [](std::vector<std::string> *output) {
            output->push_back("One Fish");
            output->push_back("Two Fish");
            output->push_back("Red Fish");
            output->push_back("Blue Fish");
            return AWS_OP_SUCCESS;
        };
        task->RegisterCustomMetricStringList("CustomStringList", local_metric_str_list_func);

        std::function<int(std::vector<std::string> *)> local_metric_ip_list_func = [](std::vector<std::string> *output) {
            output->push_back("192.0.2.0");
            output->push_back("198.51.100.0");
            output->push_back("203.0.113.0");
            output->push_back("233.252.0.0");
            return AWS_OP_SUCCESS;
        };
        task->RegisterCustomMetricIpAddressList("CustomIPList", local_metric_ip_list_func);

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
        std::function<int(double *)> number_metric_func = [](double *output) {
            *output = 10;
            return AWS_OP_ERR;
        };
        task->RegisterCustomMetricNumber("CustomNumberOne", number_metric_func);

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
