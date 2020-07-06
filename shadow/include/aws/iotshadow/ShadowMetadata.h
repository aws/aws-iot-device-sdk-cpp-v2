#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/JsonObject.h>

#include <aws/iotshadow/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotshadow
    {

        class AWS_IOTSHADOW_API ShadowMetadata final
        {
          public:
            ShadowMetadata() = default;

            ShadowMetadata(const Crt::JsonView &doc);
            ShadowMetadata &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<Aws::Crt::JsonObject> Desired;
            Aws::Crt::Optional<Aws::Crt::JsonObject> Reported;

          private:
            static void LoadFromObject(ShadowMetadata &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotshadow
} // namespace Aws
