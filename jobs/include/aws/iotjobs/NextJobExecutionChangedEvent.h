#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/crt/DateTime.h>
#include <aws/iotjobs/JobExecutionData.h>

#include <aws/iotjobs/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotjobs
    {

        /**
         * Sent whenever there is a change to which job execution is next on the list of pending job executions for a
         * thing, as defined for DescribeJobExecution with jobId $next. This message is not sent when the next job's
         * execution details change, only when the next job that would be returned by DescribeJobExecution with jobId
         * $next has changed.
         *
         */
        class AWS_IOTJOBS_API NextJobExecutionChangedEvent final
        {
          public:
            NextJobExecutionChangedEvent() = default;

            NextJobExecutionChangedEvent(const Crt::JsonView &doc);
            NextJobExecutionChangedEvent &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * Contains data about a job execution.
             *
             */
            Aws::Crt::Optional<Aws::Iotjobs::JobExecutionData> Execution;

            /**
             * The time when the message was sent.
             *
             */
            Aws::Crt::Optional<Aws::Crt::DateTime> Timestamp;

          private:
            static void LoadFromObject(NextJobExecutionChangedEvent &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotjobs
} // namespace Aws
