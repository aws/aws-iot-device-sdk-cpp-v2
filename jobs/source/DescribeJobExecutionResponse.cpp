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
#include <aws/iotjobs/DescribeJobExecutionResponse.h>

namespace Aws
{
namespace Iotjobs
{

    void DescribeJobExecutionResponse::LoadFromObject(DescribeJobExecutionResponse& val, const Aws::Crt::JsonView &doc)
    {
        (void)val;
        (void)doc;

        if (doc.ValueExists("execution"))
        {
            val.Execution = doc.GetJsonObject("execution");
        }

        if (doc.ValueExists("clientToken"))
        {
            val.ClientToken = doc.GetString("clientToken");
        }

        if (doc.ValueExists("timestamp"))
        {
            val.Timestamp = doc.GetDouble("timestamp");
        }

    }

    void DescribeJobExecutionResponse::SerializeToObject(Aws::Crt::JsonObject& object) const
    {
        (void)object;

        if (Execution)
        {
            Aws::Crt::JsonObject jsonObject;
            Execution->SerializeToObject(jsonObject);
            object.WithObject("execution", std::move(jsonObject));
        }

        if (ClientToken)
        {
            object.WithString("clientToken", *ClientToken);
        }

        if (Timestamp)
        {
            object.WithDouble("timestamp", Timestamp->SecondsWithMSPrecision());
        }

    }

    DescribeJobExecutionResponse::DescribeJobExecutionResponse(const Crt::JsonView& doc)
    {
        LoadFromObject(*this, doc);
    }

    DescribeJobExecutionResponse& DescribeJobExecutionResponse::operator=(const Crt::JsonView& doc)
    {
        *this = DescribeJobExecutionResponse(doc);
        return *this;
    }

}
}
