/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotshadow/GetNamedShadowSubscriptionRequest.h>

namespace Aws
{
    namespace Iotshadow
    {

        void GetNamedShadowSubscriptionRequest::LoadFromObject(
            GetNamedShadowSubscriptionRequest &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;
        }

        void GetNamedShadowSubscriptionRequest::SerializeToObject(Aws::Crt::JsonObject &object) const { (void)object; }

        GetNamedShadowSubscriptionRequest::GetNamedShadowSubscriptionRequest(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        GetNamedShadowSubscriptionRequest &GetNamedShadowSubscriptionRequest::operator=(const Crt::JsonView &doc)
        {
            *this = GetNamedShadowSubscriptionRequest(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
