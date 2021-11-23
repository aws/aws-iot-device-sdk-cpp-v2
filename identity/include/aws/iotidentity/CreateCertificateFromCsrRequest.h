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
         * Data needed to perform a CreateCertificateFromCsr operation.
         *
         */
        class AWS_IOTIDENTITY_API CreateCertificateFromCsrRequest final
        {
          public:
            CreateCertificateFromCsrRequest() = default;

            CreateCertificateFromCsrRequest(const Crt::JsonView &doc);
            CreateCertificateFromCsrRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * The CSR, in PEM format.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> CertificateSigningRequest;

          private:
            static void LoadFromObject(CreateCertificateFromCsrRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotidentity
} // namespace Aws
