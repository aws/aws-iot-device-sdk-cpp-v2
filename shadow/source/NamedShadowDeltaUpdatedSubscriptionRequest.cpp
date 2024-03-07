/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotshadow/NamedShadowDeltaUpdatedSubscriptionRequest.h>

namespace Aws
{
    namespace Iotshadow
    {

        void NamedShadowDeltaUpdatedSubscriptionRequest::LoadFromObject(
            NamedShadowDeltaUpdatedSubscriptionRequest &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;
        }

        void NamedShadowDeltaUpdatedSubscriptionRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;
        }

        NamedShadowDeltaUpdatedSubscriptionRequest::NamedShadowDeltaUpdatedSubscriptionRequest(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        NamedShadowDeltaUpdatedSubscriptionRequest &NamedShadowDeltaUpdatedSubscriptionRequest::operator=(
            const Crt::JsonView &doc)
        {
            *this = NamedShadowDeltaUpdatedSubscriptionRequest(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
