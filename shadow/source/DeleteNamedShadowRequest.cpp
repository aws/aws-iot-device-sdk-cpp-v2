/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotshadow/DeleteNamedShadowRequest.h>

namespace Aws
{
    namespace Iotshadow
    {

        void DeleteNamedShadowRequest::LoadFromObject(DeleteNamedShadowRequest &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("clientToken"))
            {
                val.ClientToken = doc.GetString("clientToken");
            }
        }

        void DeleteNamedShadowRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (ClientToken)
            {
                object.WithString("clientToken", *ClientToken);
            }
        }

        DeleteNamedShadowRequest::DeleteNamedShadowRequest(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        DeleteNamedShadowRequest &DeleteNamedShadowRequest::operator=(const Crt::JsonView &doc)
        {
            *this = DeleteNamedShadowRequest(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
