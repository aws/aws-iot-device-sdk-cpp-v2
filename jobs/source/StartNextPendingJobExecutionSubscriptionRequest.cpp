/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotjobs/StartNextPendingJobExecutionSubscriptionRequest.h>

namespace Aws
{
    namespace Iotjobs
    {

        void StartNextPendingJobExecutionSubscriptionRequest::LoadFromObject(
            StartNextPendingJobExecutionSubscriptionRequest &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;
        }

        void StartNextPendingJobExecutionSubscriptionRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;
        }

        StartNextPendingJobExecutionSubscriptionRequest::StartNextPendingJobExecutionSubscriptionRequest(
            const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        StartNextPendingJobExecutionSubscriptionRequest &StartNextPendingJobExecutionSubscriptionRequest::operator=(
            const Crt::JsonView &doc)
        {
            *this = StartNextPendingJobExecutionSubscriptionRequest(doc);
            return *this;
        }

    } // namespace Iotjobs
} // namespace Aws
