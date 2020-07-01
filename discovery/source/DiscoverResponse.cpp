/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
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
