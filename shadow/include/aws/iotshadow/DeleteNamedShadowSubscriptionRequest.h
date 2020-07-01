#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/iotshadow/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotshadow
    {

        class AWS_IOTSHADOW_API DeleteNamedShadowSubscriptionRequest final
        {
          public:
            DeleteNamedShadowSubscriptionRequest() = default;

            DeleteNamedShadowSubscriptionRequest(const Crt::JsonView &doc);
            DeleteNamedShadowSubscriptionRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<Aws::Crt::String> ThingName;
            Aws::Crt::Optional<Aws::Crt::String> ShadowName;

          private:
            static void LoadFromObject(DeleteNamedShadowSubscriptionRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotshadow
} // namespace Aws
