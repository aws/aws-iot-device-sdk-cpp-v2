/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotshadow/UpdateShadowResponse.h>

namespace Aws
{
    namespace Iotshadow
    {

        void UpdateShadowResponse::LoadFromObject(UpdateShadowResponse &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("state"))
            {
                val.State = doc.GetJsonObject("state");
            }

            if (doc.ValueExists("clientToken"))
            {
                val.ClientToken = doc.GetString("clientToken");
            }

            if (doc.ValueExists("version"))
            {
                val.Version = doc.GetInteger("version");
            }

            if (doc.ValueExists("metadata"))
            {
                val.Metadata = doc.GetJsonObject("metadata");
            }

            if (doc.ValueExists("timestamp"))
            {
                val.Timestamp = doc.GetDouble("timestamp");
            }
        }

        void UpdateShadowResponse::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (State)
            {
                Aws::Crt::JsonObject jsonObject;
                State->SerializeToObject(jsonObject);
                object.WithObject("state", std::move(jsonObject));
            }

            if (ClientToken)
            {
                object.WithString("clientToken", *ClientToken);
            }

            if (Version)
            {
                object.WithInteger("version", *Version);
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
        }

        UpdateShadowResponse::UpdateShadowResponse(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        UpdateShadowResponse &UpdateShadowResponse::operator=(const Crt::JsonView &doc)
        {
            *this = UpdateShadowResponse(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
