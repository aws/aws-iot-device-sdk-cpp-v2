#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotidentity/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotidentity
    {

        /**
         * Response payload to a RegisterThing request.
         *
         */
        class AWS_IOTIDENTITY_API RegisterThingResponse final
        {
          public:
            RegisterThingResponse() = default;

            RegisterThingResponse(const Crt::JsonView &doc);
            RegisterThingResponse &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * The device configuration defined in the template.
             *
             */
            Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String>> DeviceConfiguration;

            /**
             * The name of the IoT thing created during provisioning.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ThingName;

          private:
            static void LoadFromObject(RegisterThingResponse &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotidentity
} // namespace Aws
