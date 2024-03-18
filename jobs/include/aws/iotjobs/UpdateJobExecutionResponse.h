#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/crt/DateTime.h>
#include <aws/crt/JsonObject.h>
#include <aws/iotjobs/JobExecutionState.h>

#include <aws/iotjobs/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotjobs
    {

        /**
         * Response payload to an UpdateJobExecution request.
         *
         */
        class AWS_IOTJOBS_API UpdateJobExecutionResponse final
        {
          public:
            UpdateJobExecutionResponse() = default;

            UpdateJobExecutionResponse(const Crt::JsonView &doc);
            UpdateJobExecutionResponse &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * A client token used to correlate requests and responses.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ClientToken;

            /**
             * Contains data about the state of a job execution.
             *
             */
            Aws::Crt::Optional<Aws::Iotjobs::JobExecutionState> ExecutionState;

            /**
             * A UTF-8 encoded JSON document that contains information that your devices need to perform the job.
             *
             */
            Aws::Crt::Optional<Aws::Crt::JsonObject> JobDocument;

            /**
             * The time when the message was sent.
             *
             */
            Aws::Crt::Optional<Aws::Crt::DateTime> Timestamp;

          private:
            static void LoadFromObject(UpdateJobExecutionResponse &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotjobs
} // namespace Aws
