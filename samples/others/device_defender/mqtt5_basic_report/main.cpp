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

/* --------------------------------- ARGUMENT PARSING ----------------------------------------- */
struct CmdArgs
{
    String endpoint;
    String cert;
    String key;
    String clientId;
    String thingName = "TestThing";
    String proxyHost;
    uint32_t port = 0;
    uint32_t proxyPort = 8080;
    uint32_t reportTime = 60;
    uint32_t count = 10;
};

void printHelp()
{
    printf("MQTT5 Device Defender Sample\n");
    printf("options:\n");
    printf("  --help        show this help message and exit\n");
    printf("required arguments:\n");
    printf("  --endpoint    IoT endpoint hostname\n");
    printf("  --cert        Path to the certificate file\n");
    printf("  --key         Path to the private key file\n");
    printf("optional arguments:\n");
    printf("  --client_id   Client ID (default: test-<uuid>)\n");
    printf("  --thing_name  Thing name (default: TestThing)\n");
    printf("  --proxy_host  HTTP proxy host\n");
    printf("  --proxy_port  HTTP proxy port (default: 8080)\n");
    printf("  --port        Port override\n");
    printf("  --report_time Report frequency in seconds (default: 60)\n");
    printf("  --count       Number of reports to send (default: 10)\n");
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
            else if (strcmp(argv[i], "--client_id") == 0)
            {
                args.clientId = argv[++i];
            }

            else if (strcmp(argv[i], "--thing_name") == 0)
            {
                args.thingName = argv[++i];
            }
            else if (strcmp(argv[i], "--proxy_host") == 0)
            {
                args.proxyHost = argv[++i];
            }
            else if (strcmp(argv[i], "--proxy_port") == 0)
            {
                args.proxyPort = atoi(argv[++i]);
            }
            else if (strcmp(argv[i], "--port") == 0)
            {
                args.port = atoi(argv[++i]);
            }
            else if (strcmp(argv[i], "--report_time") == 0)
            {
                args.reportTime = atoi(argv[++i]);
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
    {
        args.clientId = String("test-") + UUID().ToString();
    }
    return args;
}
/* --------------------------------- ARGUMENT PARSING END ----------------------------------------- */

int main(int argc, char *argv[])
{
    /************************ Setup ****************************/

    // Parse command line arguments
    CmdArgs cmdData = parseArgs(argc, argv);

    // Do the global initialization for the API.
    ApiHandle apiHandle;

    // Create the MQTT builder and populate it with data from cmdData.
    auto clientConfigBuilder = std::unique_ptr<Aws::Iot::Mqtt5ClientBuilder>(
        Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(
            cmdData.endpoint, cmdData.cert.c_str(), cmdData.key.c_str()));
    if (clientConfigBuilder == nullptr)
    {
        fprintf(
            stdout,
            "Failed to setup MQTT5 client builder with error code %d: %s",
            LastError(),
            ErrorDebugString(LastError()));
        return -1;
    }

    if (!cmdData.proxyHost.empty())
    {
        Aws::Crt::Http::HttpClientConnectionProxyOptions proxyOptions;
        proxyOptions.HostName = cmdData.proxyHost;
        proxyOptions.Port = cmdData.proxyPort;
        proxyOptions.AuthType = Aws::Crt::Http::AwsHttpProxyAuthenticationType::None;
        clientConfigBuilder->WithHttpProxyOptions(proxyOptions);
    }
    if (cmdData.port != 0)
    {
        clientConfigBuilder->WithPort(cmdData.port);
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
    std::shared_ptr<Mqtt5::ConnectPacket> connectOptions =
        Aws::Crt::MakeShared<Mqtt5::ConnectPacket>(Aws::Crt::DefaultAllocatorImplementation());
    connectOptions->WithClientId(cmdData.clientId);
    clientConfigBuilder->WithConnectOptions(connectOptions);

    // Create Mqtt5Client
    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> client = clientConfigBuilder->Build();

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
            allocator, client, *eventLoopGroup, cmdData.thingName);
        taskBuilder.WithTaskPeriodSeconds(cmdData.reportTime)
            .WithNetworkConnectionSamplePeriodSeconds(cmdData.reportTime)
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
        while (publishedCount < cmdData.count &&
               (int)task->GetStatus() == (int)Aws::Iotdevicedefenderv1::ReportTaskStatus::Running)
        {
            ++publishedCount;
            fprintf(stdout, "Publishing next Device Defender report...\n");

            if (publishedCount != cmdData.count)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(cmdData.reportTime * 1000));
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
