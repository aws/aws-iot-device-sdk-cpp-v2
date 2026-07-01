/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotcommands/UpdateCommandExecutionRequest.h>

namespace Aws
{
    namespace Iotcommands
    {

        void UpdateCommandExecutionRequest::LoadFromObject(
            UpdateCommandExecutionRequest &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("status"))
            {
                val.Status = CommandExecutionStatusMarshaller::FromString(doc.GetString("status"));
            }

            if (doc.ValueExists("statusReason"))
            {
                val.StatusReason = doc.GetJsonObject("statusReason");
            }

            if (doc.ValueExists("result"))
            {
                auto resultMap = doc.GetJsonObject("result");
                val.Result = Aws::Crt::Map<Aws::Crt::String, Aws::Iotcommands::CommandExecutionResult>();
                for (auto &resultMapMember : resultMap.GetAllObjects())
                {
                    Aws::Iotcommands::CommandExecutionResult resultMapValMember;
                    resultMapValMember = resultMapMember.second.AsObject();
                    val.Result->emplace(resultMapMember.first, std::move(resultMapValMember));
                }
            }
        }

        void UpdateCommandExecutionRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (Status)
            {
                object.WithString("status", CommandExecutionStatusMarshaller::ToString(*Status));
            }

            if (StatusReason)
            {
                Aws::Crt::JsonObject jsonObject;
                StatusReason->SerializeToObject(jsonObject);
                object.WithObject("statusReason", std::move(jsonObject));
            }

            if (Result)
            {
                Aws::Crt::JsonObject resultMap;
                for (auto &resultMapMember : *Result)
                {
                    Aws::Crt::JsonObject resultMapValMember;
                    Aws::Crt::JsonObject jsonObject;
                    resultMapMember.second.SerializeToObject(jsonObject);
                    resultMapValMember.AsObject(std::move(jsonObject));
                    resultMap.WithObject(resultMapMember.first, std::move(resultMapValMember));
                }
                object.WithObject("result", std::move(resultMap));
            }
        }

        UpdateCommandExecutionRequest::UpdateCommandExecutionRequest(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        UpdateCommandExecutionRequest &UpdateCommandExecutionRequest::operator=(const Crt::JsonView &doc)
        {
            *this = UpdateCommandExecutionRequest(doc);
            return *this;
        }

    } // namespace Iotcommands
} // namespace Aws
