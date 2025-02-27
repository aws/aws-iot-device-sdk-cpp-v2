/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotshadow/V2ErrorResponse.h>

namespace Aws
{
    namespace Iotshadow
    {

        void V2ErrorResponse::LoadFromObject(V2ErrorResponse &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("clientToken"))
            {
                val.ClientToken = doc.GetString("clientToken");
            }

            if (doc.ValueExists("code"))
            {
                val.Code = doc.GetInteger("code");
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

        void V2ErrorResponse::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (ClientToken)
            {
                object.WithString("clientToken", *ClientToken);
            }

            if (Code)
            {
                object.WithInteger("code", *Code);
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

        V2ErrorResponse::V2ErrorResponse(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        V2ErrorResponse &V2ErrorResponse::operator=(const Crt::JsonView &doc)
        {
            *this = V2ErrorResponse(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
