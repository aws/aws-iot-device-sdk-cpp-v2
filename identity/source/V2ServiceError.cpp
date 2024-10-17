/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotidentity/V2ServiceError.h>

namespace Aws
{
    namespace Iotidentity
    {

        void V2ServiceError::LoadFromObject(V2ServiceError &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("statusCode"))
            {
                val.StatusCode = doc.GetInteger("statusCode");
            }

            if (doc.ValueExists("errorCode"))
            {
                val.ErrorCode = doc.GetString("errorCode");
            }

            if (doc.ValueExists("errorMessage"))
            {
                val.ErrorMessage = doc.GetString("errorMessage");
            }
        }

        void V2ServiceError::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (StatusCode)
            {
                object.WithInteger("statusCode", *StatusCode);
            }

            if (ErrorCode)
            {
                object.WithString("errorCode", *ErrorCode);
            }

            if (ErrorMessage)
            {
                object.WithString("errorMessage", *ErrorMessage);
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

    } // namespace Iotidentity
} // namespace Aws
