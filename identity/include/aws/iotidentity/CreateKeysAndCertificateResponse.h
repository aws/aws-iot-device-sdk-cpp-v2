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
         * Response payload to a CreateKeysAndCertificate request.
         *
         */
        class AWS_IOTIDENTITY_API CreateKeysAndCertificateResponse final
        {
          public:
            CreateKeysAndCertificateResponse() = default;

            CreateKeysAndCertificateResponse(const Crt::JsonView &doc);
            CreateKeysAndCertificateResponse &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * The certificate id.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> CertificateId;

            /**
             * The certificate data, in PEM format.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> CertificatePem;

            /**
             * The private key.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> PrivateKey;

            /**
             * The token to prove ownership of the certificate during provisioning.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> CertificateOwnershipToken;

          private:
            static void LoadFromObject(CreateKeysAndCertificateResponse &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotidentity
} // namespace Aws
