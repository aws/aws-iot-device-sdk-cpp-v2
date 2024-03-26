/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotshadow/UpdateShadowRequest.h>

namespace Aws
{
    namespace Iotshadow
    {

        void UpdateShadowRequest::LoadFromObject(UpdateShadowRequest &val, const Aws::Crt::JsonView &doc)
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

            if (doc.ValueExists("version"))
            {
                val.Version = doc.GetInteger("version");
            }
        }

        void UpdateShadowRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
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

            if (Version)
            {
                object.WithInteger("version", *Version);
            }
        }

        UpdateShadowRequest::UpdateShadowRequest(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        UpdateShadowRequest &UpdateShadowRequest::operator=(const Crt::JsonView &doc)
        {
            *this = UpdateShadowRequest(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
