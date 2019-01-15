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
#include <aws/iotshadow/ShadowDeltaUpdatedEvent.h>

namespace Aws
{
    namespace Iotshadow
    {

        void ShadowDeltaUpdatedEvent::LoadFromObject(ShadowDeltaUpdatedEvent &val, const Aws::Crt::JsonView &doc)
        {

            if (doc.ValueExists("metadata"))
            {
                val.Metadata = doc.Materialize();
            }

            if (doc.ValueExists("version"))
            {
                val.Version = doc.GetInteger("version");
            }

            if (doc.ValueExists("timestamp"))
            {
                val.Timestamp = doc.GetDouble("timestamp");
            }

            if (doc.ValueExists("state"))
            {
                val.State = doc.Materialize();
            }
        }

        void ShadowDeltaUpdatedEvent::SerializeToObject(Aws::Crt::JsonObject &object) const
        {

            if (Metadata)
            {
                object.WithObject("metadata", *Metadata);
            }

            if (Version)
            {
                object.WithInteger("version", *Version);
            }

            if (Timestamp)
            {
                object.WithDouble("timestamp", Timestamp->SecondsWithMSPrecision());
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
