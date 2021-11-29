/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotidentity/CreateCertificateFromCsrRequest.h>

namespace Aws
{
    namespace Iotidentity
    {

        void CreateCertificateFromCsrRequest::LoadFromObject(
            CreateCertificateFromCsrRequest &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("certificateSigningRequest"))
            {
                val.CertificateSigningRequest = doc.GetString("certificateSigningRequest");
            }
        }

        void CreateCertificateFromCsrRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (CertificateSigningRequest)
            {
                object.WithString("certificateSigningRequest", *CertificateSigningRequest);
            }
        }

        CreateCertificateFromCsrRequest::CreateCertificateFromCsrRequest(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        CreateCertificateFromCsrRequest &CreateCertificateFromCsrRequest::operator=(const Crt::JsonView &doc)
        {
            *this = CreateCertificateFromCsrRequest(doc);
            return *this;
        }

    } // namespace Iotidentity
} // namespace Aws
