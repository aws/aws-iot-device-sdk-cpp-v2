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
         * Data needed to perform a CreateKeysAndCertificate operation.
         *
         */
        class AWS_IOTIDENTITY_API CreateKeysAndCertificateRequest final
        {
          public:
            CreateKeysAndCertificateRequest() = default;

            CreateKeysAndCertificateRequest(const Crt::JsonView &doc);
            CreateKeysAndCertificateRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

          private:
            static void LoadFromObject(CreateKeysAndCertificateRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotidentity
} // namespace Aws
