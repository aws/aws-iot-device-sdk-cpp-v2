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
#include <aws/iotshadow/IotShadowClient.h>

#include <aws/iotshadow/DeleteShadowRequest.h>
#include <aws/iotshadow/DeleteShadowResponse.h>
#include <aws/iotshadow/DeleteShadowSubscriptionRequest.h>
#include <aws/iotshadow/ErrorResponse.h>
#include <aws/iotshadow/GetShadowRequest.h>
#include <aws/iotshadow/GetShadowResponse.h>
#include <aws/iotshadow/GetShadowSubscriptionRequest.h>
#include <aws/iotshadow/ShadowDeltaUpdatedEvent.h>
#include <aws/iotshadow/ShadowDeltaUpdatedSubscriptionRequest.h>
#include <aws/iotshadow/ShadowUpdatedEvent.h>
#include <aws/iotshadow/ShadowUpdatedSubscriptionRequest.h>
#include <aws/iotshadow/UpdateShadowRequest.h>
#include <aws/iotshadow/UpdateShadowResponse.h>
#include <aws/iotshadow/UpdateShadowSubscriptionRequest.h>

namespace Aws
{
    namespace Iotshadow
    {

        IotShadowClient::IotShadowClient(const std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> &connection)
            : m_connection(connection)
        {
        }

        IotShadowClient::operator bool() const noexcept { return *m_connection; }

        int IotShadowClient::GetLastError() const noexcept { return aws_last_error(); }

        bool IotShadowClient::SubscribeToUpdateShadowRejected(
            const Aws::Iotshadow::UpdateShadowSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToUpdateShadowRejectedResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            (void)request;
            auto onSubscribeComplete = [handler, onSubAck](
                                           Aws::Crt::Mqtt::MqttConnection &,
                                           uint16_t,
                                           const Aws::Crt::String &topic,
                                           Aws::Crt::Mqtt::QOS,
                                           int errorCode) {
                (void)topic;
                if (errorCode)
                {
                    handler(nullptr, errorCode);
                }

                if (onSubAck)
                {
                    onSubAck(errorCode);
                }
            };

            auto onSubscribePublish =
                [handler](
                    Aws::Crt::Mqtt::MqttConnection &, const Aws::Crt::String &, const Aws::Crt::ByteBuf &payload) {
                    Aws::Crt::String objectStr(reinterpret_cast<char *>(payload.buffer), payload.len);
                    Aws::Crt::JsonObject jsonObject(objectStr);
                    Aws::Iotshadow::ErrorResponse response(jsonObject);
                    handler(&response, AWS_ERROR_SUCCESS);
                };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "shadow"
                               << "/"
                               << "update"
                               << "/"
                               << "rejected";

            return m_connection->Subscribe(
                       subscribeTopicSStr.str().c_str(),
                       qos,
                       std::move(onSubscribePublish),
                       std::move(onSubscribeComplete)) != 0;
        }

        bool IotShadowClient::SubscribeToDeleteShadowRejected(
            const Aws::Iotshadow::DeleteShadowSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToDeleteShadowRejectedResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            (void)request;
            auto onSubscribeComplete = [handler, onSubAck](
                                           Aws::Crt::Mqtt::MqttConnection &,
                                           uint16_t,
                                           const Aws::Crt::String &topic,
                                           Aws::Crt::Mqtt::QOS,
                                           int errorCode) {
                (void)topic;
                if (errorCode)
                {
                    handler(nullptr, errorCode);
                }

                if (onSubAck)
                {
                    onSubAck(errorCode);
                }
            };

            auto onSubscribePublish =
                [handler](
                    Aws::Crt::Mqtt::MqttConnection &, const Aws::Crt::String &, const Aws::Crt::ByteBuf &payload) {
                    Aws::Crt::String objectStr(reinterpret_cast<char *>(payload.buffer), payload.len);
                    Aws::Crt::JsonObject jsonObject(objectStr);
                    Aws::Iotshadow::ErrorResponse response(jsonObject);
                    handler(&response, AWS_ERROR_SUCCESS);
                };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "shadow"
                               << "/"
                               << "delete"
                               << "/"
                               << "rejected";

            return m_connection->Subscribe(
                       subscribeTopicSStr.str().c_str(),
                       qos,
                       std::move(onSubscribePublish),
                       std::move(onSubscribeComplete)) != 0;
        }

        bool IotShadowClient::SubscribeToGetShadowAccepted(
            const Aws::Iotshadow::GetShadowSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToGetShadowAcceptedResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            (void)request;
            auto onSubscribeComplete = [handler, onSubAck](
                                           Aws::Crt::Mqtt::MqttConnection &,
                                           uint16_t,
                                           const Aws::Crt::String &topic,
                                           Aws::Crt::Mqtt::QOS,
                                           int errorCode) {
                (void)topic;
                if (errorCode)
                {
                    handler(nullptr, errorCode);
                }

                if (onSubAck)
                {
                    onSubAck(errorCode);
                }
            };

            auto onSubscribePublish =
                [handler](
                    Aws::Crt::Mqtt::MqttConnection &, const Aws::Crt::String &, const Aws::Crt::ByteBuf &payload) {
                    Aws::Crt::String objectStr(reinterpret_cast<char *>(payload.buffer), payload.len);
                    Aws::Crt::JsonObject jsonObject(objectStr);
                    Aws::Iotshadow::GetShadowResponse response(jsonObject);
                    handler(&response, AWS_ERROR_SUCCESS);
                };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "shadow"
                               << "/"
                               << "get"
                               << "/"
                               << "accepted";

            return m_connection->Subscribe(
                       subscribeTopicSStr.str().c_str(),
                       qos,
                       std::move(onSubscribePublish),
                       std::move(onSubscribeComplete)) != 0;
        }

        bool IotShadowClient::SubscribeToShadowUpdatedEvents(
            const Aws::Iotshadow::ShadowUpdatedSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToShadowUpdatedEventsResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            (void)request;
            auto onSubscribeComplete = [handler, onSubAck](
                                           Aws::Crt::Mqtt::MqttConnection &,
                                           uint16_t,
                                           const Aws::Crt::String &topic,
                                           Aws::Crt::Mqtt::QOS,
                                           int errorCode) {
                (void)topic;
                if (errorCode)
                {
                    handler(nullptr, errorCode);
                }

                if (onSubAck)
                {
                    onSubAck(errorCode);
                }
            };

            auto onSubscribePublish =
                [handler](
                    Aws::Crt::Mqtt::MqttConnection &, const Aws::Crt::String &, const Aws::Crt::ByteBuf &payload) {
                    Aws::Crt::String objectStr(reinterpret_cast<char *>(payload.buffer), payload.len);
                    Aws::Crt::JsonObject jsonObject(objectStr);
                    Aws::Iotshadow::ShadowUpdatedEvent response(jsonObject);
                    handler(&response, AWS_ERROR_SUCCESS);
                };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "shadow"
                               << "/"
                               << "update"
                               << "/"
                               << "documents";

            return m_connection->Subscribe(
                       subscribeTopicSStr.str().c_str(),
                       qos,
                       std::move(onSubscribePublish),
                       std::move(onSubscribeComplete)) != 0;
        }

        bool IotShadowClient::SubscribeToShadowDeltaUpdatedEvents(
            const Aws::Iotshadow::ShadowDeltaUpdatedSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToShadowDeltaUpdatedEventsResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            (void)request;
            auto onSubscribeComplete = [handler, onSubAck](
                                           Aws::Crt::Mqtt::MqttConnection &,
                                           uint16_t,
                                           const Aws::Crt::String &topic,
                                           Aws::Crt::Mqtt::QOS,
                                           int errorCode) {
                (void)topic;
                if (errorCode)
                {
                    handler(nullptr, errorCode);
                }

                if (onSubAck)
                {
                    onSubAck(errorCode);
                }
            };

            auto onSubscribePublish =
                [handler](
                    Aws::Crt::Mqtt::MqttConnection &, const Aws::Crt::String &, const Aws::Crt::ByteBuf &payload) {
                    Aws::Crt::String objectStr(reinterpret_cast<char *>(payload.buffer), payload.len);
                    Aws::Crt::JsonObject jsonObject(objectStr);
                    Aws::Iotshadow::ShadowDeltaUpdatedEvent response(jsonObject);
                    handler(&response, AWS_ERROR_SUCCESS);
                };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "shadow"
                               << "/"
                               << "update"
                               << "/"
                               << "delta";

            return m_connection->Subscribe(
                       subscribeTopicSStr.str().c_str(),
                       qos,
                       std::move(onSubscribePublish),
                       std::move(onSubscribeComplete)) != 0;
        }

        bool IotShadowClient::SubscribeToDeleteShadowAccepted(
            const Aws::Iotshadow::DeleteShadowSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToDeleteShadowAcceptedResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            (void)request;
            auto onSubscribeComplete = [handler, onSubAck](
                                           Aws::Crt::Mqtt::MqttConnection &,
                                           uint16_t,
                                           const Aws::Crt::String &topic,
                                           Aws::Crt::Mqtt::QOS,
                                           int errorCode) {
                (void)topic;
                if (errorCode)
                {
                    handler(nullptr, errorCode);
                }

                if (onSubAck)
                {
                    onSubAck(errorCode);
                }
            };

            auto onSubscribePublish =
                [handler](
                    Aws::Crt::Mqtt::MqttConnection &, const Aws::Crt::String &, const Aws::Crt::ByteBuf &payload) {
                    Aws::Crt::String objectStr(reinterpret_cast<char *>(payload.buffer), payload.len);
                    Aws::Crt::JsonObject jsonObject(objectStr);
                    Aws::Iotshadow::DeleteShadowResponse response(jsonObject);
                    handler(&response, AWS_ERROR_SUCCESS);
                };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "shadow"
                               << "/"
                               << "delete"
                               << "/"
                               << "accepted";

            return m_connection->Subscribe(
                       subscribeTopicSStr.str().c_str(),
                       qos,
                       std::move(onSubscribePublish),
                       std::move(onSubscribeComplete)) != 0;
        }

        bool IotShadowClient::SubscribeToGetShadowRejected(
            const Aws::Iotshadow::GetShadowSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToGetShadowRejectedResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            (void)request;
            auto onSubscribeComplete = [handler, onSubAck](
                                           Aws::Crt::Mqtt::MqttConnection &,
                                           uint16_t,
                                           const Aws::Crt::String &topic,
                                           Aws::Crt::Mqtt::QOS,
                                           int errorCode) {
                (void)topic;
                if (errorCode)
                {
                    handler(nullptr, errorCode);
                }

                if (onSubAck)
                {
                    onSubAck(errorCode);
                }
            };

            auto onSubscribePublish =
                [handler](
                    Aws::Crt::Mqtt::MqttConnection &, const Aws::Crt::String &, const Aws::Crt::ByteBuf &payload) {
                    Aws::Crt::String objectStr(reinterpret_cast<char *>(payload.buffer), payload.len);
                    Aws::Crt::JsonObject jsonObject(objectStr);
                    Aws::Iotshadow::ErrorResponse response(jsonObject);
                    handler(&response, AWS_ERROR_SUCCESS);
                };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "shadow"
                               << "/"
                               << "get"
                               << "/"
                               << "rejected";

            return m_connection->Subscribe(
                       subscribeTopicSStr.str().c_str(),
                       qos,
                       std::move(onSubscribePublish),
                       std::move(onSubscribeComplete)) != 0;
        }

        bool IotShadowClient::SubscribeToUpdateShadowAccepted(
            const Aws::Iotshadow::UpdateShadowSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToUpdateShadowAcceptedResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            (void)request;
            auto onSubscribeComplete = [handler, onSubAck](
                                           Aws::Crt::Mqtt::MqttConnection &,
                                           uint16_t,
                                           const Aws::Crt::String &topic,
                                           Aws::Crt::Mqtt::QOS,
                                           int errorCode) {
                (void)topic;
                if (errorCode)
                {
                    handler(nullptr, errorCode);
                }

                if (onSubAck)
                {
                    onSubAck(errorCode);
                }
            };

            auto onSubscribePublish =
                [handler](
                    Aws::Crt::Mqtt::MqttConnection &, const Aws::Crt::String &, const Aws::Crt::ByteBuf &payload) {
                    Aws::Crt::String objectStr(reinterpret_cast<char *>(payload.buffer), payload.len);
                    Aws::Crt::JsonObject jsonObject(objectStr);
                    Aws::Iotshadow::UpdateShadowResponse response(jsonObject);
                    handler(&response, AWS_ERROR_SUCCESS);
                };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "shadow"
                               << "/"
                               << "update"
                               << "/"
                               << "accepted";

            return m_connection->Subscribe(
                       subscribeTopicSStr.str().c_str(),
                       qos,
                       std::move(onSubscribePublish),
                       std::move(onSubscribeComplete)) != 0;
        }

        bool IotShadowClient::PublishGetShadow(
            const Aws::Iotshadow::GetShadowRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnPublishComplete &onPubAck)
        {
            Aws::Crt::StringStream publishTopicSStr;
            publishTopicSStr << "$aws"
                             << "/"
                             << "things"
                             << "/" << *request.ThingName << "/"
                             << "shadow"
                             << "/"
                             << "get";

            Aws::Crt::JsonObject jsonObject;
            request.SerializeToObject(jsonObject);
            Aws::Crt::String outgoingJson = jsonObject.View().WriteCompact(true);
            Aws::Crt::ByteBuf buf = Aws::Crt::ByteBufNewCopy(
                Aws::Crt::g_allocator, reinterpret_cast<const uint8_t *>(outgoingJson.data()), outgoingJson.length());

            auto onPublishComplete = [buf, onPubAck](Aws::Crt::Mqtt::MqttConnection &, uint16_t, int errorCode) {
                onPubAck(errorCode);
                Aws::Crt::ByteBufDelete(const_cast<Aws::Crt::ByteBuf &>(buf));
            };

            return m_connection->Publish(
                       publishTopicSStr.str().c_str(), qos, false, buf, std::move(onPublishComplete)) != 0;
        }

        bool IotShadowClient::PublishDeleteShadow(
            const Aws::Iotshadow::DeleteShadowRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnPublishComplete &onPubAck)
        {
            Aws::Crt::StringStream publishTopicSStr;
            publishTopicSStr << "$aws"
                             << "/"
                             << "things"
                             << "/" << *request.ThingName << "/"
                             << "shadow"
                             << "/"
                             << "delete";

            Aws::Crt::JsonObject jsonObject;
            request.SerializeToObject(jsonObject);
            Aws::Crt::String outgoingJson = jsonObject.View().WriteCompact(true);
            Aws::Crt::ByteBuf buf = Aws::Crt::ByteBufNewCopy(
                Aws::Crt::g_allocator, reinterpret_cast<const uint8_t *>(outgoingJson.data()), outgoingJson.length());

            auto onPublishComplete = [buf, onPubAck](Aws::Crt::Mqtt::MqttConnection &, uint16_t, int errorCode) {
                onPubAck(errorCode);
                Aws::Crt::ByteBufDelete(const_cast<Aws::Crt::ByteBuf &>(buf));
            };

            return m_connection->Publish(
                       publishTopicSStr.str().c_str(), qos, false, buf, std::move(onPublishComplete)) != 0;
        }

        bool IotShadowClient::PublishUpdateShadow(
            const Aws::Iotshadow::UpdateShadowRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnPublishComplete &onPubAck)
        {
            Aws::Crt::StringStream publishTopicSStr;
            publishTopicSStr << "$aws"
                             << "/"
                             << "things"
                             << "/" << *request.ThingName << "/"
                             << "shadow"
                             << "/"
                             << "update";

            Aws::Crt::JsonObject jsonObject;
            request.SerializeToObject(jsonObject);
            Aws::Crt::String outgoingJson = jsonObject.View().WriteCompact(true);
            Aws::Crt::ByteBuf buf = Aws::Crt::ByteBufNewCopy(
                Aws::Crt::g_allocator, reinterpret_cast<const uint8_t *>(outgoingJson.data()), outgoingJson.length());

            auto onPublishComplete = [buf, onPubAck](Aws::Crt::Mqtt::MqttConnection &, uint16_t, int errorCode) {
                onPubAck(errorCode);
                Aws::Crt::ByteBufDelete(const_cast<Aws::Crt::ByteBuf &>(buf));
            };

            return m_connection->Publish(
                       publishTopicSStr.str().c_str(), qos, false, buf, std::move(onPublishComplete)) != 0;
        }

    } // namespace Iotshadow

} // namespace Aws
