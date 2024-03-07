/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotjobs/GetPendingJobExecutionsResponse.h>

namespace Aws
{
    namespace Iotjobs
    {

        void GetPendingJobExecutionsResponse::LoadFromObject(
            GetPendingJobExecutionsResponse &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("inProgressJobs"))
            {
                auto inProgressJobsList = doc.GetArray("inProgressJobs");
                val.InProgressJobs = Aws::Crt::Vector<Aws::Iotjobs::JobExecutionSummary>();
                val.InProgressJobs->reserve(inProgressJobsList.size());
                for (auto &inProgressJobsListMember : inProgressJobsList)
                {
                    Aws::Iotjobs::JobExecutionSummary inProgressJobsListValMember;
                    inProgressJobsListValMember = inProgressJobsListMember.AsObject();
                    val.InProgressJobs->push_back(std::move(inProgressJobsListValMember));
                }
            }

            if (doc.ValueExists("queuedJobs"))
            {
                auto queuedJobsList = doc.GetArray("queuedJobs");
                val.QueuedJobs = Aws::Crt::Vector<Aws::Iotjobs::JobExecutionSummary>();
                val.QueuedJobs->reserve(queuedJobsList.size());
                for (auto &queuedJobsListMember : queuedJobsList)
                {
                    Aws::Iotjobs::JobExecutionSummary queuedJobsListValMember;
                    queuedJobsListValMember = queuedJobsListMember.AsObject();
                    val.QueuedJobs->push_back(std::move(queuedJobsListValMember));
                }
            }

            if (doc.ValueExists("timestamp"))
            {
                val.Timestamp = doc.GetDouble("timestamp");
            }

            if (doc.ValueExists("clientToken"))
            {
                val.ClientToken = doc.GetString("clientToken");
            }
        }

        void GetPendingJobExecutionsResponse::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (InProgressJobs)
            {
                Aws::Crt::Vector<Aws::Crt::JsonObject> inProgressJobsList;
                inProgressJobsList.reserve(InProgressJobs->size());
                for (auto &inProgressJobsListMember : *InProgressJobs)
                {
                    Aws::Crt::JsonObject inProgressJobsListValMember;
                    Aws::Crt::JsonObject jsonObject;
                    inProgressJobsListMember.SerializeToObject(jsonObject);
                    inProgressJobsListValMember.AsObject(std::move(jsonObject));
                    inProgressJobsList.push_back(inProgressJobsListValMember);
                }
                object.WithArray("inProgressJobs", std::move(inProgressJobsList));
            }

            if (QueuedJobs)
            {
                Aws::Crt::Vector<Aws::Crt::JsonObject> queuedJobsList;
                queuedJobsList.reserve(QueuedJobs->size());
                for (auto &queuedJobsListMember : *QueuedJobs)
                {
                    Aws::Crt::JsonObject queuedJobsListValMember;
                    Aws::Crt::JsonObject jsonObject;
                    queuedJobsListMember.SerializeToObject(jsonObject);
                    queuedJobsListValMember.AsObject(std::move(jsonObject));
                    queuedJobsList.push_back(queuedJobsListValMember);
                }
                object.WithArray("queuedJobs", std::move(queuedJobsList));
            }

            if (Timestamp)
            {
                object.WithDouble("timestamp", Timestamp->SecondsWithMSPrecision());
            }

            if (ClientToken)
            {
                object.WithString("clientToken", *ClientToken);
            }
        }

        GetPendingJobExecutionsResponse::GetPendingJobExecutionsResponse(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        GetPendingJobExecutionsResponse &GetPendingJobExecutionsResponse::operator=(const Crt::JsonView &doc)
        {
            *this = GetPendingJobExecutionsResponse(doc);
            return *this;
        }

    } // namespace Iotjobs
} // namespace Aws
