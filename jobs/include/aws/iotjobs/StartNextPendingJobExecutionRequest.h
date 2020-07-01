#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/iotjobs/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotjobs
    {

        class AWS_IOTJOBS_API StartNextPendingJobExecutionRequest final
        {
          public:
            StartNextPendingJobExecutionRequest() = default;

            StartNextPendingJobExecutionRequest(const Crt::JsonView &doc);
            StartNextPendingJobExecutionRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<Aws::Crt::String> ThingName;
            Aws::Crt::Optional<int64_t> StepTimeoutInMinutes;
            Aws::Crt::Optional<Aws::Crt::String> ClientToken;
            Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String>> StatusDetails;

          private:
            static void LoadFromObject(StartNextPendingJobExecutionRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotjobs
} // namespace Aws
