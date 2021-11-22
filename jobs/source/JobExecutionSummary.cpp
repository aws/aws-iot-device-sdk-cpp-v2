/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotjobs/JobExecutionSummary.h>

namespace Aws
{
    namespace Iotjobs
    {

        void JobExecutionSummary::LoadFromObject(JobExecutionSummary &val, const Aws::Crt::JsonView &doc)
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

        void JobExecutionSummary::SerializeToObject(Aws::Crt::JsonObject &object) const
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

        JobExecutionSummary::JobExecutionSummary(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        JobExecutionSummary &JobExecutionSummary::operator=(const Crt::JsonView &doc)
        {
            *this = JobExecutionSummary(doc);
            return *this;
        }

    } // namespace Iotjobs
} // namespace Aws
