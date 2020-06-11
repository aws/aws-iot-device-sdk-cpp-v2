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
#include <aws/iotjobs/StartNextPendingJobExecutionRequest.h>

namespace Aws
{
namespace Iotjobs
{

    void StartNextPendingJobExecutionRequest::LoadFromObject(StartNextPendingJobExecutionRequest& val, const Aws::Crt::JsonView &doc)
    {
        (void)val;
        (void)doc;

        if (doc.ValueExists("stepTimeoutInMinutes"))
        {
            val.StepTimeoutInMinutes = doc.GetInt64("stepTimeoutInMinutes");
        }

        if (doc.ValueExists("clientToken"))
        {
            val.ClientToken = doc.GetString("clientToken");
        }

        if (doc.ValueExists("statusDetails"))
        {
            auto statusDetailsMap = doc.GetJsonObject("statusDetails");
            val.StatusDetails = Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String>();
            for (auto& statusDetailsMapMember : statusDetailsMap.GetAllObjects())
            {
                Aws::Crt::String statusDetailsMapValMember;
                statusDetailsMapValMember = statusDetailsMapMember.second.AsString();
                val.StatusDetails->emplace(statusDetailsMapMember.first, std::move(statusDetailsMapValMember));
            }

        }

    }

    void StartNextPendingJobExecutionRequest::SerializeToObject(Aws::Crt::JsonObject& object) const
    {
        (void)object;

        if (StepTimeoutInMinutes)
        {
            object.WithInt64("stepTimeoutInMinutes", *StepTimeoutInMinutes);
        }

        if (ClientToken)
        {
            object.WithString("clientToken", *ClientToken);
        }

        if (StatusDetails)
        {
            Aws::Crt::JsonObject statusDetailsMap;
            for (auto& statusDetailsMapMember : *StatusDetails)
            {
                Aws::Crt::JsonObject statusDetailsMapValMember;
                statusDetailsMapValMember.AsString(statusDetailsMapMember.second);
                statusDetailsMap.WithObject(statusDetailsMapMember.first, std::move(statusDetailsMapValMember));
            }
            object.WithObject("statusDetails", std::move(statusDetailsMap));
        }

    }

    StartNextPendingJobExecutionRequest::StartNextPendingJobExecutionRequest(const Crt::JsonView& doc)
    {
        LoadFromObject(*this, doc);
    }

    StartNextPendingJobExecutionRequest& StartNextPendingJobExecutionRequest::operator=(const Crt::JsonView& doc)
    {
        *this = StartNextPendingJobExecutionRequest(doc);
        return *this;
    }

}
}
