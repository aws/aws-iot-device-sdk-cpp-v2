#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/iotshadow/Exports.h>

#include <aws/crt/StlAllocator.h>
#include <aws/crt/Types.h>

#include <aws/crt/mqtt/MqttClient.h>

namespace Aws
{
    namespace Iotshadow
    {

        class DeleteNamedShadowRequest;
        class DeleteNamedShadowSubscriptionRequest;
        class DeleteShadowRequest;
        class DeleteShadowResponse;
        class DeleteShadowSubscriptionRequest;
        class ErrorResponse;
        class GetNamedShadowRequest;
        class GetNamedShadowSubscriptionRequest;
        class GetShadowRequest;
        class GetShadowResponse;
        class GetShadowSubscriptionRequest;
        class NamedShadowDeltaUpdatedSubscriptionRequest;
        class NamedShadowUpdatedSubscriptionRequest;
        class ShadowDeltaUpdatedEvent;
        class ShadowDeltaUpdatedSubscriptionRequest;
        class ShadowUpdatedEvent;
        class ShadowUpdatedSubscriptionRequest;
        class UpdateNamedShadowRequest;
        class UpdateNamedShadowSubscriptionRequest;
        class UpdateShadowRequest;
        class UpdateShadowResponse;
        class UpdateShadowSubscriptionRequest;

        using OnSubscribeComplete = std::function<void(int ioErr)>;
        using OnPublishComplete = std::function<void(int ioErr)>;

        using OnSubscribeToDeleteNamedShadowRejectedResponse =
            std::function<void(Aws::Iotshadow::ErrorResponse *, int ioErr)>;

        using OnSubscribeToGetNamedShadowAcceptedResponse =
            std::function<void(Aws::Iotshadow::GetShadowResponse *, int ioErr)>;

        using OnSubscribeToShadowDeltaUpdatedEventsResponse =
            std::function<void(Aws::Iotshadow::ShadowDeltaUpdatedEvent *, int ioErr)>;

        using OnSubscribeToDeleteShadowAcceptedResponse =
            std::function<void(Aws::Iotshadow::DeleteShadowResponse *, int ioErr)>;

        using OnSubscribeToUpdateNamedShadowAcceptedResponse =
            std::function<void(Aws::Iotshadow::UpdateShadowResponse *, int ioErr)>;

        using OnSubscribeToDeleteNamedShadowAcceptedResponse =
            std::function<void(Aws::Iotshadow::DeleteShadowResponse *, int ioErr)>;

        using OnSubscribeToUpdateShadowAcceptedResponse =
            std::function<void(Aws::Iotshadow::UpdateShadowResponse *, int ioErr)>;

        using OnSubscribeToUpdateShadowRejectedResponse =
            std::function<void(Aws::Iotshadow::ErrorResponse *, int ioErr)>;

        using OnSubscribeToDeleteShadowRejectedResponse =
            std::function<void(Aws::Iotshadow::ErrorResponse *, int ioErr)>;

        using OnSubscribeToUpdateNamedShadowRejectedResponse =
            std::function<void(Aws::Iotshadow::ErrorResponse *, int ioErr)>;

        using OnSubscribeToNamedShadowUpdatedEventsResponse =
            std::function<void(Aws::Iotshadow::ShadowUpdatedEvent *, int ioErr)>;

        using OnSubscribeToGetShadowAcceptedResponse =
            std::function<void(Aws::Iotshadow::GetShadowResponse *, int ioErr)>;

        using OnSubscribeToShadowUpdatedEventsResponse =
            std::function<void(Aws::Iotshadow::ShadowUpdatedEvent *, int ioErr)>;

        using OnSubscribeToNamedShadowDeltaUpdatedEventsResponse =
            std::function<void(Aws::Iotshadow::ShadowDeltaUpdatedEvent *, int ioErr)>;

        using OnSubscribeToGetNamedShadowRejectedResponse =
            std::function<void(Aws::Iotshadow::ErrorResponse *, int ioErr)>;

        using OnSubscribeToGetShadowRejectedResponse = std::function<void(Aws::Iotshadow::ErrorResponse *, int ioErr)>;

        class AWS_IOTSHADOW_API IotShadowClient final
        {
          public:
            IotShadowClient(const std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> &connection);

            operator bool() const noexcept;
            int GetLastError() const noexcept;

            bool SubscribeToDeleteNamedShadowRejected(
                const Aws::Iotshadow::DeleteNamedShadowSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToDeleteNamedShadowRejectedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToGetNamedShadowAccepted(
                const Aws::Iotshadow::GetNamedShadowSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToGetNamedShadowAcceptedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToShadowDeltaUpdatedEvents(
                const Aws::Iotshadow::ShadowDeltaUpdatedSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToShadowDeltaUpdatedEventsResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToDeleteShadowAccepted(
                const Aws::Iotshadow::DeleteShadowSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToDeleteShadowAcceptedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToUpdateNamedShadowAccepted(
                const Aws::Iotshadow::UpdateNamedShadowSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToUpdateNamedShadowAcceptedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToDeleteNamedShadowAccepted(
                const Aws::Iotshadow::DeleteNamedShadowSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToDeleteNamedShadowAcceptedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToUpdateShadowAccepted(
                const Aws::Iotshadow::UpdateShadowSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToUpdateShadowAcceptedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToUpdateShadowRejected(
                const Aws::Iotshadow::UpdateShadowSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToUpdateShadowRejectedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToDeleteShadowRejected(
                const Aws::Iotshadow::DeleteShadowSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToDeleteShadowRejectedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToUpdateNamedShadowRejected(
                const Aws::Iotshadow::UpdateNamedShadowSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToUpdateNamedShadowRejectedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToNamedShadowUpdatedEvents(
                const Aws::Iotshadow::NamedShadowUpdatedSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToNamedShadowUpdatedEventsResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToGetShadowAccepted(
                const Aws::Iotshadow::GetShadowSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToGetShadowAcceptedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToShadowUpdatedEvents(
                const Aws::Iotshadow::ShadowUpdatedSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToShadowUpdatedEventsResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToNamedShadowDeltaUpdatedEvents(
                const Aws::Iotshadow::NamedShadowDeltaUpdatedSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToNamedShadowDeltaUpdatedEventsResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToGetNamedShadowRejected(
                const Aws::Iotshadow::GetNamedShadowSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToGetNamedShadowRejectedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToGetShadowRejected(
                const Aws::Iotshadow::GetShadowSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToGetShadowRejectedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool PublishGetShadow(
                const Aws::Iotshadow::GetShadowRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnPublishComplete &onPubAck);

            bool PublishDeleteShadow(
                const Aws::Iotshadow::DeleteShadowRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnPublishComplete &onPubAck);

            bool PublishUpdateShadow(
                const Aws::Iotshadow::UpdateShadowRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnPublishComplete &onPubAck);

            bool PublishDeleteNamedShadow(
                const Aws::Iotshadow::DeleteNamedShadowRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnPublishComplete &onPubAck);

            bool PublishGetNamedShadow(
                const Aws::Iotshadow::GetNamedShadowRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnPublishComplete &onPubAck);

            bool PublishUpdateNamedShadow(
                const Aws::Iotshadow::UpdateNamedShadowRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnPublishComplete &onPubAck);

          private:
            std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> m_connection;
        };

    } // namespace Iotshadow

} // namespace Aws
