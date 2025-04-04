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
         * Data needed to subscribe to CommandExecutions events.
         *
         */
        class AWS_IOTCOMMAND_API CommandExecutionsSubscriptionRequest final
        {
          public:
            CommandExecutionsSubscriptionRequest() = default;

            CommandExecutionsSubscriptionRequest(const Crt::JsonView &doc);
            CommandExecutionsSubscriptionRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             *
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> DeviceType;

            /**
             *
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> DeviceId;

          private:
            static void LoadFromObject(CommandExecutionsSubscriptionRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotcommand
} // namespace Aws
