/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/UUID.h>
#include <aws/crt/mqtt/Mqtt5Packets.h>
#include <aws/iot/Mqtt5Client.h>

#include <aws/iotjobs/DescribeJobExecutionRequest.h>
#include <aws/iotjobs/DescribeJobExecutionResponse.h>
#include <aws/iotjobs/GetPendingJobExecutionsRequest.h>
#include <aws/iotjobs/GetPendingJobExecutionsResponse.h>
#include <aws/iotjobs/IotJobsClientV2.h>
#include <aws/iotjobs/JobExecutionsChangedEvent.h>
#include <aws/iotjobs/JobExecutionsChangedSubscriptionRequest.h>
#include <aws/iotjobs/NextJobExecutionChangedEvent.h>
#include <aws/iotjobs/NextJobExecutionChangedSubscriptionRequest.h>
#include <aws/iotjobs/StartNextJobExecutionResponse.h>
#include <aws/iotjobs/StartNextPendingJobExecutionRequest.h>
#include <aws/iotjobs/UpdateJobExecutionRequest.h>
#include <aws/iotjobs/UpdateJobExecutionResponse.h>
#include <aws/iotjobs/V2ErrorResponse.h>

#include <algorithm>
#include <condition_variable>
#include <iostream>

using namespace Aws::Crt;
using namespace Aws::Iotjobs;

struct ApplicationContext
{

    std::shared_ptr<Mqtt5::Mqtt5Client> m_protocolClient;

    std::shared_ptr<IClientV2> m_jobsClient;

    String m_thingName;

    std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> m_jobExecutionsChangedStream;
    std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> m_nextJobExecutionChangedStream;
};

static void s_onConnectionFailure(const Mqtt5::OnConnectionFailureEventData &eventData)
{
    fprintf(
        stdout, "Mqtt5 client connection attempt failed with error: %s.\n", aws_error_debug_str(eventData.errorCode));
}

static String s_nibbleNextToken(String &input)
{
    String token;
    String remaining;
    auto delimPosition = input.find(' ');
    if (delimPosition != Aws::Crt::String::npos)
    {
        token = input.substr(0, delimPosition);

        auto untrimmedRemaining = input.substr(delimPosition, Aws::Crt::String::npos);
        auto firstNonSpacePosition = untrimmedRemaining.find_first_not_of(' ');
        if (firstNonSpacePosition != Aws::Crt::String::npos)
        {
            remaining = untrimmedRemaining.substr(firstNonSpacePosition, Aws::Crt::String::npos);
        }
        else
        {
            remaining = "";
        }
    }
    else
    {
        token = input;
        remaining = "";
    }

    input = remaining;
    return token;
}

static void s_onServiceError(const ServiceErrorV2<V2ErrorResponse> &serviceError, String operationName)
{
    fprintf(
        stdout,
        "%s failed with error code: %s\n",
        operationName.c_str(),
        aws_error_debug_str(serviceError.GetErrorCode()));
    if (serviceError.HasModeledError())
    {
        const auto &modeledError = serviceError.GetModeledError();

        JsonObject jsonObject;
        modeledError.SerializeToObject(jsonObject);
        String outgoingJson = jsonObject.View().WriteCompact(true);
        fprintf(stdout, "modeled error: %s\n", outgoingJson.c_str());
    }
}

static void s_onDescribeJobExecutionResult(DescribeJobExecutionResult &&result)
{
    if (result.IsSuccess())
    {
        const auto &response = result.GetResponse();

        JsonObject jsonObject;
        response.SerializeToObject(jsonObject);
        String outgoingJson = jsonObject.View().WriteCompact(true);
        fprintf(stdout, "describe-job-execution result: %s\n", outgoingJson.c_str());
    }
    else
    {
        const auto &serviceError = result.GetError();
        s_onServiceError(serviceError, "describe-job-execution");
    }
}

static void s_handleDescribeJobExecution(const String &params, const ApplicationContext &context)
{
    String paramCopy = params;
    String jobId = s_nibbleNextToken(paramCopy);

    Aws::Iotjobs::DescribeJobExecutionRequest request;
    request.ThingName = context.m_thingName;
    request.JobId = jobId;

    std::promise<bool> describeWaiter;
    context.m_jobsClient->DescribeJobExecution(
        request,
        [&describeWaiter](DescribeJobExecutionResult &&result)
        {
            s_onDescribeJobExecutionResult(std::move(result));
            describeWaiter.set_value(true);
        });

    describeWaiter.get_future().wait();
}

static void s_onGetPendingJobExecutionsResult(GetPendingJobExecutionsResult &&result)
{
    if (result.IsSuccess())
    {
        const auto &response = result.GetResponse();

        JsonObject jsonObject;
        response.SerializeToObject(jsonObject);
        String outgoingJson = jsonObject.View().WriteCompact(true);
        fprintf(stdout, "get-pending-job-executions result: %s\n", outgoingJson.c_str());
    }
    else
    {
        const auto &serviceError = result.GetError();
        s_onServiceError(serviceError, "get-pending-job-executions");
    }
}

static void s_handleGetPendingJobExecutions(const ApplicationContext &context)
{
    Aws::Iotjobs::GetPendingJobExecutionsRequest request;
    request.ThingName = context.m_thingName;

    std::promise<bool> getWaiter;
    context.m_jobsClient->GetPendingJobExecutions(
        request,
        [&getWaiter](GetPendingJobExecutionsResult &&result)
        {
            s_onGetPendingJobExecutionsResult(std::move(result));
            getWaiter.set_value(true);
        });

    getWaiter.get_future().wait();
}

static void s_onStartNextPendingJobExecutionResult(StartNextPendingJobExecutionResult &&result)
{
    if (result.IsSuccess())
    {
        const auto &response = result.GetResponse();

        JsonObject jsonObject;
        response.SerializeToObject(jsonObject);
        String outgoingJson = jsonObject.View().WriteCompact(true);
        fprintf(stdout, "start-next-pending-job-execution result: %s\n", outgoingJson.c_str());
    }
    else
    {
        const auto &serviceError = result.GetError();
        s_onServiceError(serviceError, "start-next-pending-job-execution");
    }
}

static void s_handleStartNextPendingJobExecution(const ApplicationContext &context)
{
    Aws::Iotjobs::StartNextPendingJobExecutionRequest request;
    request.ThingName = context.m_thingName;

    std::promise<bool> startWaiter;
    context.m_jobsClient->StartNextPendingJobExecution(
        request,
        [&startWaiter](StartNextPendingJobExecutionResult &&result)
        {
            s_onStartNextPendingJobExecutionResult(std::move(result));
            startWaiter.set_value(true);
        });

    startWaiter.get_future().wait();
}

static void s_onUpdateJobExecutionResult(UpdateJobExecutionResult &&result)
{
    if (result.IsSuccess())
    {
        const auto &response = result.GetResponse();

        JsonObject jsonObject;
        response.SerializeToObject(jsonObject);
        String outgoingJson = jsonObject.View().WriteCompact(true);
        fprintf(stdout, "update-job-execution result: %s\n", outgoingJson.c_str());
    }
    else
    {
        const auto &serviceError = result.GetError();
        s_onServiceError(serviceError, "update-job-execution");
    }
}

static void s_handleUpdateJobExecution(const String &params, const ApplicationContext &context)
{
    String paramCopy = params;
    String jobId = s_nibbleNextToken(paramCopy);
    String status = s_nibbleNextToken(paramCopy);

    Aws::Iotjobs::UpdateJobExecutionRequest request;
    request.ThingName = context.m_thingName;
    request.JobId = jobId;
    request.Status = JobStatusMarshaller::FromString(status);

    std::promise<bool> updateWaiter;
    context.m_jobsClient->UpdateJobExecution(
        request,
        [&updateWaiter](UpdateJobExecutionResult &&result)
        {
            s_onUpdateJobExecutionResult(std::move(result));
            updateWaiter.set_value(true);
        });

    updateWaiter.get_future().wait();
}

static void s_printHelp()
{
    fprintf(stdout, "\nJobs sandbox:\n\n");
    fprintf(stdout, "  quit -- quits the program\n");
    fprintf(
        stdout,
        "  describe-job-execution <jobId> -- gets the service status of a job execution with the specified job id\n");
    fprintf(stdout, "  get-pending-job-executions -- gets all incomplete job executions\n");
    fprintf(
        stdout,
        "  start-next-pending-job-execution -- moves the next pending job execution into the IN_PROGRESS state\n");
    fprintf(
        stdout,
        "  update-job-execution <jobId> <SUCCEEDED | IN_PROGRESS | FAILED | CANCELED> -- updates a job execution with "
        "a new status\n\n");
}

static bool s_handleInput(const Aws::Crt::String &input, ApplicationContext &context)
{
    Aws::Crt::String remaining = input;
    Aws::Crt::String command = s_nibbleNextToken(remaining);

    if (command == "quit")
    {
        fprintf(stdout, "Quitting!\n");
        return true;
    }
    else if (command == "describe-job-execution")
    {
        s_handleDescribeJobExecution(remaining, context);
    }
    else if (command == "get-pending-job-executions")
    {
        s_handleGetPendingJobExecutions(context);
    }
    else if (command == "start-next-pending-job-execution")
    {
        s_handleStartNextPendingJobExecution(context);
    }
    else if (command == "update-job-execution")
    {
        s_handleUpdateJobExecution(remaining, context);
    }
    else
    {
        if (command != "help")
        {
            fprintf(stdout, "Command not recognized: %s\n", command.c_str());
        }
        s_printHelp();
    }

    return false;
}

static std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> s_createJobExecutionsChangedStream(
    const ApplicationContext &context)
{
    std::promise<bool> subscribedWaiter;

    JobExecutionsChangedSubscriptionRequest request;
    request.ThingName = context.m_thingName;

    Aws::Iot::RequestResponse::StreamingOperationOptions<Aws::Iotjobs::JobExecutionsChangedEvent> options;
    options.WithSubscriptionStatusEventHandler(
        [&subscribedWaiter](Aws::Iot::RequestResponse::SubscriptionStatusEvent &&event)
        {
            if (event.GetType() == Aws::Iot::RequestResponse::SubscriptionStatusEventType::SubscriptionEstablished)
            {
                subscribedWaiter.set_value(true);
            }
        });
    options.WithStreamHandler(
        [](Aws::Iotjobs::JobExecutionsChangedEvent &&event)
        {
            JsonObject jsonObject;
            event.SerializeToObject(jsonObject);
            Aws::Crt::String json = jsonObject.View().WriteCompact(true);
            fprintf(stdout, "Received JobExecutionsChanged event:  %s\n", json.c_str());
        });

    auto stream = context.m_jobsClient->CreateJobExecutionsChangedStream(request, options);
    stream->Open();

    subscribedWaiter.get_future().wait();

    return stream;
}

static std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> s_createNextJobExecutionChangedStream(
    const ApplicationContext &context)
{
    std::promise<bool> subscribedWaiter;

    NextJobExecutionChangedSubscriptionRequest request;
    request.ThingName = context.m_thingName;

    Aws::Iot::RequestResponse::StreamingOperationOptions<Aws::Iotjobs::NextJobExecutionChangedEvent> options;
    options.WithSubscriptionStatusEventHandler(
        [&subscribedWaiter](Aws::Iot::RequestResponse::SubscriptionStatusEvent &&event)
        {
            if (event.GetType() == Aws::Iot::RequestResponse::SubscriptionStatusEventType::SubscriptionEstablished)
            {
                subscribedWaiter.set_value(true);
            }
        });
    options.WithStreamHandler(
        [](Aws::Iotjobs::NextJobExecutionChangedEvent &&event)
        {
            JsonObject jsonObject;
            event.SerializeToObject(jsonObject);
            Aws::Crt::String json = jsonObject.View().WriteCompact(true);
            fprintf(stdout, "Received NextJobExecutionChanged event:  %s\n", json.c_str());
        });

    auto stream = context.m_jobsClient->CreateNextJobExecutionChangedStream(request, options);
    stream->Open();

    subscribedWaiter.get_future().wait();

    return stream;
}

/* --------------------------------- ARGUMENT PARSING ----------------------------------------- */
struct CmdArgs
{
    String endpoint;
    String cert;
    String key;
    String clientId;
    String thingName;
};

void printHelp()
{
    printf("Jobs Sandbox Sample\n");
    printf("options:\n");
    printf("  --help        show this help message and exit\n");
    printf("required arguments:\n");
    printf("  --endpoint    IoT endpoint hostname\n");
    printf("  --cert        Path to the certificate file\n");
    printf("  --key         Path to the private key file\n");
    printf("  --thing_name  Thing name\n");
    printf("optional arguments:\n");
    printf("  --client_id   Client ID (default: test-<uuid>)\n");
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
            else if (strcmp(argv[i], "--thing_name") == 0)
            {
                args.thingName = argv[++i];
            }
            else if (strcmp(argv[i], "--client_id") == 0)
            {
                args.clientId = argv[++i];
            }

            else
            {
                fprintf(stderr, "Unknown argument: %s\n", argv[i]);
                printHelp();
                exit(1);
            }
        }
    }
    if (args.endpoint.empty() || args.cert.empty() || args.key.empty() || args.thingName.empty())
    {
        fprintf(stderr, "Error: --endpoint, --cert, --key, and --thing_name are required\n");
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

    ApplicationContext context;
    context.m_thingName = cmdData.thingName;

    // Create the MQTT5 builder and populate it with data from cmdData.
    auto builder = std::unique_ptr<Aws::Iot::Mqtt5ClientBuilder>(
        Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(
            cmdData.endpoint, cmdData.cert.c_str(), cmdData.key.c_str()));
    // Check if the builder setup correctly.
    if (builder == nullptr)
    {
        printf(
            "Failed to setup mqtt5 client builder with error code %d: %s", LastError(), ErrorDebugString(LastError()));
        return -1;
    }

    auto connectPacket = MakeShared<Mqtt5::ConnectPacket>(DefaultAllocatorImplementation());
    connectPacket->WithClientId(cmdData.clientId);

    builder->WithConnectOptions(connectPacket);

    std::promise<bool> connectedWaiter;

    // Setup lifecycle callbacks
    builder->WithClientConnectionSuccessCallback(
        [&connectedWaiter](const Mqtt5::OnConnectionSuccessEventData &)
        {
            fprintf(stdout, "Mqtt5 Client connection succeeded!\n");
            connectedWaiter.set_value(true);
        });
    builder->WithClientConnectionFailureCallback(s_onConnectionFailure);

    auto protocolClient = builder->Build();
    if (!protocolClient)
    {
        printf("Failed to create mqtt5 client with error code %d: %s", LastError(), ErrorDebugString(LastError()));
        return -1;
    }

    context.m_protocolClient = protocolClient;

    Aws::Iot::RequestResponse::RequestResponseClientOptions requestResponseOptions;
    requestResponseOptions.WithMaxRequestResponseSubscriptions(4);
    requestResponseOptions.WithMaxStreamingSubscriptions(10);
    requestResponseOptions.WithOperationTimeoutInSeconds(30);

    auto jobsClient = Aws::Iotjobs::NewClientFrom5(*context.m_protocolClient, requestResponseOptions);
    if (!jobsClient)
    {
        printf("Failed to create jobs client with error code %d: %s", LastError(), ErrorDebugString(LastError()));
        return -1;
    }

    context.m_jobsClient = jobsClient;

    fprintf(stdout, "Starting protocol client!\n");
    context.m_protocolClient->Start();

    connectedWaiter.get_future().wait();

    context.m_jobExecutionsChangedStream = s_createJobExecutionsChangedStream(context);
    context.m_nextJobExecutionChangedStream = s_createNextJobExecutionChangedStream(context);

    while (true)
    {
        fprintf(stdout, "\nEnter command:\n");

        String input;
        std::getline(std::cin, input);

        if (s_handleInput(input, context) || !std::cin)
        {
            fprintf(stdout, "Exiting...");
            break;
        }
    }

    return 0;
}
