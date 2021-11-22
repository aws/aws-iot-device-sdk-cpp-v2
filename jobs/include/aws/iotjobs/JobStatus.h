#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotjobs/Exports.h>

#include <aws/crt/Types.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
namespace Iotjobs
{

    /**
     * The status of the job execution.
     *
     */
    enum class JobStatus
    {
        /**
         */
        IN_PROGRESS,

        /**
         */
        QUEUED,

        /**
         */
        TIMED_OUT,

        /**
         */
        SUCCEEDED,

        /**
         */
        CANCELED,

        /**
         */
        REJECTED,

        /**
         */
        REMOVED,

    };

namespace JobStatusMarshaller
{
    const char* AWS_IOTJOBS_API ToString(JobStatus val);
    JobStatus AWS_IOTJOBS_API FromString(const Aws::Crt::String& val);
}
}
}

