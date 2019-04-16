/* Copyright 2010-2019 Amazon.com, Inc. or its affiliates. All Rights Reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License").
* You may not use this file except in compliance with the License.
* A copy of the License is located at
*
*  http://aws.amazon.com/apache2.0
*
* or in the "license" file accompanying this file. This file is distributed
* on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
* express or implied. See the License for the specific language governing
* permissions and limitations under the License.

* This file is generated
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
                obj.Port = static_cast<uint16_t>(doc.GetInteger("PortNumber"));
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
