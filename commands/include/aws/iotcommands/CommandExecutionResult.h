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
         * The result value of the command execution. The device can use the result field to share additional details
         * about the execution such as a return value of a remote function call.
         *
         */
        class AWS_IOTCOMMANDS_API CommandExecutionResult final
        {
          public:
            CommandExecutionResult() = default;

            CommandExecutionResult(const Crt::JsonView &doc);
            CommandExecutionResult &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * An attribute of type String.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> S;

            /**
             * An attribute of type Boolean.
             *
             */
            Aws::Crt::Optional<bool> B;

            /**
             * An attribute of type Binary.
             *
             */
            Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>> Bin;

          private:
            static void LoadFromObject(CommandExecutionResult &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotcommands
} // namespace Aws
