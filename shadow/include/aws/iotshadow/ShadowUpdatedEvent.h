#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/DateTime.h>
#include <aws/iotshadow/ShadowUpdatedSnapshot.h>

#include <aws/iotshadow/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotshadow
    {

        class AWS_IOTSHADOW_API ShadowUpdatedEvent final
        {
          public:
            ShadowUpdatedEvent() = default;

            ShadowUpdatedEvent(const Crt::JsonView &doc);
            ShadowUpdatedEvent &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<Aws::Iotshadow::ShadowUpdatedSnapshot> Previous;
            Aws::Crt::Optional<Aws::Iotshadow::ShadowUpdatedSnapshot> Current;
            Aws::Crt::Optional<Aws::Crt::DateTime> Timestamp;

          private:
            static void LoadFromObject(ShadowUpdatedEvent &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotshadow
} // namespace Aws
