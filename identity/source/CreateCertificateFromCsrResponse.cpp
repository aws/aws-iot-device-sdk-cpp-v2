/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotidentity/CreateCertificateFromCsrResponse.h>

namespace Aws
{
    namespace Iotidentity
    {

        void CreateCertificateFromCsrResponse::LoadFromObject(
            CreateCertificateFromCsrResponse &val,
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

            if (doc.ValueExists("certificateOwnershipToken"))
            {
                val.CertificateOwnershipToken = doc.GetString("certificateOwnershipToken");
            }
        }

        void CreateCertificateFromCsrResponse::SerializeToObject(Aws::Crt::JsonObject &object) const
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

            if (CertificateOwnershipToken)
            {
                object.WithString("certificateOwnershipToken", *CertificateOwnershipToken);
            }
        }

        CreateCertificateFromCsrResponse::CreateCertificateFromCsrResponse(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        CreateCertificateFromCsrResponse &CreateCertificateFromCsrResponse::operator=(const Crt::JsonView &doc)
        {
            *this = CreateCertificateFromCsrResponse(doc);
            return *this;
        }

    } // namespace Iotidentity
} // namespace Aws
