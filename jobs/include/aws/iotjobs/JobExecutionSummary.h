#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/DateTime.h>

#include <aws/iotjobs/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotjobs
    {

        class AWS_IOTJOBS_API JobExecutionSummary final
        {
          public:
            JobExecutionSummary() = default;

            JobExecutionSummary(const Crt::JsonView &doc);
            JobExecutionSummary &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<Aws::Crt::DateTime> LastUpdatedAt;
            Aws::Crt::Optional<int64_t> ExecutionNumber;
            Aws::Crt::Optional<Aws::Crt::DateTime> StartedAt;
            Aws::Crt::Optional<int32_t> VersionNumber;
            Aws::Crt::Optional<Aws::Crt::String> JobId;
            Aws::Crt::Optional<Aws::Crt::DateTime> QueuedAt;

          private:
            static void LoadFromObject(JobExecutionSummary &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotjobs
} // namespace Aws
