#pragma once
/* Copyright 2010-2019 Amazon.com, Inc. or its affiliates. All Rights Reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License").
* You may not use this file except in compliance with the License.
* A copy of the License is located at
*
*  http://aws.amazon.com/apache2.0
*
* or in the "license" file accompanying this file. This file is distributed
* on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
* express or implied. See the License for the specific language governing
* permissions and limitations under the License.

* This file is generated
*/
#include <aws/crt/DateTime.h>
#include <aws/crt/JsonObject.h>
#include <aws/iotjobs/JobStatus.h>

#include <aws/iotjobs/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotjobs
    {

        class AWS_IOTJOBS_API JobExecutionData final
        {
          public:
            JobExecutionData() = default;

            JobExecutionData(const Crt::JsonView &doc);
            JobExecutionData &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<Aws::Crt::String> JobId;
            Aws::Crt::Optional<Aws::Crt::String> ThingName;
            Aws::Crt::Optional<Aws::Crt::JsonObject> JobDocument;
            Aws::Crt::Optional<int64_t> ExecutionNumber;
            Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String>> StatusDetails;
            Aws::Crt::Optional<Aws::Iotjobs::JobStatus> Status;
            Aws::Crt::Optional<int32_t> VersionNumber;
            Aws::Crt::Optional<Aws::Crt::DateTime> QueuedAt;
            Aws::Crt::Optional<Aws::Crt::DateTime> LastUpdatedAt;
            Aws::Crt::Optional<Aws::Crt::DateTime> StartedAt;

          private:
            static void LoadFromObject(JobExecutionData &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotjobs
} // namespace Aws
