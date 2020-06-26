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

        class AWS_IOTJOBS_API UpdateJobExecutionRequest final
        {
          public:
            UpdateJobExecutionRequest() = default;

            UpdateJobExecutionRequest(const Crt::JsonView &doc);
            UpdateJobExecutionRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<Aws::Crt::String> ThingName;
            Aws::Crt::Optional<int64_t> ExecutionNumber;
            Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String>> StatusDetails;
            Aws::Crt::Optional<bool> IncludeJobExecutionState;
            Aws::Crt::Optional<Aws::Crt::String> JobId;
            Aws::Crt::Optional<int32_t> ExpectedVersion;
            Aws::Crt::Optional<bool> IncludeJobDocument;
            Aws::Crt::Optional<Aws::Iotjobs::JobStatus> Status;
            Aws::Crt::Optional<int64_t> StepTimeoutInMinutes;
            Aws::Crt::Optional<Aws::Crt::String> ClientToken;

          private:
            static void LoadFromObject(UpdateJobExecutionRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotjobs
} // namespace Aws
