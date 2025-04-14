/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotcommand/V2ErrorResponse.h>

namespace Aws
{
    namespace Iotcommand
    {

        void V2ErrorResponse::LoadFromObject(V2ErrorResponse &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("error"))
            {
                val.Error = RejectedErrorCodeMarshaller::FromString(doc.GetString("error"));
            }

            if (doc.ValueExists("errorMessage"))
            {
                val.ErrorMessage = doc.GetString("errorMessage");
            }

            if (doc.ValueExists("executionId"))
            {
                val.ExecutionId = doc.GetString("executionId");
            }
        }

        void V2ErrorResponse::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (Error)
            {
                object.WithString("error", RejectedErrorCodeMarshaller::ToString(*Error));
            }

            if (ErrorMessage)
            {
                object.WithString("errorMessage", *ErrorMessage);
            }

            if (ExecutionId)
            {
                object.WithString("executionId", *ExecutionId);
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

    } // namespace Iotcommand
} // namespace Aws
