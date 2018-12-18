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
#include <aws/iotsdk/jobs/IotJobsClient.h>

#include <aws/crt/JsonObject.h>

namespace Aws
{
    namespace IotSdk
    {
        namespace Jobs
        {
            IotJobsClient::IotJobsClient(const std::shared_ptr<Crt::Mqtt::MqttConnection>& connection) :
                m_connection(connection)
            {
            }

            IotJobsClient::operator bool() const noexcept
            {
                return *m_connection;
            }

            int IotJobsClient::GetLastError() const noexcept
            {
                return aws_last_error();
            }

            bool IotJobsClient::SubscribeToDescribeJobExecutionAccepted(const DescribeJobExecutionSubscriptionRequest& request,
                Crt::Mqtt::QOS qos, const OnDescribeJobExecutionAcceptedResponse& handler, const OnSubscribeComplete& onSubAckHandler)
            {
                auto onSubscribeComplete = [handler, onSubAckHandler](Crt::Mqtt::MqttConnection&, uint16_t, const Crt::String& topic, Crt::Mqtt::QOS, int errorCode)
                {
                    (void)topic;
                    if (errorCode)
                    {
                        handler(nullptr, errorCode);
                    }

                    if (onSubAckHandler)
                    {
                        onSubAckHandler(errorCode);
                    }
                };

                auto onSubscribePublish = [handler](Crt::Mqtt::MqttConnection&,
                    const Crt::String&, const Crt::ByteBuf& payload)
                {
                    Crt::String objectStr(reinterpret_cast<char*>(payload.buffer), payload.len);
                    DescribeJobExecutionResponse response(objectStr);
                    handler(&response, AWS_ERROR_SUCCESS);
                };

                Crt::StringStream subscribeTopicSStr;
                subscribeTopicSStr << "$aws/things/" << request.ThingName << "/jobs/" << request.JobId
                    << "/get/accepted";

                return m_connection->Subscribe(subscribeTopicSStr.str().c_str(), qos,
                    std::move(onSubscribePublish), std::move(onSubscribeComplete)) != 0;
            }            

            bool IotJobsClient::SubscribeToDescribeJobExecutionRejected(const DescribeJobExecutionSubscriptionRequest& request,
                Crt::Mqtt::QOS qos, const OnDescribeJobExecutionRejectedResponse& handler, const OnSubscribeComplete& onSubAckHandler)
            {
                auto onSubscribeComplete = [handler, onSubAckHandler](Crt::Mqtt::MqttConnection&, uint16_t, const Crt::String& topic, Crt::Mqtt::QOS, int errorCode)
                {
                    (void)topic;
                    if (errorCode)
                    {
                        handler(nullptr, errorCode);
                    }

                    if (onSubAckHandler)
                    {
                        onSubAckHandler(errorCode);
                    }
                };

                auto onSubscribePublish = [handler](Crt::Mqtt::MqttConnection&,
                    const Crt::String&, const Crt::ByteBuf& payload)
                {
                    Crt::String objectStr(reinterpret_cast<char*>(payload.buffer), payload.len);

                    Crt::JsonObject object(objectStr);
                    JobsError error(object);
                    handler(&error, AWS_ERROR_SUCCESS);
                };

                Crt::StringStream subscribeTopicSStr;
                subscribeTopicSStr << "$aws/things/" << request.ThingName << "/jobs/" << request.JobId
                    << "/get/rejected";

                return m_connection->Subscribe(subscribeTopicSStr.str().c_str(), qos,
                    std::move(onSubscribePublish), std::move(onSubscribeComplete)) != 0;
            }

            bool IotJobsClient::PublishDescribeJobExecution(const DescribeJobExecutionRequest& request,
                                                     Crt::Mqtt::QOS qos, const OnPublishComplete& handler)
            {
                Crt::StringStream publishTopic;
                publishTopic << "$aws/things/" << request.ThingName << "/jobs/" << request.JobId << "/get";                

                auto&& outgoingJson = request.Serialize();
                Crt::ByteBuf buf = Crt::ByteBufNewCopy(Crt::g_allocator,
                                                       reinterpret_cast<const uint8_t*>(outgoingJson.data()),
                                                       outgoingJson.length());

                auto onPublishComplete = [buf, handler](Crt::Mqtt::MqttConnection&,
                        uint16_t, int errorCode)
                {
                    handler(errorCode);
                    Crt::ByteBufDelete(const_cast<Crt::ByteBuf&>(buf));
                };

                return m_connection->Publish(publishTopic.str().c_str(), qos, false, buf, onPublishComplete) != 0;
            }
        }
    }
}
