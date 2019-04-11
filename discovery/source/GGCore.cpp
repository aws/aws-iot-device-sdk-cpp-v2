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
#include <aws/discovery/GGCore.h>

namespace Aws
{
    namespace Discovery
    {
        void GGCore::LoadFromObject(GGCore &obj, const Crt::JsonView &doc)
        {
            if (doc.KeyExists("thingArn"))
            {
                obj.ThingArn = doc.GetString("thingArn");
            }

            if (doc.KeyExists("Connectivity"))
            {
                auto connectivityArray = doc.GetArray("Connectivity");
                obj.Connectivity = Aws::Crt::Vector<ConnectivityInfo>();
                obj.Connectivity->reserve(connectivityArray.size());

                for (auto &connectivity : connectivityArray)
                {
                    ConnectivityInfo connectivityInfo(connectivity.AsObject());
                    obj.Connectivity->push_back(std::move(connectivity));
                }
            }
        }

        GGCore::GGCore(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        GGCore &GGCore::operator=(const Crt::JsonView &doc)
        {
            *this = GGCore(doc);
            return *this;
        }
    } // namespace Discovery
} // namespace Aws
