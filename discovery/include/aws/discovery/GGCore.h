#pragma once
/* Copyright 2010-2019 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
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
