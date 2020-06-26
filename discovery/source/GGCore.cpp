/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
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
