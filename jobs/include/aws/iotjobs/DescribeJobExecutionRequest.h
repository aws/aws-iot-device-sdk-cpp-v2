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

        class AWS_IOTJOBS_API DescribeJobExecutionRequest final
        {
          public:
            DescribeJobExecutionRequest() = default;

            DescribeJobExecutionRequest(const Crt::JsonView &doc);
            DescribeJobExecutionRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<int64_t> ExecutionNumber;
            Aws::Crt::Optional<Aws::Crt::String> ThingName;
            Aws::Crt::Optional<bool> IncludeJobDocument;
            Aws::Crt::Optional<Aws::Crt::String> JobId;
            Aws::Crt::Optional<Aws::Crt::String> ClientToken;

          private:
            static void LoadFromObject(DescribeJobExecutionRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotjobs
} // namespace Aws
