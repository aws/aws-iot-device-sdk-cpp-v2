/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotjobs/UpdateJobExecutionSubscriptionRequest.h>

namespace Aws
{
    namespace Iotjobs
    {

        void UpdateJobExecutionSubscriptionRequest::LoadFromObject(
            UpdateJobExecutionSubscriptionRequest &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;
        }

        void UpdateJobExecutionSubscriptionRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;
        }

        UpdateJobExecutionSubscriptionRequest::UpdateJobExecutionSubscriptionRequest(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        UpdateJobExecutionSubscriptionRequest &UpdateJobExecutionSubscriptionRequest::operator=(
            const Crt::JsonView &doc)
        {
            *this = UpdateJobExecutionSubscriptionRequest(doc);
            return *this;
        }

    } // namespace Iotjobs
} // namespace Aws
