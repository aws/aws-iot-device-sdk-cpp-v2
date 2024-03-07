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
         * Data needed to subscribe to a device's NamedShadowDelta events.
         *
         */
        class AWS_IOTSHADOW_API NamedShadowDeltaUpdatedSubscriptionRequest final
        {
          public:
            NamedShadowDeltaUpdatedSubscriptionRequest() = default;

            NamedShadowDeltaUpdatedSubscriptionRequest(const Crt::JsonView &doc);
            NamedShadowDeltaUpdatedSubscriptionRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * Name of the AWS IoT thing to get NamedShadowDelta events for.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ThingName;

            /**
             * Name of the shadow to get ShadowDelta events for.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ShadowName;

          private:
            static void LoadFromObject(NamedShadowDeltaUpdatedSubscriptionRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotshadow
} // namespace Aws
