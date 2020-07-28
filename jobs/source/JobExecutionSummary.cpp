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
#include <aws/iotjobs/JobExecutionSummary.h>

namespace Aws
{
namespace Iotjobs
{

    void JobExecutionSummary::LoadFromObject(JobExecutionSummary& val, const Aws::Crt::JsonView &doc)
    {
        (void)val;
        (void)doc;

        if (doc.ValueExists("lastUpdatedAt"))
        {
            val.LastUpdatedAt = doc.GetDouble("lastUpdatedAt");
        }

        if (doc.ValueExists("executionNumber"))
        {
            val.ExecutionNumber = doc.GetInt64("executionNumber");
        }

        if (doc.ValueExists("startedAt"))
        {
            val.StartedAt = doc.GetDouble("startedAt");
        }

        if (doc.ValueExists("versionNumber"))
        {
            val.VersionNumber = doc.GetInteger("versionNumber");
        }

        if (doc.ValueExists("jobId"))
        {
            val.JobId = doc.GetString("jobId");
        }

        if (doc.ValueExists("queuedAt"))
        {
            val.QueuedAt = doc.GetDouble("queuedAt");
        }

    }

    void JobExecutionSummary::SerializeToObject(Aws::Crt::JsonObject& object) const
    {
        (void)object;

        if (LastUpdatedAt)
        {
            object.WithDouble("lastUpdatedAt", LastUpdatedAt->SecondsWithMSPrecision());
        }

        if (ExecutionNumber)
        {
            object.WithInt64("executionNumber", *ExecutionNumber);
        }

        if (StartedAt)
        {
            object.WithDouble("startedAt", StartedAt->SecondsWithMSPrecision());
        }

        if (VersionNumber)
        {
            object.WithInteger("versionNumber", *VersionNumber);
        }

        if (JobId)
        {
            object.WithString("jobId", *JobId);
        }

        if (QueuedAt)
        {
            object.WithDouble("queuedAt", QueuedAt->SecondsWithMSPrecision());
        }

    }

    JobExecutionSummary::JobExecutionSummary(const Crt::JsonView& doc)
    {
        LoadFromObject(*this, doc);
    }

    JobExecutionSummary& JobExecutionSummary::operator=(const Crt::JsonView& doc)
    {
        *this = JobExecutionSummary(doc);
        return *this;
    }

}
}
