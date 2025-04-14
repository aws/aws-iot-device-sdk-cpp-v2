/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotcommands/CommandExecutionEvent.h>

namespace Aws
{
    namespace Iotcommands
    {

        void CommandExecutionEvent::LoadFromObject(CommandExecutionEvent &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;
        }

        void CommandExecutionEvent::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;
        }

        CommandExecutionEvent::CommandExecutionEvent(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        CommandExecutionEvent &CommandExecutionEvent::operator=(const Crt::JsonView &doc)
        {
            *this = CommandExecutionEvent(doc);
            return *this;
        }

        void CommandExecutionEvent::SetExecutionId(Aws::Crt::ByteCursor value)
        {
            ExecutionId = Aws::Crt::String((const char *)value.ptr, value.len);
        }

        void CommandExecutionEvent::SetPayload(Aws::Crt::ByteCursor payload)
        {
            Payload = Aws::Crt::Vector<uint8_t>(payload.ptr, payload.ptr + payload.len);
        }

        void CommandExecutionEvent::SetContentType(Aws::Crt::ByteCursor contentType)
        {
            ContentType = Aws::Crt::String((const char *)contentType.ptr, contentType.len);
            ;
        }

        void CommandExecutionEvent::SetTimeout(int32_t messageExpiryInterval)
        {
            Timeout = messageExpiryInterval;
        }

    } // namespace Iotcommands
} // namespace Aws
