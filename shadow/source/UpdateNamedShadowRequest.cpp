/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotshadow/UpdateNamedShadowRequest.h>

namespace Aws
{
    namespace Iotshadow
    {

        void UpdateNamedShadowRequest::LoadFromObject(UpdateNamedShadowRequest &val, const Aws::Crt::JsonView &doc)
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

        void UpdateNamedShadowRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
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

        UpdateNamedShadowRequest::UpdateNamedShadowRequest(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        UpdateNamedShadowRequest &UpdateNamedShadowRequest::operator=(const Crt::JsonView &doc)
        {
            *this = UpdateNamedShadowRequest(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
