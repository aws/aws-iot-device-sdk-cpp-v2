/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/UUID.h>
#include <aws/crt/io/HostResolver.h>
#include <aws/crt/mqtt/Mqtt5Packets.h>
#include <aws/iot/Mqtt5Client.h>
#include <aws/iot/MqttClient.h>
#include <aws/iotjobs/DescribeJobExecutionRequest.h>
#include <aws/iotjobs/DescribeJobExecutionResponse.h>
#include <aws/iotjobs/DescribeJobExecutionSubscriptionRequest.h>
#include <aws/iotjobs/GetPendingJobExecutionsResponse.h>
#include <aws/iotjobs/GetPendingJobExecutionsSubscriptionRequest.h>
#include <aws/iotjobs/IotJobsClient.h>
#include <aws/iotjobs/JobExecutionSummary.h>
#include <aws/iotjobs/RejectedError.h>
#include <aws/iotjobs/StartNextJobExecutionResponse.h>
#include <aws/iotjobs/StartNextPendingJobExecutionRequest.h>
#include <aws/iotjobs/StartNextPendingJobExecutionSubscriptionRequest.h>
#include <aws/iotjobs/UpdateJobExecutionRequest.h>
#include <aws/iotjobs/UpdateJobExecutionSubscriptionRequest.h>

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

#include "../../utils/CommandLineUtils.h"

using namespace Aws::Crt;
using namespace Aws::Iotjobs;

void updateJobExecution(
    JobStatus status,
    String thingName,
    String currentJobId,
    IotJobsClient &jobsClient,
    int32_t &currentVersionNumber,
    int64_t &currentExecutionNumber);

int main(int argc, char *argv[])
{
    /************************ Setup ****************************/

    // Do the global initialization for the API
    ApiHandle apiHandle;

    /**
     * cmdData is the arguments/input from the command line placed into a single struct for
     * use in this sample. This handles all of the command line parsing, validating, etc.
     * See the Utils/CommandLineUtils for more information.
     */
    Utils::cmdData cmdData = Utils::parseSampleInputJobs(argc, argv, &apiHandle);

    // Create the MQTT5 builder and populate it with data from cmdData.
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(
        cmdData.input_endpoint, cmdData.input_cert.c_str(), cmdData.input_key.c_str());

    // Check if the builder setup correctly.
    if (builder == nullptr)
    {
        printf(
            "Failed to setup mqtt5 client builder with error code %d: %s", LastError(), ErrorDebugString(LastError()));
        return -1;
    }

    // Setup connection options
    std::shared_ptr<Mqtt5::ConnectPacket> connectOptions = std::make_shared<Mqtt5::ConnectPacket>();
    connectOptions->WithClientId(cmdData.input_clientId);
    builder->WithConnectOptions(connectOptions);
    if (cmdData.input_port != 0)
    {
        builder->WithPort(static_cast<uint32_t>(cmdData.input_port));
    }

    std::promise<bool> connectionPromise;
    std::promise<void> stoppedPromise;

    // Setup lifecycle callbacks
    builder->WithClientConnectionSuccessCallback(
        [&connectionPromise](const Mqtt5::OnConnectionSuccessEventData &eventData) {
            fprintf(
                stdout,
                "Mqtt5 Client connection succeed, clientid: %s.\n",
                eventData.negotiatedSettings->getClientId().c_str());
            connectionPromise.set_value(true);
        });
    builder->WithClientConnectionFailureCallback([&connectionPromise](
                                                     const Mqtt5::OnConnectionFailureEventData &eventData) {
        fprintf(stdout, "Mqtt5 Client connection failed with error: %s.\n", aws_error_debug_str(eventData.errorCode));
        connectionPromise.set_value(false);
    });
    builder->WithClientStoppedCallback([&stoppedPromise](const Mqtt5::OnStoppedEventData &) {
        fprintf(stdout, "Mqtt5 Client stopped.\n");
        stoppedPromise.set_value();
    });

    // Create Mqtt5Client
    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> client = builder->Build();
    delete builder;
    /************************ Run the sample ****************************/

    fprintf(stdout, "Connecting...\n");
    if (!client->Start())
    {
        fprintf(stderr, "MQTT5 Connection failed to start");
        exit(-1);
    }

    if (connectionPromise.get_future().get())
    {
        IotJobsClient jobsClient(client);

        DescribeJobExecutionSubscriptionRequest describeJobExecutionSubscriptionRequest;
        describeJobExecutionSubscriptionRequest.ThingName = cmdData.input_thingName;
        describeJobExecutionSubscriptionRequest.JobId = cmdData.input_jobId;

        /**
         * This isn't absolutely necessary but since we're doing a publish almost immediately afterwards,
         * to be cautious make sure the subscribe has finished before doing the publish.
         */
        std::promise<void> subAckedPromise;
        auto subAckHandler = [&](int) {
            // if error code returns it will be recorded by the other callback
            subAckedPromise.set_value();
        };
        auto subscriptionHandler = [&](DescribeJobExecutionResponse *response, int ioErr) {
            if (ioErr)
            {
                fprintf(stderr, "Error %d occurred\n", ioErr);
                return;
            }
            fprintf(stdout, "Received Job:\n");
            fprintf(stdout, "Job Id: %s\n", response->Execution->JobId->c_str());
            fprintf(stdout, "ClientToken: %s\n", response->ClientToken->c_str());
            fprintf(stdout, "Execution Status: %s\n", JobStatusMarshaller::ToString(*response->Execution->Status));
        };

        jobsClient.SubscribeToDescribeJobExecutionAccepted(
            describeJobExecutionSubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, subscriptionHandler, subAckHandler);
        subAckedPromise.get_future().wait();

        subAckedPromise = std::promise<void>();
        auto failureHandler = [&](RejectedError *rejectedError, int ioErr) {
            if (ioErr)
            {
                fprintf(stderr, "Error %d occurred\n", ioErr);
                return;
            }
            if (rejectedError)
            {
                fprintf(stderr, "Service Error %d occurred\n", (int)rejectedError->Code.value());
                return;
            }
        };

        jobsClient.SubscribeToDescribeJobExecutionRejected(
            describeJobExecutionSubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, failureHandler, subAckHandler);
        subAckedPromise.get_future().wait();

        DescribeJobExecutionRequest describeJobExecutionRequest;
        describeJobExecutionRequest.ThingName = cmdData.input_thingName;
        describeJobExecutionRequest.JobId = cmdData.input_jobId;
        describeJobExecutionRequest.IncludeJobDocument = true;
        Aws::Crt::UUID uuid;
        describeJobExecutionRequest.ClientToken = uuid.ToString();
        std::promise<void> publishDescribeJobExeCompletedPromise;

        auto publishHandler = [&](int ioErr) {
            if (ioErr)
            {
                fprintf(stderr, "Error %d occurred\n", ioErr);
            }
            publishDescribeJobExeCompletedPromise.set_value();
        };

        jobsClient.PublishDescribeJobExecution(
            std::move(describeJobExecutionRequest), AWS_MQTT_QOS_AT_LEAST_ONCE, publishHandler);
        publishDescribeJobExeCompletedPromise.get_future().wait();

        if (cmdData.input_isCI == false)
        {
            Aws::Crt::String currentJobId;
            int64_t currentExecutionNumber;
            int32_t currentVersionNumber;

            std::promise<void> pendingExecutionPromise;

            {
                auto OnSubscribeToStartNextPendingJobExecutionAcceptedResponse =
                    [&](StartNextJobExecutionResponse *response, int ioErr) {
                        if (ioErr)
                        {
                            fprintf(stderr, "Error %d occurred\n", ioErr);
                            exit(-1);
                        }
                        if (response)
                        {
                            if (response->Execution.has_value())
                            {
                                fprintf(stdout, "Start Job %s\n", response->Execution.value().JobId.value().c_str());
                                currentJobId = response->Execution->JobId.value();
                                currentExecutionNumber = response->Execution->ExecutionNumber.value();
                                currentVersionNumber = response->Execution->VersionNumber.value();
                            }
                            else
                            {
                                exit(-1);
                            }
                        }
                        else
                        {
                            fprintf(stdout, "Could not get Job Id exiting\n");
                            exit(-1);
                        }
                        pendingExecutionPromise.set_value();
                    };

                StartNextPendingJobExecutionSubscriptionRequest subscriptionRequest;
                subscriptionRequest.ThingName = cmdData.input_thingName;
                subAckedPromise = std::promise<void>();
                jobsClient.SubscribeToStartNextPendingJobExecutionAccepted(
                    subscriptionRequest,
                    AWS_MQTT_QOS_AT_LEAST_ONCE,
                    OnSubscribeToStartNextPendingJobExecutionAcceptedResponse,
                    subAckHandler);

                subAckedPromise.get_future().wait();

                subAckedPromise = std::promise<void>();
                jobsClient.SubscribeToStartNextPendingJobExecutionRejected(
                    subscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, failureHandler, subAckHandler);

                subAckedPromise.get_future().wait();

                StartNextPendingJobExecutionRequest publishRequest;
                publishRequest.ThingName = cmdData.input_thingName;
                publishRequest.StepTimeoutInMinutes = 15L;

                publishDescribeJobExeCompletedPromise = std::promise<void>();
                jobsClient.PublishStartNextPendingJobExecution(
                    publishRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, publishHandler);

                pendingExecutionPromise.get_future().wait();
            }

            updateJobExecution(
                JobStatus::IN_PROGRESS,
                cmdData.input_thingName,
                currentJobId,
                jobsClient,
                currentVersionNumber,
                currentExecutionNumber);
            // Pretend doing some work
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            updateJobExecution(
                JobStatus::SUCCEEDED,
                cmdData.input_thingName,
                currentJobId,
                jobsClient,
                currentVersionNumber,
                currentExecutionNumber);
        }
    }
    // Wait just a little bit to let the console print
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    // Disconnect
    if (client->Stop())
    {
        stoppedPromise.get_future().wait();
    }
    return 0;
}

void updateJobExecution(
    JobStatus status,
    String thingName,
    String currentJobId,
    IotJobsClient &jobsClient,
    int32_t &currentVersionNumber,
    int64_t &currentExecutionNumber)
{
    std::promise<void> publishDescribeJobExeCompletedPromise;
    std::promise<void> pendingExecutionPromise = std::promise<void>();
    std::promise<void> subAckedPromise;

    UpdateJobExecutionSubscriptionRequest subscriptionRequest;
    subscriptionRequest.ThingName = thingName;
    subscriptionRequest.JobId = currentJobId;

    auto subAckHandler = [&](int) {
        // if error code returns it will be recorded by the other callback
        subAckedPromise.set_value();
    };
    auto failureHandler = [&](RejectedError *rejectedError, int ioErr) {
        if (ioErr)
        {
            fprintf(stderr, "Error %d occurred\n", ioErr);
            exit(-1);
        }
        if (rejectedError)
        {
            fprintf(
                stderr,
                "Service Error %d occurred. Message %s\n",
                (int)rejectedError->Code.value(),
                rejectedError->Message->c_str());
            return;
        }
    };
    auto subscribeHandler = [&](UpdateJobExecutionResponse *response, int ioErr) {
        (void)response;
        if (ioErr)
        {
            fprintf(stderr, "Error %d occurred\n", ioErr);
            exit(-1);
        }
        fprintf(stdout, "Marked job %s %s\n", currentJobId.c_str(), JobStatusMarshaller::ToString(status));
        pendingExecutionPromise.set_value();
    };

    auto publishHandler = [&](int ioErr) {
        if (ioErr)
        {
            fprintf(stderr, "Error %d occurred\n", ioErr);
            exit(-1);
        }
        publishDescribeJobExeCompletedPromise.set_value();
    };
    subAckedPromise = std::promise<void>();
    jobsClient.SubscribeToUpdateJobExecutionAccepted(
        subscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, subscribeHandler, subAckHandler);
    subAckedPromise.get_future().wait();

    subAckedPromise = std::promise<void>();
    jobsClient.SubscribeToUpdateJobExecutionRejected(
        subscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, failureHandler, subAckHandler);
    subAckedPromise.get_future().wait();

    UpdateJobExecutionRequest publishRequest;
    publishRequest.ThingName = thingName;
    publishRequest.JobId = currentJobId;
    publishRequest.ExecutionNumber = currentExecutionNumber;
    publishRequest.Status = status;
    publishRequest.ExpectedVersion = currentVersionNumber++;

    jobsClient.PublishUpdateJobExecution(publishRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, publishHandler);

    pendingExecutionPromise.get_future().wait();
}
