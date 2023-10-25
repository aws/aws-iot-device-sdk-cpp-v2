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

static int s_TestDeviceDefenderResourceSafety(Aws::Crt::Allocator *allocator, void *ctx)
{
    (void)ctx;
    {
        Aws::Crt::ApiHandle apiHandle(allocator);
        Aws::Iotdevicecommon::DeviceApiHandle deviceApiHandle(allocator);
        Aws::Crt::Io::TlsContextOptions tlsCtxOptions = Aws::Crt::Io::TlsContextOptions::InitDefaultClient();

        Aws::Crt::Io::TlsContext tlsContext(tlsCtxOptions, Aws::Crt::Io::TlsMode::CLIENT, allocator);
        ASSERT_TRUE(tlsContext);

        Aws::Crt::Io::SocketOptions socketOptions;
        socketOptions.SetConnectTimeoutMs(3000);

        Aws::Crt::Io::EventLoopGroup eventLoopGroup(0, allocator);
        ASSERT_TRUE(eventLoopGroup);

        Aws::Crt::Io::DefaultHostResolver defaultHostResolver(eventLoopGroup, 8, 30, allocator);
        ASSERT_TRUE(defaultHostResolver);

        Aws::Crt::Io::ClientBootstrap clientBootstrap(eventLoopGroup, defaultHostResolver, allocator);
        ASSERT_TRUE(allocator);
        clientBootstrap.EnableBlockingShutdown();

        Aws::Crt::Mqtt::MqttClient mqttClient(clientBootstrap, allocator);
        ASSERT_TRUE(mqttClient);

        auto mqttConnection = mqttClient.NewConnection("www.example.com", 443, socketOptions, tlsContext);

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

        ASSERT_INT_EQUALS((int)Aws::Iotdevicedefenderv1::ReportTaskStatus::Stopped, (int)task->GetStatus());
    }

    return AWS_ERROR_SUCCESS;
}

AWS_TEST_CASE(DeviceDefenderResourceSafety, s_TestDeviceDefenderResourceSafety)

static int s_TestDeviceDefenderFailedTest(Aws::Crt::Allocator *allocator, void *ctx)
{
    (void)ctx;
    {
        Aws::Crt::ApiHandle apiHandle(allocator);
        Aws::Iotdevicecommon::DeviceApiHandle deviceApiHandle(allocator);
        Aws::Crt::Io::TlsContextOptions tlsCtxOptions = Aws::Crt::Io::TlsContextOptions::InitDefaultClient();

        Aws::Crt::Io::TlsContext tlsContext(tlsCtxOptions, Aws::Crt::Io::TlsMode::CLIENT, allocator);
        ASSERT_TRUE(tlsContext);

        Aws::Crt::Io::SocketOptions socketOptions;
        socketOptions.SetConnectTimeoutMs(3000);

        Aws::Crt::Io::EventLoopGroup eventLoopGroup(0, allocator);
        ASSERT_TRUE(eventLoopGroup);

        Aws::Crt::Io::DefaultHostResolver defaultHostResolver(eventLoopGroup, 8, 30, allocator);
        ASSERT_TRUE(defaultHostResolver);

        Aws::Crt::Io::ClientBootstrap clientBootstrap(eventLoopGroup, defaultHostResolver, allocator);
        ASSERT_TRUE(allocator);
        clientBootstrap.EnableBlockingShutdown();

        Aws::Crt::Mqtt::MqttClient mqttClient(clientBootstrap, allocator);
        ASSERT_TRUE(mqttClient);

        auto mqttConnection = mqttClient.NewConnection("www.example.com", 443, socketOptions, tlsContext);

        const Aws::Crt::String thingName("TestThing");
        Aws::Crt::String data("TestData");

        Aws::Iotdevicedefenderv1::ReportTaskBuilder taskBuilder(allocator, mqttConnection, eventLoopGroup, thingName);
        taskBuilder.WithTaskPeriodSeconds((uint32_t)1UL)
            .WithTaskPeriodSeconds((uint32_t)1UL)
            .WithReportFormat(Aws::Iotdevicedefenderv1::ReportFormat::AWS_IDDRF_SHORT_JSON);

        std::shared_ptr<Aws::Iotdevicedefenderv1::ReportTask> task = taskBuilder.Build();

        ASSERT_INT_EQUALS((int)Aws::Iotdevicedefenderv1::ReportTaskStatus::Ready, (int)task->GetStatus());

        ASSERT_INT_EQUALS(AWS_ERROR_IOTDEVICE_DEFENDER_UNSUPPORTED_REPORT_FORMAT, task->LastError());
        ASSERT_FAILS(task->StartTask());
        ASSERT_TRUE(aws_last_error() == AWS_ERROR_INVALID_STATE);
    }

    return AWS_ERROR_SUCCESS;
}

AWS_TEST_CASE(DeviceDefenderFailedTest, s_TestDeviceDefenderFailedTest)

static int s_TestMqtt5DeviceDefenderResourceSafety(Aws::Crt::Allocator *allocator, void *ctx)
{
    (void)ctx;
    Aws::Crt::ApiHandle apiHandle(allocator);
    Aws::Iotdevicecommon::DeviceApiHandle deviceApiHandle(allocator);
    Aws::Crt::Io::TlsContextOptions tlsCtxOptions = Aws::Crt::Io::TlsContextOptions::InitDefaultClient();

    Aws::Crt::Io::TlsContext tlsContext(tlsCtxOptions, Aws::Crt::Io::TlsMode::CLIENT, allocator);
    ASSERT_TRUE(tlsContext);

    Aws::Crt::Io::SocketOptions socketOptions;
    socketOptions.SetConnectTimeoutMs(3000);

    Aws::Crt::Io::EventLoopGroup eventLoopGroup(0, allocator);
    ASSERT_TRUE(eventLoopGroup);

    Aws::Crt::Io::DefaultHostResolver defaultHostResolver(eventLoopGroup, 8, 30, allocator);
    ASSERT_TRUE(defaultHostResolver);

    Aws::Crt::Io::ClientBootstrap clientBootstrap(eventLoopGroup, defaultHostResolver, allocator);
    ASSERT_TRUE(allocator);
    clientBootstrap.EnableBlockingShutdown();

    Aws::Crt::Mqtt5::Mqtt5ClientOptions mqtt5Options(allocator);
    mqtt5Options.WithBootstrap(&clientBootstrap);
    mqtt5Options.WithSocketOptions(socketOptions);
    mqtt5Options.WithTlsConnectionOptions(tlsContext.NewConnectionOptions());
    mqtt5Options.WithHostName("www.example.come");
    mqtt5Options.WithPort(443);
    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> mqtt5Client =
        Aws::Crt::Mqtt5::Mqtt5Client::NewMqtt5Client(mqtt5Options, allocator);
    ASSERT_TRUE(mqtt5Client);

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

    Aws::Iotdevicedefenderv1::ReportTaskBuilder taskBuilder(allocator, mqtt5Client, eventLoopGroup, thingName);
    taskBuilder.WithTaskPeriodSeconds((uint32_t)1UL)
        .WithNetworkConnectionSamplePeriodSeconds((uint32_t)1UL)
        .WithTaskCancelledHandler(onCancelled)
        .WithTaskCancellationUserData(&callbackSuccess);

    std::shared_ptr<Aws::Iotdevicedefenderv1::ReportTask> task = taskBuilder.Build();

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

    ASSERT_INT_EQUALS((int)Aws::Iotdevicedefenderv1::ReportTaskStatus::Stopped, (int)task->GetStatus());

    return AWS_ERROR_SUCCESS;
}

AWS_TEST_CASE(Mqtt5DeviceDefenderResourceSafety, s_TestMqtt5DeviceDefenderResourceSafety)

static int s_TestMqtt5DeviceDefenderFailedTest(Aws::Crt::Allocator *allocator, void *ctx)
{
    (void)ctx;
    Aws::Crt::ApiHandle apiHandle(allocator);
    Aws::Iotdevicecommon::DeviceApiHandle deviceApiHandle(allocator);
    Aws::Crt::Io::TlsContextOptions tlsCtxOptions = Aws::Crt::Io::TlsContextOptions::InitDefaultClient();

    Aws::Crt::Io::TlsContext tlsContext(tlsCtxOptions, Aws::Crt::Io::TlsMode::CLIENT, allocator);
    ASSERT_TRUE(tlsContext);

    Aws::Crt::Io::SocketOptions socketOptions;
    socketOptions.SetConnectTimeoutMs(3000);

    Aws::Crt::Io::EventLoopGroup eventLoopGroup(0, allocator);
    ASSERT_TRUE(eventLoopGroup);

    Aws::Crt::Io::DefaultHostResolver defaultHostResolver(eventLoopGroup, 8, 30, allocator);
    ASSERT_TRUE(defaultHostResolver);

    Aws::Crt::Io::ClientBootstrap clientBootstrap(eventLoopGroup, defaultHostResolver, allocator);
    ASSERT_TRUE(allocator);
    clientBootstrap.EnableBlockingShutdown();

    Aws::Crt::Mqtt5::Mqtt5ClientOptions mqtt5Options(allocator);
    mqtt5Options.WithBootstrap(&clientBootstrap);
    mqtt5Options.WithSocketOptions(socketOptions);
    mqtt5Options.WithTlsConnectionOptions(tlsContext.NewConnectionOptions());
    mqtt5Options.WithHostName("www.example.come");
    mqtt5Options.WithPort(443);

    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> mqtt5Client =
        Aws::Crt::Mqtt5::Mqtt5Client::NewMqtt5Client(mqtt5Options, allocator);
    ASSERT_TRUE(mqtt5Client);

    const Aws::Crt::String thingName("TestThing");
    Aws::Crt::String data("TestData");

    Aws::Iotdevicedefenderv1::ReportTaskBuilder taskBuilder(allocator, mqtt5Client, eventLoopGroup, thingName);
    taskBuilder.WithTaskPeriodSeconds((uint32_t)1UL)
        .WithTaskPeriodSeconds((uint32_t)1UL)
        .WithReportFormat(Aws::Iotdevicedefenderv1::ReportFormat::AWS_IDDRF_SHORT_JSON);

    std::shared_ptr<Aws::Iotdevicedefenderv1::ReportTask> task = taskBuilder.Build();

    ASSERT_INT_EQUALS((int)Aws::Iotdevicedefenderv1::ReportTaskStatus::Ready, (int)task->GetStatus());

    ASSERT_INT_EQUALS(AWS_ERROR_IOTDEVICE_DEFENDER_UNSUPPORTED_REPORT_FORMAT, task->LastError());
    ASSERT_FAILS(task->StartTask());
    ASSERT_TRUE(aws_last_error() == AWS_ERROR_INVALID_STATE);

    return AWS_ERROR_SUCCESS;
}

AWS_TEST_CASE(Mqtt5DeviceDefenderFailedTest, s_TestMqtt5DeviceDefenderFailedTest)
