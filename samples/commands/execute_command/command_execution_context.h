/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <aws/crt/Types.h>
#include <aws/iotcommands/CommandExecutionEvent.h>
#include <aws/iotcommands/DeviceType.h>

namespace Aws
{
    namespace IotcommandsSample
    {

        /**
         * TODO
         */
        struct CommandExecutionContext
        {
            Aws::Iotcommands::DeviceType deviceType;
            Aws::Crt::String deviceId;
            Aws::Iotcommands::CommandExecutionEvent event;
        };

    } // namespace IotcommandsSample
} // namespace Aws
