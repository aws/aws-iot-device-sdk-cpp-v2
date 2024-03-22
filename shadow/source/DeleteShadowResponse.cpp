/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
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

            if (doc.ValueExists("clientToken"))
            {
                val.ClientToken = doc.GetString("clientToken");
            }

            if (doc.ValueExists("timestamp"))
            {
                val.Timestamp = doc.GetDouble("timestamp");
            }

            if (doc.ValueExists("version"))
            {
                val.Version = doc.GetInteger("version");
            }
        }

        void DeleteShadowResponse::SerializeToObject(Aws::Crt::JsonObject &object) const
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

            if (Version)
            {
                object.WithInteger("version", *Version);
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
