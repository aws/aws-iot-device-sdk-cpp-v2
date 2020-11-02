/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/iotdevice/IotDevice.h>
#include <aws/iotdevice/iotdevice.h>

namespace Aws
{

    namespace Crt
    {

    }

    namespace Iotdevice
    {

        DeviceApiHandle::DeviceApiHandle(Crt::Allocator *allocator) noexcept { aws_iotdevice_library_init(allocator); }

        DeviceApiHandle::~DeviceApiHandle() { aws_iotdevice_library_clean_up(); }
    }

} // namespace Aws
