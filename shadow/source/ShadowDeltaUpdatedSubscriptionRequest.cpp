/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotshadow/ShadowDeltaUpdatedSubscriptionRequest.h>

namespace Aws
{
    namespace Iotshadow
    {

        void ShadowDeltaUpdatedSubscriptionRequest::LoadFromObject(
            ShadowDeltaUpdatedSubscriptionRequest &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;
        }

        void ShadowDeltaUpdatedSubscriptionRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;
        }

        ShadowDeltaUpdatedSubscriptionRequest::ShadowDeltaUpdatedSubscriptionRequest(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        ShadowDeltaUpdatedSubscriptionRequest &ShadowDeltaUpdatedSubscriptionRequest::operator=(
            const Crt::JsonView &doc)
        {
            *this = ShadowDeltaUpdatedSubscriptionRequest(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
