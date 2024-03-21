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
         * Data needed to make a StartNextPendingJobExecution request.
         *
         */
        class AWS_IOTJOBS_API StartNextPendingJobExecutionRequest final
        {
          public:
            StartNextPendingJobExecutionRequest() = default;

            StartNextPendingJobExecutionRequest(const Crt::JsonView &doc);
            StartNextPendingJobExecutionRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * IoT Thing the request is relative to.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ThingName;

            /**
             * Optional. A client token used to correlate requests and responses. Enter an arbitrary value here and it
             * is reflected in the response.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ClientToken;

            /**
             * Specifies the amount of time this device has to finish execution of this job.
             *
             */
            Aws::Crt::Optional<int64_t> StepTimeoutInMinutes;

            /**
             * A collection of name-value pairs that describe the status of the job execution. If not specified, the
             * statusDetails are unchanged.
             *
             */
            Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String>> StatusDetails;

          private:
            static void LoadFromObject(StartNextPendingJobExecutionRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotjobs
} // namespace Aws
