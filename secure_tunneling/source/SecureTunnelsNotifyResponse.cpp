/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotsecuretunneling/SecureTunnelsNotifyResponse.h>

namespace Aws
{
    namespace Iotsecuretunneling
    {

        void SecureTunnelsNotifyResponse::LoadFromObject(
            SecureTunnelsNotifyResponse &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("clientAccessToken"))
            {
                val.ClientAccessToken = doc.GetString("clientAccessToken");
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

            if (doc.ValueExists("clientMode"))
            {
                val.ClientMode = doc.GetString("clientMode");
            }

            if (doc.ValueExists("region"))
            {
                val.Region = doc.GetString("region");
            }
        }

        void SecureTunnelsNotifyResponse::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (ClientAccessToken)
            {
                object.WithString("clientAccessToken", *ClientAccessToken);
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

            if (ClientMode)
            {
                object.WithString("clientMode", *ClientMode);
            }

            if (Region)
            {
                object.WithString("region", *Region);
            }
        }

        SecureTunnelsNotifyResponse::SecureTunnelsNotifyResponse(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        SecureTunnelsNotifyResponse &SecureTunnelsNotifyResponse::operator=(const Crt::JsonView &doc)
        {
            *this = SecureTunnelsNotifyResponse(doc);
            return *this;
        }

    } // namespace Iotsecuretunneling
} // namespace Aws
