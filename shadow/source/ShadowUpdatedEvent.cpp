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
#include <aws/iotshadow/ShadowUpdatedEvent.h>

namespace Aws
{
namespace Iotshadow
{

    void ShadowUpdatedEvent::LoadFromObject(ShadowUpdatedEvent& val, const Aws::Crt::JsonView &doc)
    {
        (void)val;
        (void)doc;

        if (doc.ValueExists("previous"))
        {
            val.Previous = doc.GetJsonObject("previous");
        }

        if (doc.ValueExists("current"))
        {
            val.Current = doc.GetJsonObject("current");
        }

        if (doc.ValueExists("timestamp"))
        {
            val.Timestamp = doc.GetDouble("timestamp");
        }

    }

    void ShadowUpdatedEvent::SerializeToObject(Aws::Crt::JsonObject& object) const
    {
        (void)object;

        if (Previous)
        {
            Aws::Crt::JsonObject jsonObject;
            Previous->SerializeToObject(jsonObject);
            object.WithObject("previous", std::move(jsonObject));
        }

        if (Current)
        {
            Aws::Crt::JsonObject jsonObject;
            Current->SerializeToObject(jsonObject);
            object.WithObject("current", std::move(jsonObject));
        }

        if (Timestamp)
        {
            object.WithDouble("timestamp", Timestamp->SecondsWithMSPrecision());
        }

    }

    ShadowUpdatedEvent::ShadowUpdatedEvent(const Crt::JsonView& doc)
    {
        LoadFromObject(*this, doc);
    }

    ShadowUpdatedEvent& ShadowUpdatedEvent::operator=(const Crt::JsonView& doc)
    {
        *this = ShadowUpdatedEvent(doc);
        return *this;
    }

}
}
