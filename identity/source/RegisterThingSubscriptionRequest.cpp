/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotidentity/RegisterThingSubscriptionRequest.h>

namespace Aws
{
    namespace Iotidentity
    {

        void RegisterThingSubscriptionRequest::LoadFromObject(
            RegisterThingSubscriptionRequest &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;
        }

        void RegisterThingSubscriptionRequest::SerializeToObject(Aws::Crt::JsonObject &object) const { (void)object; }

        RegisterThingSubscriptionRequest::RegisterThingSubscriptionRequest(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        RegisterThingSubscriptionRequest &RegisterThingSubscriptionRequest::operator=(const Crt::JsonView &doc)
        {
            *this = RegisterThingSubscriptionRequest(doc);
            return *this;
        }

    } // namespace Iotidentity
} // namespace Aws
