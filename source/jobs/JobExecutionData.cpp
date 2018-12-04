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
                    val.JobId = cJSON_GetObjectItem(&node, "jobId")->string;
                }

                if (cJSON_HasObjectItem(&node, "thingName"))
                {
                    val.ThingName = cJSON_GetObjectItem(&node, "thingName")->string;
                }

                if (cJSON_HasObjectItem(&node, "jobDocument"))
                {
                    val.JobDocument = cJSON_GetObjectItem(&node, "jobDocument")->string;
                }

                if (cJSON_HasObjectItem(&node, "jobStatus"))
                {
                    val.Status = JobStatusMarshaller::FromString(cJSON_GetObjectItem(&node, "jobStatus")->string);
                }

                if (cJSON_HasObjectItem(&node, "queuedAt"))
                {
                    val.QueuedAt = (uint64_t)cJSON_GetObjectItem(&node, "queuedAt")->valuedouble;
                }

                if (cJSON_HasObjectItem(&node, "startedAt"))
                {
                    val.StartedAt = (uint64_t)cJSON_GetObjectItem(&node, "startedAt")->valuedouble;
                }

                if (cJSON_HasObjectItem(&node, "lastUpdatedAt"))
                {
                    val.LastUpdatedAt = (uint64_t)cJSON_GetObjectItem(&node, "lastUpdatedAt")->valuedouble;
                }

                if (cJSON_HasObjectItem(&node, "versionNumber"))
                {
                    val.VersionNumber = (uint32_t)cJSON_GetObjectItem(&node, "versionNumber")->valueint;
                }

                if (cJSON_HasObjectItem(&node, "executionNumber"))
                {
                    val.ExecutionNumber = (uint64_t)cJSON_GetObjectItem(&node, "executionNumber")->valueint;
                }
            }

            JobExecutionData::JobExecutionData() noexcept
            {
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
        }
    }
}