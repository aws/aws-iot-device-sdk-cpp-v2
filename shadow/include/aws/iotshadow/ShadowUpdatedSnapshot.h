#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/iotshadow/ShadowMetadata.h>
#include <aws/iotshadow/ShadowState.h>

#include <aws/iotshadow/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotshadow
    {

        class AWS_IOTSHADOW_API ShadowUpdatedSnapshot final
        {
          public:
            ShadowUpdatedSnapshot() = default;

            ShadowUpdatedSnapshot(const Crt::JsonView &doc);
            ShadowUpdatedSnapshot &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<Aws::Iotshadow::ShadowState> State;
            Aws::Crt::Optional<Aws::Iotshadow::ShadowMetadata> Metadata;
            Aws::Crt::Optional<int32_t> Version;

          private:
            static void LoadFromObject(ShadowUpdatedSnapshot &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotshadow
} // namespace Aws
