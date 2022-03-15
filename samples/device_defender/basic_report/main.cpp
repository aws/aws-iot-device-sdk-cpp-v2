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

#include "../../utils/CommandLineUtils.h"

using namespace Aws::Crt;

int s_getCustomMetricNumber(int64_t *output, void *data)
{
    /** Set to a random number between -50 and 50 */
    *output = (rand() % 100 + 1) - 50;
    return AWS_OP_SUCCESS;
}

int s_getCustomMetricNumberList(aws_array_list *output, void *data)
{
    aws_array_list_init_dynamic(output, Aws::Crt::DefaultAllocator(), 0, sizeof(int64_t *));
    int64_t s_metricListNumberOne = 1;
    int64_t s_metricListNumberTwo = 2;
    int64_t s_metricListNumberThree = 3;
    int64_t s_metricListNumberFour = 4;
    aws_array_list_push_back(output, &s_metricListNumberOne);
    aws_array_list_push_back(output, &s_metricListNumberTwo);
    aws_array_list_push_back(output, &s_metricListNumberThree);
    aws_array_list_push_back(output, &s_metricListNumberFour);
    return AWS_OP_SUCCESS;
}

int s_getCustomMetricStringList(aws_array_list *output, void *data)
{
    aws_array_list_init_dynamic(output, Aws::Crt::DefaultAllocator(), 0, sizeof(aws_string *));
    aws_string *list_str_01 = aws_string_new_from_c_str(Aws::Crt::DefaultAllocator(), "One Fish");
    aws_array_list_push_back(output, &list_str_01);
    aws_string *list_str_02 = aws_string_new_from_c_str(Aws::Crt::DefaultAllocator(), "Two Fish");
    aws_array_list_push_back(output, &list_str_02);
    aws_string *list_str_03 = aws_string_new_from_c_str(Aws::Crt::DefaultAllocator(), "Red Fish");
    aws_array_list_push_back(output, &list_str_03);
    aws_string *list_str_04 = aws_string_new_from_c_str(Aws::Crt::DefaultAllocator(), "Blue Fish");
    aws_array_list_push_back(output, &list_str_04);
    return AWS_OP_SUCCESS;
}

int s_getCustomMetricIPAddressList(aws_array_list *output, void *data)
{
    aws_array_list_init_dynamic(output, Aws::Crt::DefaultAllocator(), 0, sizeof(aws_string *));
    aws_string *list_str_01 = aws_string_new_from_c_str(Aws::Crt::DefaultAllocator(), "192.0.2.0");
    aws_array_list_push_back(output, &list_str_01);
    aws_string *list_str_02 = aws_string_new_from_c_str(Aws::Crt::DefaultAllocator(), "198.51.100.0");
    aws_array_list_push_back(output, &list_str_02);
    aws_string *list_str_03 = aws_string_new_from_c_str(Aws::Crt::DefaultAllocator(), "203.0.113.0");
    aws_array_list_push_back(output, &list_str_03);
    aws_string *list_str_04 = aws_string_new_from_c_str(Aws::Crt::DefaultAllocator(), "233.252.0.0");
    aws_array_list_push_back(output, &list_str_04);
    return AWS_OP_SUCCESS;
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
    String thingName("TestThing");
    int reportTime = 60u;
    int count = 10;
    String clientId(String("test-") + Aws::Crt::UUID().ToString());

    /*********************** Parse Arguments ***************************/
    Utils::CommandLineUtils cmdUtils = Utils::CommandLineUtils();
    cmdUtils.RegisterProgramName("basic-report");
    cmdUtils.AddCommonMQTTCommands();
    cmdUtils.RegisterCommand("thing_name", "<str>", "The name of your IOT thing (optional, default='TestThing').");
    cmdUtils.RegisterCommand(
        "report_time", "<int>", "The frequency to send Device Defender reports in seconds (optional, default='60')");
    cmdUtils.RegisterCommand("count", "<int>", "The number of reports to send (optional, default='10')");
    cmdUtils.RegisterCommand("help", "", "Prints this message");
    const char **const_argv = (const char **)argv;
    cmdUtils.SendArguments(const_argv, const_argv + argc);

    if (cmdUtils.HasCommand("help"))
    {
        cmdUtils.PrintHelp();
        exit(-1);
    }
    endpoint = cmdUtils.GetCommandRequired("endpoint");
    keyPath = cmdUtils.GetCommandRequired("key");
    certificatePath = cmdUtils.GetCommandRequired("cert");
    caFile = cmdUtils.GetCommandOrDefault("ca_file", caFile);
    thingName = cmdUtils.GetCommandOrDefault("thing_name", thingName);

    if (cmdUtils.HasCommand("report_time"))
    {
        int tmpReportTime = atoi(cmdUtils.GetCommand("report_time").c_str());
        if (tmpReportTime > 0)
        {
            reportTime = tmpReportTime;
        }
    }
    if (cmdUtils.HasCommand("count"))
    {
        int tmpCount = atoi(cmdUtils.GetCommand("count").c_str());
        if (tmpCount > 0)
        {
            count = tmpCount;
        }
    }

    /********************** Now Setup an Mqtt Client ******************/
    // To enable logging, uncomment the code below
    // apiHandle.InitializeLogging(Aws::Crt::LogLevel::Trace, stdout);

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
                fprintf(stdout, "Connection completed successfully.\n");
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
        // Device defender setup and metric registration
        // ======================================================================
        Aws::Crt::Allocator *allocator = Aws::Crt::DefaultAllocator();
        Aws::Crt::Io::EventLoopGroup *eventLoopGroup = Aws::Crt::ApiHandle::GetOrCreateStaticDefaultEventLoopGroup();

        bool callbackSuccess = false;
        auto onCancelled = [&](void *a) -> void {
            auto *data = reinterpret_cast<bool *>(a);
            *data = true;
        };

        Aws::Iotdevicedefenderv1::ReportTaskBuilder taskBuilder(allocator, connection, *eventLoopGroup, thingName);
        taskBuilder.WithTaskPeriodSeconds((uint32_t)reportTime)
            .WithNetworkConnectionSamplePeriodSeconds((uint32_t)reportTime)
            .WithTaskCancelledHandler(onCancelled)
            .WithTaskCancellationUserData(&callbackSuccess);
        std::shared_ptr<Aws::Iotdevicedefenderv1::ReportTask> task = taskBuilder.Build();

        // Add the custom metrics
        // (Inline function example)
        aws_iotdevice_defender_get_number_fn *s_localGetCustomMetricNumber = [](int64_t *output, void *data) {
            *output = 10;
            return AWS_OP_SUCCESS;
        };

        task->RegisterCustomMetricNumber(aws_byte_cursor_from_c_str("CustomNumber"), s_localGetCustomMetricNumber);
        task->RegisterCustomMetricNumber(aws_byte_cursor_from_c_str("CustomNumberTwo"), &s_getCustomMetricNumber);
        task->RegisterCustomMetricNumberList(
            aws_byte_cursor_from_c_str("CustomNumberList"), &s_getCustomMetricNumberList);
        task->RegisterCustomMetricStringList(
            aws_byte_cursor_from_c_str("CustomStringList"), s_getCustomMetricStringList);
        task->RegisterCustomMetricIpAddressList(
            aws_byte_cursor_from_c_str("CustomIPList"), s_getCustomMetricIPAddressList);

        // Send additional device custom metrics
        task->RegisterCustomMetricCpuUsage();
        task->RegisterCustomMetricMemoryUsage();
        task->RegisterCustomMetricProcessorCount();

        // Start the Device Defender task
        if (task->StartTask() != AWS_OP_SUCCESS)
        {
            fprintf(stdout, "Device Defender failed to initialize task.\n");
            exit(-1);
        }
        else
        {
            fprintf(stdout, "Device Defender initialized.\n");
        }

        if ((int)task->GetStatus() == (int)Aws::Iotdevicedefenderv1::ReportTaskStatus::Running)
        {
            fprintf(stdout, "Device Defender task running.\n");
        }
        else
        {
            fprintf(stdout, "Device Defender task in unknown status. Status: %d\n", (int)task->GetStatus());
            exit(-1);
        }
        // ======================================================================

        int publishedCount = 0;
        while (publishedCount < count &&
               (int)task->GetStatus() == (int)Aws::Iotdevicedefenderv1::ReportTaskStatus::Running)
        {
            ++publishedCount;
            fprintf(stdout, "Publishing Device Defender report %d...\n", publishedCount);

            if (publishedCount != count)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(reportTime * 1000));
            }
        }

        // Stop the task so we stop sending device defender metrics
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
