#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/discovery/GGCore.h>

namespace Aws
{
    namespace Discovery
    {
        class AWS_DISCOVERY_API GGGroup final
        {
          public:
            GGGroup() = default;

            GGGroup(const Crt::JsonView &doc);
            GGGroup &operator=(const Crt::JsonView &doc);

            Aws::Crt::Optional<Aws::Crt::String> GGGroupId;
            Aws::Crt::Optional<Aws::Crt::Vector<GGCore>> Cores;
            Aws::Crt::Optional<Aws::Crt::Vector<Aws::Crt::String>> CAs;

          private:
            static void LoadFromObject(GGGroup &obj, const Crt::JsonView &doc);
        };
    } // namespace Discovery
} // namespace Aws
