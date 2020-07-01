#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
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

        class AWS_IOTJOBS_API JobExecutionData final
        {
          public:
            JobExecutionData() = default;

            JobExecutionData(const Crt::JsonView &doc);
            JobExecutionData &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<Aws::Crt::String> JobId;
            Aws::Crt::Optional<Aws::Crt::JsonObject> JobDocument;
            Aws::Crt::Optional<Aws::Iotjobs::JobStatus> Status;
            Aws::Crt::Optional<int32_t> VersionNumber;
            Aws::Crt::Optional<Aws::Crt::DateTime> QueuedAt;
            Aws::Crt::Optional<Aws::Crt::String> ThingName;
            Aws::Crt::Optional<int64_t> ExecutionNumber;
            Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String>> StatusDetails;
            Aws::Crt::Optional<Aws::Crt::DateTime> LastUpdatedAt;
            Aws::Crt::Optional<Aws::Crt::DateTime> StartedAt;

          private:
            static void LoadFromObject(JobExecutionData &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotjobs
} // namespace Aws
