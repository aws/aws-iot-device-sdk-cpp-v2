#pragma once
/*
* Copyright 2010-2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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
*/
#include <aws/iotsdk/jobs/JobStatus.h>

#include <aws/crt/DateTime.h>
#include <aws/crt/Types.h>
#include <aws/crt/StlAllocator.h>

#include <aws/iotsdk/external/cJSON.h>


namespace Aws
{
    namespace IotSdk
    {
        namespace Jobs
        {
            class JobExecutionState final
            {
            public:
                JobExecutionState() = default;

                JobExecutionState(const cJSON& node);
                JobExecutionState& operator=(const cJSON& node);

                void SerializeToNode(cJSON& node) const;

                Crt::Optional<JobStatus> Status;
                Crt::Optional<Crt::Map<Crt::String, Crt::String>> StatusDetails;
                Crt::Optional<int32_t> VersionNumber;

            private:
                static void LoadFromNode(JobExecutionState&, const cJSON& node);
            };
        }
    }
}
