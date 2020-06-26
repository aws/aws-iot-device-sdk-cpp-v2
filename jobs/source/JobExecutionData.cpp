/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/iotjobs/JobExecutionData.h>

namespace Aws
{
    namespace Iotjobs
    {

        void JobExecutionData::LoadFromObject(JobExecutionData &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("jobId"))
            {
                val.JobId = doc.GetString("jobId");
            }

            if (doc.ValueExists("jobDocument"))
            {
                val.JobDocument = doc.GetJsonObjectCopy("jobDocument");
            }

            if (doc.ValueExists("status"))
            {
                val.Status = JobStatusMarshaller::FromString(doc.GetString("status"));
            }

            if (doc.ValueExists("versionNumber"))
            {
                val.VersionNumber = doc.GetInteger("versionNumber");
            }

            if (doc.ValueExists("queuedAt"))
            {
                val.QueuedAt = doc.GetDouble("queuedAt");
            }

            if (doc.ValueExists("thingName"))
            {
                val.ThingName = doc.GetString("thingName");
            }

            if (doc.ValueExists("executionNumber"))
            {
                val.ExecutionNumber = doc.GetInt64("executionNumber");
            }

            if (doc.ValueExists("statusDetails"))
            {
                auto statusDetailsMap = doc.GetJsonObject("statusDetails");
                val.StatusDetails = Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String>();
                for (auto &statusDetailsMapMember : statusDetailsMap.GetAllObjects())
                {
                    Aws::Crt::String statusDetailsMapValMember;
                    statusDetailsMapValMember = statusDetailsMapMember.second.AsString();
                    val.StatusDetails->emplace(statusDetailsMapMember.first, std::move(statusDetailsMapValMember));
                }
            }

            if (doc.ValueExists("lastUpdatedAt"))
            {
                val.LastUpdatedAt = doc.GetDouble("lastUpdatedAt");
            }

            if (doc.ValueExists("startedAt"))
            {
                val.StartedAt = doc.GetDouble("startedAt");
            }
        }

        void JobExecutionData::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (JobId)
            {
                object.WithString("jobId", *JobId);
            }

            if (JobDocument)
            {
                object.WithObject("jobDocument", *JobDocument);
            }

            if (Status)
            {
                object.WithString("status", JobStatusMarshaller::ToString(*Status));
            }

            if (VersionNumber)
            {
                object.WithInteger("versionNumber", *VersionNumber);
            }

            if (QueuedAt)
            {
                object.WithDouble("queuedAt", QueuedAt->SecondsWithMSPrecision());
            }

            if (ThingName)
            {
                object.WithString("thingName", *ThingName);
            }

            if (ExecutionNumber)
            {
                object.WithInt64("executionNumber", *ExecutionNumber);
            }

            if (StatusDetails)
            {
                Aws::Crt::JsonObject statusDetailsMap;
                for (auto &statusDetailsMapMember : *StatusDetails)
                {
                    Aws::Crt::JsonObject statusDetailsMapValMember;
                    statusDetailsMapValMember.AsString(statusDetailsMapMember.second);
                    statusDetailsMap.WithObject(statusDetailsMapMember.first, std::move(statusDetailsMapValMember));
                }
                object.WithObject("statusDetails", std::move(statusDetailsMap));
            }

            if (LastUpdatedAt)
            {
                object.WithDouble("lastUpdatedAt", LastUpdatedAt->SecondsWithMSPrecision());
            }

            if (StartedAt)
            {
                object.WithDouble("startedAt", StartedAt->SecondsWithMSPrecision());
            }
        }

        JobExecutionData::JobExecutionData(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        JobExecutionData &JobExecutionData::operator=(const Crt::JsonView &doc)
        {
            *this = JobExecutionData(doc);
            return *this;
        }

    } // namespace Iotjobs
} // namespace Aws
