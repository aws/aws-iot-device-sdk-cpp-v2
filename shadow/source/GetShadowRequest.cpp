/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotshadow/GetShadowRequest.h>

namespace Aws
{
    namespace Iotshadow
    {

        void GetShadowRequest::LoadFromObject(GetShadowRequest &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("clientToken"))
            {
                val.ClientToken = doc.GetString("clientToken");
            }
        }

        void GetShadowRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (ClientToken)
            {
                object.WithString("clientToken", *ClientToken);
            }
        }

        GetShadowRequest::GetShadowRequest(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        GetShadowRequest &GetShadowRequest::operator=(const Crt::JsonView &doc)
        {
            *this = GetShadowRequest(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
