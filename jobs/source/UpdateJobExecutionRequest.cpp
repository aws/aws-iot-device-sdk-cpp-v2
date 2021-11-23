/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotjobs/UpdateJobExecutionRequest.h>

namespace Aws
{
    namespace Iotjobs
    {

        void UpdateJobExecutionRequest::LoadFromObject(UpdateJobExecutionRequest &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

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

            if (doc.ValueExists("includeJobExecutionState"))
            {
                val.IncludeJobExecutionState = doc.GetBool("includeJobExecutionState");
            }

            if (doc.ValueExists("expectedVersion"))
            {
                val.ExpectedVersion = doc.GetInteger("expectedVersion");
            }

            if (doc.ValueExists("includeJobDocument"))
            {
                val.IncludeJobDocument = doc.GetBool("includeJobDocument");
            }

            if (doc.ValueExists("status"))
            {
                val.Status = JobStatusMarshaller::FromString(doc.GetString("status"));
            }

            if (doc.ValueExists("stepTimeoutInMinutes"))
            {
                val.StepTimeoutInMinutes = doc.GetInt64("stepTimeoutInMinutes");
            }

            if (doc.ValueExists("clientToken"))
            {
                val.ClientToken = doc.GetString("clientToken");
            }
        }

        void UpdateJobExecutionRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

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

            if (IncludeJobExecutionState)
            {
                object.WithBool("includeJobExecutionState", *IncludeJobExecutionState);
            }

            if (ExpectedVersion)
            {
                object.WithInteger("expectedVersion", *ExpectedVersion);
            }

            if (IncludeJobDocument)
            {
                object.WithBool("includeJobDocument", *IncludeJobDocument);
            }

            if (Status)
            {
                object.WithString("status", JobStatusMarshaller::ToString(*Status));
            }

            if (StepTimeoutInMinutes)
            {
                object.WithInt64("stepTimeoutInMinutes", *StepTimeoutInMinutes);
            }

            if (ClientToken)
            {
                object.WithString("clientToken", *ClientToken);
            }
        }

        UpdateJobExecutionRequest::UpdateJobExecutionRequest(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        UpdateJobExecutionRequest &UpdateJobExecutionRequest::operator=(const Crt::JsonView &doc)
        {
            *this = UpdateJobExecutionRequest(doc);
            return *this;
        }

    } // namespace Iotjobs
} // namespace Aws
