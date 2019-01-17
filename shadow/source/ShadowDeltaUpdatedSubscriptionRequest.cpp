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
#include <aws/iotshadow/ShadowDeltaUpdatedSubscriptionRequest.h>

namespace Aws
{
    namespace Iotshadow
    {

        void ShadowDeltaUpdatedSubscriptionRequest::LoadFromObject(
            ShadowDeltaUpdatedSubscriptionRequest &,
            const Aws::Crt::JsonView &)
        {
        }

        void ShadowDeltaUpdatedSubscriptionRequest::SerializeToObject(Aws::Crt::JsonObject &) const {}

        ShadowDeltaUpdatedSubscriptionRequest::ShadowDeltaUpdatedSubscriptionRequest(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        ShadowDeltaUpdatedSubscriptionRequest &ShadowDeltaUpdatedSubscriptionRequest::operator=(
            const Crt::JsonView &doc)
        {
            *this = ShadowDeltaUpdatedSubscriptionRequest(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
