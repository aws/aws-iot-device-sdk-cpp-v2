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

            bool IotJobsClient::DescribeJobExecution(DescribeJobExecutionRequest&& request,
                                                     Crt::Mqtt::QOS qos, OnDescribeJobExecutionResponse&& onResponse)
            {
                auto onSubscribeComplete = [onResponse](Crt::Mqtt::MqttConnection& connection, uint16_t packetId)
                {
                    if (!packetId)
                    {
                        onResponse(nullptr, nullptr, aws_last_error());
                    }
                };

                Crt::StringStream subscribeTopic;
                subscribeTopic << "$aws/things/" << request.ThingName << "/jobs/" << request.JobId
                    << "/get/accepted";

                auto onSubscribePublish = [onResponse](Crt::Mqtt::MqttConnection& connection,
                                                       const Crt::ByteBuf& topic, const Crt::ByteBuf& payload)
                {
                    Crt::String objectStr(reinterpret_cast<char*>(payload.buffer), payload.len);
                    DescribeJobExecutionResponse response(objectStr);
                    onResponse(&response, nullptr, AWS_ERROR_SUCCESS);
                };

                Crt::StringStream errorTopic;
                errorTopic << "$aws/things/" << request.ThingName << "/jobs/" << request.JobId
                    << "/get/rejected";

                auto onErrorPublish = [onResponse](Crt::Mqtt::MqttConnection& connection,
                                                   const Crt::ByteBuf& topic, const Crt::ByteBuf& payload)
                {
                    Crt::String objectStr(reinterpret_cast<char*>(payload.buffer), payload.len);

                    Crt::JsonObject object(objectStr);
                    JobsError error(object);
                    onResponse(nullptr, &error, AWS_ERROR_SUCCESS);
                };

                Crt::StringStream publishTopic;
                publishTopic << "$aws/things/" << request.ThingName << "/jobs/" << request.JobId << "/get";

                if (!m_connection->Subscribe(subscribeTopic.str().c_str(), qos,
                        std::move(onSubscribePublish), onSubscribeComplete))
                {
                    return false;
                }

                if (!m_connection->Subscribe(errorTopic.str().c_str(), qos,
                                             std::move(onErrorPublish), onSubscribeComplete))
                {
                    return false;
                }

                auto&& outgoingJson = request.Serialize();
                Crt::ByteBuf buf = Crt::ByteBufNewCopy(Crt::g_allocator,
                                                       reinterpret_cast<const uint8_t*>(outgoingJson.data()),
                                                       outgoingJson.length());

                auto onPublishComplete = [buf, onResponse](Crt::Mqtt::MqttConnection& connection,
                        uint16_t packetId)
                {
                    if (!packetId)
                    {
                        onResponse(nullptr, nullptr, aws_last_error());
                    }

                    Crt::ByteBufDelete(const_cast<Crt::ByteBuf&>(buf));
                };

                return m_connection->Publish(publishTopic.str().c_str(), qos, false, buf, onPublishComplete) != 0;

            }
        }
    }
}