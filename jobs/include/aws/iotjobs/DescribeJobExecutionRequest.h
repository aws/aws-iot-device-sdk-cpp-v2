#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotjobs/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotjobs
    {

        /**
         * Data needed to make a DescribeJobExecution request.
         *
         */
        class AWS_IOTJOBS_API DescribeJobExecutionRequest final
        {
          public:
            DescribeJobExecutionRequest() = default;

            DescribeJobExecutionRequest(const Crt::JsonView &doc);
            DescribeJobExecutionRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * The name of the thing associated with the device.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ThingName;

            /**
             * The unique identifier assigned to this job when it was created. Or use $next to return the next pending
             * job execution for a thing (status IN_PROGRESS or QUEUED). In this case, any job executions with status
             * IN_PROGRESS are returned first. Job executions are returned in the order in which they were created.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> JobId;

            /**
             * An opaque string used to correlate requests and responses. Enter an arbitrary value here and it is
             * reflected in the response.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ClientToken;

            /**
             * Optional. A number that identifies a job execution on a device. If not specified, the latest job
             * execution is returned.
             *
             */
            Aws::Crt::Optional<int64_t> ExecutionNumber;

            /**
             * Optional. Unless set to false, the response contains the job document. The default is true.
             *
             */
            Aws::Crt::Optional<bool> IncludeJobDocument;

          private:
            static void LoadFromObject(DescribeJobExecutionRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotjobs
} // namespace Aws
