/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotjobs/GetPendingJobExecutionsSubscriptionRequest.h>

namespace Aws
{
    namespace Iotjobs
    {

        void GetPendingJobExecutionsSubscriptionRequest::LoadFromObject(
            GetPendingJobExecutionsSubscriptionRequest &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;
        }

        void GetPendingJobExecutionsSubscriptionRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;
        }

        GetPendingJobExecutionsSubscriptionRequest::GetPendingJobExecutionsSubscriptionRequest(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        GetPendingJobExecutionsSubscriptionRequest &GetPendingJobExecutionsSubscriptionRequest::operator=(
            const Crt::JsonView &doc)
        {
            *this = GetPendingJobExecutionsSubscriptionRequest(doc);
            return *this;
        }

    } // namespace Iotjobs
} // namespace Aws
