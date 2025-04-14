/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotcommands/UpdateCommandExecutionResponse.h>

namespace Aws
{
    namespace Iotcommands
    {

        void UpdateCommandExecutionResponse::LoadFromObject(
            UpdateCommandExecutionResponse &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("executionId"))
            {
                val.ExecutionId = doc.GetString("executionId");
            }
        }

        void UpdateCommandExecutionResponse::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (ExecutionId)
            {
                object.WithString("executionId", *ExecutionId);
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

    } // namespace Iotcommands
} // namespace Aws
