/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/UUID.h>
#include <aws/crt/io/HostResolver.h>

#include <aws/iot/MqttClient.h>

#include <aws/iotjobs/DescribeJobExecutionRequest.h>
#include <aws/iotjobs/DescribeJobExecutionResponse.h>
#include <aws/iotjobs/DescribeJobExecutionSubscriptionRequest.h>
#include <aws/iotjobs/IotJobsClient.h>
#include <aws/iotjobs/RejectedError.h>

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

#include "../../utils/CommandLineUtils.h"

using namespace Aws::Crt;
using namespace Aws::Iotjobs;

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

    // Create the MQTT builder and populate it with data from cmdData.
    auto clientConfigBuilder =
        Aws::Iot::MqttClientConnectionConfigBuilder(cmdData.input_cert.c_str(), cmdData.input_key.c_str());
    clientConfigBuilder.WithEndpoint(cmdData.input_endpoint);
    if (cmdData.input_ca != "")
    {
        clientConfigBuilder.WithCertificateAuthority(cmdData.input_ca.c_str());
    }

    // Create the MQTT connection from the MQTT builder
    auto clientConfig = clientConfigBuilder.Build();
    if (!clientConfig)
    {
        fprintf(
            stderr,
            "Client Configuration initialization failed with error %s\n",
            Aws::Crt::ErrorDebugString(clientConfig.LastError()));
        exit(-1);
    }
    Aws::Iot::MqttClient client = Aws::Iot::MqttClient();
    auto connection = client.NewConnection(clientConfig);
    if (!*connection)
    {
        fprintf(
            stderr,
            "MQTT Connection Creation failed with error %s\n",
            Aws::Crt::ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    /**
     * In a real world application you probably don't want to enforce synchronous behavior
     * but this is a sample console application, so we'll just do that with a condition variable.
     */
    std::promise<bool> connectionCompletedPromise;
    std::promise<void> connectionClosedPromise;

    // Invoked when a MQTT connect has completed or failed
    auto onConnectionCompleted = [&](Mqtt::MqttConnection &, int errorCode, Mqtt::ReturnCode returnCode, bool) {
        if (errorCode)
        {
            fprintf(stdout, "Connection failed with error %s\n", ErrorDebugString(errorCode));
            connectionCompletedPromise.set_value(false);
        }
        else
        {
            fprintf(stdout, "Connection completed with return code %d\n", returnCode);
            connectionCompletedPromise.set_value(true);
        }
    };

    // Invoked when a disconnect has been completed
    auto onDisconnect = [&](Mqtt::MqttConnection & /*conn*/) {
        {
            fprintf(stdout, "Disconnect completed\n");
            connectionClosedPromise.set_value();
        }
    };

    connection->OnConnectionCompleted = std::move(onConnectionCompleted);
    connection->OnDisconnect = std::move(onDisconnect);

    /************************ Run the sample ****************************/

    fprintf(stdout, "Connecting...\n");
    if (!connection->Connect(cmdData.input_clientId.c_str(), true, 0))
    {
        fprintf(stderr, "MQTT Connection failed with error %s\n", ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    if (connectionCompletedPromise.get_future().get())
    {
        IotJobsClient jobsClient(connection);

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
    }

    // Wait just a little bit to let the console print
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Disconnect
    if (connection->Disconnect())
    {
        connectionClosedPromise.get_future().wait();
    }

    return 0;
}
