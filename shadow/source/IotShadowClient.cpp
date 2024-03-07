/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotshadow/IotShadowClient.h>

#include <aws/iotshadow/DeleteNamedShadowRequest.h>
#include <aws/iotshadow/DeleteNamedShadowSubscriptionRequest.h>
#include <aws/iotshadow/DeleteShadowRequest.h>
#include <aws/iotshadow/DeleteShadowResponse.h>
#include <aws/iotshadow/DeleteShadowSubscriptionRequest.h>
#include <aws/iotshadow/ErrorResponse.h>
#include <aws/iotshadow/GetNamedShadowRequest.h>
#include <aws/iotshadow/GetNamedShadowSubscriptionRequest.h>
#include <aws/iotshadow/GetShadowRequest.h>
#include <aws/iotshadow/GetShadowResponse.h>
#include <aws/iotshadow/GetShadowSubscriptionRequest.h>
#include <aws/iotshadow/NamedShadowDeltaUpdatedSubscriptionRequest.h>
#include <aws/iotshadow/NamedShadowUpdatedSubscriptionRequest.h>
#include <aws/iotshadow/ShadowDeltaUpdatedEvent.h>
#include <aws/iotshadow/ShadowDeltaUpdatedSubscriptionRequest.h>
#include <aws/iotshadow/ShadowUpdatedEvent.h>
#include <aws/iotshadow/ShadowUpdatedSubscriptionRequest.h>
#include <aws/iotshadow/UpdateNamedShadowRequest.h>
#include <aws/iotshadow/UpdateNamedShadowSubscriptionRequest.h>
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

        IotShadowClient::IotShadowClient(const std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> &mqtt5Client)
        {
            m_connection = Aws::Crt::Mqtt::MqttConnection::NewConnectionFromMqtt5Client(mqtt5Client);
        }

        IotShadowClient::operator bool() const noexcept { return m_connection && *m_connection; }

        int IotShadowClient::GetLastError() const noexcept { return aws_last_error(); }

        bool IotShadowClient::SubscribeToDeleteNamedShadowAccepted(
            const Aws::Iotshadow::DeleteNamedShadowSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToDeleteNamedShadowAcceptedResponse &handler,
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
                               << "name"
                               << "/" << *request.ShadowName << "/"
                               << "delete"
                               << "/"
                               << "accepted";

            return m_connection->Subscribe(
                       subscribeTopicSStr.str().c_str(),
                       qos,
                       std::move(onSubscribePublish),
                       std::move(onSubscribeComplete)) != 0;
        }

        bool IotShadowClient::SubscribeToDeleteNamedShadowRejected(
            const Aws::Iotshadow::DeleteNamedShadowSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToDeleteNamedShadowRejectedResponse &handler,
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
                               << "name"
                               << "/" << *request.ShadowName << "/"
                               << "delete"
                               << "/"
                               << "rejected";

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

        bool IotShadowClient::SubscribeToGetNamedShadowAccepted(
            const Aws::Iotshadow::GetNamedShadowSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToGetNamedShadowAcceptedResponse &handler,
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
                               << "name"
                               << "/" << *request.ShadowName << "/"
                               << "get"
                               << "/"
                               << "accepted";

            return m_connection->Subscribe(
                       subscribeTopicSStr.str().c_str(),
                       qos,
                       std::move(onSubscribePublish),
                       std::move(onSubscribeComplete)) != 0;
        }

        bool IotShadowClient::SubscribeToGetNamedShadowRejected(
            const Aws::Iotshadow::GetNamedShadowSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToGetNamedShadowRejectedResponse &handler,
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
                               << "name"
                               << "/" << *request.ShadowName << "/"
                               << "get"
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

        bool IotShadowClient::SubscribeToNamedShadowDeltaUpdatedEvents(
            const Aws::Iotshadow::NamedShadowDeltaUpdatedSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToNamedShadowDeltaUpdatedEventsResponse &handler,
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
                               << "name"
                               << "/" << *request.ShadowName << "/"
                               << "update"
                               << "/"
                               << "delta";

            return m_connection->Subscribe(
                       subscribeTopicSStr.str().c_str(),
                       qos,
                       std::move(onSubscribePublish),
                       std::move(onSubscribeComplete)) != 0;
        }

        bool IotShadowClient::SubscribeToNamedShadowUpdatedEvents(
            const Aws::Iotshadow::NamedShadowUpdatedSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToNamedShadowUpdatedEventsResponse &handler,
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
                               << "name"
                               << "/" << *request.ShadowName << "/"
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

        bool IotShadowClient::SubscribeToUpdateNamedShadowAccepted(
            const Aws::Iotshadow::UpdateNamedShadowSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToUpdateNamedShadowAcceptedResponse &handler,
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
                               << "name"
                               << "/" << *request.ShadowName << "/"
                               << "update"
                               << "/"
                               << "accepted";

            return m_connection->Subscribe(
                       subscribeTopicSStr.str().c_str(),
                       qos,
                       std::move(onSubscribePublish),
                       std::move(onSubscribeComplete)) != 0;
        }

        bool IotShadowClient::SubscribeToUpdateNamedShadowRejected(
            const Aws::Iotshadow::UpdateNamedShadowSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToUpdateNamedShadowRejectedResponse &handler,
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
                               << "name"
                               << "/" << *request.ShadowName << "/"
                               << "update"
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

        bool IotShadowClient::PublishDeleteNamedShadow(
            const Aws::Iotshadow::DeleteNamedShadowRequest &request,
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
                             << "name"
                             << "/" << *request.ShadowName << "/"
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

        bool IotShadowClient::PublishGetNamedShadow(
            const Aws::Iotshadow::GetNamedShadowRequest &request,
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
                             << "name"
                             << "/" << *request.ShadowName << "/"
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

        bool IotShadowClient::PublishUpdateNamedShadow(
            const Aws::Iotshadow::UpdateNamedShadowRequest &request,
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
                             << "name"
                             << "/" << *request.ShadowName << "/"
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
