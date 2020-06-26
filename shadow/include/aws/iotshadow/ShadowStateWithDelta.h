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

        class AWS_IOTSHADOW_API ShadowStateWithDelta final
        {
          public:
            ShadowStateWithDelta() = default;

            ShadowStateWithDelta(const Crt::JsonView &doc);
            ShadowStateWithDelta &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<Aws::Crt::JsonObject> Delta;
            Aws::Crt::Optional<Aws::Crt::JsonObject> Reported;
            Aws::Crt::Optional<Aws::Crt::JsonObject> Desired;

          private:
            static void LoadFromObject(ShadowStateWithDelta &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotshadow
} // namespace Aws
