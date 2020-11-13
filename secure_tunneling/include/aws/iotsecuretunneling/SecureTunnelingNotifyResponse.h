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

#include <aws/iotsecuretunneling/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotsecuretunneling
    {

        class AWS_IOTSECURETUNNELING_API SecureTunnelingNotifyResponse final
        {
          public:
            SecureTunnelingNotifyResponse() = default;

            SecureTunnelingNotifyResponse(const Crt::JsonView &doc);
            SecureTunnelingNotifyResponse &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<Aws::Crt::String> Region;
            Aws::Crt::Optional<Aws::Crt::String> ClientMode;
            Aws::Crt::Optional<Aws::Crt::Vector<Aws::Crt::String>> Services;
            Aws::Crt::Optional<Aws::Crt::String> ClientAccessToken;

          private:
            static void LoadFromObject(SecureTunnelingNotifyResponse &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotsecuretunneling
} // namespace Aws
