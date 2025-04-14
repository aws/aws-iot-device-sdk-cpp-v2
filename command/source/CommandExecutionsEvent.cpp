/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotcommand/CommandExecutionsEvent.h>

namespace Aws
{
    namespace Iotcommand
    {

        void CommandExecutionsEvent::LoadFromObject(CommandExecutionsEvent &val, const Aws::Crt::JsonView &doc) {}

        void CommandExecutionsEvent::SerializeToObject(Aws::Crt::JsonObject &object) const {}

        CommandExecutionsEvent::CommandExecutionsEvent(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        CommandExecutionsEvent &CommandExecutionsEvent::operator=(const Crt::JsonView &doc)
        {
            *this = CommandExecutionsEvent(doc);
            return *this;
        }

        void CommandExecutionsEvent::SetExecutionId(Aws::Crt::ByteCursor value)
        {
            ExecutionId = Aws::Crt::String((const char *)value.ptr, value.len);
        }

        void CommandExecutionsEvent::SetPayload(Aws::Crt::ByteCursor payload)
        {
            Payload = Aws::Crt::Vector<uint8_t>(payload.ptr, payload.ptr + payload.len);
        }

        void CommandExecutionsEvent::SetContentType(Aws::Crt::ByteCursor contentType)
        {
            ContentType = Aws::Crt::String((const char *)contentType.ptr, contentType.len);
            ;
        }

        void CommandExecutionsEvent::SetTimeout(int32_t messageExpiryInterval)
        {
            Timeout = messageExpiryInterval;
        }

    } // namespace Iotcommand
} // namespace Aws
