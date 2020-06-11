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
#include <aws/iotidentity/CreateKeysAndCertificateResponse.h>

namespace Aws
{
namespace Iotidentity
{

    void CreateKeysAndCertificateResponse::LoadFromObject(CreateKeysAndCertificateResponse& val, const Aws::Crt::JsonView &doc)
    {
        (void)val;
        (void)doc;

        if (doc.ValueExists("certificateId"))
        {
            val.CertificateId = doc.GetString("certificateId");
        }

        if (doc.ValueExists("certificateOwnershipToken"))
        {
            val.CertificateOwnershipToken = doc.GetString("certificateOwnershipToken");
        }

        if (doc.ValueExists("certificatePem"))
        {
            val.CertificatePem = doc.GetString("certificatePem");
        }

        if (doc.ValueExists("privateKey"))
        {
            val.PrivateKey = doc.GetString("privateKey");
        }

    }

    void CreateKeysAndCertificateResponse::SerializeToObject(Aws::Crt::JsonObject& object) const
    {
        (void)object;

        if (CertificateId)
        {
            object.WithString("certificateId", *CertificateId);
        }

        if (CertificateOwnershipToken)
        {
            object.WithString("certificateOwnershipToken", *CertificateOwnershipToken);
        }

        if (CertificatePem)
        {
            object.WithString("certificatePem", *CertificatePem);
        }

        if (PrivateKey)
        {
            object.WithString("privateKey", *PrivateKey);
        }

    }

    CreateKeysAndCertificateResponse::CreateKeysAndCertificateResponse(const Crt::JsonView& doc)
    {
        LoadFromObject(*this, doc);
    }

    CreateKeysAndCertificateResponse& CreateKeysAndCertificateResponse::operator=(const Crt::JsonView& doc)
    {
        *this = CreateKeysAndCertificateResponse(doc);
        return *this;
    }

}
}
