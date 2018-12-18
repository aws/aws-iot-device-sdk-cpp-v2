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
#include <aws/iotsdk/jobs/JobExecutionState.h>

#include <aws/crt/JsonObject.h>

namespace Aws
{
    namespace IotSdk
    {
        namespace Jobs
        {
            void JobExecutionState::LoadFromObject(JobExecutionState& val, const Crt::JsonView& doc)
            {
                if (doc.ValueExists("status"))
                {
                    val.Status = JobStatusMarshaller::FromString(doc.GetString("status"));
                }

                if (doc.ValueExists("statusDetails"))
                {
                    auto obj = doc.GetJsonObject("statusDetails");
                    for (auto& item : obj.GetAllObjects())
                    {
                        (*val.StatusDetails)[item.first] = item.second.AsString();
                    }
                }

                if (doc.ValueExists("versionNumber"))
                {
                    val.VersionNumber = doc.GetInteger("versionNumber");
                }
            }

            JobExecutionState::JobExecutionState(const Crt::JsonView& doc)
            {
                LoadFromObject(*this, doc);
            }

            JobExecutionState& JobExecutionState::operator=(const Crt::JsonView& doc)
            {
                *this = JobExecutionState(doc);
                return *this;
            }

            void JobExecutionState::SerializeToObject(Crt::JsonObject& doc) const
            {
                if (Status)
                {
                    doc.WithString("status", JobStatusMarshaller::ToString(*Status));
                }

                if (StatusDetails)
                {
                    Crt::JsonObject object;
                    for (auto& item : *StatusDetails)
                    {
                        object.WithString(item.first, item.second);
                    }
                    doc.WithObject("statusDetails", object);
                }

                if (VersionNumber)
                {
                    doc.WithInteger("versionNumber", *VersionNumber);
                }
            }
        }
    }
}
