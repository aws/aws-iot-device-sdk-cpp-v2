/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotshadow/UpdateShadowSubscriptionRequest.h>

namespace Aws
{
    namespace Iotshadow
    {

        void UpdateShadowSubscriptionRequest::LoadFromObject(
            UpdateShadowSubscriptionRequest &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;
        }

        void UpdateShadowSubscriptionRequest::SerializeToObject(Aws::Crt::JsonObject &object) const { (void)object; }

        UpdateShadowSubscriptionRequest::UpdateShadowSubscriptionRequest(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        UpdateShadowSubscriptionRequest &UpdateShadowSubscriptionRequest::operator=(const Crt::JsonView &doc)
        {
            *this = UpdateShadowSubscriptionRequest(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
