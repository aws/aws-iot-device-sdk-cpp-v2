/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotidentity/CreateKeysAndCertificateResponse.h>

namespace Aws
{
    namespace Iotidentity
    {

        void CreateKeysAndCertificateResponse::LoadFromObject(
            CreateKeysAndCertificateResponse &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("certificateId"))
            {
                val.CertificateId = doc.GetString("certificateId");
            }

            if (doc.ValueExists("certificatePem"))
            {
                val.CertificatePem = doc.GetString("certificatePem");
            }

            if (doc.ValueExists("privateKey"))
            {
                val.PrivateKey = doc.GetString("privateKey");
            }

            if (doc.ValueExists("certificateOwnershipToken"))
            {
                val.CertificateOwnershipToken = doc.GetString("certificateOwnershipToken");
            }
        }

        void CreateKeysAndCertificateResponse::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (CertificateId)
            {
                object.WithString("certificateId", *CertificateId);
            }

            if (CertificatePem)
            {
                object.WithString("certificatePem", *CertificatePem);
            }

            if (PrivateKey)
            {
                object.WithString("privateKey", *PrivateKey);
            }

            if (CertificateOwnershipToken)
            {
                object.WithString("certificateOwnershipToken", *CertificateOwnershipToken);
            }
        }

        CreateKeysAndCertificateResponse::CreateKeysAndCertificateResponse(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        CreateKeysAndCertificateResponse &CreateKeysAndCertificateResponse::operator=(const Crt::JsonView &doc)
        {
            *this = CreateKeysAndCertificateResponse(doc);
            return *this;
        }

    } // namespace Iotidentity
} // namespace Aws
