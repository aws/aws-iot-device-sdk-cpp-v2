#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotjobs/Exports.h>

#include <aws/crt/StlAllocator.h>
#include <aws/crt/Types.h>

namespace Aws
{
    namespace Iotjobs
    {

        /**
         * A value indicating the kind of error encountered while processing an AWS IoT Jobs request
         *
         */
        enum class RejectedErrorCode
        {
            /**
             * The request was sent to a topic in the AWS IoT Jobs namespace that does not map to any API.
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
             * An update attempted to change the job execution to a state that is invalid because of the job execution's
             * current state. In this case, the body of the error message also contains the executionState field.
             */
            InvalidStateTransition,

            /**
             * The JobExecution specified by the request topic does not exist.
             */
            ResourceNotFound,

            /**
             * The expected version specified in the request does not match the version of the job execution in the AWS
             * IoT Jobs service. In this case, the body of the error message also contains the executionState field.
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
             * Occurs when a command to describe a job is performed on a job that is in a terminal state.
             */
            TerminalStateReached,

        };

        namespace RejectedErrorCodeMarshaller
        {
            AWS_IOTJOBS_API const char *ToString(RejectedErrorCode val);
            AWS_IOTJOBS_API RejectedErrorCode FromString(const Aws::Crt::String &val);
        } // namespace RejectedErrorCodeMarshaller
    }     // namespace Iotjobs
} // namespace Aws
