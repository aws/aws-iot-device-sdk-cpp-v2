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

        class AWS_IOTIDENTITY_API RegisterThingRequest final
        {
          public:
            RegisterThingRequest() = default;

            RegisterThingRequest(const Crt::JsonView &doc);
            RegisterThingRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<Aws::Crt::String> TemplateName;
            Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String>> Parameters;
            Aws::Crt::Optional<Aws::Crt::String> CertificateOwnershipToken;

          private:
            static void LoadFromObject(RegisterThingRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotidentity
} // namespace Aws
