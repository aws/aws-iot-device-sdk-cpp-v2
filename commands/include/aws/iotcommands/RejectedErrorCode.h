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
         * A value indicating the kind of error encountered while processing an AWS IoT Commands request
         *
         */
        enum class RejectedErrorCode
        {
            /**
             * The request was sent to a topic in the AWS IoT Commands namespace that does not map to any API.
             */
            InvalidTopic,

            /**
             * The contents of the request could not be interpreted as valid UTF-8-encoded JSON.
             */
            InvalidJson,

            /**
             * The contents of the request were invalid. The message contains details about the error.
             */
            InvalidRequest,

            /**
             * An update attempted to change the command execution to a state that is invalid because of the command
             * execution's current state. In this case, the body of the error message also contains the executionState
             * field.
             */
            InvalidStateTransition,

            /**
             * The CommandExecution specified by the request topic does not exist.
             */
            ResourceNotFound,

            /**
             * The expected version specified in the request does not match the version of the command execution in the
             * AWS IoT Commands service. In this case, the body of the error message also contains the executionState
             * field.
             */
            VersionMismatch,

            /**
             * There was an internal error during the processing of the request.
             */
            InternalError,

            /**
             * The request was throttled.
             */
            RequestThrottled,

            /**
             * Occurs when a command to describe a command is performed on a command that is in a terminal state.
             */
            TerminalStateReached,

        };

        namespace RejectedErrorCodeMarshaller
        {
            AWS_IOTCOMMANDS_API const char *ToString(RejectedErrorCode val);
            AWS_IOTCOMMANDS_API RejectedErrorCode FromString(const Aws::Crt::String &val);
        } // namespace RejectedErrorCodeMarshaller
    } // namespace Iotcommands
} // namespace Aws
