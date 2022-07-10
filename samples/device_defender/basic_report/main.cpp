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

int s_getCustomMetricNumber(double *output)
{
    /** Set to a random number between -50 and 50 */
    //*output = (double)((rand() % 100 + 1) - 50);
    *output = 100;
    return AWS_OP_SUCCESS;
}

int s_getCustomMetricNumberList(Vector<double> *output)
{
    output->push_back(1.5);
    output->push_back(2.2);
    output->push_back(3.9);
    output->push_back(9.9);
    return AWS_OP_SUCCESS;
}

int s_getCustomMetricStringList(Vector<String> *output)
{
    output->push_back("One Fish");
    output->push_back("Two Fish");
    output->push_back("Red Fish");
    output->push_back("Blue Fish");
    return AWS_OP_SUCCESS;
}

int s_getCustomMetricIpAddressList(Vector<String> *output)
{
    output->push_back("192.0.2.0");
    output->push_back("198.51.100.0");
    output->push_back("203.0.113.0");
    output->push_back("233.252.0.0");
    return AWS_OP_SUCCESS;
}

int main(int argc, char *argv[])
{

    /************************ Setup the Lib ****************************/
    /*
     * Do the global initialization for the API.
     */
    ApiHandle apiHandle;

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
    cmdUtils.AddLoggingCommands();
    const char **const_argv = (const char **)argv;
    cmdUtils.SendArguments(const_argv, const_argv + argc);
    cmdUtils.StartLoggingBasedOnCommand(&apiHandle);

    if (cmdUtils.HasCommand("help"))
    {
        cmdUtils.PrintHelp();
        exit(-1);
    }
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

    // Make a MQTT client and create a connection using a certificate and key
    // Note: The data for the connection is gotten from cmdUtils
    // (see BuildDirectMQTTConnection for implementation)
    Aws::Iot::MqttClient mqttClient = Aws::Iot::MqttClient();
    std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> connection = cmdUtils.BuildDirectMQTTConnection(&mqttClient);

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
        Aws::Iotdevicedefenderv1::CustomMetricNumberFunction s_localGetCustomMetricNumber = [](double *output) {
            *output = 8.4;
            return AWS_OP_SUCCESS;
        };
        task->RegisterCustomMetricNumber("CustomNumber", std::move(s_localGetCustomMetricNumber));

        Aws::Iotdevicedefenderv1::CustomMetricNumberFunction s_getCustomMetricNumberFunc = s_getCustomMetricNumber;
        task->RegisterCustomMetricNumber("CustomNumberTwo", std::move(s_getCustomMetricNumberFunc));
        Aws::Iotdevicedefenderv1::CustomMetricNumberListFunction s_getCustomMetricNumberListFunc =
            s_getCustomMetricNumberList;
        task->RegisterCustomMetricNumberList("CustomNumberList", std::move(s_getCustomMetricNumberListFunc));
        Aws::Iotdevicedefenderv1::CustomMetricStringListFunction s_getCustomMetricStringListFunc =
            s_getCustomMetricStringList;
        task->RegisterCustomMetricStringList("CustomStringList", std::move(s_getCustomMetricStringListFunc));
        Aws::Iotdevicedefenderv1::CustomMetricIpListFunction s_getCustomMetricIpAddressListFunc =
            s_getCustomMetricIpAddressList;
        task->RegisterCustomMetricIpAddressList("CustomIPList", std::move(s_getCustomMetricIpAddressListFunc));

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
