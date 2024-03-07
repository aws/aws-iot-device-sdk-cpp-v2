/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotshadow/GetNamedShadowRequest.h>

namespace Aws
{
    namespace Iotshadow
    {

        void GetNamedShadowRequest::LoadFromObject(GetNamedShadowRequest &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("clientToken"))
            {
                val.ClientToken = doc.GetString("clientToken");
            }
        }

        void GetNamedShadowRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (ClientToken)
            {
                object.WithString("clientToken", *ClientToken);
            }
        }

        GetNamedShadowRequest::GetNamedShadowRequest(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        GetNamedShadowRequest &GetNamedShadowRequest::operator=(const Crt::JsonView &doc)
        {
            *this = GetNamedShadowRequest(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
