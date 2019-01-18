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
#include <aws/iotshadow/DeleteShadowResponse.h>

namespace Aws
{
    namespace Iotshadow
    {

        void DeleteShadowResponse::LoadFromObject(DeleteShadowResponse &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("timestamp"))
            {
                val.Timestamp = doc.GetDouble("timestamp");
            }

            if (doc.ValueExists("version"))
            {
                val.Version = doc.GetInteger("version");
            }
        }

        void DeleteShadowResponse::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (Timestamp)
            {
                object.WithDouble("timestamp", Timestamp->SecondsWithMSPrecision());
            }

            if (Version)
            {
                object.WithInteger("version", *Version);
            }
        }

        DeleteShadowResponse::DeleteShadowResponse(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        DeleteShadowResponse &DeleteShadowResponse::operator=(const Crt::JsonView &doc)
        {
            *this = DeleteShadowResponse(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
