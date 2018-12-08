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
#include <aws/iotsdk/jobs/DescribeJobExecutionRequest.h>

namespace Aws
{
    namespace IotSdk
    {
        namespace Jobs
        {
            DescribeJobExecutionRequest::DescribeJobExecutionRequest(
                    Crt::String&& thingName,
                    Crt::String&& jobId,
                    Crt::String&& clientToken) :
                    ThingName(std::move(thingName)),
                    JobId(std::move(jobId)),
                    ClientToken(std::move(clientToken))
            {
            }

            DescribeJobExecutionRequest::DescribeJobExecutionRequest(
                    const Crt::String& thingName,
                    const Crt::String& jobId,
                    const Crt::String& clientToken) :
                    ThingName(thingName),
                    JobId(jobId),
                    ClientToken(clientToken)
            {
            }


            Crt::String DescribeJobExecutionRequest::Serialize() const
            {
                Crt::JsonObject newDoc;
                SerializeToObject(newDoc);
                return newDoc.View().WriteReadable(true);
            }

            void DescribeJobExecutionRequest::SerializeToObject(Crt::JsonObject& object) const
            {
                object.WithString("clientToken", ClientToken);

                if (ExecutionNumber)
                {
                    object.WithInt64("executionNumber", *ExecutionNumber);
                }

                if (IncludeJobDocument)
                {
                    object.WithBool("includeJobDocument", *IncludeJobDocument);
                }
            }
        }
    }
}