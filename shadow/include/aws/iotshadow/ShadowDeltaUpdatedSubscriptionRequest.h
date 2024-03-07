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
         * Data needed to subscribe to a device's ShadowDelta events.
         *
         */
        class AWS_IOTSHADOW_API ShadowDeltaUpdatedSubscriptionRequest final
        {
          public:
            ShadowDeltaUpdatedSubscriptionRequest() = default;

            ShadowDeltaUpdatedSubscriptionRequest(const Crt::JsonView &doc);
            ShadowDeltaUpdatedSubscriptionRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * Name of the AWS IoT thing to get ShadowDelta events for.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ThingName;

          private:
            static void LoadFromObject(ShadowDeltaUpdatedSubscriptionRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotshadow
} // namespace Aws
