/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
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
