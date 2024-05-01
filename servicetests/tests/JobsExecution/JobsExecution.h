#pragma once

/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/iotjobs/IotJobsClient.h>
#include <aws/iotjobs/JobStatus.h>

#include <future>

class JobsExecution
{
  public:
    JobsExecution(std::shared_ptr<Aws::Iotjobs::IotJobsClient> jobsClient, Aws::Crt::String thingName);

    Aws::Crt::Vector<Aws::Crt::String> getAvailableJobs();

    void describeJob(const Aws::Crt::String &jobId);

    void startNextPendingJob();

    void updateCurrentJobStatus(Aws::Iotjobs::JobStatus jobStatus);

  private:
    std::shared_ptr<Aws::Iotjobs::IotJobsClient> m_jobsClient;
    const Aws::Crt::String m_thingName;

    Aws::Crt::Vector<Aws::Crt::String> m_availableJobs;
    std::mutex m_jobsMutex;
    std::promise<void> m_getResponse;

    Aws::Crt::String m_currentJobId;
    int64_t m_currentExecutionNumber;
    int32_t m_currentVersionNumber;

    std::promise<void> m_pendingExecutionPromise;
};
