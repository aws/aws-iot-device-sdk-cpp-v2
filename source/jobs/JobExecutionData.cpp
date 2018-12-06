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
#include <aws/iotsdk/jobs/JobExecutionData.h>

namespace Aws
{
    namespace IotSdk
    {
        namespace Jobs
        {
            void JobExecutionData::LoadFromNode(JobExecutionData& val, const cJSON& node)
            {
                if (cJSON_HasObjectItem(&node, "jobId"))
                {
                    val.JobId = cJSON_GetObjectItem(&node, "jobId")->valuestring;
                }

                if (cJSON_HasObjectItem(&node, "thingName"))
                {
                    val.ThingName = cJSON_GetObjectItem(&node, "thingName")->valuestring;
                }

                if (cJSON_HasObjectItem(&node, "jobDocument"))
                {
                    val.JobDocument = cJSON_GetObjectItem(&node, "jobDocument")->valuestring;
                }

                if (cJSON_HasObjectItem(&node, "status"))
                {
                    val.Status = JobStatusMarshaller::FromString(cJSON_GetObjectItem(&node, "status")->valuestring);
                }

                if (cJSON_HasObjectItem(&node, "queuedAt"))
                {
                    val.QueuedAt = cJSON_GetObjectItem(&node, "queuedAt")->valuedouble;
                }

                if (cJSON_HasObjectItem(&node, "startedAt"))
                {
                    val.StartedAt = cJSON_GetObjectItem(&node, "startedAt")->valuedouble;
                }

                if (cJSON_HasObjectItem(&node, "lastUpdatedAt"))
                {
                    val.LastUpdatedAt = cJSON_GetObjectItem(&node, "lastUpdatedAt")->valuedouble;
                }

                if (cJSON_HasObjectItem(&node, "versionNumber"))
                {
                    val.VersionNumber = static_cast<int32_t>(cJSON_GetObjectItem(&node, "versionNumber")->valueint);
                }

                if (cJSON_HasObjectItem(&node, "executionNumber"))
                {
                    val.ExecutionNumber = static_cast<int64_t>(cJSON_GetObjectItem(&node, "executionNumber")->valueint);
                }
            }

            JobExecutionData::JobExecutionData(const cJSON& node)
            {
                LoadFromNode(*this, node);
            }

            JobExecutionData& JobExecutionData::operator=(const cJSON& node)
            {
                *this = JobExecutionData(node);
                return *this;
            }

            void JobExecutionData::SerializeToNode(cJSON& node) const
            {
                if (JobId)
                {
                    cJSON_AddStringToObject(&node, "jobId", JobId->c_str());
                }

                if (ThingName)
                {
                    cJSON_AddStringToObject(&node, "thingName", ThingName->c_str());
                }

                if (JobDocument)
                {
                    cJSON_AddStringToObject(&node, "jobDocument", JobDocument->c_str());
                }

                if (Status)
                {
                    cJSON_AddStringToObject(&node, "status", JobStatusMarshaller::ToString(*Status));
                }

                if (QueuedAt)
                {
                    cJSON_AddNumberToObject(&node, "queuedAt", QueuedAt->SecondsWithMSPrecision());
                }

                if (StartedAt)
                {
                    cJSON_AddNumberToObject(&node, "startedAt", StartedAt->SecondsWithMSPrecision());
                }

                if (LastUpdatedAt)
                {
                    cJSON_AddNumberToObject(&node, "lastUpdatedAt", LastUpdatedAt->SecondsWithMSPrecision());
                }

                if (VersionNumber)
                {
                    cJSON_AddNumberToObject(&node, "versionNumber", static_cast<double>(*VersionNumber));
                }

                if (ExecutionNumber)
                {
                    cJSON_AddNumberToObject(&node, "executionNumber", static_cast<double>(*ExecutionNumber));
                }
            }
        }
    }
}