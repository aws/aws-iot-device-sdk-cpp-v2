#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "datest_utils.h"

using namespace Aws::Crt;
using namespace DATest_Utils;

bool DeviceAdvisorEnvironment::init(TestType type)
{
    struct aws_allocator *allocator = aws_default_allocator();

    // aws_string AWS_ENDPONT;
    aws_string *s_endpoint = NULL;
    aws_string *s_key = NULL;
    aws_string *s_topic = NULL;
    aws_string *s_certificate = NULL;
    aws_string *s_thing_name = NULL;
    aws_string *s_shadow_property = NULL;
    aws_string *s_shadow_value = NULL;

    aws_get_environment_value(allocator, AWS_ENDPONT, &s_endpoint);
    aws_get_environment_value(allocator, AWS_CERTI, &s_certificate);
    aws_get_environment_value(allocator, AWS_KEY, &s_key);
    aws_get_environment_value(allocator, AWS_TOPIC, &s_topic);
    aws_get_environment_value(allocator, AWS_THING_NAME, &s_thing_name);
    aws_get_environment_value(allocator, AWS_SHADOW_PROPERTY, &s_shadow_property);
    aws_get_environment_value(allocator, AWS_SHADOW_VALUE_SET, &s_shadow_value);

    endpoint = aws_string_c_str(s_endpoint);
    certificatePath = aws_string_c_str(s_certificate);
    keyPath = aws_string_c_str(s_key);
    topic = aws_string_c_str(s_topic);
    thing_name = aws_string_c_str(s_thing_name);
    shadowProperty = aws_string_c_str(s_shadow_property);
    shadowValue = aws_string_c_str(s_shadow_value);

    if (endpoint.empty() || certificatePath.empty() || keyPath.empty())
    {
        return false;
    }

    if (topic.empty() && type == SUB_PUB)
    {
        return false;
    }

    if ((thing_name.empty() || shadowProperty.empty() || shadowValue.empty()) && type == SHADOW)
    {
        return false;
    }

    return true;
}