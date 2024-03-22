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
         * Data needed to subscribe to a device's NamedShadowUpdated events.
         *
         */
        class AWS_IOTSHADOW_API NamedShadowUpdatedSubscriptionRequest final
        {
          public:
            NamedShadowUpdatedSubscriptionRequest() = default;

            NamedShadowUpdatedSubscriptionRequest(const Crt::JsonView &doc);
            NamedShadowUpdatedSubscriptionRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * Name of the AWS IoT thing to get NamedShadowUpdated events for.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ThingName;

            /**
             * Name of the shadow to get NamedShadowUpdated events for.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ShadowName;

          private:
            static void LoadFromObject(NamedShadowUpdatedSubscriptionRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotshadow
} // namespace Aws
