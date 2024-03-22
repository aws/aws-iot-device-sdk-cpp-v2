/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/StlAllocator.h>
#include <aws/crt/auth/Credentials.h>
#include <aws/crt/io/TlsOptions.h>

#include <aws/crt/mqtt/Mqtt5Packets.h>
#include <aws/iot/Mqtt5Client.h>

#include <aws/iotdevicecommon/IotDevice.h>
#include <aws/iotdevicedefender/DeviceDefender.h>

#include <algorithm>
#include <aws/crt/UUID.h>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

#include "../../utils/CommandLineUtils.h"

using namespace Aws::Crt;

int s_getCustomMetricNumber(double *output)
{
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
    /************************ Setup ****************************/

    // Do the global initialization for the API.
    ApiHandle apiHandle;

    /**
     * cmdData is the arguments/input from the command line placed into a single struct for
     * use in this sample. This handles all of the command line parsing, validating, etc.
     * See the Utils/CommandLineUtils for more information.
     */
    Utils::cmdData cmdData = Utils::parseSampleInputDeviceDefender(argc, argv, &apiHandle);

    // Create the MQTT builder and populate it with data from cmdData.
    auto clientConfigBuilder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(
        cmdData.input_endpoint, cmdData.input_cert.c_str(), cmdData.input_key.c_str());
    if (cmdData.input_ca != "")
    {
        clientConfigBuilder->WithCertificateAuthority(cmdData.input_ca.c_str());
    }
    if (cmdData.input_proxyHost != "")
    {
        Aws::Crt::Http::HttpClientConnectionProxyOptions proxyOptions;
        proxyOptions.HostName = cmdData.input_proxyHost;
        proxyOptions.Port = static_cast<uint32_t>(cmdData.input_proxyPort);
        proxyOptions.AuthType = Aws::Crt::Http::AwsHttpProxyAuthenticationType::None;
        clientConfigBuilder->WithHttpProxyOptions(proxyOptions);
    }
    if (cmdData.input_port != 0)
    {
        clientConfigBuilder->WithPort(static_cast<uint32_t>(cmdData.input_port));
    }

    std::promise<bool> connectionPromise;
    std::promise<void> stoppedPromise;

    // Setup lifecycle callbacks
    clientConfigBuilder->WithClientConnectionSuccessCallback(
        [&connectionPromise](const Mqtt5::OnConnectionSuccessEventData &eventData) {
            fprintf(stdout, "Mqtt5 Client connection succeed");
            connectionPromise.set_value(true);
        });
    clientConfigBuilder->WithClientConnectionFailureCallback([&connectionPromise](
                                                                 const Mqtt5::OnConnectionFailureEventData &eventData) {
        fprintf(stdout, "Mqtt5 Client connection failed with error: %s.\n", aws_error_debug_str(eventData.errorCode));
        connectionPromise.set_value(false);
    });
    clientConfigBuilder->WithClientStoppedCallback([&stoppedPromise](const Mqtt5::OnStoppedEventData &) {
        fprintf(stdout, "Mqtt5 Client stopped.\n");
        stoppedPromise.set_value();
    });

    // Setup connection options
    std::shared_ptr<Mqtt5::ConnectPacket> connectOptions = std::make_shared<Mqtt5::ConnectPacket>();
    connectOptions->WithClientId(cmdData.input_clientId);
    clientConfigBuilder->WithConnectOptions(connectOptions);

    // Create Mqtt5Client
    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> client = clientConfigBuilder->Build();

    // Clean up the builder
    delete clientConfigBuilder;

    if (client == nullptr)
    {
        fprintf(
            stdout, "Failed to Init Mqtt5Client with error code %d: %s", LastError(), ErrorDebugString(LastError()));
        return -1;
    }

    /************************ Run the sample ****************************/

    // Connect
    fprintf(stdout, "Connecting...\n");
    if (client->Start())
    {
        if (connectionPromise.get_future().get() == false)
        {
            return -1;
        }

        // Device defender setup and metric registration
        // ======================================================================
        Aws::Crt::Allocator *allocator = Aws::Crt::DefaultAllocator();
        Aws::Crt::Io::EventLoopGroup *eventLoopGroup = Aws::Crt::ApiHandle::GetOrCreateStaticDefaultEventLoopGroup();

        bool callbackSuccess = false;
        auto onCancelled = [&](void *a) -> void {
            auto *data = reinterpret_cast<bool *>(a);
            *data = true;
        };

        Aws::Iotdevicedefenderv1::ReportTaskBuilder taskBuilder(
            allocator, client, *eventLoopGroup, cmdData.input_thingName);
        taskBuilder.WithTaskPeriodSeconds((uint32_t)cmdData.input_reportTime)
            .WithNetworkConnectionSamplePeriodSeconds((uint32_t)cmdData.input_reportTime)
            .WithTaskCancelledHandler(onCancelled)
            .WithTaskCancellationUserData(&callbackSuccess);
        std::shared_ptr<Aws::Iotdevicedefenderv1::ReportTask> task = taskBuilder.Build();

        // Add the custom metrics (Inline function example)
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

        // Wait until the the desire amount of publishes has been complete
        uint64_t publishedCount = 0;
        while (publishedCount < cmdData.input_count &&
               (int)task->GetStatus() == (int)Aws::Iotdevicedefenderv1::ReportTaskStatus::Running)
        {
            ++publishedCount;
            fprintf(stdout, "Publishing next Device Defender report...\n");

            if (publishedCount != cmdData.input_count)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(cmdData.input_reportTime * 1000));
            }
        }

        // Stop the task so we stop sending device defender metrics
        task->StopTask();

        // Disconnect
        if (!client->Stop())
        {
            fprintf(stdout, "Failed to disconnect from the mqtt connection. Exiting..\n");
            return -1;
        }
        stoppedPromise.get_future().wait();
    }
    else
    {
        exit(-1);
    }

    return 0;
}
