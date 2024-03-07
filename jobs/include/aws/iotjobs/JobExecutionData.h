#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/crt/DateTime.h>
#include <aws/crt/JsonObject.h>
#include <aws/iotjobs/JobStatus.h>

#include <aws/iotjobs/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotjobs
    {

        /**
         * Data about a job execution.
         *
         */
        class AWS_IOTJOBS_API JobExecutionData final
        {
          public:
            JobExecutionData() = default;

            JobExecutionData(const Crt::JsonView &doc);
            JobExecutionData &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * The unique identifier you assigned to this job when it was created.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> JobId;

            /**
             * The name of the thing that is executing the job.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ThingName;

            /**
             * The content of the job document.
             *
             */
            Aws::Crt::Optional<Aws::Crt::JsonObject> JobDocument;

            /**
             * The status of the job execution. Can be one of: QUEUED, IN_PROGRESS, FAILED, SUCCEEDED, CANCELED,
             * TIMED_OUT, REJECTED, or REMOVED.
             *
             */
            Aws::Crt::Optional<Aws::Iotjobs::JobStatus> Status;

            /**
             * A collection of name-value pairs that describe the status of the job execution.
             *
             */
            Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String>> StatusDetails;

            /**
             * The time when the job execution was enqueued.
             *
             */
            Aws::Crt::Optional<Aws::Crt::DateTime> QueuedAt;

            /**
             * The time when the job execution started.
             *
             */
            Aws::Crt::Optional<Aws::Crt::DateTime> StartedAt;

            /**
             * The time when the job execution started.
             *
             */
            Aws::Crt::Optional<Aws::Crt::DateTime> LastUpdatedAt;

            /**
             * The version of the job execution. Job execution versions are incremented each time they are updated by a
             * device.
             *
             */
            Aws::Crt::Optional<int32_t> VersionNumber;

            /**
             * A number that identifies a job execution on a device. It can be used later in commands that return or
             * update job execution information.
             *
             */
            Aws::Crt::Optional<int64_t> ExecutionNumber;

          private:
            static void LoadFromObject(JobExecutionData &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotjobs
} // namespace Aws
