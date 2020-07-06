#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/iotjobs/Exports.h>

#include <aws/crt/StlAllocator.h>
#include <aws/crt/Types.h>

#include <aws/crt/mqtt/MqttClient.h>

namespace Aws
{
    namespace Iotjobs
    {

        class DescribeJobExecutionRequest;
        class DescribeJobExecutionResponse;
        class DescribeJobExecutionSubscriptionRequest;
        class GetPendingJobExecutionsRequest;
        class GetPendingJobExecutionsResponse;
        class GetPendingJobExecutionsSubscriptionRequest;
        class JobExecutionsChangedEvent;
        class JobExecutionsChangedSubscriptionRequest;
        class NextJobExecutionChangedEvent;
        class NextJobExecutionChangedSubscriptionRequest;
        class RejectedError;
        class StartNextJobExecutionResponse;
        class StartNextPendingJobExecutionRequest;
        class StartNextPendingJobExecutionSubscriptionRequest;
        class UpdateJobExecutionRequest;
        class UpdateJobExecutionResponse;
        class UpdateJobExecutionSubscriptionRequest;

        using OnSubscribeComplete = std::function<void(int ioErr)>;
        using OnPublishComplete = std::function<void(int ioErr)>;

        using OnSubscribeToUpdateJobExecutionAcceptedResponse =
            std::function<void(Aws::Iotjobs::UpdateJobExecutionResponse *, int ioErr)>;

        using OnSubscribeToGetPendingJobExecutionsRejectedResponse =
            std::function<void(Aws::Iotjobs::RejectedError *, int ioErr)>;

        using OnSubscribeToDescribeJobExecutionAcceptedResponse =
            std::function<void(Aws::Iotjobs::DescribeJobExecutionResponse *, int ioErr)>;

        using OnSubscribeToDescribeJobExecutionRejectedResponse =
            std::function<void(Aws::Iotjobs::RejectedError *, int ioErr)>;

        using OnSubscribeToUpdateJobExecutionRejectedResponse =
            std::function<void(Aws::Iotjobs::RejectedError *, int ioErr)>;

        using OnSubscribeToJobExecutionsChangedEventsResponse =
            std::function<void(Aws::Iotjobs::JobExecutionsChangedEvent *, int ioErr)>;

        using OnSubscribeToStartNextPendingJobExecutionRejectedResponse =
            std::function<void(Aws::Iotjobs::RejectedError *, int ioErr)>;

        using OnSubscribeToNextJobExecutionChangedEventsResponse =
            std::function<void(Aws::Iotjobs::NextJobExecutionChangedEvent *, int ioErr)>;

        using OnSubscribeToGetPendingJobExecutionsAcceptedResponse =
            std::function<void(Aws::Iotjobs::GetPendingJobExecutionsResponse *, int ioErr)>;

        using OnSubscribeToStartNextPendingJobExecutionAcceptedResponse =
            std::function<void(Aws::Iotjobs::StartNextJobExecutionResponse *, int ioErr)>;

        class AWS_IOTJOBS_API IotJobsClient final
        {
          public:
            IotJobsClient(const std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> &connection);

            operator bool() const noexcept;
            int GetLastError() const noexcept;

            bool SubscribeToUpdateJobExecutionAccepted(
                const Aws::Iotjobs::UpdateJobExecutionSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToUpdateJobExecutionAcceptedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToGetPendingJobExecutionsRejected(
                const Aws::Iotjobs::GetPendingJobExecutionsSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToGetPendingJobExecutionsRejectedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToDescribeJobExecutionAccepted(
                const Aws::Iotjobs::DescribeJobExecutionSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToDescribeJobExecutionAcceptedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToDescribeJobExecutionRejected(
                const Aws::Iotjobs::DescribeJobExecutionSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToDescribeJobExecutionRejectedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToUpdateJobExecutionRejected(
                const Aws::Iotjobs::UpdateJobExecutionSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToUpdateJobExecutionRejectedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToJobExecutionsChangedEvents(
                const Aws::Iotjobs::JobExecutionsChangedSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToJobExecutionsChangedEventsResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToStartNextPendingJobExecutionRejected(
                const Aws::Iotjobs::StartNextPendingJobExecutionSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToStartNextPendingJobExecutionRejectedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToNextJobExecutionChangedEvents(
                const Aws::Iotjobs::NextJobExecutionChangedSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToNextJobExecutionChangedEventsResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToGetPendingJobExecutionsAccepted(
                const Aws::Iotjobs::GetPendingJobExecutionsSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToGetPendingJobExecutionsAcceptedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToStartNextPendingJobExecutionAccepted(
                const Aws::Iotjobs::StartNextPendingJobExecutionSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToStartNextPendingJobExecutionAcceptedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool PublishDescribeJobExecution(
                const Aws::Iotjobs::DescribeJobExecutionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnPublishComplete &onPubAck);

            bool PublishGetPendingJobExecutions(
                const Aws::Iotjobs::GetPendingJobExecutionsRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnPublishComplete &onPubAck);

            bool PublishUpdateJobExecution(
                const Aws::Iotjobs::UpdateJobExecutionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnPublishComplete &onPubAck);

            bool PublishStartNextPendingJobExecution(
                const Aws::Iotjobs::StartNextPendingJobExecutionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnPublishComplete &onPubAck);

          private:
            std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> m_connection;
        };

    } // namespace Iotjobs

} // namespace Aws
