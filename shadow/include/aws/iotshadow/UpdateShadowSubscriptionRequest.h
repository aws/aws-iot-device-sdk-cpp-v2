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
         * Data needed to subscribe to UpdateShadow responses.
         *
         */
        class AWS_IOTSHADOW_API UpdateShadowSubscriptionRequest final
        {
          public:
            UpdateShadowSubscriptionRequest() = default;

            UpdateShadowSubscriptionRequest(const Crt::JsonView &doc);
            UpdateShadowSubscriptionRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * Name of the AWS IoT thing to listen to UpdateShadow responses for.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ThingName;

          private:
            static void LoadFromObject(UpdateShadowSubscriptionRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotshadow
} // namespace Aws
