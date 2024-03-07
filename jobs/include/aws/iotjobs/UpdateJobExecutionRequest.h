#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotjobs/JobStatus.h>

#include <aws/iotjobs/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotjobs
    {

        /**
         * Data needed to make an UpdateJobExecution request.
         *
         */
        class AWS_IOTJOBS_API UpdateJobExecutionRequest final
        {
          public:
            UpdateJobExecutionRequest() = default;

            UpdateJobExecutionRequest(const Crt::JsonView &doc);
            UpdateJobExecutionRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * The name of the thing associated with the device.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ThingName;

            /**
             * The unique identifier assigned to this job when it was created.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> JobId;

            /**
             * The new status for the job execution (IN_PROGRESS, FAILED, SUCCEEDED, or REJECTED). This must be
             * specified on every update.
             *
             */
            Aws::Crt::Optional<Aws::Iotjobs::JobStatus> Status;

            /**
             * A client token used to correlate requests and responses. Enter an arbitrary value here and it is
             * reflected in the response.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ClientToken;

            /**
             * A collection of name-value pairs that describe the status of the job execution. If not specified, the
             * statusDetails are unchanged.
             *
             */
            Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String>> StatusDetails;

            /**
             * The expected current version of the job execution. Each time you update the job execution, its version is
             * incremented. If the version of the job execution stored in the AWS IoT Jobs service does not match, the
             * update is rejected with a VersionMismatch error, and an ErrorResponse that contains the current job
             * execution status data is returned.
             *
             */
            Aws::Crt::Optional<int32_t> ExpectedVersion;

            /**
             * Optional. A number that identifies a job execution on a device. If not specified, the latest job
             * execution is used.
             *
             */
            Aws::Crt::Optional<int64_t> ExecutionNumber;

            /**
             * Optional. When included and set to true, the response contains the JobExecutionState field. The default
             * is false.
             *
             */
            Aws::Crt::Optional<bool> IncludeJobExecutionState;

            /**
             * Optional. When included and set to true, the response contains the JobDocument. The default is false.
             *
             */
            Aws::Crt::Optional<bool> IncludeJobDocument;

            /**
             * Specifies the amount of time this device has to finish execution of this job. If the job execution status
             * is not set to a terminal state before this timer expires, or before the timer is reset (by again calling
             * UpdateJobExecution, setting the status to IN_PROGRESS and specifying a new timeout value in this field)
             * the job execution status is set to TIMED_OUT. Setting or resetting this timeout has no effect on the job
             * execution timeout that might have been specified when the job was created (by using CreateJob with the
             * timeoutConfig).
             *
             */
            Aws::Crt::Optional<int64_t> StepTimeoutInMinutes;

          private:
            static void LoadFromObject(UpdateJobExecutionRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotjobs
} // namespace Aws
