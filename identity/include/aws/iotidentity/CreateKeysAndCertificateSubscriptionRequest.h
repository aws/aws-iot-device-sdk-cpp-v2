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

        class AWS_IOTIDENTITY_API CreateKeysAndCertificateSubscriptionRequest final
        {
          public:
            CreateKeysAndCertificateSubscriptionRequest() = default;

            CreateKeysAndCertificateSubscriptionRequest(const Crt::JsonView &doc);
            CreateKeysAndCertificateSubscriptionRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

          private:
            static void LoadFromObject(CreateKeysAndCertificateSubscriptionRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotidentity
} // namespace Aws
