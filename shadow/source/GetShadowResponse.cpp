/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotshadow/GetShadowResponse.h>

namespace Aws
{
    namespace Iotshadow
    {

        void GetShadowResponse::LoadFromObject(GetShadowResponse &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("clientToken"))
            {
                val.ClientToken = doc.GetString("clientToken");
            }

            if (doc.ValueExists("state"))
            {
                val.State = doc.GetJsonObject("state");
            }

            if (doc.ValueExists("metadata"))
            {
                val.Metadata = doc.GetJsonObject("metadata");
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

        void GetShadowResponse::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (ClientToken)
            {
                object.WithString("clientToken", *ClientToken);
            }

            if (State)
            {
                Aws::Crt::JsonObject jsonObject;
                State->SerializeToObject(jsonObject);
                object.WithObject("state", std::move(jsonObject));
            }

            if (Metadata)
            {
                Aws::Crt::JsonObject jsonObject;
                Metadata->SerializeToObject(jsonObject);
                object.WithObject("metadata", std::move(jsonObject));
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

        GetShadowResponse::GetShadowResponse(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        GetShadowResponse &GetShadowResponse::operator=(const Crt::JsonView &doc)
        {
            *this = GetShadowResponse(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
