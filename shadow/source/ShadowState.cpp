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
#include <aws/iotshadow/ShadowState.h>

namespace Aws
{
    namespace Iotshadow
    {

        void ShadowState::LoadFromObject(ShadowState &val, const Aws::Crt::JsonView &doc)
        {

            if (doc.ValueExists("desired"))
            {
                val.Desired = doc.Materialize();
            }

            if (doc.ValueExists("reported"))
            {
                val.Reported = doc.Materialize();
            }
        }

        void ShadowState::SerializeToObject(Aws::Crt::JsonObject &object) const
        {

            if (Desired)
            {
                object.WithObject("desired", *Desired);
            }

            if (Reported)
            {
                object.WithObject("reported", *Reported);
            }
        }

        ShadowState::ShadowState(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        ShadowState &ShadowState::operator=(const Crt::JsonView &doc)
        {
            *this = ShadowState(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
