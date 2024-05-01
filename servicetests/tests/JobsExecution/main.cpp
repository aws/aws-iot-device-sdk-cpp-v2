/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/UUID.h>

#include <aws/crt/mqtt/Mqtt5Packets.h>
#include <aws/iot/Mqtt5Client.h>
#include <aws/iot/MqttClient.h>

#include <aws/iotjobs/DescribeJobExecutionRequest.h>
#include <aws/iotjobs/DescribeJobExecutionResponse.h>
#include <aws/iotjobs/DescribeJobExecutionSubscriptionRequest.h>
#include <aws/iotjobs/GetPendingJobExecutionsRequest.h>
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
#include <condition_variable>
#include <thread>

#include "../../../samples/utils/CommandLineUtils.h"
#include "JobsExecution.h"

using namespace Aws::Crt;
using namespace Aws::Iotjobs;

std::shared_ptr<IotJobsClient> build_mqtt3_client(
    Utils::cmdData &cmdData,
    std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> &connection,
    std::promise<bool> &connectionCompletedPromise,
    std::promise<void> &connectionClosedPromise)
{
    Aws::Iot::MqttClientConnectionConfigBuilder clientConfigBuilder;
    // Create the MQTT builder and populate it with data from cmdData.
    clientConfigBuilder =
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

    Aws::Iot::MqttClient client3 = Aws::Iot::MqttClient();
    connection = client3.NewConnection(clientConfig);
    if (!*connection)
    {
        fprintf(
            stderr,
            "MQTT Connection Creation failed with error %s\n",
            Aws::Crt::ErrorDebugString(connection->LastError()));
        exit(-1);
    }

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

    if (!connection->Connect(cmdData.input_clientId.c_str(), true, 0))
    {
        fprintf(stderr, "MQTT Connection failed with error %s\n", ErrorDebugString(connection->LastError()));
        exit(-1);
    }
    return std::make_shared<IotJobsClient>(connection);
}

std::shared_ptr<IotJobsClient> build_mqtt5_client(
    Utils::cmdData &cmdData,
    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> &client5,
    std::promise<bool> &connectionCompletedPromise,
    std::promise<void> &connectionClosedPromise)
{
    std::shared_ptr<Aws::Iot::Mqtt5ClientBuilder> builder(
        Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(
            cmdData.input_endpoint, cmdData.input_cert.c_str(), cmdData.input_key.c_str()));

    // Check if the builder setup correctly.
    if (builder == nullptr)
    {
        printf(
            "Failed to setup mqtt5 client builder with error code %d: %s", LastError(), ErrorDebugString(LastError()));
        return nullptr;
    }
    // Create the MQTT5 builder and populate it with data from cmdData.
    // Setup connection options
    std::shared_ptr<Mqtt5::ConnectPacket> connectOptions = std::make_shared<Mqtt5::ConnectPacket>();
    connectOptions->WithClientId(cmdData.input_clientId);
    builder->WithConnectOptions(connectOptions);
    if (cmdData.input_port != 0)
    {
        builder->WithPort(static_cast<uint32_t>(cmdData.input_port));
    }
    // Setup lifecycle callbacks
    builder->WithClientConnectionSuccessCallback(
        [&connectionCompletedPromise](const Mqtt5::OnConnectionSuccessEventData &eventData) {
            fprintf(
                stdout,
                "Mqtt5 Client connection succeed, clientid: %s.\n",
                eventData.negotiatedSettings->getClientId().c_str());
            connectionCompletedPromise.set_value(true);
        });
    builder->WithClientConnectionFailureCallback([&connectionCompletedPromise](
                                                     const Mqtt5::OnConnectionFailureEventData &eventData) {
        fprintf(stdout, "Mqtt5 Client connection failed with error: %s.\n", aws_error_debug_str(eventData.errorCode));
        connectionCompletedPromise.set_value(false);
    });
    builder->WithClientStoppedCallback([&connectionClosedPromise](const Mqtt5::OnStoppedEventData &) {
        fprintf(stdout, "Mqtt5 Client stopped.\n");
        connectionClosedPromise.set_value();
    });

    client5 = builder->Build();
    if (client5 == nullptr)
    {
        fprintf(
            stdout, "Failed to Init Mqtt5Client with error code %d: %s.\n", LastError(), ErrorDebugString(LastError()));
        exit(-1);
    }

    if (!client5->Start())
    {
        fprintf(stderr, "MQTT5 Connection failed to start");
        exit(-1);
    }
    return std::make_shared<IotJobsClient>(client5);
}

int main(int argc, char *argv[])
{
    fprintf(stdout, "Starting the jobs execution program\n");
    /************************ Setup ****************************/

    // Do the global initialization for the API
    ApiHandle apiHandle;

    /**
     * cmdData is the arguments/input from the command line placed into a single struct for
     * use in this sample. This handles all of the command line parsing, validating, etc.
     * See the Utils/CommandLineUtils for more information.
     */
    Utils::cmdData cmdData = Utils::parseSampleInputJobs(argc, argv, &apiHandle);

    /**
     * In a real world application you probably don't want to enforce synchronous behavior
     * but this is a sample console application, so we'll just do that with a condition variable.
     */
    std::promise<bool> connectionCompletedPromise;
    std::promise<void> connectionClosedPromise;
    std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> connection = nullptr;
    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> client5 = nullptr;
    std::shared_ptr<IotJobsClient> jobsClient = nullptr;

    if (cmdData.input_mqtt_version == 5UL)
    {
        jobsClient = build_mqtt5_client(cmdData, client5, connectionCompletedPromise, connectionClosedPromise);
    }
    else if (cmdData.input_mqtt_version == 3UL)
    {
        jobsClient = build_mqtt3_client(cmdData, connection, connectionCompletedPromise, connectionClosedPromise);
    }
    else
    {
        fprintf(stderr, "MQTT Version not supported\n");
        exit(-1);
    }
    /************************ Run the sample ****************************/
    if (connectionCompletedPromise.get_future().get())
    {
        JobsExecution jobsExecution(jobsClient, cmdData.input_thingName);
        auto availableJobs = jobsExecution.getAvailableJobs();
        for (const auto &jobId : availableJobs)
        {
            jobsExecution.describeJob(jobId);
        }

        for (size_t idx = 0; idx < availableJobs.size(); ++idx)
        {
            jobsExecution.startNextPendingJob();
            jobsExecution.updateCurrentJobStatus(Aws::Iotjobs::JobStatus::SUCCEEDED);
        }
    }
    // Wait just a little bit to let the console print
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    if (cmdData.input_mqtt_version == 5UL)
    {
        // Disconnect
        if (client5->Stop() == true)
        {
            connectionClosedPromise.get_future().wait();
        }
    }
    else
    { // mqtt3

        // Disconnect
        if (connection->Disconnect() == true)
        {
            connectionClosedPromise.get_future().wait();
        }
    }
    return 0;
}
