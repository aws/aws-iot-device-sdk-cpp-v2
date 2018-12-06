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

namespace Aws
{
    namespace IotSdk
    {
        namespace Jobs
        {
            void JobExecutionState::LoadFromNode(JobExecutionState& val, const cJSON& node)
            {

                if (cJSON_HasObjectItem(&node, "status"))
                {
                    val.Status = JobStatusMarshaller::FromString(cJSON_GetObjectItem(&node, "status")->valuestring);
                }

                if (cJSON_HasObjectItem(&node, "statusDetails"))
                {
                    auto objectItem = cJSON_GetObjectItem(&node, "statusDetails")->child;
                    while (objectItem)
                    {
                        (*val.StatusDetails)[objectItem->string] = objectItem->valuestring;
                        objectItem = objectItem->next;
                    }
                }

                if (cJSON_HasObjectItem(&node, "versionNumber"))
                {
                    val.VersionNumber = static_cast<int32_t>(cJSON_GetObjectItem(&node, "versionNumber")->valueint);
                }

            }

            JobExecutionState::JobExecutionState(const cJSON& node)
            {
                LoadFromNode(*this, node);
            }

            JobExecutionState& JobExecutionState::operator=(const cJSON& node)
            {
                *this = JobExecutionState(node);
                return *this;
            }

            void JobExecutionState::SerializeToNode(cJSON& node) const
            {
                if (Status)
                {
                    cJSON_AddStringToObject(&node, "status", JobStatusMarshaller::ToString(*Status));
                }

                if (StatusDetails)
                {
                    auto map = cJSON_AddObjectToObject(&node, "statusDetails");
                    for (auto& item : *StatusDetails)
                    {
                        cJSON_AddStringToObject(map, item.first.c_str(), item.second.c_str());
                    }
                }

                if (VersionNumber)
                {
                    cJSON_AddNumberToObject(&node, "versionNumber", static_cast<double>(*VersionNumber));
                }
            }
        }
    }
}