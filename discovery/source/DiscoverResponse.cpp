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
#include <aws/discovery/DiscoverResponse.h>

namespace Aws
{
    namespace Discovery
    {
        void DiscoverResponse::LoadFromObject(DiscoverResponse &obj, const Crt::JsonView &doc)
        {
            if (doc.KeyExists("GGGroups"))
            {
                auto ggGroupsArray = doc.GetArray("GGGroups");
                obj.GGGroups = Aws::Crt::Vector<GGGroup>();

                for (auto &ggGroup : ggGroupsArray)
                {
                    obj.GGGroups->push_back(ggGroup);
                }
            }
        }

        DiscoverResponse::DiscoverResponse(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        DiscoverResponse &DiscoverResponse::operator=(const Crt::JsonView &doc)
        {
            *this = DiscoverResponse(doc);
            return *this;
        }
    } // namespace Discovery
} // namespace Aws
