#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/JsonObject.h>

#include <aws/discovery/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Discovery
    {
        class AWS_DISCOVERY_API ConnectivityInfo final
        {
          public:
            ConnectivityInfo() = default;

            ConnectivityInfo(const Crt::JsonView &doc);
            ConnectivityInfo &operator=(const Crt::JsonView &doc);

            Aws::Crt::Optional<Aws::Crt::String> ID;
            Aws::Crt::Optional<Aws::Crt::String> HostAddress;
            Aws::Crt::Optional<Aws::Crt::String> Metadata;
            Aws::Crt::Optional<uint32_t> Port;

          private:
            static void LoadFromObject(ConnectivityInfo &obj, const Crt::JsonView &doc);
        };
    } // namespace Discovery
} // namespace Aws
