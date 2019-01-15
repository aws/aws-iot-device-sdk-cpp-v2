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
#include <aws/iotjobs/JobExecutionsChangedEvent.h>

namespace Aws
{
    namespace Iotjobs
    {

        void JobExecutionsChangedEvent::LoadFromObject(JobExecutionsChangedEvent &val, const Aws::Crt::JsonView &doc)
        {

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

            if (Jobs)
            {
                Aws::Crt::JsonObject jobsMap;
                for (auto &jobsMapMember : *Jobs)
                {
                    Aws::Crt::JsonObject jobsMapValMember;
                    Aws::Crt::Vector<Aws::Crt::JsonObject> valueList(jobsMapMember.second.size());
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
