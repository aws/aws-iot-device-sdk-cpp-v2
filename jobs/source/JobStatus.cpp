/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotjobs/JobStatus.h>

#include <aws/crt/StlAllocator.h>
#include <aws/crt/StringUtils.h>

#include <assert.h>

static const size_t QUEUED_HASH = Aws::Crt::HashString("QUEUED");
static const size_t IN_PROGRESS_HASH = Aws::Crt::HashString("IN_PROGRESS");
static const size_t TIMED_OUT_HASH = Aws::Crt::HashString("TIMED_OUT");
static const size_t FAILED_HASH = Aws::Crt::HashString("FAILED");
static const size_t SUCCEEDED_HASH = Aws::Crt::HashString("SUCCEEDED");
static const size_t CANCELED_HASH = Aws::Crt::HashString("CANCELED");
static const size_t REJECTED_HASH = Aws::Crt::HashString("REJECTED");
static const size_t REMOVED_HASH = Aws::Crt::HashString("REMOVED");

namespace Aws
{
    namespace Iotjobs
    {

        namespace JobStatusMarshaller
        {
            const char *ToString(JobStatus status)
            {
                switch (status)
                {
                    case JobStatus::QUEUED:
                        return "QUEUED";
                    case JobStatus::IN_PROGRESS:
                        return "IN_PROGRESS";
                    case JobStatus::TIMED_OUT:
                        return "TIMED_OUT";
                    case JobStatus::FAILED:
                        return "FAILED";
                    case JobStatus::SUCCEEDED:
                        return "SUCCEEDED";
                    case JobStatus::CANCELED:
                        return "CANCELED";
                    case JobStatus::REJECTED:
                        return "REJECTED";
                    case JobStatus::REMOVED:
                        return "REMOVED";
                    default:
                        assert(0);
                        return "UNKNOWN_VALUE";
                }
            }

            JobStatus FromString(const Crt::String &str)
            {
                size_t hash = Crt::HashString(str.c_str());

                if (hash == QUEUED_HASH)
                {
                    return JobStatus::QUEUED;
                }

                if (hash == IN_PROGRESS_HASH)
                {
                    return JobStatus::IN_PROGRESS;
                }

                if (hash == TIMED_OUT_HASH)
                {
                    return JobStatus::TIMED_OUT;
                }

                if (hash == FAILED_HASH)
                {
                    return JobStatus::FAILED;
                }

                if (hash == SUCCEEDED_HASH)
                {
                    return JobStatus::SUCCEEDED;
                }

                if (hash == CANCELED_HASH)
                {
                    return JobStatus::CANCELED;
                }

                if (hash == REJECTED_HASH)
                {
                    return JobStatus::REJECTED;
                }

                if (hash == REMOVED_HASH)
                {
                    return JobStatus::REMOVED;
                }

                assert(0);
                return static_cast<JobStatus>(-1);
            }
        } // namespace JobStatusMarshaller
    }     // namespace Iotjobs
} // namespace Aws
