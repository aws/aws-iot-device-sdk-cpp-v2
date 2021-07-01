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
#include <aws/iotsecuretunneling/SecureTunnelingNotifyResponse.h>

namespace Aws
{
    namespace Iotsecuretunneling
    {

        void SecureTunnelingNotifyResponse::LoadFromObject(
            SecureTunnelingNotifyResponse &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("region"))
            {
                val.Region = doc.GetString("region");
            }

            if (doc.ValueExists("clientMode"))
            {
                val.ClientMode = doc.GetString("clientMode");
            }

            if (doc.ValueExists("services"))
            {
                auto servicesList = doc.GetArray("services");
                val.Services = Aws::Crt::Vector<Aws::Crt::String>();
                val.Services->reserve(servicesList.size());
                for (auto &servicesListMember : servicesList)
                {
                    Aws::Crt::String servicesListValMember;
                    servicesListValMember = servicesListMember.AsString();
                    val.Services->push_back(std::move(servicesListValMember));
                }
            }

            if (doc.ValueExists("clientAccessToken"))
            {
                val.ClientAccessToken = doc.GetString("clientAccessToken");
            }
        }

        void SecureTunnelingNotifyResponse::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (Region)
            {
                object.WithString("region", *Region);
            }

            if (ClientMode)
            {
                object.WithString("clientMode", *ClientMode);
            }

            if (Services)
            {
                Aws::Crt::Vector<Aws::Crt::JsonObject> servicesList;
                servicesList.reserve(Services->size());
                for (auto &servicesListMember : *Services)
                {
                    Aws::Crt::JsonObject servicesListValMember;
                    servicesListValMember.AsString(servicesListMember);
                    servicesList.push_back(servicesListValMember);
                }
                object.WithArray("services", std::move(servicesList));
            }

            if (ClientAccessToken)
            {
                object.WithString("clientAccessToken", *ClientAccessToken);
            }
        }

        SecureTunnelingNotifyResponse::SecureTunnelingNotifyResponse(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        SecureTunnelingNotifyResponse &SecureTunnelingNotifyResponse::operator=(const Crt::JsonView &doc)
        {
            *this = SecureTunnelingNotifyResponse(doc);
            return *this;
        }

    } // namespace Iotsecuretunneling
} // namespace Aws
