/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotjobs/JobExecutionsChangedEvent.h>

namespace Aws
{
    namespace Iotjobs
    {

        void JobExecutionsChangedEvent::LoadFromObject(JobExecutionsChangedEvent &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("jobs"))
            {
                auto jobsMap = doc.GetJsonObject("jobs");
                val.Jobs =
                    Aws::Crt::Map<Aws::Iotjobs::JobStatus, Aws::Crt::Vector<Aws::Iotjobs::JobExecutionSummary>>();
                for (auto &jobsMapMember : jobsMap.GetAllObjects())
                {
                    Aws::Crt::Vector<Aws::Iotjobs::JobExecutionSummary> jobsMapValMember;
                    auto valueList = jobsMapMember.second.AsArray();
                    for (auto &valueListMember : valueList)
                    {
                        Aws::Iotjobs::JobExecutionSummary valueListValMember;
                        valueListValMember = valueListMember.AsObject();
                        jobsMapValMember.push_back(std::move(valueListValMember));
                    }
                    val.Jobs->emplace(
                        JobStatusMarshaller::FromString(jobsMapMember.first), std::move(jobsMapValMember));
                }
            }

            if (doc.ValueExists("timestamp"))
            {
                val.Timestamp = doc.GetDouble("timestamp");
            }
        }

        void JobExecutionsChangedEvent::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (Jobs)
            {
                Aws::Crt::JsonObject jobsMap;
                for (auto &jobsMapMember : *Jobs)
                {
                    Aws::Crt::JsonObject jobsMapValMember;
                    Aws::Crt::Vector<Aws::Crt::JsonObject> valueList;
                    valueList.reserve(jobsMapMember.second.size());
                    for (auto &valueListMember : jobsMapMember.second)
                    {
                        Aws::Crt::JsonObject valueListValMember;
                        Aws::Crt::JsonObject jsonObject;
                        valueListMember.SerializeToObject(jsonObject);
                        valueListValMember.AsObject(std::move(jsonObject));
                        valueList.push_back(valueListValMember);
                    }
                    jobsMapValMember.AsArray(std::move(valueList));
                    jobsMap.WithObject(JobStatusMarshaller::ToString(jobsMapMember.first), std::move(jobsMapValMember));
                }
                object.WithObject("jobs", std::move(jobsMap));
            }

            if (Timestamp)
            {
                object.WithDouble("timestamp", Timestamp->SecondsWithMSPrecision());
            }
        }

        JobExecutionsChangedEvent::JobExecutionsChangedEvent(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        JobExecutionsChangedEvent &JobExecutionsChangedEvent::operator=(const Crt::JsonView &doc)
        {
            *this = JobExecutionsChangedEvent(doc);
            return *this;
        }

    } // namespace Iotjobs
} // namespace Aws
