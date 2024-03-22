/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/discovery/ConnectivityInfo.h>

namespace Aws
{
    namespace Discovery
    {
        void ConnectivityInfo::LoadFromObject(ConnectivityInfo &obj, const Crt::JsonView &doc)
        {
            if (doc.KeyExists("Id"))
            {
                obj.ID = doc.GetString("Id");
            }

            if (doc.KeyExists("HostAddress"))
            {
                obj.HostAddress = doc.GetString("HostAddress");
            }

            if (doc.KeyExists("PortNumber"))
            {
                obj.Port = static_cast<uint32_t>(doc.GetInteger("PortNumber"));
            }

            if (doc.KeyExists("Metadata"))
            {
                obj.Metadata = doc.GetString("Metadata");
            }
        }

        ConnectivityInfo::ConnectivityInfo(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        ConnectivityInfo &ConnectivityInfo::operator=(const Crt::JsonView &doc)
        {
            *this = ConnectivityInfo(doc);
            return *this;
        }
    } // namespace Discovery
} // namespace Aws
