#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotcommand/Exports.h>

#include <aws/crt/StlAllocator.h>
#include <aws/crt/Types.h>

namespace Aws
{
    namespace Iotcommand
    {

        /**
         * Valid device types for command executions.
         *
         */
        enum class CommandDeviceType
        {
            /**
             */
            things,

            /**
             */
            clients,

        };

        namespace CommandDeviceTypeMarshaller
        {
            AWS_IOTCOMMAND_API const char *ToString(CommandDeviceType val);
            AWS_IOTCOMMAND_API CommandDeviceType FromString(const Aws::Crt::String &val);
        } // namespace CommandDeviceTypeMarshaller
    } // namespace Iotcommand
} // namespace Aws
