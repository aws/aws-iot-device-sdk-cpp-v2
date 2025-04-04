/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotcommand/UpdateCommandExecutionResponse.h>

namespace Aws
{
    namespace Iotcommand
    {

        void UpdateCommandExecutionResponse::LoadFromObject(
            UpdateCommandExecutionResponse &val,
            const Aws::Crt::JsonView &doc)
        {
            if (doc.ValueExists("timestamp"))
            {
                val.Timestamp = doc.GetDouble("timestamp");
            }
        }

        void UpdateCommandExecutionResponse::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            if (Timestamp)
            {
                object.WithDouble("timestamp", Timestamp->SecondsWithMSPrecision());
            }
        }

        UpdateCommandExecutionResponse::UpdateCommandExecutionResponse(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        UpdateCommandExecutionResponse &UpdateCommandExecutionResponse::operator=(const Crt::JsonView &doc)
        {
            *this = UpdateCommandExecutionResponse(doc);
            return *this;
        }

    } // namespace Iotcommand
} // namespace Aws
