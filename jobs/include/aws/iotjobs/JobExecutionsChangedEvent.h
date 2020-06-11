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
#include <aws/iotjobs/JobExecutionSummary.h>
#include <aws/iotjobs/JobStatus.h>

#include <aws/iotjobs/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
namespace Iotjobs
{

    class AWS_IOTJOBS_API JobExecutionsChangedEvent final
    {
    public:
        JobExecutionsChangedEvent() = default;

        JobExecutionsChangedEvent(const Crt::JsonView& doc);
        JobExecutionsChangedEvent& operator=(const Crt::JsonView& doc);

        void SerializeToObject(Crt::JsonObject& doc) const;


        Aws::Crt::Optional<Aws::Crt::Map<Aws::Iotjobs::JobStatus, Aws::Crt::Vector<Aws::Iotjobs::JobExecutionSummary>>> Jobs;
        Aws::Crt::Optional<Aws::Crt::DateTime> Timestamp;

    private:
        static void LoadFromObject(JobExecutionsChangedEvent& obj, const Crt::JsonView &doc);
    };
}
}

