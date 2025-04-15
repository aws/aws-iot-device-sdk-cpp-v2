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
             * The device is currently processing the received command.
             */
            IN_PROGRESS,

            /**
             * The device successfully completed the command.
             */
            SUCCEEDED,

            /**
             * The divice failed to complete the command.
             */
            FAILED,

            /**
             * The device received an invalid or incomplete request.
             */
            REJECTED,

            /**
             * When the command execution timed out, this status can be used to provide additional information in the
             * statusReason field in the UpdateCommandExecutionRequest request.
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
