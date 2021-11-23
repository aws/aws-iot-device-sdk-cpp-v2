/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotshadow/ShadowDeltaUpdatedEvent.h>

namespace Aws
{
    namespace Iotshadow
    {

        void ShadowDeltaUpdatedEvent::LoadFromObject(ShadowDeltaUpdatedEvent &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("version"))
            {
                val.Version = doc.GetInteger("version");
            }

            if (doc.ValueExists("timestamp"))
            {
                val.Timestamp = doc.GetDouble("timestamp");
            }

            if (doc.ValueExists("metadata"))
            {
                val.Metadata = doc.GetJsonObjectCopy("metadata");
            }

            if (doc.ValueExists("state"))
            {
                val.State = doc.GetJsonObjectCopy("state");
            }
        }

        void ShadowDeltaUpdatedEvent::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (Version)
            {
                object.WithInteger("version", *Version);
            }

            if (Timestamp)
            {
                object.WithDouble("timestamp", Timestamp->SecondsWithMSPrecision());
            }

            if (Metadata)
            {
                object.WithObject("metadata", *Metadata);
            }

            if (State)
            {
                object.WithObject("state", *State);
            }
        }

        ShadowDeltaUpdatedEvent::ShadowDeltaUpdatedEvent(const Crt::JsonView &doc) { LoadFromObject(*this, doc); }

        ShadowDeltaUpdatedEvent &ShadowDeltaUpdatedEvent::operator=(const Crt::JsonView &doc)
        {
            *this = ShadowDeltaUpdatedEvent(doc);
            return *this;
        }

    } // namespace Iotshadow
} // namespace Aws
