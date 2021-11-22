/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotjobs/NextJobExecutionChangedEvent.h>

namespace Aws
{
    namespace Iotjobs
    {

        void NextJobExecutionChangedEvent::LoadFromObject(
            NextJobExecutionChangedEvent &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("execution"))
            {
                val.Execution = doc.GetJsonObject("execution");
            }

            if (doc.ValueExists("timestamp"))
            {
                val.Timestamp = doc.GetDouble("timestamp");
            }
        }

        void NextJobExecutionChangedEvent::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (Execution)
            {
                Aws::Crt::JsonObject jsonObject;
                Execution->SerializeToObject(jsonObject);
                object.WithObject("execution", std::move(jsonObject));
            }

            if (Timestamp)
            {
                object.WithDouble("timestamp", Timestamp->SecondsWithMSPrecision());
            }
        }

        NextJobExecutionChangedEvent::NextJobExecutionChangedEvent(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        NextJobExecutionChangedEvent &NextJobExecutionChangedEvent::operator=(const Crt::JsonView &doc)
        {
            *this = NextJobExecutionChangedEvent(doc);
            return *this;
        }

    } // namespace Iotjobs
} // namespace Aws
