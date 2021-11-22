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
         * Data needed to subscribe to UpdateNamedShadow responses.
         *
         */
        class AWS_IOTSHADOW_API UpdateNamedShadowSubscriptionRequest final
        {
          public:
            UpdateNamedShadowSubscriptionRequest() = default;

            UpdateNamedShadowSubscriptionRequest(const Crt::JsonView &doc);
            UpdateNamedShadowSubscriptionRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * Name of the AWS IoT thing to listen to UpdateNamedShadow responses for.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ThingName;

            /**
             * Name of the shadow to listen to UpdateNamedShadow responses for.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ShadowName;

          private:
            static void LoadFromObject(UpdateNamedShadowSubscriptionRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotshadow
} // namespace Aws
