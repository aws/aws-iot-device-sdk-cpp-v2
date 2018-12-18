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
#include <aws/iotsdk/jobs/DescribeJobExecutionSubscriptionRequest.h>

namespace Aws
{
    namespace IotSdk
    {
        namespace Jobs
        {
            DescribeJobExecutionSubscriptionRequest::DescribeJobExecutionSubscriptionRequest(
                Crt::String&& thingName,
                Crt::String&& jobId) :
                ThingName(std::move(thingName)),
                JobId(std::move(jobId))
            {
            }

            DescribeJobExecutionSubscriptionRequest::DescribeJobExecutionSubscriptionRequest(
                const Crt::String& thingName,
                const Crt::String& jobId) :
                ThingName(thingName),
                JobId(jobId)
            {
            }           
        }
    }
}