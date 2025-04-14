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
         * Sent whenever a command execution is added for a thing or a client.
         *
         */
        class AWS_IOTCOMMAND_API CommandExecutionEvent final
        {
          public:
            CommandExecutionEvent() = default;

            CommandExecutionEvent(const Crt::JsonView &doc);
            CommandExecutionEvent &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             */
            Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>> Payload;

            /**
             */
            Aws::Crt::Optional<Aws::Crt::String> ExecutionId;

            /**
             */
            Aws::Crt::Optional<Aws::Crt::String> ContentType;

            /**
             */
            Aws::Crt::Optional<int32_t> Timeout;

            void SetExecutionId(Aws::Crt::ByteCursor value);

            void SetPayload(Aws::Crt::ByteCursor payload);

            void SetContentType(Aws::Crt::ByteCursor contentType);

            void SetTimeout(int32_t messageExpiryInterval);

          private:
            static void LoadFromObject(CommandExecutionEvent &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotcommand
} // namespace Aws
