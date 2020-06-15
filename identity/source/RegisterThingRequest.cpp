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
#include <aws/iotidentity/RegisterThingRequest.h>

namespace Aws
{
    namespace Iotidentity
    {

        void RegisterThingRequest::LoadFromObject(RegisterThingRequest &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

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

            if (doc.ValueExists("certificateOwnershipToken"))
            {
                val.CertificateOwnershipToken = doc.GetString("certificateOwnershipToken");
            }
        }

        void RegisterThingRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

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

            if (CertificateOwnershipToken)
            {
                object.WithString("certificateOwnershipToken", *CertificateOwnershipToken);
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
