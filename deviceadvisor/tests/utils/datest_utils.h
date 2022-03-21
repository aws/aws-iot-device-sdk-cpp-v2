#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/environment.h>
#include <aws/common/string.h>
#include <aws/crt/Types.h>
#include <iostream>
#include <stdlib.h>

using namespace Aws::Crt;

namespace DATest_Utils
{
    enum TestType
    {
        CONNECT,
        SUB_PUB,
        SHADOW
    };

    struct DeviceAdvisorEnvironment
    {
        String endpoint;
        String certificatePath;
        String keyPath;
        String topic;
        String thing_name;
        String shadowProperty;
        String shadowValue;
        bool init(TestType type);
    };
} // namespace DATest_Utils
