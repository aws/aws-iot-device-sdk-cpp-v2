/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotjobs/RejectedErrorCode.h>

#include <aws/crt/StlAllocator.h>
#include <aws/crt/StringUtils.h>

#include <assert.h>

static const size_t INVALID_TOPIC_HASH = Aws::Crt::HashString("InvalidTopic");
static const size_t INVALID_STATE_TRANSITION_HASH = Aws::Crt::HashString("InvalidStateTransition");
static const size_t RESOURCE_NOT_FOUND_HASH = Aws::Crt::HashString("ResourceNotFound");
static const size_t INVALID_REQUEST_HASH = Aws::Crt::HashString("InvalidRequest");
static const size_t REQUEST_THROTTLED_HASH = Aws::Crt::HashString("RequestThrottled");
static const size_t INTERNAL_ERROR_HASH = Aws::Crt::HashString("InternalError");
static const size_t TERMINAL_STATE_REACHED_HASH = Aws::Crt::HashString("TerminalStateReached");
static const size_t INVALID_JSON_HASH = Aws::Crt::HashString("InvalidJson");
static const size_t VERSION_MISMATCH_HASH = Aws::Crt::HashString("VersionMismatch");

namespace Aws
{
    namespace Iotjobs
    {

        namespace RejectedErrorCodeMarshaller
        {
            const char *ToString(RejectedErrorCode status)
            {
                switch (status)
                {
                    case RejectedErrorCode::INVALID_TOPIC:
                        return "InvalidTopic";
                    case RejectedErrorCode::INVALID_STATE_TRANSITION:
                        return "InvalidStateTransition";
                    case RejectedErrorCode::RESOURCE_NOT_FOUND:
                        return "ResourceNotFound";
                    case RejectedErrorCode::INVALID_REQUEST:
                        return "InvalidRequest";
                    case RejectedErrorCode::REQUEST_THROTTLED:
                        return "RequestThrottled";
                    case RejectedErrorCode::INTERNAL_ERROR:
                        return "InternalError";
                    case RejectedErrorCode::TERMINAL_STATE_REACHED:
                        return "TerminalStateReached";
                    case RejectedErrorCode::INVALID_JSON:
                        return "InvalidJson";
                    case RejectedErrorCode::VERSION_MISMATCH:
                        return "VersionMismatch";
                    default:
                        assert(0);
                        return "UNKNOWN_VALUE";
                }
            }

            RejectedErrorCode FromString(const Crt::String &str)
            {
                size_t hash = Crt::HashString(str.c_str());

                if (hash == INVALID_TOPIC_HASH)
                {
                    return RejectedErrorCode::InvalidTopic;
                }

                if (hash == INVALID_STATE_TRANSITION_HASH)
                {
                    return RejectedErrorCode::InvalidStateTransition;
                }

                if (hash == RESOURCE_NOT_FOUND_HASH)
                {
                    return RejectedErrorCode::ResourceNotFound;
                }

                if (hash == INVALID_REQUEST_HASH)
                {
                    return RejectedErrorCode::InvalidRequest;
                }

                if (hash == REQUEST_THROTTLED_HASH)
                {
                    return RejectedErrorCode::RequestThrottled;
                }

                if (hash == INTERNAL_ERROR_HASH)
                {
                    return RejectedErrorCode::InternalError;
                }

                if (hash == TERMINAL_STATE_REACHED_HASH)
                {
                    return RejectedErrorCode::TerminalStateReached;
                }

                if (hash == INVALID_JSON_HASH)
                {
                    return RejectedErrorCode::InvalidJson;
                }

                if (hash == VERSION_MISMATCH_HASH)
                {
                    return RejectedErrorCode::VersionMismatch;
                }

                assert(0);
                return static_cast<RejectedErrorCode>(-1);
            }
        } // namespace RejectedErrorCodeMarshaller
    }     // namespace Iotjobs
} // namespace Aws
