/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
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
            (void)val;
            (void)doc;

            if (doc.ValueExists("executionNumber"))
            {
                val.ExecutionNumber = doc.GetInt64("executionNumber");
            }

            if (doc.ValueExists("includeJobDocument"))
            {
                val.IncludeJobDocument = doc.GetBool("includeJobDocument");
            }

            if (doc.ValueExists("clientToken"))
            {
                val.ClientToken = doc.GetString("clientToken");
            }
        }

        void DescribeJobExecutionRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (ExecutionNumber)
            {
                object.WithInt64("executionNumber", *ExecutionNumber);
            }

            if (IncludeJobDocument)
            {
                object.WithBool("includeJobDocument", *IncludeJobDocument);
            }

            if (ClientToken)
            {
                object.WithString("clientToken", *ClientToken);
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
