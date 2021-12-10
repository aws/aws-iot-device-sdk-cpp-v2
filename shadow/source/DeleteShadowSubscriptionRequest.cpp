/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotshadow/DeleteShadowSubscriptionRequest.h>

namespace Aws
{
    namespace Iotshadow
    {

        void DeleteShadowSubscriptionRequest::LoadFromObject(
            DeleteShadowSubscriptionRequest &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;
        }

        void DeleteShadowSubscriptionRequest::SerializeToObject(Aws::Crt::JsonObject &object) const { (void)object; }

        DeleteShadowSubscriptionRequest::DeleteShadowSubscriptionRequest(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        DeleteShadowSubscriptionRequest &DeleteShadowSubscriptionRequest::operator=(const Crt::JsonView &doc)
        {
            *this = DeleteShadowSubscriptionRequest(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
