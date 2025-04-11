/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <aws/crt/Types.h>
#include <aws/iotcommand/CommandDeviceType.h>
#include <aws/iotcommand/CommandExecutionEvent.h>

namespace Aws
{
    namespace IotcommandSample
    {

        /**
         * TODO
         */
        struct CommandExecutionContext
        {
            Aws::Iotcommand::CommandDeviceType deviceType;
            Aws::Crt::String deviceId;
            Aws::Iotcommand::CommandExecutionEvent event;
        };

    } // namespace IotcommandSample
} // namespace Aws
