/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotjobs/JobStatus.h>

#include <aws/crt/StlAllocator.h>
#include <aws/crt/StringUtils.h>

#include <assert.h>

static const size_t _HASH = Aws::Crt::HashString("");
static const size_t _HASH = Aws::Crt::HashString("");
static const size_t _HASH = Aws::Crt::HashString("");
static const size_t _HASH = Aws::Crt::HashString("");
static const size_t _HASH = Aws::Crt::HashString("");
static const size_t _HASH = Aws::Crt::HashString("");
static const size_t _HASH = Aws::Crt::HashString("");


namespace Aws
{
namespace Iotjobs
{

namespace JobStatusMarshaller
{
    const char* ToString(JobStatus status)
    {
        switch(status)
        {
        case JobStatus:::
            return "";
        case JobStatus:::
            return "";
        case JobStatus:::
            return "";
        case JobStatus:::
            return "";
        case JobStatus:::
            return "";
        case JobStatus:::
            return "";
        case JobStatus:::
            return "";
        default:
            assert(0);
            return "UNKNOWN_VALUE";
        }
    }

    JobStatus FromString(const Crt::String& str)
    {
        size_t hash = Crt::HashString(str.c_str());

        if (hash == _HASH)
        {
            return JobStatus::;
        }

        if (hash == _HASH)
        {
            return JobStatus::;
        }

        if (hash == _HASH)
        {
            return JobStatus::;
        }

        if (hash == _HASH)
        {
            return JobStatus::;
        }

        if (hash == _HASH)
        {
            return JobStatus::;
        }

        if (hash == _HASH)
        {
            return JobStatus::;
        }

        if (hash == _HASH)
        {
            return JobStatus::;
        }


        assert(0);
        return static_cast<JobStatus>(-1);
    }
}
}
}
