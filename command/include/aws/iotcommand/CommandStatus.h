#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotcommand/Exports.h>

#include <aws/crt/StlAllocator.h>
#include <aws/crt/Types.h>

namespace Aws
{
    namespace Iotcommand
    {

        /**
         * The status of the job execution.
         *
         */
        enum class CommandStatus
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

        namespace CommandStatusMarshaller
        {
            AWS_IOTCOMMAND_API const char *ToString(CommandStatus val);
            AWS_IOTCOMMAND_API CommandStatus FromString(const Aws::Crt::String &val);
        } // namespace CommandStatusMarshaller
    } // namespace Iotcommand
} // namespace Aws
