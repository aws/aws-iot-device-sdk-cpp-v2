#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/iotjobs/Exports.h>

#include <aws/crt/Types.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
namespace Iotjobs
{

    enum class JobStatus
    {
        QUEUED,
        IN_PROGRESS,
        TIMED_OUT,
        FAILED,
        SUCCEEDED,
        CANCELED,
        REJECTED,
        REMOVED,
    };

namespace JobStatusMarshaller
{
    AWS_IOTJOBS_API const char* ToString(JobStatus val);
    JobStatus AWS_IOTJOBS_API FromString(const Aws::Crt::String& val);
}
}
}

