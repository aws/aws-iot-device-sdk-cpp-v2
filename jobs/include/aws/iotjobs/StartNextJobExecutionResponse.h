#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
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

        class AWS_IOTJOBS_API StartNextJobExecutionResponse final
        {
          public:
            StartNextJobExecutionResponse() = default;

            StartNextJobExecutionResponse(const Crt::JsonView &doc);
            StartNextJobExecutionResponse &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<Aws::Crt::String> ClientToken;
            Aws::Crt::Optional<Aws::Crt::DateTime> Timestamp;
            Aws::Crt::Optional<Aws::Iotjobs::JobExecutionData> Execution;

          private:
            static void LoadFromObject(StartNextJobExecutionResponse &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotjobs
} // namespace Aws
