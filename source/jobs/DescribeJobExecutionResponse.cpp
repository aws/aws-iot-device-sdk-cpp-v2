/*
* Copyright 2010-2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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
*/
#include <aws/iotsdk/jobs/DescribeJobExecutionResponse.h>

#include <aws/crt/JsonObject.h>

namespace Aws
{
    namespace IotSdk
    {
        namespace Jobs
        {
            void DescribeJobExecutionResponse::LoadFromObject(DescribeJobExecutionResponse &val,
                                                              const Crt::JsonView &doc)
            {
                if (doc.ValueExists("clientToken"))
                {
                    val.ClientToken = doc.GetString("clientToken");
                }

                if (doc.ValueExists("execution"))
                {
                    val.Execution = doc.GetObject("execution");
                }

                if (doc.ValueExists("timestamp"))
                {
                    val.Timestamp = doc.GetDouble("timestamp");
                }
            }

            DescribeJobExecutionResponse::DescribeJobExecutionResponse(const Crt::String& responseDoc) :
                m_good(false)
            {
                Crt::JsonObject doc(responseDoc);

                if (doc.WasParseSuccessful())
                {
                    m_good = true;
                }

                LoadFromObject(*this, doc.View());
            }
        }
    }
}