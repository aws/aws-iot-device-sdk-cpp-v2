/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/iotshadow/DeleteShadowResponse.h>

namespace Aws
{
    namespace Iotshadow
    {

        void DeleteShadowResponse::LoadFromObject(DeleteShadowResponse &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("version"))
            {
                val.Version = doc.GetInteger("version");
            }

            if (doc.ValueExists("clientToken"))
            {
                val.ClientToken = doc.GetString("clientToken");
            }

            if (doc.ValueExists("timestamp"))
            {
                val.Timestamp = doc.GetDouble("timestamp");
            }
        }

        void DeleteShadowResponse::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (Version)
            {
                object.WithInteger("version", *Version);
            }

            if (ClientToken)
            {
                object.WithString("clientToken", *ClientToken);
            }

            if (Timestamp)
            {
                object.WithDouble("timestamp", Timestamp->SecondsWithMSPrecision());
            }
        }

        DeleteShadowResponse::DeleteShadowResponse(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        DeleteShadowResponse &DeleteShadowResponse::operator=(const Crt::JsonView &doc)
        {
            *this = DeleteShadowResponse(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
