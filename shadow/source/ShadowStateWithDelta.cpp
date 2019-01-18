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
#include <aws/iotshadow/ShadowStateWithDelta.h>

namespace Aws
{
    namespace Iotshadow
    {

        void ShadowStateWithDelta::LoadFromObject(ShadowStateWithDelta &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("reported"))
            {
                val.Reported = doc.GetJsonObjectCopy("reported");
            }

            if (doc.ValueExists("delta"))
            {
                val.Delta = doc.GetJsonObjectCopy("delta");
            }

            if (doc.ValueExists("desired"))
            {
                val.Desired = doc.GetJsonObjectCopy("desired");
            }
        }

        void ShadowStateWithDelta::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (Reported)
            {
                object.WithObject("reported", *Reported);
            }

            if (Delta)
            {
                object.WithObject("delta", *Delta);
            }

            if (Desired)
            {
                object.WithObject("desired", *Desired);
            }
        }

        ShadowStateWithDelta::ShadowStateWithDelta(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        ShadowStateWithDelta &ShadowStateWithDelta::operator=(const Crt::JsonView &doc)
        {
            *this = ShadowStateWithDelta(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
