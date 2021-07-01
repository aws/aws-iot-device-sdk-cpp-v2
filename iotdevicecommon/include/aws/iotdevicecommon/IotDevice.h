#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/crt/Types.h>
#include <aws/iotdevicecommon/Exports.h>

namespace Aws
{

    namespace Crt
    {

    }

    namespace Iotdevicecommon
    {

        class AWS_IOTDEVICECOMMON_API DeviceApiHandle final
        {
          public:
            DeviceApiHandle(Crt::Allocator *allocator) noexcept;
            ~DeviceApiHandle();
            DeviceApiHandle(const DeviceApiHandle &) = delete;
            DeviceApiHandle(DeviceApiHandle &&) = delete;
            DeviceApiHandle &operator=(const DeviceApiHandle &) = delete;
            DeviceApiHandle &operator=(DeviceApiHandle &&) = delete;
        };

    } // namespace Iotdevicecommon
} // namespace Aws
