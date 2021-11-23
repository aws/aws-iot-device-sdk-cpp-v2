/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotshadow/NamedShadowUpdatedSubscriptionRequest.h>

namespace Aws
{
    namespace Iotshadow
    {

        void NamedShadowUpdatedSubscriptionRequest::LoadFromObject(
            NamedShadowUpdatedSubscriptionRequest &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;
        }

        void NamedShadowUpdatedSubscriptionRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;
        }

        NamedShadowUpdatedSubscriptionRequest::NamedShadowUpdatedSubscriptionRequest(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        NamedShadowUpdatedSubscriptionRequest &NamedShadowUpdatedSubscriptionRequest::operator=(
            const Crt::JsonView &doc)
        {
            *this = NamedShadowUpdatedSubscriptionRequest(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
