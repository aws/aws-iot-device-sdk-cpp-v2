/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "datest_utils.h"

using namespace Aws::Crt;
using namespace DATest_Utils;

namespace DATest_Utils
{
    AWS_STATIC_STRING_FROM_LITERAL(AWS_ENDPONT, "DA_ENDPOINT");
    AWS_STATIC_STRING_FROM_LITERAL(AWS_CERTI, "DA_CERTI");
    AWS_STATIC_STRING_FROM_LITERAL(AWS_KEY, "DA_KEY");
    AWS_STATIC_STRING_FROM_LITERAL(AWS_TOPIC, "DA_TOPIC");
    AWS_STATIC_STRING_FROM_LITERAL(AWS_THING_NAME, "DA_THING_NAME");
    AWS_STATIC_STRING_FROM_LITERAL(AWS_SHADOW_PROPERTY, "DA_SHADOW_PROPERTY");
    AWS_STATIC_STRING_FROM_LITERAL(AWS_SHADOW_VALUE_SET, "DA_SHADOW_VALUE_SET");
} // namespace DATest_Utils

bool DeviceAdvisorEnvironment::init(TestType type)
{
    /*
     * Use aws_string to retrieve the environment variables.
     */
    aws_string *aws_string_endpoint = NULL;
    aws_string *aws_string_key = NULL;
    aws_string *aws_string_topic = NULL;
    aws_string *aws_string_certificate = NULL;
    aws_string *aws_string_thing_name = NULL;
    aws_string *aws_string_shadow_property = NULL;
    aws_string *aws_string_shadow_value = NULL;

    aws_get_environment_value(g_allocator, AWS_ENDPONT, &aws_string_endpoint);
    aws_get_environment_value(g_allocator, AWS_CERTI, &aws_string_certificate);
    aws_get_environment_value(g_allocator, AWS_KEY, &aws_string_key);
    aws_get_environment_value(g_allocator, AWS_TOPIC, &aws_string_topic);
    aws_get_environment_value(g_allocator, AWS_THING_NAME, &aws_string_thing_name);
    aws_get_environment_value(g_allocator, AWS_SHADOW_PROPERTY, &aws_string_shadow_property);
    aws_get_environment_value(g_allocator, AWS_SHADOW_VALUE_SET, &aws_string_shadow_value);

    /*
     * Assign environment variable value to the parameters.
     */
    endpoint = aws_string_endpoint == nullptr ? "" : aws_string_c_str(aws_string_endpoint);
    certificatePath = aws_string_certificate == nullptr ? "" : aws_string_c_str(aws_string_certificate);
    keyPath = aws_string_key == nullptr ? "" : aws_string_c_str(aws_string_key);
    topic = aws_string_topic == nullptr ? "" : aws_string_c_str(aws_string_topic);
    thing_name = aws_string_thing_name == nullptr ? "" : aws_string_c_str(aws_string_thing_name);
    shadowProperty = aws_string_shadow_property == nullptr ? "" : aws_string_c_str(aws_string_shadow_property);
    shadowValue = aws_string_shadow_value == nullptr ? "" : aws_string_c_str(aws_string_shadow_value);

    /*
     * Destroy the aws_string.
     */
    aws_string_destroy(aws_string_endpoint);
    aws_string_destroy(aws_string_certificate);
    aws_string_destroy(aws_string_key);
    aws_string_destroy(aws_string_topic);
    aws_string_destroy(aws_string_thing_name);
    aws_string_destroy(aws_string_shadow_property);
    aws_string_destroy(aws_string_shadow_value);

    /*
     * Validate environment variables.
     */
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