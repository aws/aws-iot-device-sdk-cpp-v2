/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotcommand/CommandExecutionsSubscriptionRequest.h>

namespace Aws
{
    namespace Iotcommand
    {

        void CommandExecutionsSubscriptionRequest::LoadFromObject(
            CommandExecutionsSubscriptionRequest &val,
            const Aws::Crt::JsonView &doc)
        {
        }

        void CommandExecutionsSubscriptionRequest::SerializeToObject(Aws::Crt::JsonObject &object) const {}

        CommandExecutionsSubscriptionRequest::CommandExecutionsSubscriptionRequest(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        CommandExecutionsSubscriptionRequest &CommandExecutionsSubscriptionRequest::operator=(const Crt::JsonView &doc)
        {
            *this = CommandExecutionsSubscriptionRequest(doc);
            return *this;
        }

    } // namespace Iotcommand
} // namespace Aws
