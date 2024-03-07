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

            if (doc.ValueExists("state"))
            {
                val.State = doc.GetJsonObjectCopy("state");
            }

            if (doc.ValueExists("metadata"))
            {
                val.Metadata = doc.GetJsonObjectCopy("metadata");
            }

            if (doc.ValueExists("timestamp"))
            {
                val.Timestamp = doc.GetDouble("timestamp");
            }

            if (doc.ValueExists("version"))
            {
                val.Version = doc.GetInteger("version");
            }

            if (doc.ValueExists("clientToken"))
            {
                val.ClientToken = doc.GetString("clientToken");
            }
        }

        void ShadowDeltaUpdatedEvent::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (State)
            {
                object.WithObject("state", *State);
            }

            if (Metadata)
            {
                object.WithObject("metadata", *Metadata);
            }

            if (Timestamp)
            {
                object.WithDouble("timestamp", Timestamp->SecondsWithMSPrecision());
            }

            if (Version)
            {
                object.WithInteger("version", *Version);
            }

            if (ClientToken)
            {
                object.WithString("clientToken", *ClientToken);
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
