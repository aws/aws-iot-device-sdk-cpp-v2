/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotjobs/JobExecutionsChangedSubscriptionRequest.h>

namespace Aws
{
    namespace Iotjobs
    {

        void JobExecutionsChangedSubscriptionRequest::LoadFromObject(
            JobExecutionsChangedSubscriptionRequest &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;
        }

        void JobExecutionsChangedSubscriptionRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;
        }

        JobExecutionsChangedSubscriptionRequest::JobExecutionsChangedSubscriptionRequest(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        JobExecutionsChangedSubscriptionRequest &JobExecutionsChangedSubscriptionRequest::operator=(
            const Crt::JsonView &doc)
        {
            *this = JobExecutionsChangedSubscriptionRequest(doc);
            return *this;
        }

    } // namespace Iotjobs
} // namespace Aws
