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

namespace Aws
{
    namespace Crt
    {
        class JsonObject;
        class JsonView;
    }

    namespace IotSdk
    {
        namespace Jobs
        {
            class AWS_CRT_CPP_API JobExecutionState final
            {
            public:
                JobExecutionState() = default;

                JobExecutionState(const Crt::JsonView& doc);
                JobExecutionState& operator=(const Crt::JsonView& doc);

                void SerializeToObject(Crt::JsonObject& doc) const;

                Crt::Optional<JobStatus> Status;
                Crt::Optional<Crt::Map<Crt::String, Crt::String>> StatusDetails;
                Crt::Optional<int32_t> VersionNumber;

            private:
                static void LoadFromObject(JobExecutionState&, const Crt::JsonView& doc);
            };
        }
    }
}
