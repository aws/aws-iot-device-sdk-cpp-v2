/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/iotidentity/ErrorResponse.h>

namespace Aws
{
    namespace Iotidentity
    {

        void ErrorResponse::LoadFromObject(ErrorResponse &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("statusCode"))
            {
                val.StatusCode = doc.GetInteger("statusCode");
            }

            if (doc.ValueExists("errorMessage"))
            {
                val.ErrorMessage = doc.GetString("errorMessage");
            }

            if (doc.ValueExists("errorCode"))
            {
                val.ErrorCode = doc.GetString("errorCode");
            }
        }

        void ErrorResponse::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (StatusCode)
            {
                object.WithInteger("statusCode", *StatusCode);
            }

            if (ErrorMessage)
            {
                object.WithString("errorMessage", *ErrorMessage);
            }

            if (ErrorCode)
            {
                object.WithString("errorCode", *ErrorCode);
            }
        }

        ErrorResponse::ErrorResponse(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        ErrorResponse &ErrorResponse::operator=(const Crt::JsonView &doc)
        {
            *this = ErrorResponse(doc);
            return *this;
        }

    } // namespace Iotidentity
} // namespace Aws
