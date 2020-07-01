/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/iotjobs/RejectedErrorCode.h>

#include <aws/crt/StlAllocator.h>
#include <aws/crt/StringUtils.h>

#include <assert.h>

static const size_t INVALIDTOPIC_HASH = Aws::Crt::HashString("InvalidTopic");
static const size_t INVALIDJSON_HASH = Aws::Crt::HashString("InvalidJson");
static const size_t INVALIDREQUEST_HASH = Aws::Crt::HashString("InvalidRequest");
static const size_t INVALIDSTATETRANSITION_HASH = Aws::Crt::HashString("InvalidStateTransition");
static const size_t RESOURCENOTFOUND_HASH = Aws::Crt::HashString("ResourceNotFound");
static const size_t VERSIONMISMATCH_HASH = Aws::Crt::HashString("VersionMismatch");
static const size_t INTERNALERROR_HASH = Aws::Crt::HashString("InternalError");
static const size_t REQUESTTHROTTLED_HASH = Aws::Crt::HashString("RequestThrottled");
static const size_t TERMINALSTATEREACHED_HASH = Aws::Crt::HashString("TerminalStateReached");

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

                if (hash == INVALIDTOPIC_HASH)
                {
                    return RejectedErrorCode::InvalidTopic;
                }

                if (hash == INVALIDJSON_HASH)
                {
                    return RejectedErrorCode::InvalidJson;
                }

                if (hash == INVALIDREQUEST_HASH)
                {
                    return RejectedErrorCode::InvalidRequest;
                }

                if (hash == INVALIDSTATETRANSITION_HASH)
                {
                    return RejectedErrorCode::InvalidStateTransition;
                }

                if (hash == RESOURCENOTFOUND_HASH)
                {
                    return RejectedErrorCode::ResourceNotFound;
                }

                if (hash == VERSIONMISMATCH_HASH)
                {
                    return RejectedErrorCode::VersionMismatch;
                }

                if (hash == INTERNALERROR_HASH)
                {
                    return RejectedErrorCode::InternalError;
                }

                if (hash == REQUESTTHROTTLED_HASH)
                {
                    return RejectedErrorCode::RequestThrottled;
                }

                if (hash == TERMINALSTATEREACHED_HASH)
                {
                    return RejectedErrorCode::TerminalStateReached;
                }

                assert(0);
                return static_cast<RejectedErrorCode>(-1);
            }
        } // namespace RejectedErrorCodeMarshaller
    }     // namespace Iotjobs
} // namespace Aws
