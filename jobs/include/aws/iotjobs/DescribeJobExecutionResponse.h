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

        class AWS_IOTJOBS_API DescribeJobExecutionResponse final
        {
          public:
            DescribeJobExecutionResponse() = default;

            DescribeJobExecutionResponse(const Crt::JsonView &doc);
            DescribeJobExecutionResponse &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<Aws::Iotjobs::JobExecutionData> Execution;
            Aws::Crt::Optional<Aws::Crt::String> ClientToken;
            Aws::Crt::Optional<Aws::Crt::DateTime> Timestamp;

          private:
            static void LoadFromObject(DescribeJobExecutionResponse &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotjobs
} // namespace Aws
