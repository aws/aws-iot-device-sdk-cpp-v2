#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotidentity/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotidentity
    {

        /**
         * Response payload to a CreateCertificateFromCsr request.
         *
         */
        class AWS_IOTIDENTITY_API CreateCertificateFromCsrResponse final
        {
          public:
            CreateCertificateFromCsrResponse() = default;

            CreateCertificateFromCsrResponse(const Crt::JsonView &doc);
            CreateCertificateFromCsrResponse &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * The ID of the certificate.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> CertificateId;

            /**
             * The certificate data, in PEM format.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> CertificatePem;

            /**
             * The token to prove ownership of the certificate during provisioning.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> CertificateOwnershipToken;

          private:
            static void LoadFromObject(CreateCertificateFromCsrResponse &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotidentity
} // namespace Aws
