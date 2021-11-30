/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotshadow/ShadowUpdatedEvent.h>

namespace Aws
{
    namespace Iotshadow
    {

        void ShadowUpdatedEvent::LoadFromObject(ShadowUpdatedEvent &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("previous"))
            {
                val.Previous = doc.GetJsonObject("previous");
            }

            if (doc.ValueExists("current"))
            {
                val.Current = doc.GetJsonObject("current");
            }

            if (doc.ValueExists("timestamp"))
            {
                val.Timestamp = doc.GetDouble("timestamp");
            }
        }

        void ShadowUpdatedEvent::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (Previous)
            {
                Aws::Crt::JsonObject jsonObject;
                Previous->SerializeToObject(jsonObject);
                object.WithObject("previous", std::move(jsonObject));
            }

            if (Current)
            {
                Aws::Crt::JsonObject jsonObject;
                Current->SerializeToObject(jsonObject);
                object.WithObject("current", std::move(jsonObject));
            }

            if (Timestamp)
            {
                object.WithDouble("timestamp", Timestamp->SecondsWithMSPrecision());
            }
        }

        ShadowUpdatedEvent::ShadowUpdatedEvent(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        ShadowUpdatedEvent &ShadowUpdatedEvent::operator=(const Crt::JsonView &doc)
        {
            *this = ShadowUpdatedEvent(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
