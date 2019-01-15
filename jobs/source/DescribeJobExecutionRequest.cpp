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
#include <aws/iotjobs/DescribeJobExecutionRequest.h>

namespace Aws
{
    namespace Iotjobs
    {

        void DescribeJobExecutionRequest::LoadFromObject(
            DescribeJobExecutionRequest &val,
            const Aws::Crt::JsonView &doc)
        {

            if (doc.ValueExists("includeJobDocument"))
            {
                val.IncludeJobDocument = doc.GetBool("includeJobDocument");
            }

            if (doc.ValueExists("clientToken"))
            {
                val.ClientToken = doc.GetString("clientToken");
            }

            if (doc.ValueExists("executionNumber"))
            {
                val.ExecutionNumber = doc.GetInt64("executionNumber");
            }
        }

        void DescribeJobExecutionRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
        {

            if (IncludeJobDocument)
            {
                object.WithBool("includeJobDocument", *IncludeJobDocument);
            }

            if (ClientToken)
            {
                object.WithString("clientToken", *ClientToken);
            }

            if (ExecutionNumber)
            {
                object.WithInt64("executionNumber", *ExecutionNumber);
            }
        }

        DescribeJobExecutionRequest::DescribeJobExecutionRequest(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        DescribeJobExecutionRequest &DescribeJobExecutionRequest::operator=(const Crt::JsonView &doc)
        {
            *this = DescribeJobExecutionRequest(doc);
            return *this;
        }

    } // namespace Iotjobs
} // namespace Aws
