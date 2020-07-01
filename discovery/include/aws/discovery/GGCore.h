#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/discovery/ConnectivityInfo.h>

namespace Aws
{
    namespace Discovery
    {
        class AWS_DISCOVERY_API GGCore final
        {
          public:
            GGCore() = default;

            GGCore(const Crt::JsonView &doc);
            GGCore &operator=(const Crt::JsonView &doc);

            Aws::Crt::Optional<Aws::Crt::String> ThingArn;
            Aws::Crt::Optional<Aws::Crt::Vector<ConnectivityInfo>> Connectivity;

          private:
            static void LoadFromObject(GGCore &obj, const Crt::JsonView &doc);
        };
    } // namespace Discovery
} // namespace Aws
