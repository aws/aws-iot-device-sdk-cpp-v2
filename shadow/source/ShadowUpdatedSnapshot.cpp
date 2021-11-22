/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotshadow/ShadowUpdatedSnapshot.h>

namespace Aws
{
    namespace Iotshadow
    {

        void ShadowUpdatedSnapshot::LoadFromObject(ShadowUpdatedSnapshot &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("state"))
            {
                val.State = doc.GetJsonObject("state");
            }

            if (doc.ValueExists("metadata"))
            {
                val.Metadata = doc.GetJsonObject("metadata");
            }

            if (doc.ValueExists("version"))
            {
                val.Version = doc.GetInteger("version");
            }
        }

        void ShadowUpdatedSnapshot::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (State)
            {
                Aws::Crt::JsonObject jsonObject;
                State->SerializeToObject(jsonObject);
                object.WithObject("state", std::move(jsonObject));
            }

            if (Metadata)
            {
                Aws::Crt::JsonObject jsonObject;
                Metadata->SerializeToObject(jsonObject);
                object.WithObject("metadata", std::move(jsonObject));
            }

            if (Version)
            {
                object.WithInteger("version", *Version);
            }
        }

        ShadowUpdatedSnapshot::ShadowUpdatedSnapshot(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        ShadowUpdatedSnapshot &ShadowUpdatedSnapshot::operator=(const Crt::JsonView &doc)
        {
            *this = ShadowUpdatedSnapshot(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
