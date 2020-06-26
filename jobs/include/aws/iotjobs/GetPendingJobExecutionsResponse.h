#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
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

        class AWS_IOTJOBS_API GetPendingJobExecutionsResponse final
        {
          public:
            GetPendingJobExecutionsResponse() = default;

            GetPendingJobExecutionsResponse(const Crt::JsonView &doc);
            GetPendingJobExecutionsResponse &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<Aws::Crt::Vector<Aws::Iotjobs::JobExecutionSummary>> QueuedJobs;
            Aws::Crt::Optional<Aws::Crt::DateTime> Timestamp;
            Aws::Crt::Optional<Aws::Crt::String> ClientToken;
            Aws::Crt::Optional<Aws::Crt::Vector<Aws::Iotjobs::JobExecutionSummary>> InProgressJobs;

          private:
            static void LoadFromObject(GetPendingJobExecutionsResponse &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotjobs
} // namespace Aws
