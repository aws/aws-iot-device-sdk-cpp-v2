/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "JobsExecution.h"

#include <aws/crt/UUID.h>

#include <aws/iotjobs/DescribeJobExecutionRequest.h>
#include <aws/iotjobs/DescribeJobExecutionResponse.h>
#include <aws/iotjobs/DescribeJobExecutionSubscriptionRequest.h>
#include <aws/iotjobs/GetPendingJobExecutionsRequest.h>
#include <aws/iotjobs/GetPendingJobExecutionsResponse.h>
#include <aws/iotjobs/GetPendingJobExecutionsSubscriptionRequest.h>
#include <aws/iotjobs/JobExecutionSummary.h>
#include <aws/iotjobs/RejectedError.h>
#include <aws/iotjobs/StartNextJobExecutionResponse.h>
#include <aws/iotjobs/StartNextPendingJobExecutionRequest.h>
#include <aws/iotjobs/StartNextPendingJobExecutionSubscriptionRequest.h>
#include <aws/iotjobs/UpdateJobExecutionRequest.h>
#include <aws/iotjobs/UpdateJobExecutionSubscriptionRequest.h>

using namespace Aws::Crt;
using namespace Aws::Iotjobs;

JobsExecution::JobsExecution(std::shared_ptr<Aws::Iotjobs::IotJobsClient> jobsClient, Aws::Crt::String thingName)
    : m_jobsClient(std::move(jobsClient)), m_thingName(std::move(thingName)), m_currentExecutionNumber(), m_currentVersionNumber()
{
}

Aws::Crt::Vector<Aws::Crt::String> JobsExecution::getAvailableJobs()
{
    GetPendingJobExecutionsSubscriptionRequest subscriptionRequest;
    subscriptionRequest.ThingName = m_thingName;

    auto handler = [this](Aws::Iotjobs::GetPendingJobExecutionsResponse *response, int ioErr) {
        fprintf(stderr, "running the jobs handler\n");
        if (ioErr)
        {
            fprintf(stderr, "Error %d occurred\n", ioErr);
            exit(1);
        }
        if (response)
        {
            if (response->InProgressJobs.has_value())
            {
                for (const JobExecutionSummary &job : response->InProgressJobs.value())
                {
                    std::lock_guard<std::mutex> lock(m_jobsMutex);
                    m_availableJobs.push_back(job.JobId.value());
                    fprintf(stderr, "In Progress jobs %s\n", job.JobId->c_str());
                }
            }
            else
            {
                fprintf(stderr, "In Progress jobs: empty\n");
            }
            if (response->QueuedJobs.has_value())
            {
                for (const JobExecutionSummary &job : response->QueuedJobs.value())
                {
                    std::lock_guard<std::mutex> lock(m_jobsMutex);
                    m_availableJobs.push_back(job.JobId.value());
                    fprintf(stderr, "Queued  jobs %s\n", job.JobId->c_str());
                }
            }
            else
            {
                fprintf(stderr, "Queued  jobs: empty\n");
            }
        }
        m_getResponse.set_value();
    };

    auto err_handler = [](Aws::Iotjobs::RejectedError *rejectedError, int ioErr) {
        if (ioErr)
        {
            fprintf(stderr, "Error %d occurred\n", ioErr);
            exit(1);
        }
        if (rejectedError)
        {
            fprintf(
                stderr,
                "Service Error %d occurred. Message %s\n",
                (int)rejectedError->Code.value(),
                rejectedError->Message->c_str());
        }
        fprintf(stderr, "Error handler\n");
        exit(-1);
    };

    std::promise<void> publishDescribeJobExeCompletedPromise;

    auto publishHandler = [&publishDescribeJobExeCompletedPromise](int ioErr) {
        if (ioErr)
        {
            fprintf(stderr, "Error %d occurred\n", ioErr);
            exit(1);
        }
        publishDescribeJobExeCompletedPromise.set_value();
    };

    m_jobsClient->SubscribeToGetPendingJobExecutionsAccepted(
        subscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, handler, publishHandler);
    publishDescribeJobExeCompletedPromise.get_future().wait();

    publishDescribeJobExeCompletedPromise = std::promise<void>();
    m_jobsClient->SubscribeToGetPendingJobExecutionsRejected(
        subscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, err_handler, publishHandler);
    publishDescribeJobExeCompletedPromise.get_future().wait();

    publishDescribeJobExeCompletedPromise = std::promise<void>();
    GetPendingJobExecutionsRequest publishRequest;
    publishRequest.ThingName = m_thingName;
    m_jobsClient->PublishGetPendingJobExecutions(publishRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, publishHandler);
    publishDescribeJobExeCompletedPromise.get_future().wait();

    if (m_getResponse.get_future().wait_for(std::chrono::seconds(10)) == std::future_status::timeout)
    {
        fprintf(stderr, "get available jobs error timedout\n");
        exit(-1);
    }

    std::lock_guard<std::mutex> lock(m_jobsMutex);
    return m_availableJobs;
}

void JobsExecution::describeJob(const String &jobId) {
    DescribeJobExecutionSubscriptionRequest describeJobExecutionSubscriptionRequest;
    describeJobExecutionSubscriptionRequest.ThingName = m_thingName;
    describeJobExecutionSubscriptionRequest.JobId = jobId;

    /**
     * This isn't absolutely necessary but since we're doing a publish almost immediately afterwards,
     * to be cautious make sure the subscribe has finished before doing the publish.
     */
    std::promise<void> subAckedPromise;
    auto subAckHandler = [&subAckedPromise](int) {
        // if error code returns it will be recorded by the other callback
        subAckedPromise.set_value();
    };
    auto subscriptionHandler = [&](DescribeJobExecutionResponse *response, int ioErr) {
        if (ioErr)
        {
            fprintf(stderr, "Error %d occurred\n", ioErr);
            return;
        }
        if (response)
        {
            fprintf(stdout, "Received Job:\n");
            fprintf(stdout, "Job Id: %s\n", response->Execution->JobId->c_str());
            fprintf(stdout, "ClientToken: %s\n", response->ClientToken->c_str());
            fprintf(stdout, "Execution Status: %s\n", JobStatusMarshaller::ToString(*response->Execution->Status));
        }
    };

    m_jobsClient->SubscribeToDescribeJobExecutionAccepted(
        describeJobExecutionSubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, subscriptionHandler, subAckHandler);
    subAckedPromise.get_future().wait();

    subAckedPromise = std::promise<void>();

    auto failureHandler = [](RejectedError *rejectedError, int ioErr) {
        if (ioErr)
        {
            fprintf(stderr, "Error %d occurred\n", ioErr);
            return;
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

    m_jobsClient->SubscribeToDescribeJobExecutionRejected(
        describeJobExecutionSubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, failureHandler, subAckHandler);
    subAckedPromise.get_future().wait();

    DescribeJobExecutionRequest describeJobExecutionRequest;
    describeJobExecutionRequest.ThingName = m_thingName;
    describeJobExecutionRequest.JobId = jobId;
    describeJobExecutionRequest.IncludeJobDocument = true;
    Aws::Crt::UUID uuid;
    describeJobExecutionRequest.ClientToken = uuid.ToString();
    std::promise<void> publishDescribeJobExeCompletedPromise;

    auto publishHandler = [&publishDescribeJobExeCompletedPromise](int ioErr) {
        if (ioErr)
        {
            fprintf(stderr, "Error %d occurred\n", ioErr);
            exit(1);
        }
        publishDescribeJobExeCompletedPromise.set_value();
    };

    m_jobsClient->PublishDescribeJobExecution(describeJobExecutionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, publishHandler);
    publishDescribeJobExeCompletedPromise.get_future().wait();
}

void JobsExecution::startNextPendingJob() {
    std::promise<void> subAckedPromise;
    auto subAckHandler = [&subAckedPromise](int) {
        // if error code returns it will be recorded by the other callback
        subAckedPromise.set_value();
    };

    auto failureHandler = [](RejectedError *rejectedError, int ioErr) {
        if (ioErr)
        {
            fprintf(stderr, "Error %d occurred\n", ioErr);
            return;
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

    Aws::Crt::String jobId;

    auto OnSubscribeToStartNextPendingJobExecutionAcceptedResponse = [this](StartNextJobExecutionResponse *response,
                                                                         int ioErr) {
        if (ioErr)
        {
            fprintf(stderr, "Error %d occurred\n", ioErr);
            exit(1);
        }
        if (response && response->Execution.has_value())
        {
            fprintf(stdout, "Start Job %s\n", response->Execution.value().JobId.value().c_str());
            // Make tsan happy.
            std::lock_guard<std::mutex> lock(m_jobsMutex);
            m_currentJobId = response->Execution->JobId.value();
            m_currentExecutionNumber = response->Execution->ExecutionNumber.value();
            m_currentVersionNumber = response->Execution->VersionNumber.value();
        }
        else
        {
            fprintf(stdout, "Could not get Job Id, exiting\n");
            exit(-1);
        }

        m_pendingExecutionPromise.set_value();
    };

    StartNextPendingJobExecutionSubscriptionRequest subscriptionRequest;
    subscriptionRequest.ThingName = m_thingName;
    subAckedPromise = std::promise<void>();
    m_jobsClient->SubscribeToStartNextPendingJobExecutionAccepted(
        subscriptionRequest,
        AWS_MQTT_QOS_AT_LEAST_ONCE,
        OnSubscribeToStartNextPendingJobExecutionAcceptedResponse,
        subAckHandler);

    subAckedPromise.get_future().wait();

    subAckedPromise = std::promise<void>();
    m_jobsClient->SubscribeToStartNextPendingJobExecutionRejected(
        subscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, failureHandler, subAckHandler);

    subAckedPromise.get_future().wait();

    StartNextPendingJobExecutionRequest publishRequest;
    publishRequest.ThingName = m_thingName;
    publishRequest.StepTimeoutInMinutes = 15L;

    std::promise<void> publishDescribeJobExeCompletedPromise;

    auto publishHandler = [&publishDescribeJobExeCompletedPromise](int ioErr) {
        if (ioErr)
        {
            fprintf(stderr, "Error %d occurred\n", ioErr);
            exit(1);
        }
        publishDescribeJobExeCompletedPromise.set_value();
    };
    m_jobsClient->PublishStartNextPendingJobExecution(publishRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, publishHandler);

    m_pendingExecutionPromise.get_future().wait();
}

void JobsExecution::updateCurrentJobStatus(Aws::Iotjobs::JobStatus jobStatus)
{
    Aws::Crt::String jobId;
    int64_t currentExecutionNumber;
    int32_t currentVersionNumber;

    {
        // Make tsan happy.
        std::lock_guard<std::mutex> lock(m_jobsMutex);
        jobId = m_currentJobId;
        currentExecutionNumber = m_currentExecutionNumber;
        currentVersionNumber = m_currentVersionNumber;
    }

    std::promise<void> subAckedPromise;
    auto subAckHandler = [&subAckedPromise](int) {
        // if error code returns it will be recorded by the other callback
        subAckedPromise.set_value();
    };

    auto failureHandler = [](RejectedError *rejectedError, int ioErr) {
        if (ioErr)
        {
            fprintf(stderr, "Error %d occurred\n", ioErr);
            return;
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

    m_pendingExecutionPromise = std::promise<void>();
    auto OnSubscribeToUpdateJobExecutionAcceptedResponse = [this, jobId](UpdateJobExecutionResponse *response, int ioErr) {
        (void)response;
        if (ioErr)
        {
            fprintf(stderr, "Error %d occurred\n", ioErr);
            exit(1);
        }
        fprintf(stdout, "Marked Job %s IN_PROGRESS", jobId.c_str());
        m_pendingExecutionPromise.set_value();
    };
    UpdateJobExecutionSubscriptionRequest subscriptionRequest;
    subscriptionRequest.ThingName = m_thingName;
    subscriptionRequest.JobId = jobId;

    subAckedPromise = std::promise<void>();
    m_jobsClient->SubscribeToUpdateJobExecutionAccepted(
        subscriptionRequest,
        AWS_MQTT_QOS_AT_LEAST_ONCE,
        OnSubscribeToUpdateJobExecutionAcceptedResponse,
        subAckHandler);
    subAckedPromise.get_future().wait();

    subAckedPromise = std::promise<void>();
    m_jobsClient->SubscribeToUpdateJobExecutionRejected(
        subscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, failureHandler, subAckHandler);
    subAckedPromise.get_future().wait();

    std::promise<void> publishDescribeJobExeCompletedPromise;

    auto publishHandler = [&publishDescribeJobExeCompletedPromise](int ioErr) {
        if (ioErr)
        {
            fprintf(stderr, "Error %d occurred\n", ioErr);
            exit(1);
        }
        publishDescribeJobExeCompletedPromise.set_value();
    };

    UpdateJobExecutionRequest publishRequest;
    publishRequest.ThingName = m_thingName;
    publishRequest.JobId = jobId;
    publishRequest.ExecutionNumber = currentExecutionNumber;
    publishRequest.Status = jobStatus;
    publishRequest.ExpectedVersion = currentVersionNumber++;
    m_jobsClient->PublishUpdateJobExecution(publishRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, publishHandler);

    m_pendingExecutionPromise.get_future().wait();
}
