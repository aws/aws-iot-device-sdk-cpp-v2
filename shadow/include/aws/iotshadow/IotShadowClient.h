#pragma once
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
#include <aws/iotshadow/Exports.h>

#include <aws/crt/StlAllocator.h>
#include <aws/crt/Types.h>

#include <aws/crt/mqtt/MqttClient.h>

namespace Aws
{
    namespace Iotshadow
    {

        class DeleteShadowRequest;
        class DeleteShadowResponse;
        class DeleteShadowSubscriptionRequest;
        class ErrorResponse;
        class GetShadowRequest;
        class GetShadowResponse;
        class GetShadowSubscriptionRequest;
        class ShadowDeltaUpdatedEvent;
        class ShadowDeltaUpdatedSubscriptionRequest;
        class ShadowUpdatedEvent;
        class ShadowUpdatedSubscriptionRequest;
        class UpdateShadowRequest;
        class UpdateShadowResponse;
        class UpdateShadowSubscriptionRequest;

        using OnSubscribeComplete = std::function<void(int ioErr)>;
        using OnPublishComplete = std::function<void(int ioErr)>;

        using OnSubscribeToUpdateShadowRejectedResponse =
            std::function<void(Aws::Iotshadow::ErrorResponse *, int ioErr)>;

        using OnSubscribeToDeleteShadowRejectedResponse =
            std::function<void(Aws::Iotshadow::ErrorResponse *, int ioErr)>;

        using OnSubscribeToGetShadowAcceptedResponse =
            std::function<void(Aws::Iotshadow::GetShadowResponse *, int ioErr)>;

        using OnSubscribeToShadowUpdatedEventsResponse =
            std::function<void(Aws::Iotshadow::ShadowUpdatedEvent *, int ioErr)>;

        using OnSubscribeToShadowDeltaUpdatedEventsResponse =
            std::function<void(Aws::Iotshadow::ShadowDeltaUpdatedEvent *, int ioErr)>;

        using OnSubscribeToDeleteShadowAcceptedResponse =
            std::function<void(Aws::Iotshadow::DeleteShadowResponse *, int ioErr)>;

        using OnSubscribeToGetShadowRejectedResponse = std::function<void(Aws::Iotshadow::ErrorResponse *, int ioErr)>;

        using OnSubscribeToUpdateShadowAcceptedResponse =
            std::function<void(Aws::Iotshadow::UpdateShadowResponse *, int ioErr)>;

        class AWS_IOTSHADOW_API IotShadowClient final
        {
          public:
            IotShadowClient(const std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> &connection);

            operator bool() const noexcept;
            int GetLastError() const noexcept;

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

            bool SubscribeToGetShadowRejected(
                const Aws::Iotshadow::GetShadowSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToGetShadowRejectedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToUpdateShadowAccepted(
                const Aws::Iotshadow::UpdateShadowSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToUpdateShadowAcceptedResponse &handler,
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

          private:
            std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> m_connection;
        };

    } // namespace Iotshadow

} // namespace Aws
