/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotjobs/RejectedError.h>

namespace Aws
{
    namespace Iotjobs
    {

        void RejectedError::LoadFromObject(RejectedError &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("clientToken"))
            {
                val.ClientToken = doc.GetString("clientToken");
            }

            if (doc.ValueExists("code"))
            {
                val.Code = RejectedErrorCodeMarshaller::FromString(doc.GetString("code"));
            }

            if (doc.ValueExists("message"))
            {
                val.Message = doc.GetString("message");
            }

            if (doc.ValueExists("timestamp"))
            {
                val.Timestamp = doc.GetDouble("timestamp");
            }

            if (doc.ValueExists("executionState"))
            {
                val.ExecutionState = doc.GetJsonObject("executionState");
            }
        }

        void RejectedError::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (ClientToken)
            {
                object.WithString("clientToken", *ClientToken);
            }

            if (Code)
            {
                object.WithString("code", RejectedErrorCodeMarshaller::ToString(*Code));
            }

            if (Message)
            {
                object.WithString("message", *Message);
            }

            if (Timestamp)
            {
                object.WithDouble("timestamp", Timestamp->SecondsWithMSPrecision());
            }

            if (ExecutionState)
            {
                Aws::Crt::JsonObject jsonObject;
                ExecutionState->SerializeToObject(jsonObject);
                object.WithObject("executionState", std::move(jsonObject));
            }
        }

        RejectedError::RejectedError(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        RejectedError &RejectedError::operator=(const Crt::JsonView &doc)
        {
            *this = RejectedError(doc);
            return *this;
        }

    } // namespace Iotjobs
} // namespace Aws
