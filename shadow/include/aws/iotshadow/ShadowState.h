#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/crt/JsonObject.h>

#include <aws/iotshadow/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotshadow
    {

        /**
         * (Potentially partial) state of an AWS IoT thing's shadow.
         *
         */
        class AWS_IOTSHADOW_API ShadowState final
        {
          public:
            ShadowState() = default;

            ShadowState(const Crt::JsonView &doc);
            ShadowState &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * The desired shadow state (from external services and devices).
             *
             */
            Aws::Crt::Optional<Aws::Crt::JsonObject> Desired;

            /**
             * The (last) reported shadow state from the device.
             *
             */
            Aws::Crt::Optional<Aws::Crt::JsonObject> Reported;

          private:
            static void LoadFromObject(ShadowState &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotshadow
} // namespace Aws
