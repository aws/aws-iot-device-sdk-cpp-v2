/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotshadow/GetShadowSubscriptionRequest.h>

namespace Aws
{
    namespace Iotshadow
    {

        void GetShadowSubscriptionRequest::LoadFromObject(
            GetShadowSubscriptionRequest &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;
        }

        void GetShadowSubscriptionRequest::SerializeToObject(Aws::Crt::JsonObject &object) const { (void)object; }

        GetShadowSubscriptionRequest::GetShadowSubscriptionRequest(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        GetShadowSubscriptionRequest &GetShadowSubscriptionRequest::operator=(const Crt::JsonView &doc)
        {
            *this = GetShadowSubscriptionRequest(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
