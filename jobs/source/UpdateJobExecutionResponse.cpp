/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/iotjobs/UpdateJobExecutionResponse.h>

namespace Aws
{
    namespace Iotjobs
    {

        void UpdateJobExecutionResponse::LoadFromObject(UpdateJobExecutionResponse &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("clientToken"))
            {
                val.ClientToken = doc.GetString("clientToken");
            }

            if (doc.ValueExists("timestamp"))
            {
                val.Timestamp = doc.GetDouble("timestamp");
            }

            if (doc.ValueExists("jobDocument"))
            {
                val.JobDocument = doc.GetJsonObjectCopy("jobDocument");
            }

            if (doc.ValueExists("executionState"))
            {
                val.ExecutionState = doc.GetJsonObject("executionState");
            }
        }

        void UpdateJobExecutionResponse::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (ClientToken)
            {
                object.WithString("clientToken", *ClientToken);
            }

            if (Timestamp)
            {
                object.WithDouble("timestamp", Timestamp->SecondsWithMSPrecision());
            }

            if (JobDocument)
            {
                object.WithObject("jobDocument", *JobDocument);
            }

            if (ExecutionState)
            {
                Aws::Crt::JsonObject jsonObject;
                ExecutionState->SerializeToObject(jsonObject);
                object.WithObject("executionState", std::move(jsonObject));
            }
        }

        UpdateJobExecutionResponse::UpdateJobExecutionResponse(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        UpdateJobExecutionResponse &UpdateJobExecutionResponse::operator=(const Crt::JsonView &doc)
        {
            *this = UpdateJobExecutionResponse(doc);
            return *this;
        }

    } // namespace Iotjobs
} // namespace Aws
