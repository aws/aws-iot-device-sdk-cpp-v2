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
#include <aws/iotshadow/ShadowUpdatedSnapshot.h>

namespace Aws
{
namespace Iotshadow
{

    void ShadowUpdatedSnapshot::LoadFromObject(ShadowUpdatedSnapshot& val, const Aws::Crt::JsonView &doc)
    {
        (void)val;
        (void)doc;

        if (doc.ValueExists("state"))
        {
            val.State = doc.GetJsonObject("state");
        }

        if (doc.ValueExists("metadata"))
        {
            val.Metadata = doc.GetJsonObject("metadata");
        }

        if (doc.ValueExists("version"))
        {
            val.Version = doc.GetInteger("version");
        }

    }

    void ShadowUpdatedSnapshot::SerializeToObject(Aws::Crt::JsonObject& object) const
    {
        (void)object;

        if (State)
        {
            Aws::Crt::JsonObject jsonObject;
            State->SerializeToObject(jsonObject);
            object.WithObject("state", std::move(jsonObject));
        }

        if (Metadata)
        {
            Aws::Crt::JsonObject jsonObject;
            Metadata->SerializeToObject(jsonObject);
            object.WithObject("metadata", std::move(jsonObject));
        }

        if (Version)
        {
            object.WithInteger("version", *Version);
        }

    }

    ShadowUpdatedSnapshot::ShadowUpdatedSnapshot(const Crt::JsonView& doc)
    {
        LoadFromObject(*this, doc);
    }

    ShadowUpdatedSnapshot& ShadowUpdatedSnapshot::operator=(const Crt::JsonView& doc)
    {
        *this = ShadowUpdatedSnapshot(doc);
        return *this;
    }

}
}
