#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/iotidentity/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotidentity
    {

        class AWS_IOTIDENTITY_API RegisterThingSubscriptionRequest final
        {
          public:
            RegisterThingSubscriptionRequest() = default;

            RegisterThingSubscriptionRequest(const Crt::JsonView &doc);
            RegisterThingSubscriptionRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<Aws::Crt::String> TemplateName;

          private:
            static void LoadFromObject(RegisterThingSubscriptionRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotidentity
} // namespace Aws
