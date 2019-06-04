/*
 * Copyright 2010-2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
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
#include <condition_variable>
#include <iostream>
#include <mutex>

using namespace Aws::Crt;
using namespace Aws::Iotjobs;

static void s_printHelp()
{
    fprintf(stdout, "Usage:\n");
    fprintf(
        stdout,
        "describe-job-execution --endpoint <endpoint> --cert <path to cert>"
        " --key <path to key> --ca_file <optional: path to custom ca>"
        " --thing_name <thing name> --job_id <job id>\n\n");
    fprintf(stdout, "endpoint: the endpoint of the mqtt server not including a port\n");
    fprintf(stdout, "cert: path to your client certificate in PEM format\n");
    fprintf(stdout, "key: path to your key in PEM format\n");
    fprintf(
        stdout,
        "ca_file: Optional, if the mqtt server uses a certificate that's not already"
        " in your trust store, set this.\n");
    fprintf(stdout, "\tIt's the path to a CA file in PEM format\n");
    fprintf(stdout, "thing_name: the name of your IOT thing\n");
    fprintf(stdout, "job_id: the job id you want to describe.\n");
}

bool s_cmdOptionExists(char **begin, char **end, const String &option)
{
    return std::find(begin, end, option) != end;
}

char *s_getCmdOption(char **begin, char **end, const String &option)
{
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    /************************ Setup the Lib ****************************/
    /*
     * These make debug output via ErrorDebugString() work.
     */
    LoadErrorStrings();

    /*
     * Do the global initialization for the API.
     */
    ApiHandle apiHandle;

    String endpoint;
    String certificatePath;
    String keyPath;
    String caFile;
    String thingName;
    String jobId;

    /*********************** Parse Arguments ***************************/
    if (!(s_cmdOptionExists(argv, argv + argc, "--endpoint") && s_cmdOptionExists(argv, argv + argc, "--cert") &&
          s_cmdOptionExists(argv, argv + argc, "--key") && s_cmdOptionExists(argv, argv + argc, "--thing_name") &&
          s_cmdOptionExists(argv, argv + argc, "--job_id")))
    {
        s_printHelp();
        return 0;
    }

    endpoint = s_getCmdOption(argv, argv + argc, "--endpoint");
    certificatePath = s_getCmdOption(argv, argv + argc, "--cert");
    keyPath = s_getCmdOption(argv, argv + argc, "--key");
    thingName = s_getCmdOption(argv, argv + argc, "--thing_name");
    jobId = s_getCmdOption(argv, argv + argc, "--job_id");

    if (s_cmdOptionExists(argv, argv + argc, "--ca_file"))
    {
        caFile = s_getCmdOption(argv, argv + argc, "--ca_file");
    }

    /********************** Now Setup an Mqtt Client ******************/
    /*
     * You need an event loop group to process IO events.
     * If you only have a few connections, 1 thread is ideal
     */
    Io::EventLoopGroup eventLoopGroup(1);
    if (!eventLoopGroup)
    {
        fprintf(
            stderr, "Event Loop Group Creation failed with error %s\n", ErrorDebugString(eventLoopGroup.LastError()));
        exit(-1);
    }

    Io::DefaultHostResolver defaultHostResolver(eventLoopGroup, 2, 30);
    Io::ClientBootstrap bootstrap(eventLoopGroup, defaultHostResolver);

    if (!bootstrap)
    {
        fprintf(stderr, "ClientBootstrap failed with error %s\n", ErrorDebugString(bootstrap.LastError()));
        exit(-1);
    }

    auto clientConfig = Aws::Iot::MqttClientConnectionConfigBuilder(certificatePath.c_str(), keyPath.c_str())
                            .WithEndpoint(endpoint)
                            .WithCertificateAuthority(caFile.c_str())
                            .Build();

    if (!clientConfig)
    {
        fprintf(stderr, "Client Configuration initialization failed with error %s\n", ErrorDebugString(LastError()));
        exit(-1);
    }

    /*
     * Now Create a client. This can not throw.
     * An instance of a client must outlive its connections.
     * It is the users responsibility to make sure of this.
     */
    Aws::Iot::MqttClient mqttClient(bootstrap);

    /*
     * Since no exceptions are used, always check the bool operator
     * when an error could have occured.
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

    if (!*connection)
    {
        fprintf(stderr, "MQTT Connection Creation failed with error %s\n", ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    /*
     * In a real world application you probably don't want to enforce synchronous behavior
     * but this is a sample console application, so we'll just do that with a condition variable.
     */
    std::condition_variable conditionVariable;
    std::atomic<bool> connectionSucceeded(false);
    std::atomic<bool> connectionClosed(false);
    std::atomic<bool> connectionCompleted(false);

    /*
     * This will execute when an mqtt connect has completed or failed.
     */
    auto onConnectionCompleted = [&](Mqtt::MqttConnection &, int errorCode, Mqtt::ReturnCode returnCode, bool) {
        if (errorCode)
        {
            fprintf(stdout, "Connection failed with error %s\n", ErrorDebugString(errorCode));
            connectionSucceeded = false;
        }
        else
        {
            fprintf(stdout, "Connection completed with return code %d\n", returnCode);
            connectionSucceeded = true;
        }

        connectionCompleted = true;
        conditionVariable.notify_one();
    };

    /*
     * Invoked when a disconnect message has completed.
     */
    auto onDisconnect = [&](Mqtt::MqttConnection &conn) {
        {
            fprintf(stdout, "Disconnect completed\n");
            connectionClosed = true;
        }
        conditionVariable.notify_one();
    };

    connection->OnConnectionCompleted = std::move(onConnectionCompleted);
    connection->OnDisconnect = std::move(onDisconnect);

    /*
     * Actually perform the connect dance.
     */
    fprintf(stdout, "Connecting...\n");
    if (!connection->Connect("client_id12335456", true, 0))
    {
        fprintf(stderr, "MQTT Connection failed with error %s\n", ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    std::mutex mutex;
    std::unique_lock<std::mutex> uniqueLock(mutex);
    conditionVariable.wait(uniqueLock, [&]() { return connectionSucceeded || connectionClosed; });

    if (connectionSucceeded)
    {
        IotJobsClient client(connection);

        DescribeJobExecutionSubscriptionRequest describeJobExecutionSubscriptionRequest;
        describeJobExecutionSubscriptionRequest.ThingName = thingName;
        describeJobExecutionSubscriptionRequest.JobId = jobId;

        // This isn't absolutely neccessary but since we're doing a publish almost immediately afterwards,
        // to be cautious make sure the subscribe has finished before doing the publish.
        auto subAckHandler = [&](int ioErr) {
            if (!ioErr)
            {
                conditionVariable.notify_one();
            }
        };

        auto subscriptionHandler = [&](DescribeJobExecutionResponse *response, int ioErr) {
            if (ioErr)
            {
                fprintf(stderr, "Error %d occurred\n", ioErr);
                conditionVariable.notify_one();
                return;
            }

            fprintf(stdout, "Received Job:\n");
            fprintf(stdout, "Job Id: %s\n", response->Execution->JobId->c_str());
            fprintf(stdout, "ClientToken: %s\n", response->ClientToken->c_str());
            fprintf(stdout, "Execution Status: %s\n", JobStatusMarshaller::ToString(*response->Execution->Status));
            conditionVariable.notify_one();
        };

        client.SubscribeToDescribeJobExecutionAccepted(
            describeJobExecutionSubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, subscriptionHandler, subAckHandler);
        conditionVariable.wait(uniqueLock);

        auto failureHandler = [&](RejectedError *rejectedError, int ioErr) {
            if (ioErr)
            {
                fprintf(stderr, "Error %d occurred\n", ioErr);
                conditionVariable.notify_one();
                return;
            }

            if (rejectedError)
            {
                fprintf(stderr, "Service Error %d occured\n", (int)rejectedError->Code.value());
                conditionVariable.notify_one();
                return;
            }
        };

        client.SubscribeToDescribeJobExecutionRejected(
            describeJobExecutionSubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, failureHandler, subAckHandler);
        conditionVariable.wait(uniqueLock);

        DescribeJobExecutionRequest describeJobExecutionRequest;
        describeJobExecutionRequest.ThingName = thingName;
        describeJobExecutionRequest.JobId = jobId;
        describeJobExecutionRequest.IncludeJobDocument = true;
        describeJobExecutionRequest.ClientToken = Aws::Crt::UUID();

        auto publishHandler = [&](int ioErr) {
            if (ioErr)
            {
                fprintf(stderr, "Error %d occurred\n", ioErr);
                conditionVariable.notify_one();
                return;
            }
        };

        client.PublishDescribeJobExecution(
            std::move(describeJobExecutionRequest), AWS_MQTT_QOS_AT_LEAST_ONCE, publishHandler);
        conditionVariable.wait(uniqueLock);
    }

    if (!connectionClosed)
    {
        /* Disconnect */
        connection->Disconnect();
        conditionVariable.wait(uniqueLock, [&]() { return connectionClosed.load(); });
    }
    return 0;
}
