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
         * Sent whenever a command execution is added for a thing or a client.
         *
         */
        class AWS_IOTCOMMANDS_API CommandExecutionEvent final
        {
          public:
            CommandExecutionEvent() = default;

            CommandExecutionEvent(const Crt::JsonView &doc);
            CommandExecutionEvent &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * Opaque data containing instructions sent from the IoT commands service.
             *
             */
            Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>> Payload;

            /**
             * Unique ID for the specific execution of a command. A command can have multiple executions, each with a
             * unique ID.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ExecutionId;

            /**
             * Data format of the payload. It is supposed to be a MIME type (IANA media type), but can be an arbitrary
             * string.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ContentType;

            /**
             * Number of seconds before the IoT commands service decides that this command execution is timed out.
             *
             */
            Aws::Crt::Optional<int32_t> Timeout;

            void SetExecutionId(Aws::Crt::ByteCursor value);

            void SetPayload(Aws::Crt::ByteCursor payload);

            void SetContentType(Aws::Crt::ByteCursor contentType);

            void SetTimeout(int32_t messageExpiryInterval);

          private:
            static void LoadFromObject(CommandExecutionEvent &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotcommands
} // namespace Aws
