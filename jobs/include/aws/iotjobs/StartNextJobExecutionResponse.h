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
         * Response payload to a StartNextJobExecution request.
         *
         */
        class AWS_IOTJOBS_API StartNextJobExecutionResponse final
        {
          public:
            StartNextJobExecutionResponse() = default;

            StartNextJobExecutionResponse(const Crt::JsonView &doc);
            StartNextJobExecutionResponse &operator=(const Crt::JsonView &doc);

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
             * The time when the message was sent to the device.
             *
             */
            Aws::Crt::Optional<Aws::Crt::DateTime> Timestamp;

          private:
            static void LoadFromObject(StartNextJobExecutionResponse &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotjobs
} // namespace Aws
