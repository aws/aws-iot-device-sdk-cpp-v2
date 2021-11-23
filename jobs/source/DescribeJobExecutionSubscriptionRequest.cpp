/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotjobs/DescribeJobExecutionSubscriptionRequest.h>

namespace Aws
{
    namespace Iotjobs
    {

        void DescribeJobExecutionSubscriptionRequest::LoadFromObject(
            DescribeJobExecutionSubscriptionRequest &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;
        }

        void DescribeJobExecutionSubscriptionRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;
        }

        DescribeJobExecutionSubscriptionRequest::DescribeJobExecutionSubscriptionRequest(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        DescribeJobExecutionSubscriptionRequest &DescribeJobExecutionSubscriptionRequest::operator=(
            const Crt::JsonView &doc)
        {
            *this = DescribeJobExecutionSubscriptionRequest(doc);
            return *this;
        }

    } // namespace Iotjobs
} // namespace Aws
