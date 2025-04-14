#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotcommand/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotcommand
    {

        /**
         * Additional information on provided update.
         *
         */
        class AWS_IOTCOMMAND_API StatusReason final
        {
          public:
            StatusReason() = default;

            StatusReason(const Crt::JsonView &doc);
            StatusReason &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * Reason code in the [A-Z0-9_-]+ format and not exceeding 64 characters in length.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ReasonCode;

            /**
             * Detailed description of the reason.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ReasonDescription;

          private:
            static void LoadFromObject(StatusReason &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotcommand
} // namespace Aws
