/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
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

            if (doc.ValueExists("delta"))
            {
                val.Delta = doc.GetJsonObjectCopy("delta");
            }

            if (doc.ValueExists("reported"))
            {
                val.Reported = doc.GetJsonObjectCopy("reported");
            }

            if (doc.ValueExists("desired"))
            {
                val.Desired = doc.GetJsonObjectCopy("desired");
            }
        }

        void ShadowStateWithDelta::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (Delta)
            {
                object.WithObject("delta", *Delta);
            }

            if (Reported)
            {
                object.WithObject("reported", *Reported);
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
