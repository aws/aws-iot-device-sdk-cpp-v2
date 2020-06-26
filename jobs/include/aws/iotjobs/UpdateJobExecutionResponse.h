#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
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

        class AWS_IOTJOBS_API UpdateJobExecutionResponse final
        {
          public:
            UpdateJobExecutionResponse() = default;

            UpdateJobExecutionResponse(const Crt::JsonView &doc);
            UpdateJobExecutionResponse &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<Aws::Crt::String> ClientToken;
            Aws::Crt::Optional<Aws::Crt::DateTime> Timestamp;
            Aws::Crt::Optional<Aws::Crt::JsonObject> JobDocument;
            Aws::Crt::Optional<Aws::Iotjobs::JobExecutionState> ExecutionState;

          private:
            static void LoadFromObject(UpdateJobExecutionResponse &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotjobs
} // namespace Aws
