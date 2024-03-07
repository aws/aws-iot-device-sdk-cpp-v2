/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotsecuretunneling/SubscribeToTunnelsNotifyRequest.h>

namespace Aws
{
namespace Iotsecuretunneling
{

    void SubscribeToTunnelsNotifyRequest::LoadFromObject(SubscribeToTunnelsNotifyRequest& val, const Aws::Crt::JsonView &doc)
    {
        (void)val;
        (void)doc;

    }

    void SubscribeToTunnelsNotifyRequest::SerializeToObject(Aws::Crt::JsonObject& object) const
    {
        (void)object;

    }

    SubscribeToTunnelsNotifyRequest::SubscribeToTunnelsNotifyRequest(const Crt::JsonView& doc)
    {
        LoadFromObject(*this, doc);
    }

    SubscribeToTunnelsNotifyRequest& SubscribeToTunnelsNotifyRequest::operator=(const Crt::JsonView& doc)
    {
        *this = SubscribeToTunnelsNotifyRequest(doc);
        return *this;
    }

}
}
