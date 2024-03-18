#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/crt/DateTime.h>

#include <aws/iotjobs/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotjobs
    {

        /**
         * Contains a subset of information about a job execution.
         *
         */
        class AWS_IOTJOBS_API JobExecutionSummary final
        {
          public:
            JobExecutionSummary() = default;

            JobExecutionSummary(const Crt::JsonView &doc);
            JobExecutionSummary &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * The unique identifier you assigned to this job when it was created.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> JobId;

            /**
             * A number that identifies a job execution on a device.
             *
             */
            Aws::Crt::Optional<int64_t> ExecutionNumber;

            /**
             * The version of the job execution. Job execution versions are incremented each time the AWS IoT Jobs
             * service receives an update from a device.
             *
             */
            Aws::Crt::Optional<int32_t> VersionNumber;

            /**
             * The time when the job execution was last updated.
             *
             */
            Aws::Crt::Optional<Aws::Crt::DateTime> LastUpdatedAt;

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

          private:
            static void LoadFromObject(JobExecutionSummary &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotjobs
} // namespace Aws
