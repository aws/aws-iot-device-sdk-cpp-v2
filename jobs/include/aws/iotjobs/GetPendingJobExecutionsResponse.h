#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/crt/DateTime.h>
#include <aws/iotjobs/JobExecutionSummary.h>

#include <aws/iotjobs/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotjobs
    {

        /**
         * Response payload to a GetPendingJobExecutions request.
         *
         */
        class AWS_IOTJOBS_API GetPendingJobExecutionsResponse final
        {
          public:
            GetPendingJobExecutionsResponse() = default;

            GetPendingJobExecutionsResponse(const Crt::JsonView &doc);
            GetPendingJobExecutionsResponse &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * A list of JobExecutionSummary objects with status IN_PROGRESS.
             *
             */
            Aws::Crt::Optional<Aws::Crt::Vector<Aws::Iotjobs::JobExecutionSummary>> InProgressJobs;

            /**
             * A list of JobExecutionSummary objects with status QUEUED.
             *
             */
            Aws::Crt::Optional<Aws::Crt::Vector<Aws::Iotjobs::JobExecutionSummary>> QueuedJobs;

            /**
             * The time when the message was sent.
             *
             */
            Aws::Crt::Optional<Aws::Crt::DateTime> Timestamp;

            /**
             * A client token used to correlate requests and responses.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ClientToken;

          private:
            static void LoadFromObject(GetPendingJobExecutionsResponse &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotjobs
} // namespace Aws
