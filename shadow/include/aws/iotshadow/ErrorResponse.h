#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/DateTime.h>

#include <aws/iotshadow/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotshadow
    {

        class AWS_IOTSHADOW_API ErrorResponse final
        {
          public:
            ErrorResponse() = default;

            ErrorResponse(const Crt::JsonView &doc);
            ErrorResponse &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<Aws::Crt::DateTime> Timestamp;
            Aws::Crt::Optional<Aws::Crt::String> Message;
            Aws::Crt::Optional<Aws::Crt::String> ClientToken;
            Aws::Crt::Optional<int32_t> Code;

          private:
            static void LoadFromObject(ErrorResponse &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotshadow
} // namespace Aws
