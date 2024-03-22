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
         * (Potentially partial) state of an AWS IoT thing's shadow.  Includes the delta between the reported and
         * desired states.
         *
         */
        class AWS_IOTSHADOW_API ShadowStateWithDelta final
        {
          public:
            ShadowStateWithDelta() = default;

            ShadowStateWithDelta(const Crt::JsonView &doc);
            ShadowStateWithDelta &operator=(const Crt::JsonView &doc);

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

            /**
             * The delta between the reported and desired states.
             *
             */
            Aws::Crt::Optional<Aws::Crt::JsonObject> Delta;

          private:
            static void LoadFromObject(ShadowStateWithDelta &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotshadow
} // namespace Aws
