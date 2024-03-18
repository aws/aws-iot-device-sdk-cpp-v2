/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotjobs/JobExecutionState.h>

namespace Aws
{
    namespace Iotjobs
    {

        void JobExecutionState::LoadFromObject(JobExecutionState &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("status"))
            {
                val.Status = JobStatusMarshaller::FromString(doc.GetString("status"));
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

            if (doc.ValueExists("versionNumber"))
            {
                val.VersionNumber = doc.GetInteger("versionNumber");
            }
        }

        void JobExecutionState::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (Status)
            {
                object.WithString("status", JobStatusMarshaller::ToString(*Status));
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

            if (VersionNumber)
            {
                object.WithInteger("versionNumber", *VersionNumber);
            }
        }

        JobExecutionState::JobExecutionState(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        JobExecutionState &JobExecutionState::operator=(const Crt::JsonView &doc)
        {
            *this = JobExecutionState(doc);
            return *this;
        }

    } // namespace Iotjobs
} // namespace Aws
