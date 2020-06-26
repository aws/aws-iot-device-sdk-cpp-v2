#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/iotidentity/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotidentity
    {

        class AWS_IOTIDENTITY_API CreateCertificateFromCsrResponse final
        {
          public:
            CreateCertificateFromCsrResponse() = default;

            CreateCertificateFromCsrResponse(const Crt::JsonView &doc);
            CreateCertificateFromCsrResponse &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<Aws::Crt::String> CertificateId;
            Aws::Crt::Optional<Aws::Crt::String> CertificateOwnershipToken;
            Aws::Crt::Optional<Aws::Crt::String> CertificatePem;

          private:
            static void LoadFromObject(CreateCertificateFromCsrResponse &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotidentity
} // namespace Aws
