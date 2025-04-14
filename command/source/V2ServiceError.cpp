/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotcommand/V2ServiceError.h>

namespace Aws
{
    namespace Iotcommand
    {

        void V2ServiceError::LoadFromObject(V2ServiceError &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

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
        }

        void V2ServiceError::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

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
        }

        V2ServiceError::V2ServiceError(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        V2ServiceError &V2ServiceError::operator=(const Crt::JsonView &doc)
        {
            *this = V2ServiceError(doc);
            return *this;
        }

    } // namespace Iotcommand
} // namespace Aws
