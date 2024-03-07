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
         * Data needed to subscribe to the responses of the CreateCertificateFromCsr operation.
         *
         */
        class AWS_IOTIDENTITY_API CreateCertificateFromCsrSubscriptionRequest final
        {
          public:
            CreateCertificateFromCsrSubscriptionRequest() = default;

            CreateCertificateFromCsrSubscriptionRequest(const Crt::JsonView &doc);
            CreateCertificateFromCsrSubscriptionRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

          private:
            static void LoadFromObject(CreateCertificateFromCsrSubscriptionRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotidentity
} // namespace Aws
