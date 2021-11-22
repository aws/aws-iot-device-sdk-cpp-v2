/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotshadow/ShadowUpdatedSubscriptionRequest.h>

namespace Aws
{
    namespace Iotshadow
    {

        void ShadowUpdatedSubscriptionRequest::LoadFromObject(
            ShadowUpdatedSubscriptionRequest &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;
        }

        void ShadowUpdatedSubscriptionRequest::SerializeToObject(Aws::Crt::JsonObject &object) const { (void)object; }

        ShadowUpdatedSubscriptionRequest::ShadowUpdatedSubscriptionRequest(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        ShadowUpdatedSubscriptionRequest &ShadowUpdatedSubscriptionRequest::operator=(const Crt::JsonView &doc)
        {
            *this = ShadowUpdatedSubscriptionRequest(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
