/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotcommands/CommandExecutionsSubscriptionRequest.h>

namespace Aws
{
    namespace Iotcommands
    {

        void CommandExecutionsSubscriptionRequest::LoadFromObject(
            CommandExecutionsSubscriptionRequest &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;
        }

        void CommandExecutionsSubscriptionRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;
        }

        CommandExecutionsSubscriptionRequest::CommandExecutionsSubscriptionRequest(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        CommandExecutionsSubscriptionRequest &CommandExecutionsSubscriptionRequest::operator=(const Crt::JsonView &doc)
        {
            *this = CommandExecutionsSubscriptionRequest(doc);
            return *this;
        }

    } // namespace Iotcommands
} // namespace Aws
