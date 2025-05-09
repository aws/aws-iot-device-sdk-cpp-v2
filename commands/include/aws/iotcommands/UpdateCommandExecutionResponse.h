#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotcommands/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotcommands
    {

        /**
         * Response payload to an UpdateCommandExecution request.
         *
         */
        class AWS_IOTCOMMANDS_API UpdateCommandExecutionResponse final
        {
          public:
            UpdateCommandExecutionResponse() = default;

            UpdateCommandExecutionResponse(const Crt::JsonView &doc);
            UpdateCommandExecutionResponse &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * Execution ID for which a response was sent
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ExecutionId;

          private:
            static void LoadFromObject(UpdateCommandExecutionResponse &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotcommands
} // namespace Aws
