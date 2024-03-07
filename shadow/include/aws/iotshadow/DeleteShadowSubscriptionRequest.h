#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotshadow/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotshadow
    {

        /**
         * Data needed to subscribe to DeleteShadow responses for an AWS IoT thing.
         *
         */
        class AWS_IOTSHADOW_API DeleteShadowSubscriptionRequest final
        {
          public:
            DeleteShadowSubscriptionRequest() = default;

            DeleteShadowSubscriptionRequest(const Crt::JsonView &doc);
            DeleteShadowSubscriptionRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * AWS IoT thing to subscribe to DeleteShadow operations for.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ThingName;

          private:
            static void LoadFromObject(DeleteShadowSubscriptionRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotshadow
} // namespace Aws
