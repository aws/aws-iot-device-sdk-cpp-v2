#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/DateTime.h>
#include <aws/iotshadow/ShadowMetadata.h>
#include <aws/iotshadow/ShadowStateWithDelta.h>

#include <aws/iotshadow/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotshadow
    {

        class AWS_IOTSHADOW_API GetShadowResponse final
        {
          public:
            GetShadowResponse() = default;

            GetShadowResponse(const Crt::JsonView &doc);
            GetShadowResponse &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<int32_t> Version;
            Aws::Crt::Optional<Aws::Crt::String> ClientToken;
            Aws::Crt::Optional<Aws::Iotshadow::ShadowStateWithDelta> State;
            Aws::Crt::Optional<Aws::Iotshadow::ShadowMetadata> Metadata;
            Aws::Crt::Optional<Aws::Crt::DateTime> Timestamp;

          private:
            static void LoadFromObject(GetShadowResponse &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotshadow
} // namespace Aws
