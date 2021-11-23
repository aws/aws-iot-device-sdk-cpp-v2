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
         * Data needed to subscribe to DeleteNamedShadow responses for an AWS IoT thing.
         *
         */
        class AWS_IOTSHADOW_API DeleteNamedShadowSubscriptionRequest final
        {
          public:
            DeleteNamedShadowSubscriptionRequest() = default;

            DeleteNamedShadowSubscriptionRequest(const Crt::JsonView &doc);
            DeleteNamedShadowSubscriptionRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * AWS IoT thing to subscribe to DeleteNamedShadow operations for.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ThingName;

            /**
             * Name of the shadow to subscribe to DeleteNamedShadow operations for.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ShadowName;

          private:
            static void LoadFromObject(DeleteNamedShadowSubscriptionRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotshadow
} // namespace Aws
