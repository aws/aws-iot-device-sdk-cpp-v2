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
         * The status of the command execution.
         *
         */
        enum class CommandExecutionStatus
        {
            /**
             */
            IN_PROGRESS,

            /**
             */
            SUCCEEDED,

            /**
             */
            FAILED,

            /**
             */
            REJECTED,

            /**
             */
            TIMED_OUT,

        };

        namespace CommandExecutionStatusMarshaller
        {
            AWS_IOTCOMMANDS_API const char *ToString(CommandExecutionStatus val);
            AWS_IOTCOMMANDS_API CommandExecutionStatus FromString(const Aws::Crt::String &val);
        } // namespace CommandExecutionStatusMarshaller
    } // namespace Iotcommands
} // namespace Aws
