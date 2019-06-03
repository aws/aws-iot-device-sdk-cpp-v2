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
