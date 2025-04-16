#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotcommands/DeviceType.h>

#include <aws/iotcommands/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotcommands
    {

        /**
         * Data needed to subscribe to CommandExecution events.
         *
         */
        class AWS_IOTCOMMANDS_API CommandExecutionsSubscriptionRequest final
        {
          public:
            CommandExecutionsSubscriptionRequest() = default;

            CommandExecutionsSubscriptionRequest(const Crt::JsonView &doc);
            CommandExecutionsSubscriptionRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * The type of a target device. Determine if the device should subscribe for commands addressed to an IoT
             * Thing or MQTT client.
             *
             */
            Aws::Crt::Optional<Aws::Iotcommands::DeviceType> DeviceType;

            /**
             * Depending on device type value, this field is either an IoT Thing name or a client ID.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> DeviceId;

          private:
            static void LoadFromObject(CommandExecutionsSubscriptionRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotcommands
} // namespace Aws
