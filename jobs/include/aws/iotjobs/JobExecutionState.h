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
         * Data about the state of a job execution.
         *
         */
        class AWS_IOTJOBS_API JobExecutionState final
        {
          public:
            JobExecutionState() = default;

            JobExecutionState(const Crt::JsonView &doc);
            JobExecutionState &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

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
             * The version of the job execution. Job execution versions are incremented each time they are updated by a
             * device.
             *
             */
            Aws::Crt::Optional<int32_t> VersionNumber;

          private:
            static void LoadFromObject(JobExecutionState &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotjobs
} // namespace Aws
