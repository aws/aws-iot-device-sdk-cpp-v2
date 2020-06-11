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
#include <aws/iotidentity/RegisterThingResponse.h>

namespace Aws
{
namespace Iotidentity
{

    void RegisterThingResponse::LoadFromObject(RegisterThingResponse& val, const Aws::Crt::JsonView &doc)
    {
        (void)val;
        (void)doc;

        if (doc.ValueExists("thingName"))
        {
            val.ThingName = doc.GetString("thingName");
        }

        if (doc.ValueExists("deviceConfiguration"))
        {
            auto deviceConfigurationMap = doc.GetJsonObject("deviceConfiguration");
            val.DeviceConfiguration = Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String>();
            for (auto& deviceConfigurationMapMember : deviceConfigurationMap.GetAllObjects())
            {
                Aws::Crt::String deviceConfigurationMapValMember;
                deviceConfigurationMapValMember = deviceConfigurationMapMember.second.AsString();
                val.DeviceConfiguration->emplace(deviceConfigurationMapMember.first, std::move(deviceConfigurationMapValMember));
            }

        }

    }

    void RegisterThingResponse::SerializeToObject(Aws::Crt::JsonObject& object) const
    {
        (void)object;

        if (ThingName)
        {
            object.WithString("thingName", *ThingName);
        }

        if (DeviceConfiguration)
        {
            Aws::Crt::JsonObject deviceConfigurationMap;
            for (auto& deviceConfigurationMapMember : *DeviceConfiguration)
            {
                Aws::Crt::JsonObject deviceConfigurationMapValMember;
                deviceConfigurationMapValMember.AsString(deviceConfigurationMapMember.second);
                deviceConfigurationMap.WithObject(deviceConfigurationMapMember.first, std::move(deviceConfigurationMapValMember));
            }
            object.WithObject("deviceConfiguration", std::move(deviceConfigurationMap));
        }

    }

    RegisterThingResponse::RegisterThingResponse(const Crt::JsonView& doc)
    {
        LoadFromObject(*this, doc);
    }

    RegisterThingResponse& RegisterThingResponse::operator=(const Crt::JsonView& doc)
    {
        *this = RegisterThingResponse(doc);
        return *this;
    }

}
}
