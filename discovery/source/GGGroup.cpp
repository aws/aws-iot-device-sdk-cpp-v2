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
#include <aws/discovery/GGGroup.h>

namespace Aws
{
    namespace Discovery
    {
        void GGGroup::LoadFromObject(GGGroup &obj, const Crt::JsonView &doc)
        {
            if (doc.KeyExists("GGGroupId"))
            {
                obj.GGGroupId = doc.GetString("GGGroupId");
            }

            if (doc.KeyExists("Cores"))
            {
                auto coresArray = doc.GetArray("Cores");
                obj.Cores = Aws::Crt::Vector<GGCore>();

                for (auto &core : coresArray)
                {
                    obj.Cores->push_back(core);
                }
            }

            if (doc.KeyExists("CAs"))
            {
                auto caArray = doc.GetArray("CAs");
                obj.CAs = Aws::Crt::Vector<Aws::Crt::String>();

                for (auto &ca : caArray)
                {
                    obj.CAs->push_back(ca.AsString());
                }
            }
        }

        GGGroup::GGGroup(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        GGGroup &GGGroup::operator=(const Crt::JsonView &doc)
        {
            *this = GGGroup(doc);
            return *this;
        }
    } // namespace Discovery
} // namespace Aws
