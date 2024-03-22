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
         * Response document containing details about a failed request.
         *
         */
        class AWS_IOTIDENTITY_API ErrorResponse final
        {
          public:
            ErrorResponse() = default;

            ErrorResponse(const Crt::JsonView &doc);
            ErrorResponse &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * Response status code
             *
             */
            Aws::Crt::Optional<int32_t> StatusCode;

            /**
             * Response error code
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ErrorCode;

            /**
             * Response error message
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ErrorMessage;

          private:
            static void LoadFromObject(ErrorResponse &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotidentity
} // namespace Aws
