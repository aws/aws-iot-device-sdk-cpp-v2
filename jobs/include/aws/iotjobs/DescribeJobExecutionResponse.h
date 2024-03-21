#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/crt/DateTime.h>
#include <aws/iotjobs/JobExecutionData.h>

#include <aws/iotjobs/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotjobs
    {

        /**
         * Response payload to a DescribeJobExecution request.
         *
         */
        class AWS_IOTJOBS_API DescribeJobExecutionResponse final
        {
          public:
            DescribeJobExecutionResponse() = default;

            DescribeJobExecutionResponse(const Crt::JsonView &doc);
            DescribeJobExecutionResponse &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * A client token used to correlate requests and responses.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ClientToken;

            /**
             * Contains data about a job execution.
             *
             */
            Aws::Crt::Optional<Aws::Iotjobs::JobExecutionData> Execution;

            /**
             * The time when the message was sent.
             *
             */
            Aws::Crt::Optional<Aws::Crt::DateTime> Timestamp;

          private:
            static void LoadFromObject(DescribeJobExecutionResponse &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotjobs
} // namespace Aws
