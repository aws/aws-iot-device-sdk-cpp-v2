/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotshadow/DeleteShadowRequest.h>

namespace Aws
{
    namespace Iotshadow
    {

        void DeleteShadowRequest::LoadFromObject(DeleteShadowRequest &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("clientToken"))
            {
                val.ClientToken = doc.GetString("clientToken");
            }
        }

        void DeleteShadowRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (ClientToken)
            {
                object.WithString("clientToken", *ClientToken);
            }
        }

        DeleteShadowRequest::DeleteShadowRequest(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        DeleteShadowRequest &DeleteShadowRequest::operator=(const Crt::JsonView &doc)
        {
            *this = DeleteShadowRequest(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
