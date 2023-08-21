/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotjobs/StartNextJobExecutionResponse.h>

namespace Aws
{
    namespace Iotjobs
    {

        void StartNextJobExecutionResponse::LoadFromObject(
            StartNextJobExecutionResponse &val,
            const Aws::Crt::JsonView &doc)
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

            if (doc.ValueExists("execution"))
            {
                val.Execution = doc.GetJsonObject("execution");
            }
        }

        void StartNextJobExecutionResponse::SerializeToObject(Aws::Crt::JsonObject &object) const
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

            if (Execution)
            {
                Aws::Crt::JsonObject jsonObject;
                Execution->SerializeToObject(jsonObject);
                object.WithObject("execution", std::move(jsonObject));
            }
        }

        StartNextJobExecutionResponse::StartNextJobExecutionResponse(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        StartNextJobExecutionResponse &StartNextJobExecutionResponse::operator=(const Crt::JsonView &doc)
        {
            *this = StartNextJobExecutionResponse(doc);
            return *this;
        }

    } // namespace Iotjobs
} // namespace Aws
