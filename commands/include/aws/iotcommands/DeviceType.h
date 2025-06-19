#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotcommands/Exports.h>

#include <aws/crt/StlAllocator.h>
#include <aws/crt/Types.h>

namespace Aws
{
    namespace Iotcommands
    {

        /**
         * Possible device types for command executions.
         *
         */
        enum class DeviceType
        {
            /**
             * A target for the commands is an IoT Thing.
             */
            THING,

            /**
             * A target for the commands is an MQTT client ID.
             */
            CLIENT,

        };

        namespace DeviceTypeMarshaller
        {
            AWS_IOTCOMMANDS_API const char *ToString(DeviceType val);
            AWS_IOTCOMMANDS_API DeviceType FromString(const Aws::Crt::String &val);
        } // namespace DeviceTypeMarshaller
    } // namespace Iotcommands
} // namespace Aws
