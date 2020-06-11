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
#include <aws/iotjobs/RejectedError.h>

namespace Aws
{
namespace Iotjobs
{

    void RejectedError::LoadFromObject(RejectedError& val, const Aws::Crt::JsonView &doc)
    {
        (void)val;
        (void)doc;

        if (doc.ValueExists("timestamp"))
        {
            val.Timestamp = doc.GetDouble("timestamp");
        }

        if (doc.ValueExists("code"))
        {
            val.Code = RejectedErrorCodeMarshaller::FromString(doc.GetString("code"));
        }

        if (doc.ValueExists("message"))
        {
            val.Message = doc.GetString("message");
        }

        if (doc.ValueExists("clientToken"))
        {
            val.ClientToken = doc.GetString("clientToken");
        }

        if (doc.ValueExists("executionState"))
        {
            val.ExecutionState = doc.GetJsonObject("executionState");
        }

    }

    void RejectedError::SerializeToObject(Aws::Crt::JsonObject& object) const
    {
        (void)object;

        if (Timestamp)
        {
            object.WithDouble("timestamp", Timestamp->SecondsWithMSPrecision());
        }

        if (Code)
        {
            object.WithString("code", RejectedErrorCodeMarshaller::ToString(*Code));
        }

        if (Message)
        {
            object.WithString("message", *Message);
        }

        if (ClientToken)
        {
            object.WithString("clientToken", *ClientToken);
        }

        if (ExecutionState)
        {
            Aws::Crt::JsonObject jsonObject;
            ExecutionState->SerializeToObject(jsonObject);
            object.WithObject("executionState", std::move(jsonObject));
        }

    }

    RejectedError::RejectedError(const Crt::JsonView& doc)
    {
        LoadFromObject(*this, doc);
    }

    RejectedError& RejectedError::operator=(const Crt::JsonView& doc)
    {
        *this = RejectedError(doc);
        return *this;
    }

}
}
