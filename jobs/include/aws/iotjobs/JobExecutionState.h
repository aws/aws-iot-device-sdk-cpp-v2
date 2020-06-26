#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/iotjobs/JobStatus.h>

#include <aws/iotjobs/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotjobs
    {

        class AWS_IOTJOBS_API JobExecutionState final
        {
          public:
            JobExecutionState() = default;

            JobExecutionState(const Crt::JsonView &doc);
            JobExecutionState &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String>> StatusDetails;
            Aws::Crt::Optional<int32_t> VersionNumber;
            Aws::Crt::Optional<Aws::Iotjobs::JobStatus> Status;

          private:
            static void LoadFromObject(JobExecutionState &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotjobs
} // namespace Aws
