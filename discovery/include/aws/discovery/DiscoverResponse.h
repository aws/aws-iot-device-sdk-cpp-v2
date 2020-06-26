#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/discovery/GGGroup.h>

namespace Aws
{
    namespace Discovery
    {
        class AWS_DISCOVERY_API DiscoverResponse final
        {
          public:
            DiscoverResponse() = default;

            DiscoverResponse(const Crt::JsonView &doc);
            DiscoverResponse &operator=(const Crt::JsonView &doc);

            Aws::Crt::Optional<Aws::Crt::Vector<GGGroup>> GGGroups;

          private:
            static void LoadFromObject(DiscoverResponse &obj, const Crt::JsonView &doc);
        };
    } // namespace Discovery
} // namespace Aws
