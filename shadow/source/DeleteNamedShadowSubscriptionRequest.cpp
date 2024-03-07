/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotshadow/DeleteNamedShadowSubscriptionRequest.h>

namespace Aws
{
    namespace Iotshadow
    {

        void DeleteNamedShadowSubscriptionRequest::LoadFromObject(
            DeleteNamedShadowSubscriptionRequest &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;
        }

        void DeleteNamedShadowSubscriptionRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;
        }

        DeleteNamedShadowSubscriptionRequest::DeleteNamedShadowSubscriptionRequest(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        DeleteNamedShadowSubscriptionRequest &DeleteNamedShadowSubscriptionRequest::operator=(const Crt::JsonView &doc)
        {
            *this = DeleteNamedShadowSubscriptionRequest(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
