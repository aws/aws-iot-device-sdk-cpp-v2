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
static const size_t INVALID_JSON_HASH = Aws::Crt::HashString("InvalidJson");
static const size_t INVALID_REQUEST_HASH = Aws::Crt::HashString("InvalidRequest");
static const size_t INVALID_STATE_TRANSITION_HASH = Aws::Crt::HashString("InvalidStateTransition");
static const size_t RESOURCE_NOT_FOUND_HASH = Aws::Crt::HashString("ResourceNotFound");
static const size_t VERSION_MISMATCH_HASH = Aws::Crt::HashString("VersionMismatch");
static const size_t INTERNAL_ERROR_HASH = Aws::Crt::HashString("InternalError");
static const size_t REQUEST_THROTTLED_HASH = Aws::Crt::HashString("RequestThrottled");
static const size_t TERMINAL_STATE_REACHED_HASH = Aws::Crt::HashString("TerminalStateReached");

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
                    case RejectedErrorCode::InvalidTopic:
                        return "InvalidTopic";
                    case RejectedErrorCode::InvalidJson:
                        return "InvalidJson";
                    case RejectedErrorCode::InvalidRequest:
                        return "InvalidRequest";
                    case RejectedErrorCode::InvalidStateTransition:
                        return "InvalidStateTransition";
                    case RejectedErrorCode::ResourceNotFound:
                        return "ResourceNotFound";
                    case RejectedErrorCode::VersionMismatch:
                        return "VersionMismatch";
                    case RejectedErrorCode::InternalError:
                        return "InternalError";
                    case RejectedErrorCode::RequestThrottled:
                        return "RequestThrottled";
                    case RejectedErrorCode::TerminalStateReached:
                        return "TerminalStateReached";
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

                if (hash == INVALID_JSON_HASH)
                {
                    return RejectedErrorCode::InvalidJson;
                }

                if (hash == INVALID_REQUEST_HASH)
                {
                    return RejectedErrorCode::InvalidRequest;
                }

                if (hash == INVALID_STATE_TRANSITION_HASH)
                {
                    return RejectedErrorCode::InvalidStateTransition;
                }

                if (hash == RESOURCE_NOT_FOUND_HASH)
                {
                    return RejectedErrorCode::ResourceNotFound;
                }

                if (hash == VERSION_MISMATCH_HASH)
                {
                    return RejectedErrorCode::VersionMismatch;
                }

                if (hash == INTERNAL_ERROR_HASH)
                {
                    return RejectedErrorCode::InternalError;
                }

                if (hash == REQUEST_THROTTLED_HASH)
                {
                    return RejectedErrorCode::RequestThrottled;
                }

                if (hash == TERMINAL_STATE_REACHED_HASH)
                {
                    return RejectedErrorCode::TerminalStateReached;
                }

                assert(0);
                return static_cast<RejectedErrorCode>(-1);
            }
        } // namespace RejectedErrorCodeMarshaller
    }     // namespace Iotjobs
} // namespace Aws
