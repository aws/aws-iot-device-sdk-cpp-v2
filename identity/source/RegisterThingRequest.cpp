/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotidentity/RegisterThingRequest.h>

namespace Aws
{
    namespace Iotidentity
    {

        void RegisterThingRequest::LoadFromObject(RegisterThingRequest &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("certificateOwnershipToken"))
            {
                val.CertificateOwnershipToken = doc.GetString("certificateOwnershipToken");
            }

            if (doc.ValueExists("parameters"))
            {
                auto parametersMap = doc.GetJsonObject("parameters");
                val.Parameters = Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String>();
                for (auto &parametersMapMember : parametersMap.GetAllObjects())
                {
                    Aws::Crt::String parametersMapValMember;
                    parametersMapValMember = parametersMapMember.second.AsString();
                    val.Parameters->emplace(parametersMapMember.first, std::move(parametersMapValMember));
                }
            }
        }

        void RegisterThingRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (CertificateOwnershipToken)
            {
                object.WithString("certificateOwnershipToken", *CertificateOwnershipToken);
            }

            if (Parameters)
            {
                Aws::Crt::JsonObject parametersMap;
                for (auto &parametersMapMember : *Parameters)
                {
                    Aws::Crt::JsonObject parametersMapValMember;
                    parametersMapValMember.AsString(parametersMapMember.second);
                    parametersMap.WithObject(parametersMapMember.first, std::move(parametersMapValMember));
                }
                object.WithObject("parameters", std::move(parametersMap));
            }
        }

        RegisterThingRequest::RegisterThingRequest(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        RegisterThingRequest &RegisterThingRequest::operator=(const Crt::JsonView &doc)
        {
            *this = RegisterThingRequest(doc);
            return *this;
        }

    } // namespace Iotidentity
} // namespace Aws
