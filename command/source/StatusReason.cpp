/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotcommand/StatusReason.h>

namespace Aws
{
    namespace Iotcommand
    {

        void StatusReason::LoadFromObject(StatusReason &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("reasonCode"))
            {
                val.ReasonCode = doc.GetString("reasonCode");
            }

            if (doc.ValueExists("reasonDescription"))
            {
                val.ReasonDescription = doc.GetString("reasonDescription");
            }
        }

        void StatusReason::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (ReasonCode)
            {
                object.WithString("reasonCode", *ReasonCode);
            }

            if (ReasonDescription)
            {
                object.WithString("reasonDescription", *ReasonDescription);
            }
        }

        StatusReason::StatusReason(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        StatusReason &StatusReason::operator=(const Crt::JsonView &doc)
        {
            *this = StatusReason(doc);
            return *this;
        }

    } // namespace Iotcommand
} // namespace Aws
