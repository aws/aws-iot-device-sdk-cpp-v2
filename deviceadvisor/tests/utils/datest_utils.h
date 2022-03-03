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

    AWS_STATIC_STRING_FROM_LITERAL(AWS_ENDPONT, "DA_ENDPOINT");
    AWS_STATIC_STRING_FROM_LITERAL(AWS_CERTI, "DA_CERTI");
    AWS_STATIC_STRING_FROM_LITERAL(AWS_KEY, "DA_KEY");
    AWS_STATIC_STRING_FROM_LITERAL(AWS_TOPIC, "DA_TOPIC");
    AWS_STATIC_STRING_FROM_LITERAL(AWS_THING_NAME, "DA_THING_NAME");
    AWS_STATIC_STRING_FROM_LITERAL(AWS_SHADOW_PROPERTY, "DA_SHADOW_PROPERTY");
    AWS_STATIC_STRING_FROM_LITERAL(AWS_SHADOW_VALUE_SET, "DA_SHADOW_VALUE_SET");

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
