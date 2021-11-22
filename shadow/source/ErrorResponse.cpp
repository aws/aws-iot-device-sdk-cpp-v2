/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotshadow/ErrorResponse.h>

namespace Aws
{
    namespace Iotshadow
    {

        void ErrorResponse::LoadFromObject(ErrorResponse &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("timestamp"))
            {
                val.Timestamp = doc.GetDouble("timestamp");
            }

            if (doc.ValueExists("message"))
            {
                val.Message = doc.GetString("message");
            }

            if (doc.ValueExists("clientToken"))
            {
                val.ClientToken = doc.GetString("clientToken");
            }

            if (doc.ValueExists("code"))
            {
                val.Code = doc.GetInteger("code");
            }
        }

        void ErrorResponse::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (Timestamp)
            {
                object.WithDouble("timestamp", Timestamp->SecondsWithMSPrecision());
            }

            if (Message)
            {
                object.WithString("message", *Message);
            }

            if (ClientToken)
            {
                object.WithString("clientToken", *ClientToken);
            }

            if (Code)
            {
                object.WithInteger("code", *Code);
            }
        }

        ErrorResponse::ErrorResponse(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        ErrorResponse &ErrorResponse::operator=(const Crt::JsonView &doc)
        {
            *this = ErrorResponse(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
