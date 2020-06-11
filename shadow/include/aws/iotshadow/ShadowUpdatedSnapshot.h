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

* This file is generated
*/
#include <aws/iotshadow/ShadowMetadata.h>
#include <aws/iotshadow/ShadowState.h>

#include <aws/iotshadow/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotshadow
    {

        class AWS_IOTSHADOW_API ShadowUpdatedSnapshot final
        {
          public:
            ShadowUpdatedSnapshot() = default;

            ShadowUpdatedSnapshot(const Crt::JsonView &doc);
            ShadowUpdatedSnapshot &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<Aws::Iotshadow::ShadowState> State;
            Aws::Crt::Optional<Aws::Iotshadow::ShadowMetadata> Metadata;
            Aws::Crt::Optional<int32_t> Version;

          private:
            static void LoadFromObject(ShadowUpdatedSnapshot &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotshadow
} // namespace Aws
