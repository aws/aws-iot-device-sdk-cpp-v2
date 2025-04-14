#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotcommand/RejectedErrorCode.h>

#include <aws/iotcommand/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotcommand
    {

        /**
         * Response document containing details about a failed request.
         *
         */
        class AWS_IOTCOMMAND_API V2ErrorResponse final
        {
          public:
            V2ErrorResponse() = default;

            V2ErrorResponse(const Crt::JsonView &doc);
            V2ErrorResponse &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * Indicates the type of error.
             *
             */
            Aws::Crt::Optional<Aws::Iotcommand::RejectedErrorCode> Error;

            /**
             * A text message that provides additional information.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ErrorMessage;

            /**
             * Execution ID for which error is related.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ExecutionId;

          private:
            static void LoadFromObject(V2ErrorResponse &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotcommand
} // namespace Aws
