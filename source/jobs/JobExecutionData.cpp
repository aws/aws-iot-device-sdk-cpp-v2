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

#include <aws/crt/JsonObject.h>

namespace Aws
{
    namespace IotSdk
    {
        namespace Jobs
        {
            void JobExecutionData::LoadFromObject(JobExecutionData &val, const Crt::JsonView &doc)
            {
                if (doc.ValueExists("jobId"))
                {
                    val.JobId = doc.GetString("jobId");
                }

                if (doc.ValueExists("thingName"))
                {
                    val.ThingName = doc.GetString("thingName");
                }

                if (doc.ValueExists("jobDocument"))
                {
                    val.JobDocument = doc.GetObjectCopy("jobDocument");
                }

                if (doc.ValueExists("status"))
                {
                    val.Status = JobStatusMarshaller::FromString(doc.GetString("status").c_str());
                }

                if (doc.ValueExists("queuedAt"))
                {
                    val.QueuedAt = doc.GetDouble("queuedAt");
                }

                if (doc.ValueExists("startedAt"))
                {
                    val.StartedAt = doc.GetDouble("startedAt");
                }

                if (doc.ValueExists("lastUpdatedAt"))
                {
                    val.LastUpdatedAt = doc.GetDouble("lastUpdatedAt");
                }

                if (doc.ValueExists("versionNumber"))
                {
                    val.VersionNumber = doc.GetInteger("versionNumber");

                }

                if (doc.ValueExists("executionNumber"))
                {
                    val.ExecutionNumber = doc.GetInt64("executionNumber");
                }
            }

            JobExecutionData::JobExecutionData(const Crt::JsonView& doc)
            {
                LoadFromObject(*this, doc);
            }

            JobExecutionData& JobExecutionData::operator=(const Crt::JsonView& doc)
            {
                *this = JobExecutionData(doc);
                return *this;
            }

            void JobExecutionData::SerializeToObject(Crt::JsonObject& doc) const
            {
                if (JobId)
                {
                    doc.WithString("jobId", *JobId);
                }

                if (ThingName)
                {
                    doc.WithString("thingName", *ThingName);
                }

                if (JobDocument)
                {
                    doc.WithObject("jobDocument", *JobDocument);
                }

                if (Status)
                {
                    doc.WithString("status", JobStatusMarshaller::ToString(*Status));
                }

                if (QueuedAt)
                {
                    doc.WithDouble("queuedAt", QueuedAt->SecondsWithMSPrecision());
                }

                if (StartedAt)
                {
                    doc.WithDouble("startedAt", StartedAt->SecondsWithMSPrecision());
                }

                if (LastUpdatedAt)
                {
                    doc.WithDouble("lastUpdatedAt", LastUpdatedAt->SecondsWithMSPrecision());
                }

                if (VersionNumber)
                {
                    doc.WithInteger("versionNumber", *VersionNumber);
                }

                if (ExecutionNumber)
                {
                    doc.WithInt64("executionNumber", *ExecutionNumber);
                }
            }
        }
    }
}