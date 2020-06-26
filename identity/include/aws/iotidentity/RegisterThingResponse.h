#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/iotidentity/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotidentity
    {

        class AWS_IOTIDENTITY_API RegisterThingResponse final
        {
          public:
            RegisterThingResponse() = default;

            RegisterThingResponse(const Crt::JsonView &doc);
            RegisterThingResponse &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<Aws::Crt::String> ThingName;
            Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String>> DeviceConfiguration;

          private:
            static void LoadFromObject(RegisterThingResponse &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotidentity
} // namespace Aws
