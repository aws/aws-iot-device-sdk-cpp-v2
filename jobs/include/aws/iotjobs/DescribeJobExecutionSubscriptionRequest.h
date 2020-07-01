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

        class AWS_IOTJOBS_API DescribeJobExecutionSubscriptionRequest final
        {
          public:
            DescribeJobExecutionSubscriptionRequest() = default;

            DescribeJobExecutionSubscriptionRequest(const Crt::JsonView &doc);
            DescribeJobExecutionSubscriptionRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<Aws::Crt::String> ThingName;
            Aws::Crt::Optional<Aws::Crt::String> JobId;

          private:
            static void LoadFromObject(DescribeJobExecutionSubscriptionRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotjobs
} // namespace Aws
