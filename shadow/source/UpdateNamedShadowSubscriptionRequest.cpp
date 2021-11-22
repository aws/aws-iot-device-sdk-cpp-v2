/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotshadow/UpdateNamedShadowSubscriptionRequest.h>

namespace Aws
{
    namespace Iotshadow
    {

        void UpdateNamedShadowSubscriptionRequest::LoadFromObject(
            UpdateNamedShadowSubscriptionRequest &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;
        }

        void UpdateNamedShadowSubscriptionRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;
        }

        UpdateNamedShadowSubscriptionRequest::UpdateNamedShadowSubscriptionRequest(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        UpdateNamedShadowSubscriptionRequest &UpdateNamedShadowSubscriptionRequest::operator=(const Crt::JsonView &doc)
        {
            *this = UpdateNamedShadowSubscriptionRequest(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
