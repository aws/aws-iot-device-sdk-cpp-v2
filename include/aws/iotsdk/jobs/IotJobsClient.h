#pragma once
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
#include <aws/iotsdk/jobs/DescribeJobExecutionResponse.h>
#include <aws/iotsdk/jobs/JobsError.h>

#include <aws/crt/DateTime.h>
#include <aws/crt/Types.h>
#include <aws/crt/StlAllocator.h>

#include <aws/crt/mqtt/MqttClient.h>

namespace Aws
{
    namespace IotSdk
    {
        namespace Jobs
        {
            using OnDescribeJobExecutionResponse =
                    std::function<void(DescribeJobExecutionResponse*, JobsError*, int ioErr)>;

            class IotJobsClient final
            {
            public:
                IotJobsClient(const std::shared_ptr<Crt::Mqtt::MqttConnection>& connection);

                operator bool() const;
                int GetLastError() const;

                bool DescribeJobExecution(DescribeJobExecutionRequest&& request,
                        Crt::Mqtt::QOS qos, OnDescribeJobExecutionResponse&& onResponse);

            private:
                std::shared_ptr<Crt::Mqtt::MqttConnection> m_connection;
            };
        }
    }
}