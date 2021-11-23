/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotjobs/NextJobExecutionChangedSubscriptionRequest.h>

namespace Aws
{
    namespace Iotjobs
    {

        void NextJobExecutionChangedSubscriptionRequest::LoadFromObject(
            NextJobExecutionChangedSubscriptionRequest &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;
        }

        void NextJobExecutionChangedSubscriptionRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;
        }

        NextJobExecutionChangedSubscriptionRequest::NextJobExecutionChangedSubscriptionRequest(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        NextJobExecutionChangedSubscriptionRequest &NextJobExecutionChangedSubscriptionRequest::operator=(
            const Crt::JsonView &doc)
        {
            *this = NextJobExecutionChangedSubscriptionRequest(doc);
            return *this;
        }

    } // namespace Iotjobs
} // namespace Aws
