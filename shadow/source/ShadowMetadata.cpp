/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotshadow/ShadowMetadata.h>

namespace Aws
{
    namespace Iotshadow
    {

        void ShadowMetadata::LoadFromObject(ShadowMetadata &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("desired"))
            {
                val.Desired = doc.GetJsonObjectCopy("desired");
            }

            if (doc.ValueExists("reported"))
            {
                val.Reported = doc.GetJsonObjectCopy("reported");
            }
        }

        void ShadowMetadata::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (Desired)
            {
                object.WithObject("desired", *Desired);
            }

            if (Reported)
            {
                object.WithObject("reported", *Reported);
            }
        }

        ShadowMetadata::ShadowMetadata(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        ShadowMetadata &ShadowMetadata::operator=(const Crt::JsonView &doc)
        {
            *this = ShadowMetadata(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
