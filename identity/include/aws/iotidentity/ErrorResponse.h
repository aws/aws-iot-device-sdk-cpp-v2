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

        class AWS_IOTIDENTITY_API ErrorResponse final
        {
          public:
            ErrorResponse() = default;

            ErrorResponse(const Crt::JsonView &doc);
            ErrorResponse &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<int32_t> StatusCode;
            Aws::Crt::Optional<Aws::Crt::String> ErrorMessage;
            Aws::Crt::Optional<Aws::Crt::String> ErrorCode;

          private:
            static void LoadFromObject(ErrorResponse &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotidentity
} // namespace Aws
