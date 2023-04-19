/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotjobs/IotJobsClient.h>

#include <aws/iotjobs/DescribeJobExecutionRequest.h>
#include <aws/iotjobs/DescribeJobExecutionResponse.h>
#include <aws/iotjobs/DescribeJobExecutionSubscriptionRequest.h>
#include <aws/iotjobs/GetPendingJobExecutionsRequest.h>
#include <aws/iotjobs/GetPendingJobExecutionsResponse.h>
#include <aws/iotjobs/GetPendingJobExecutionsSubscriptionRequest.h>
#include <aws/iotjobs/JobExecutionsChangedEvent.h>
#include <aws/iotjobs/JobExecutionsChangedSubscriptionRequest.h>
#include <aws/iotjobs/NextJobExecutionChangedEvent.h>
#include <aws/iotjobs/NextJobExecutionChangedSubscriptionRequest.h>
#include <aws/iotjobs/RejectedError.h>
#include <aws/iotjobs/StartNextJobExecutionResponse.h>
#include <aws/iotjobs/StartNextPendingJobExecutionRequest.h>
#include <aws/iotjobs/StartNextPendingJobExecutionSubscriptionRequest.h>
#include <aws/iotjobs/UpdateJobExecutionRequest.h>
#include <aws/iotjobs/UpdateJobExecutionResponse.h>
#include <aws/iotjobs/UpdateJobExecutionSubscriptionRequest.h>

#include <aws/crt/mqtt/Mqtt5Packets.h>

namespace Aws
{
    namespace Iotjobs
    {

        IotJobsClient::IotJobsClient(const std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> &connection)
            : m_connection(connection)
        {
        }

        IotJobsClient::IotJobsClient(const std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> &mqtt5Client)
            : m_mqtt5Client(mqtt5Client)
        {
            Crt::Mqtt5::Mqtt5ListenerOptions listenerOptions;
            m_mqtt5Listener =
                Crt::Mqtt5::Mqtt5Listener::NewMqtt5Listener(listenerOptions, mqtt5Client, Aws::Crt::g_allocator);
        }

        IotJobsClient::operator bool() const noexcept { return (m_connection && *m_connection) || m_mqtt5Listener; }

        int IotJobsClient::GetLastError() const noexcept { return aws_last_error(); }

        bool IotJobsClient::SubscribeToUpdateJobExecutionAccepted(
            const Aws::Iotjobs::UpdateJobExecutionSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToUpdateJobExecutionAcceptedResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            (void)request;
            if (m_connection == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt3 Service Client is not setup. Are you looking for Mqtt5 API?");
                return false;
            }
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
                    Aws::Iotjobs::UpdateJobExecutionResponse response(jsonObject);
                    handler(&response, AWS_ERROR_SUCCESS);
                };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "jobs"
                               << "/" << *request.JobId << "/"
                               << "update"
                               << "/"
                               << "accepted";

            return m_connection->Subscribe(
                       subscribeTopicSStr.str().c_str(),
                       qos,
                       std::move(onSubscribePublish),
                       std::move(onSubscribeComplete)) != 0;
        }

        bool IotJobsClient::SubscribeToUpdateJobExecutionAccepted(
            const Aws::Iotjobs::UpdateJobExecutionSubscriptionRequest &request,
            Aws::Crt::Mqtt5::QOS qos,
            const OnSubscribeToUpdateJobExecutionAcceptedResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            if (m_mqtt5Listener == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt5 Service Client is not setup. Are you looking for Mqtt3 API?");
                return false;
            }

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "jobs"
                               << "/" << *request.JobId << "/"
                               << "update"
                               << "/"
                               << "accepted";

            auto onSubscribeComplete = [handler, onSubAck](int errorCode, std::shared_ptr<Crt::Mqtt5::SubAckPacket>) {
                if (errorCode)
                {
                    handler(nullptr, errorCode);
                }

                if (onSubAck)
                {
                    onSubAck(errorCode);
                }
            };

            auto onPublishReceived = [handler](const Crt::Mqtt5::PublishReceivedEventData &data) {
                Aws::Crt::ByteCursor payload_cursor = data.publishPacket->getPayload();
                Aws::Crt::String objectStr(reinterpret_cast<char *>(payload_cursor.ptr), payload_cursor.len);
                Aws::Crt::JsonObject jsonObject(objectStr);
                Aws::Iotjobs::UpdateJobExecutionResponse response(jsonObject);
                handler(&response, AWS_ERROR_SUCCESS);
            };

            m_mqtt5Listener->Subscribe(subscribeTopicSStr.str(), onPublishReceived);

            Crt::Mqtt5::Subscription subscription(subscribeTopicSStr.str(), qos, Aws::Crt::g_allocator);
            std::shared_ptr<Crt::Mqtt5::SubscribePacket> subscribe =
                std::make_shared<Crt::Mqtt5::SubscribePacket>(Aws::Crt::g_allocator);
            subscribe->WithSubscription(std::move(subscription));

            return m_mqtt5Client->Subscribe(subscribe, std::move(onSubscribeComplete));
        }

        bool IotJobsClient::SubscribeToGetPendingJobExecutionsRejected(
            const Aws::Iotjobs::GetPendingJobExecutionsSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToGetPendingJobExecutionsRejectedResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            if (m_connection == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt3 Service Client is not setup. Are you looking for Mqtt5 API?");
                return false;
            }
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
                    Aws::Iotjobs::RejectedError response(jsonObject);
                    handler(&response, AWS_ERROR_SUCCESS);
                };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "jobs"
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

        bool IotJobsClient::SubscribeToGetPendingJobExecutionsRejected(
            const Aws::Iotjobs::GetPendingJobExecutionsSubscriptionRequest &request,
            Aws::Crt::Mqtt5::QOS qos,
            const OnSubscribeToGetPendingJobExecutionsRejectedResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            (void)request;

            if (m_mqtt5Listener == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt5 Service Client is not setup. Are you looking for Mqtt3 API?");
                return false;
            }

            auto onSubscribeComplete = [handler, onSubAck](int errorCode, std::shared_ptr<Crt::Mqtt5::SubAckPacket>) {
                if (errorCode)
                {
                    handler(nullptr, errorCode);
                }

                if (onSubAck)
                {
                    onSubAck(errorCode);
                }
            };

            auto onPublishReceived = [handler](const Crt::Mqtt5::PublishReceivedEventData &data) {
                Aws::Crt::ByteCursor payload_cursor = data.publishPacket->getPayload();
                Aws::Crt::String objectStr(reinterpret_cast<char *>(payload_cursor.ptr), payload_cursor.len);
                Aws::Crt::JsonObject jsonObject(objectStr);
                Aws::Iotjobs::RejectedError response(jsonObject);
                handler(&response, AWS_ERROR_SUCCESS);
            };
            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "jobs"
                               << "/"
                               << "get"
                               << "/"
                               << "rejected";

            m_mqtt5Listener->Subscribe(subscribeTopicSStr.str(), onPublishReceived);

            Crt::Mqtt5::Subscription subscription(subscribeTopicSStr.str(), qos, Aws::Crt::g_allocator);
            std::shared_ptr<Crt::Mqtt5::SubscribePacket> subscribe =
                std::make_shared<Crt::Mqtt5::SubscribePacket>(Aws::Crt::g_allocator);
            subscribe->WithSubscription(std::move(subscription));

            return m_mqtt5Client->Subscribe(subscribe, std::move(onSubscribeComplete));
        }

        bool IotJobsClient::SubscribeToDescribeJobExecutionAccepted(
            const Aws::Iotjobs::DescribeJobExecutionSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToDescribeJobExecutionAcceptedResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            (void)request;
            if (m_connection == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt3 Service Client is not setup. Are you looking for Mqtt5 API?");
                return false;
            }
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
                    Aws::Iotjobs::DescribeJobExecutionResponse response(jsonObject);
                    handler(&response, AWS_ERROR_SUCCESS);
                };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "jobs"
                               << "/" << *request.JobId << "/"
                               << "get"
                               << "/"
                               << "accepted";

            return m_connection->Subscribe(
                       subscribeTopicSStr.str().c_str(),
                       qos,
                       std::move(onSubscribePublish),
                       std::move(onSubscribeComplete)) != 0;
        }

        bool IotJobsClient::SubscribeToDescribeJobExecutionAccepted(
            const Aws::Iotjobs::DescribeJobExecutionSubscriptionRequest &request,
            Aws::Crt::Mqtt5::QOS qos,
            const OnSubscribeToDescribeJobExecutionAcceptedResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            (void)request;
            if (m_mqtt5Listener == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt5 Service Client is not setup. Are you looking for Mqtt3 API?");
                return false;
            }
            auto onSubscribeComplete = [handler, onSubAck](int errorCode, std::shared_ptr<Crt::Mqtt5::SubAckPacket>) {
                if (errorCode)
                {
                    handler(nullptr, errorCode);
                }

                if (onSubAck)
                {
                    onSubAck(errorCode);
                }
            };

            auto onPublishReceived = [handler](const Crt::Mqtt5::PublishReceivedEventData &data) {
                Aws::Crt::ByteCursor payload_cursor = data.publishPacket->getPayload();
                Aws::Crt::String objectStr(reinterpret_cast<char *>(payload_cursor.ptr), payload_cursor.len);
                Aws::Crt::JsonObject jsonObject(objectStr);
                Aws::Iotjobs::DescribeJobExecutionResponse response(jsonObject);
                handler(&response, AWS_ERROR_SUCCESS);
            };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "jobs"
                               << "/" << *request.JobId << "/"
                               << "get"
                               << "/"
                               << "accepted";

            m_mqtt5Listener->Subscribe(subscribeTopicSStr.str(), onPublishReceived);

            Crt::Mqtt5::Subscription subscription(subscribeTopicSStr.str(), qos, Aws::Crt::g_allocator);
            std::shared_ptr<Crt::Mqtt5::SubscribePacket> subscribe =
                std::make_shared<Crt::Mqtt5::SubscribePacket>(Aws::Crt::g_allocator);
            subscribe->WithSubscription(std::move(subscription));

            return m_mqtt5Client->Subscribe(subscribe, std::move(onSubscribeComplete));
        }

        bool IotJobsClient::SubscribeToDescribeJobExecutionRejected(
            const Aws::Iotjobs::DescribeJobExecutionSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToDescribeJobExecutionRejectedResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            if (m_connection == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt3 Service Client is not setup. Are you looking for Mqtt5 API?");
                return false;
            }
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
                    Aws::Iotjobs::RejectedError response(jsonObject);
                    handler(&response, AWS_ERROR_SUCCESS);
                };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "jobs"
                               << "/" << *request.JobId << "/"
                               << "get"
                               << "/"
                               << "rejected";

            return m_connection->Subscribe(
                       subscribeTopicSStr.str().c_str(),
                       qos,
                       std::move(onSubscribePublish),
                       std::move(onSubscribeComplete)) != 0;
        }

        bool IotJobsClient::SubscribeToDescribeJobExecutionRejected(
            const Aws::Iotjobs::DescribeJobExecutionSubscriptionRequest &request,
            Aws::Crt::Mqtt5::QOS qos,
            const OnSubscribeToDescribeJobExecutionRejectedResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            if (m_mqtt5Listener == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt5 Service Client is not setup. Are you looking for Mqtt3 API?");
                return false;
            }

            (void)request;
            auto onSubscribeComplete = [handler, onSubAck](int errorCode, std::shared_ptr<Crt::Mqtt5::SubAckPacket>) {
                if (errorCode)
                {
                    handler(nullptr, errorCode);
                }

                if (onSubAck)
                {
                    onSubAck(errorCode);
                }
            };

            auto onPublishReceived = [handler](const Crt::Mqtt5::PublishReceivedEventData &data) {
                Aws::Crt::ByteCursor payload_cursor = data.publishPacket->getPayload();
                Aws::Crt::String objectStr(reinterpret_cast<char *>(payload_cursor.ptr), payload_cursor.len);
                Aws::Crt::JsonObject jsonObject(objectStr);
                Aws::Iotjobs::RejectedError response(jsonObject);
                handler(&response, AWS_ERROR_SUCCESS);
            };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "jobs"
                               << "/" << *request.JobId << "/"
                               << "get"
                               << "/"
                               << "rejected";

            m_mqtt5Listener->Subscribe(subscribeTopicSStr.str(), onPublishReceived);

            Crt::Mqtt5::Subscription subscription(subscribeTopicSStr.str(), qos, Aws::Crt::g_allocator);
            std::shared_ptr<Crt::Mqtt5::SubscribePacket> subscribe =
                std::make_shared<Crt::Mqtt5::SubscribePacket>(Aws::Crt::g_allocator);
            subscribe->WithSubscription(std::move(subscription));

            return m_mqtt5Client->Subscribe(subscribe, std::move(onSubscribeComplete));
        }

        bool IotJobsClient::SubscribeToUpdateJobExecutionRejected(
            const Aws::Iotjobs::UpdateJobExecutionSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToUpdateJobExecutionRejectedResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            if (m_connection == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt3 Service Client is not setup. Are you looking for Mqtt5 API?");
                return false;
            }
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
                    Aws::Iotjobs::RejectedError response(jsonObject);
                    handler(&response, AWS_ERROR_SUCCESS);
                };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "jobs"
                               << "/" << *request.JobId << "/"
                               << "update"
                               << "/"
                               << "rejected";

            return m_connection->Subscribe(
                       subscribeTopicSStr.str().c_str(),
                       qos,
                       std::move(onSubscribePublish),
                       std::move(onSubscribeComplete)) != 0;
        }

        bool IotJobsClient::SubscribeToUpdateJobExecutionRejected(
            const Aws::Iotjobs::UpdateJobExecutionSubscriptionRequest &request,
            Aws::Crt::Mqtt5::QOS qos,
            const OnSubscribeToUpdateJobExecutionRejectedResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            if (m_mqtt5Listener == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt5 Service Client is not setup. Are you looking for Mqtt3 API?");
                return false;
            }
            (void)request;
            auto onSubscribeComplete = [handler, onSubAck](int errorCode, std::shared_ptr<Crt::Mqtt5::SubAckPacket>) {
                if (errorCode)
                {
                    handler(nullptr, errorCode);
                }

                if (onSubAck)
                {
                    onSubAck(errorCode);
                }
            };

            auto onPublishReceived = [handler](const Crt::Mqtt5::PublishReceivedEventData &data) {
                Aws::Crt::ByteCursor payload_cursor = data.publishPacket->getPayload();
                Aws::Crt::String objectStr(reinterpret_cast<char *>(payload_cursor.ptr), payload_cursor.len);
                Aws::Crt::JsonObject jsonObject(objectStr);
                Aws::Iotjobs::RejectedError response(jsonObject);
                handler(&response, AWS_ERROR_SUCCESS);
            };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "jobs"
                               << "/" << *request.JobId << "/"
                               << "update"
                               << "/"
                               << "rejected";

            m_mqtt5Listener->Subscribe(subscribeTopicSStr.str(), onPublishReceived);

            Crt::Mqtt5::Subscription subscription(subscribeTopicSStr.str(), qos, Aws::Crt::g_allocator);
            std::shared_ptr<Crt::Mqtt5::SubscribePacket> subscribe =
                std::make_shared<Crt::Mqtt5::SubscribePacket>(Aws::Crt::g_allocator);
            subscribe->WithSubscription(std::move(subscription));

            return m_mqtt5Client->Subscribe(subscribe, std::move(onSubscribeComplete));
        }

        bool IotJobsClient::SubscribeToJobExecutionsChangedEvents(
            const Aws::Iotjobs::JobExecutionsChangedSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToJobExecutionsChangedEventsResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            if (m_connection == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt3 Service Client is not setup. Are you looking for Mqtt5 API?");
                return false;
            }
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
                    Aws::Iotjobs::JobExecutionsChangedEvent response(jsonObject);
                    handler(&response, AWS_ERROR_SUCCESS);
                };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "jobs"
                               << "/"
                               << "notify";

            return m_connection->Subscribe(
                       subscribeTopicSStr.str().c_str(),
                       qos,
                       std::move(onSubscribePublish),
                       std::move(onSubscribeComplete)) != 0;
        }

        bool IotJobsClient::SubscribeToJobExecutionsChangedEvents(
            const Aws::Iotjobs::JobExecutionsChangedSubscriptionRequest &request,
            Aws::Crt::Mqtt5::QOS qos,
            const OnSubscribeToJobExecutionsChangedEventsResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            if (m_mqtt5Listener == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt5 Service Client is not setup. Are you looking for Mqtt3 API?");
                return false;
            }
            (void)request;
            auto onSubscribeComplete = [handler, onSubAck](int errorCode, std::shared_ptr<Crt::Mqtt5::SubAckPacket>) {
                if (errorCode)
                {
                    handler(nullptr, errorCode);
                }

                if (onSubAck)
                {
                    onSubAck(errorCode);
                }
            };

            auto onPublishReceived = [handler](const Crt::Mqtt5::PublishReceivedEventData &data) {
                Aws::Crt::ByteCursor payload_cursor = data.publishPacket->getPayload();
                Aws::Crt::String objectStr(reinterpret_cast<char *>(payload_cursor.ptr), payload_cursor.len);
                Aws::Crt::JsonObject jsonObject(objectStr);
                Aws::Iotjobs::JobExecutionsChangedEvent response(jsonObject);
                handler(&response, AWS_ERROR_SUCCESS);
            };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "jobs"
                               << "/"
                               << "notify";

            m_mqtt5Listener->Subscribe(subscribeTopicSStr.str(), onPublishReceived);

            Crt::Mqtt5::Subscription subscription(subscribeTopicSStr.str(), qos, Aws::Crt::g_allocator);
            std::shared_ptr<Crt::Mqtt5::SubscribePacket> subscribe =
                std::make_shared<Crt::Mqtt5::SubscribePacket>(Aws::Crt::g_allocator);
            subscribe->WithSubscription(std::move(subscription));

            return m_mqtt5Client->Subscribe(subscribe, std::move(onSubscribeComplete));
        }

        bool IotJobsClient::SubscribeToStartNextPendingJobExecutionRejected(
            const Aws::Iotjobs::StartNextPendingJobExecutionSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToStartNextPendingJobExecutionRejectedResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            if (m_connection == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt3 Service Client is not setup. Are you looking for Mqtt5 API?");
                return false;
            }
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
                    Aws::Iotjobs::RejectedError response(jsonObject);
                    handler(&response, AWS_ERROR_SUCCESS);
                };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "jobs"
                               << "/"
                               << "start-next"
                               << "/"
                               << "rejected";

            return m_connection->Subscribe(
                       subscribeTopicSStr.str().c_str(),
                       qos,
                       std::move(onSubscribePublish),
                       std::move(onSubscribeComplete)) != 0;
        }

        bool IotJobsClient::SubscribeToStartNextPendingJobExecutionRejected(
            const Aws::Iotjobs::StartNextPendingJobExecutionSubscriptionRequest &request,
            Aws::Crt::Mqtt5::QOS qos,
            const OnSubscribeToStartNextPendingJobExecutionRejectedResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            (void)request;
            if (m_mqtt5Listener == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt5 Service Client is not setup. Are you looking for Mqtt3 API?");
                return false;
            }
            auto onSubscribeComplete = [handler, onSubAck](int errorCode, std::shared_ptr<Crt::Mqtt5::SubAckPacket>) {
                if (errorCode)
                {
                    handler(nullptr, errorCode);
                }

                if (onSubAck)
                {
                    onSubAck(errorCode);
                }
            };

            auto onPublishReceived = [handler](const Crt::Mqtt5::PublishReceivedEventData &data) {
                Aws::Crt::ByteCursor payload_cursor = data.publishPacket->getPayload();
                Aws::Crt::String objectStr(reinterpret_cast<char *>(payload_cursor.ptr), payload_cursor.len);
                Aws::Crt::JsonObject jsonObject(objectStr);
                Aws::Iotjobs::RejectedError response(jsonObject);
                handler(&response, AWS_ERROR_SUCCESS);
            };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "jobs"
                               << "/"
                               << "start-next"
                               << "/"
                               << "rejected";

            m_mqtt5Listener->Subscribe(subscribeTopicSStr.str(), onPublishReceived);

            Crt::Mqtt5::Subscription subscription(subscribeTopicSStr.str(), qos, Aws::Crt::g_allocator);
            std::shared_ptr<Crt::Mqtt5::SubscribePacket> subscribe =
                std::make_shared<Crt::Mqtt5::SubscribePacket>(Aws::Crt::g_allocator);
            subscribe->WithSubscription(std::move(subscription));

            return m_mqtt5Client->Subscribe(subscribe, std::move(onSubscribeComplete));
        }

        bool IotJobsClient::SubscribeToNextJobExecutionChangedEvents(
            const Aws::Iotjobs::NextJobExecutionChangedSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToNextJobExecutionChangedEventsResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            if (m_connection == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt3 Service Client is not setup. Are you looking for Mqtt5 API?");
                return false;
            }
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
                    Aws::Iotjobs::NextJobExecutionChangedEvent response(jsonObject);
                    handler(&response, AWS_ERROR_SUCCESS);
                };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "jobs"
                               << "/"
                               << "notify-next";

            return m_connection->Subscribe(
                       subscribeTopicSStr.str().c_str(),
                       qos,
                       std::move(onSubscribePublish),
                       std::move(onSubscribeComplete)) != 0;
        }

        bool IotJobsClient::SubscribeToNextJobExecutionChangedEvents(
            const Aws::Iotjobs::NextJobExecutionChangedSubscriptionRequest &request,
            Aws::Crt::Mqtt5::QOS qos,
            const OnSubscribeToNextJobExecutionChangedEventsResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            if (m_mqtt5Listener == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt5 Service Client is not setup. Are you looking for Mqtt3 API?");
                return false;
            }
            (void)request;
            auto onSubscribeComplete = [handler, onSubAck](int errorCode, std::shared_ptr<Crt::Mqtt5::SubAckPacket>) {
                if (errorCode)
                {
                    handler(nullptr, errorCode);
                }

                if (onSubAck)
                {
                    onSubAck(errorCode);
                }
            };

            auto onPublishReceived = [handler](const Crt::Mqtt5::PublishReceivedEventData &data) {
                Aws::Crt::ByteCursor payload_cursor = data.publishPacket->getPayload();
                Aws::Crt::String objectStr(reinterpret_cast<char *>(payload_cursor.ptr), payload_cursor.len);
                Aws::Crt::JsonObject jsonObject(objectStr);
                Aws::Iotjobs::NextJobExecutionChangedEvent response(jsonObject);
                handler(&response, AWS_ERROR_SUCCESS);
            };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "jobs"
                               << "/"
                               << "notify-next";

            m_mqtt5Listener->Subscribe(subscribeTopicSStr.str(), onPublishReceived);

            Crt::Mqtt5::Subscription subscription(subscribeTopicSStr.str(), qos, Aws::Crt::g_allocator);
            std::shared_ptr<Crt::Mqtt5::SubscribePacket> subscribe =
                std::make_shared<Crt::Mqtt5::SubscribePacket>(Aws::Crt::g_allocator);
            subscribe->WithSubscription(std::move(subscription));

            return m_mqtt5Client->Subscribe(subscribe, std::move(onSubscribeComplete));
        }

        bool IotJobsClient::SubscribeToGetPendingJobExecutionsAccepted(
            const Aws::Iotjobs::GetPendingJobExecutionsSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToGetPendingJobExecutionsAcceptedResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            if (m_connection == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt3 Service Client is not setup. Are you looking for Mqtt5 API?");
                return false;
            }
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
                    Aws::Iotjobs::GetPendingJobExecutionsResponse response(jsonObject);
                    handler(&response, AWS_ERROR_SUCCESS);
                };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "jobs"
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

        bool IotJobsClient::SubscribeToGetPendingJobExecutionsAccepted(
            const Aws::Iotjobs::GetPendingJobExecutionsSubscriptionRequest &request,
            Aws::Crt::Mqtt5::QOS qos,
            const OnSubscribeToGetPendingJobExecutionsAcceptedResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            (void)request;
            if (m_mqtt5Listener == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt5 Service Client is not setup. Are you looking for Mqtt3 API?");
                return false;
            }
            auto onSubscribeComplete = [handler, onSubAck](int errorCode, std::shared_ptr<Crt::Mqtt5::SubAckPacket>) {
                if (errorCode)
                {
                    handler(nullptr, errorCode);
                }

                if (onSubAck)
                {
                    onSubAck(errorCode);
                }
            };

            auto onPublishReceived = [handler](const Crt::Mqtt5::PublishReceivedEventData &data) {
                Aws::Crt::ByteCursor payload_cursor = data.publishPacket->getPayload();
                Aws::Crt::String objectStr(reinterpret_cast<char *>(payload_cursor.ptr), payload_cursor.len);
                Aws::Crt::JsonObject jsonObject(objectStr);
                Aws::Iotjobs::GetPendingJobExecutionsResponse response(jsonObject);
                handler(&response, AWS_ERROR_SUCCESS);
            };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "jobs"
                               << "/"
                               << "get"
                               << "/"
                               << "accepted";

            m_mqtt5Listener->Subscribe(subscribeTopicSStr.str(), onPublishReceived);

            Crt::Mqtt5::Subscription subscription(subscribeTopicSStr.str(), qos, Aws::Crt::g_allocator);
            std::shared_ptr<Crt::Mqtt5::SubscribePacket> subscribe =
                std::make_shared<Crt::Mqtt5::SubscribePacket>(Aws::Crt::g_allocator);
            subscribe->WithSubscription(std::move(subscription));

            return m_mqtt5Client->Subscribe(subscribe, std::move(onSubscribeComplete));
        }

        bool IotJobsClient::SubscribeToStartNextPendingJobExecutionAccepted(
            const Aws::Iotjobs::StartNextPendingJobExecutionSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToStartNextPendingJobExecutionAcceptedResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            if (m_connection == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt3 Service Client is not setup. Are you looking for Mqtt5 API?");
                return false;
            }
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
                    Aws::Iotjobs::StartNextJobExecutionResponse response(jsonObject);
                    handler(&response, AWS_ERROR_SUCCESS);
                };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "jobs"
                               << "/"
                               << "start-next"
                               << "/"
                               << "accepted";

            return m_connection->Subscribe(
                       subscribeTopicSStr.str().c_str(),
                       qos,
                       std::move(onSubscribePublish),
                       std::move(onSubscribeComplete)) != 0;
        }

        bool IotJobsClient::SubscribeToStartNextPendingJobExecutionAccepted(
            const Aws::Iotjobs::StartNextPendingJobExecutionSubscriptionRequest &request,
            Aws::Crt::Mqtt5::QOS qos,
            const OnSubscribeToStartNextPendingJobExecutionAcceptedResponse &handler,
            const OnSubscribeComplete &onSubAck)
        {
            (void)request;
            if (m_mqtt5Listener == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt5 Service Client is not setup. Are you looking for Mqtt3 API?");
                return false;
            }

            auto onSubscribeComplete = [handler, onSubAck](int errorCode, std::shared_ptr<Crt::Mqtt5::SubAckPacket>) {
                if (errorCode)
                {
                    handler(nullptr, errorCode);
                }

                if (onSubAck)
                {
                    onSubAck(errorCode);
                }
            };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "jobs"
                               << "/"
                               << "start-next"
                               << "/"
                               << "accepted";

            auto onPublishReceived = [handler](const Crt::Mqtt5::PublishReceivedEventData &data) {
                Aws::Crt::ByteCursor payload_cursor = data.publishPacket->getPayload();
                Aws::Crt::String objectStr(reinterpret_cast<char *>(payload_cursor.ptr), payload_cursor.len);
                Aws::Crt::JsonObject jsonObject(objectStr);
                Aws::Iotjobs::StartNextJobExecutionResponse response(jsonObject);
                handler(&response, AWS_ERROR_SUCCESS);
            };

            m_mqtt5Listener->Subscribe(subscribeTopicSStr.str(), onPublishReceived);

            Crt::Mqtt5::Subscription subscription(subscribeTopicSStr.str(), qos, Aws::Crt::g_allocator);
            std::shared_ptr<Crt::Mqtt5::SubscribePacket> subscribe =
                std::make_shared<Crt::Mqtt5::SubscribePacket>(Aws::Crt::g_allocator);
            subscribe->WithSubscription(std::move(subscription));

            return m_mqtt5Client->Subscribe(subscribe, std::move(onSubscribeComplete));
        }

        bool IotJobsClient::PublishDescribeJobExecution(
            const Aws::Iotjobs::DescribeJobExecutionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnPublishComplete &onPubAck)
        {
            if (m_connection == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt3 Service Client is not setup. Are you looking for Mqtt5 API?");
                return false;
            }
            Aws::Crt::StringStream publishTopicSStr;
            publishTopicSStr << "$aws"
                             << "/"
                             << "things"
                             << "/" << *request.ThingName << "/"
                             << "jobs"
                             << "/" << *request.JobId << "/"
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

        bool IotJobsClient::PublishDescribeJobExecution(
            const Aws::Iotjobs::DescribeJobExecutionRequest &request,
            Aws::Crt::Mqtt5::QOS qos,
            const OnPublishComplete &onPubAck)
        {
            if (m_mqtt5Listener == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt5 Service Client is not setup. Are you looking for Mqtt3 API?");
                return false;
            }
            Aws::Crt::StringStream publishTopicSStr;
            publishTopicSStr << "$aws"
                             << "/"
                             << "things"
                             << "/" << *request.ThingName << "/"
                             << "jobs"
                             << "/" << *request.JobId << "/"
                             << "get";

            Aws::Crt::JsonObject jsonObject;
            request.SerializeToObject(jsonObject);
            Aws::Crt::String outgoingJson = jsonObject.View().WriteCompact(true);
            Aws::Crt::ByteBuf buf = Aws::Crt::ByteBufNewCopy(
                Aws::Crt::g_allocator, reinterpret_cast<const uint8_t *>(outgoingJson.data()), outgoingJson.length());

            auto onPublishComplete = [buf, onPubAck](int errorCode, std::shared_ptr<Crt::Mqtt5::PublishResult>) {
                onPubAck(errorCode);
                Aws::Crt::ByteBufDelete(const_cast<Aws::Crt::ByteBuf &>(buf));
            };

            std::shared_ptr<Crt::Mqtt5::PublishPacket> publish = std::make_shared<Crt::Mqtt5::PublishPacket>(
                publishTopicSStr.str(), Crt::ByteCursorFromByteBuf(buf), qos, Aws::Crt::g_allocator);

            return m_mqtt5Client->Publish(publish, std::move(onPublishComplete));
        }

        bool IotJobsClient::PublishGetPendingJobExecutions(
            const Aws::Iotjobs::GetPendingJobExecutionsRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnPublishComplete &onPubAck)
        {
            if (m_connection == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt3 Service Client is not setup. Are you looking for Mqtt5 API?");
                return false;
            }
            Aws::Crt::StringStream publishTopicSStr;
            publishTopicSStr << "$aws"
                             << "/"
                             << "things"
                             << "/" << *request.ThingName << "/"
                             << "jobs"
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

        bool IotJobsClient::PublishGetPendingJobExecutions(
            const Aws::Iotjobs::GetPendingJobExecutionsRequest &request,
            Aws::Crt::Mqtt5::QOS qos,
            const OnPublishComplete &onPubAck)
        {
            if (m_mqtt5Listener == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt5 Service Client is not setup. Are you looking for Mqtt3 API?");
                return false;
            }
            Aws::Crt::StringStream publishTopicSStr;
            publishTopicSStr << "$aws"
                             << "/"
                             << "things"
                             << "/" << *request.ThingName << "/"
                             << "jobs"
                             << "/"
                             << "get";

            Aws::Crt::JsonObject jsonObject;
            request.SerializeToObject(jsonObject);
            Aws::Crt::String outgoingJson = jsonObject.View().WriteCompact(true);
            Aws::Crt::ByteBuf buf = Aws::Crt::ByteBufNewCopy(
                Aws::Crt::g_allocator, reinterpret_cast<const uint8_t *>(outgoingJson.data()), outgoingJson.length());

            auto onPublishComplete = [buf, onPubAck](int errorCode, std::shared_ptr<Crt::Mqtt5::PublishResult>) {
                onPubAck(errorCode);
                Aws::Crt::ByteBufDelete(const_cast<Aws::Crt::ByteBuf &>(buf));
            };

            std::shared_ptr<Crt::Mqtt5::PublishPacket> publish = std::make_shared<Crt::Mqtt5::PublishPacket>(
                publishTopicSStr.str(), Crt::ByteCursorFromByteBuf(buf), qos, Aws::Crt::g_allocator);

            return m_mqtt5Client->Publish(publish, std::move(onPublishComplete));
        }

        bool IotJobsClient::PublishUpdateJobExecution(
            const Aws::Iotjobs::UpdateJobExecutionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnPublishComplete &onPubAck)
        {
            if (m_connection == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt3 Service Client is not setup. Are you looking for Mqtt5 API?");
                return false;
            }
            Aws::Crt::StringStream publishTopicSStr;
            publishTopicSStr << "$aws"
                             << "/"
                             << "things"
                             << "/" << *request.ThingName << "/"
                             << "jobs"
                             << "/" << *request.JobId << "/"
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

        bool IotJobsClient::PublishUpdateJobExecution(
            const Aws::Iotjobs::UpdateJobExecutionRequest &request,
            Aws::Crt::Mqtt5::QOS qos,
            const OnPublishComplete &onPubAck)
        {
            if (m_mqtt5Listener == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt5 Service Client is not setup. Are you looking for Mqtt3 API?");
                return false;
            }
            Aws::Crt::StringStream publishTopicSStr;
            publishTopicSStr << "$aws"
                             << "/"
                             << "things"
                             << "/" << *request.ThingName << "/"
                             << "jobs"
                             << "/" << *request.JobId << "/"
                             << "update";

            Aws::Crt::JsonObject jsonObject;
            request.SerializeToObject(jsonObject);
            Aws::Crt::String outgoingJson = jsonObject.View().WriteCompact(true);
            Aws::Crt::ByteBuf buf = Aws::Crt::ByteBufNewCopy(
                Aws::Crt::g_allocator, reinterpret_cast<const uint8_t *>(outgoingJson.data()), outgoingJson.length());

            auto onPublishComplete = [buf, onPubAck](int errorCode, std::shared_ptr<Crt::Mqtt5::PublishResult>) {
                onPubAck(errorCode);
                Aws::Crt::ByteBufDelete(const_cast<Aws::Crt::ByteBuf &>(buf));
            };

            std::shared_ptr<Crt::Mqtt5::PublishPacket> publish = std::make_shared<Crt::Mqtt5::PublishPacket>(
                publishTopicSStr.str(), Crt::ByteCursorFromByteBuf(buf), qos, Aws::Crt::g_allocator);

            return m_mqtt5Client->Publish(publish, std::move(onPublishComplete));
        }

        bool IotJobsClient::PublishStartNextPendingJobExecution(
            const Aws::Iotjobs::StartNextPendingJobExecutionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnPublishComplete &onPubAck)
        {
            if (m_connection == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt3 Service Client is not setup. Are you looking for Mqtt5 API?");
                return false;
            }
            Aws::Crt::StringStream publishTopicSStr;
            publishTopicSStr << "$aws"
                             << "/"
                             << "things"
                             << "/" << *request.ThingName << "/"
                             << "jobs"
                             << "/"
                             << "start-next";

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

        bool IotJobsClient::PublishStartNextPendingJobExecution(
            const Aws::Iotjobs::StartNextPendingJobExecutionRequest &request,
            Aws::Crt::Mqtt5::QOS qos,
            const OnPublishComplete &onPubAck)
        {
            if (m_mqtt5Listener == nullptr)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_MQTT5_GENERAL, "Mqtt5 Service Client is not setup. Are you looking for Mqtt3 API?");
                return false;
            }
            Aws::Crt::StringStream publishTopicSStr;
            publishTopicSStr << "$aws"
                             << "/"
                             << "things"
                             << "/" << *request.ThingName << "/"
                             << "jobs"
                             << "/"
                             << "start-next";

            Aws::Crt::JsonObject jsonObject;
            request.SerializeToObject(jsonObject);
            Aws::Crt::String outgoingJson = jsonObject.View().WriteCompact(true);
            Aws::Crt::ByteBuf buf = Aws::Crt::ByteBufNewCopy(
                Aws::Crt::g_allocator, reinterpret_cast<const uint8_t *>(outgoingJson.data()), outgoingJson.length());

            auto onPublishComplete = [buf, onPubAck](int errorCode, std::shared_ptr<Crt::Mqtt5::PublishResult>) {
                onPubAck(errorCode);
                Aws::Crt::ByteBufDelete(const_cast<Aws::Crt::ByteBuf &>(buf));
            };

            std::shared_ptr<Crt::Mqtt5::PublishPacket> publish = std::make_shared<Crt::Mqtt5::PublishPacket>(
                publishTopicSStr.str(), Crt::ByteCursorFromByteBuf(buf), qos, Aws::Crt::g_allocator);

            return m_mqtt5Client->Publish(publish, std::move(onPublishComplete));
        }

    } // namespace Iotjobs

} // namespace Aws
