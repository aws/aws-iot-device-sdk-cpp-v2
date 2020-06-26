#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/DateTime.h>
#include <aws/crt/JsonObject.h>

#include <aws/iotshadow/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotshadow
    {

        class AWS_IOTSHADOW_API ShadowDeltaUpdatedEvent final
        {
          public:
            ShadowDeltaUpdatedEvent() = default;

            ShadowDeltaUpdatedEvent(const Crt::JsonView &doc);
            ShadowDeltaUpdatedEvent &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<int32_t> Version;
            Aws::Crt::Optional<Aws::Crt::DateTime> Timestamp;
            Aws::Crt::Optional<Aws::Crt::JsonObject> Metadata;
            Aws::Crt::Optional<Aws::Crt::JsonObject> State;

          private:
            static void LoadFromObject(ShadowDeltaUpdatedEvent &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotshadow
} // namespace Aws
