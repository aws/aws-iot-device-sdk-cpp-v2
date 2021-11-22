/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotshadow/ShadowState.h>

namespace Aws
{
    namespace Iotshadow
    {

        void ShadowState::LoadFromObject(ShadowState &val, const Aws::Crt::JsonView &doc)
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

        void ShadowState::SerializeToObject(Aws::Crt::JsonObject &object) const
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

        ShadowState::ShadowState(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        ShadowState &ShadowState::operator=(const Crt::JsonView &doc)
        {
            *this = ShadowState(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
