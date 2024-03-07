#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotjobs/Exports.h>

#include <aws/crt/StlAllocator.h>
#include <aws/crt/Types.h>

#include <aws/crt/mqtt/Mqtt5Client.h>
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

        using OnSubscribeToDescribeJobExecutionAcceptedResponse =
            std::function<void(Aws::Iotjobs::DescribeJobExecutionResponse *, int ioErr)>;

        using OnSubscribeToDescribeJobExecutionRejectedResponse =
            std::function<void(Aws::Iotjobs::RejectedError *, int ioErr)>;

        using OnSubscribeToGetPendingJobExecutionsAcceptedResponse =
            std::function<void(Aws::Iotjobs::GetPendingJobExecutionsResponse *, int ioErr)>;

        using OnSubscribeToGetPendingJobExecutionsRejectedResponse =
            std::function<void(Aws::Iotjobs::RejectedError *, int ioErr)>;

        using OnSubscribeToJobExecutionsChangedEventsResponse =
            std::function<void(Aws::Iotjobs::JobExecutionsChangedEvent *, int ioErr)>;

        using OnSubscribeToNextJobExecutionChangedEventsResponse =
            std::function<void(Aws::Iotjobs::NextJobExecutionChangedEvent *, int ioErr)>;

        using OnSubscribeToStartNextPendingJobExecutionAcceptedResponse =
            std::function<void(Aws::Iotjobs::StartNextJobExecutionResponse *, int ioErr)>;

        using OnSubscribeToStartNextPendingJobExecutionRejectedResponse =
            std::function<void(Aws::Iotjobs::RejectedError *, int ioErr)>;

        using OnSubscribeToUpdateJobExecutionAcceptedResponse =
            std::function<void(Aws::Iotjobs::UpdateJobExecutionResponse *, int ioErr)>;

        using OnSubscribeToUpdateJobExecutionRejectedResponse =
            std::function<void(Aws::Iotjobs::RejectedError *, int ioErr)>;

        /**
         * The AWS IoT jobs service can be used to define a set of remote operations that are sent to and executed on
         * one or more devices connected to AWS IoT.
         *
         * https://docs.aws.amazon.com/iot/latest/developerguide/jobs-api.html#jobs-mqtt-api
         *
         */
        class AWS_IOTJOBS_API IotJobsClient final
        {
          public:
            IotJobsClient(const std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> &connection);
            IotJobsClient(const std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> &mqtt5Client);

            operator bool() const noexcept;
            int GetLastError() const noexcept;

            /**
             * Subscribes to the accepted topic for the DescribeJobExecution operation
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/jobs-api.html#mqtt-describejobexecution
             *
             * Subscribe to DescribeJobExecutionAccepted messages
             *
             * Once subscribed, `handler` is invoked each time a message matching
             * the `topic` is received. It is possible for such messages to arrive before
             * the SUBACK is received.
             *
             * @param request Subscription request configuration
             * @param qos Maximum requested QoS that server may use when sending messages to the client.
             *            The server may grant a lower QoS in the SUBACK
             * @param handler callback function to invoke with messages received on the subscription topic
             * @param onSubAck callback function invoked on receipt of the SUBACK from the server
             *
             * @return true if the subscribe was successfully queued, false if there was an error doing so
             */
            bool SubscribeToDescribeJobExecutionAccepted(
                const Aws::Iotjobs::DescribeJobExecutionSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToDescribeJobExecutionAcceptedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribes to the rejected topic for the DescribeJobExecution operation
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/jobs-api.html#mqtt-describejobexecution
             *
             * Subscribe to DescribeJobExecutionRejected messages
             *
             * Once subscribed, `handler` is invoked each time a message matching
             * the `topic` is received. It is possible for such messages to arrive before
             * the SUBACK is received.
             *
             * @param request Subscription request configuration
             * @param qos Maximum requested QoS that server may use when sending messages to the client.
             *            The server may grant a lower QoS in the SUBACK
             * @param handler callback function to invoke with messages received on the subscription topic
             * @param onSubAck callback function invoked on receipt of the SUBACK from the server
             *
             * @return true if the subscribe was successfully queued, false if there was an error doing so
             */
            bool SubscribeToDescribeJobExecutionRejected(
                const Aws::Iotjobs::DescribeJobExecutionSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToDescribeJobExecutionRejectedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribes to the accepted topic for the GetPendingJobsExecutions operation
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/jobs-api.html#mqtt-getpendingjobexecutions
             *
             * Subscribe to GetPendingJobExecutionsAccepted messages
             *
             * Once subscribed, `handler` is invoked each time a message matching
             * the `topic` is received. It is possible for such messages to arrive before
             * the SUBACK is received.
             *
             * @param request Subscription request configuration
             * @param qos Maximum requested QoS that server may use when sending messages to the client.
             *            The server may grant a lower QoS in the SUBACK
             * @param handler callback function to invoke with messages received on the subscription topic
             * @param onSubAck callback function invoked on receipt of the SUBACK from the server
             *
             * @return true if the subscribe was successfully queued, false if there was an error doing so
             */
            bool SubscribeToGetPendingJobExecutionsAccepted(
                const Aws::Iotjobs::GetPendingJobExecutionsSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToGetPendingJobExecutionsAcceptedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribes to the rejected topic for the GetPendingJobsExecutions operation
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/jobs-api.html#mqtt-getpendingjobexecutions
             *
             * Subscribe to GetPendingJobExecutionsRejected messages
             *
             * Once subscribed, `handler` is invoked each time a message matching
             * the `topic` is received. It is possible for such messages to arrive before
             * the SUBACK is received.
             *
             * @param request Subscription request configuration
             * @param qos Maximum requested QoS that server may use when sending messages to the client.
             *            The server may grant a lower QoS in the SUBACK
             * @param handler callback function to invoke with messages received on the subscription topic
             * @param onSubAck callback function invoked on receipt of the SUBACK from the server
             *
             * @return true if the subscribe was successfully queued, false if there was an error doing so
             */
            bool SubscribeToGetPendingJobExecutionsRejected(
                const Aws::Iotjobs::GetPendingJobExecutionsSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToGetPendingJobExecutionsRejectedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribes to JobExecutionsChanged notifications for a given IoT thing.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/jobs-api.html#mqtt-jobexecutionschanged
             *
             * Subscribe to JobExecutionsChangedEvents messages
             *
             * Once subscribed, `handler` is invoked each time a message matching
             * the `topic` is received. It is possible for such messages to arrive before
             * the SUBACK is received.
             *
             * @param request Subscription request configuration
             * @param qos Maximum requested QoS that server may use when sending messages to the client.
             *            The server may grant a lower QoS in the SUBACK
             * @param handler callback function to invoke with messages received on the subscription topic
             * @param onSubAck callback function invoked on receipt of the SUBACK from the server
             *
             * @return true if the subscribe was successfully queued, false if there was an error doing so
             */
            bool SubscribeToJobExecutionsChangedEvents(
                const Aws::Iotjobs::JobExecutionsChangedSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToJobExecutionsChangedEventsResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             *
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/jobs-api.html#mqtt-nextjobexecutionchanged
             *
             * Subscribe to NextJobExecutionChangedEvents messages
             *
             * Once subscribed, `handler` is invoked each time a message matching
             * the `topic` is received. It is possible for such messages to arrive before
             * the SUBACK is received.
             *
             * @param request Subscription request configuration
             * @param qos Maximum requested QoS that server may use when sending messages to the client.
             *            The server may grant a lower QoS in the SUBACK
             * @param handler callback function to invoke with messages received on the subscription topic
             * @param onSubAck callback function invoked on receipt of the SUBACK from the server
             *
             * @return true if the subscribe was successfully queued, false if there was an error doing so
             */
            bool SubscribeToNextJobExecutionChangedEvents(
                const Aws::Iotjobs::NextJobExecutionChangedSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToNextJobExecutionChangedEventsResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribes to the accepted topic for the StartNextPendingJobExecution operation
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/jobs-api.html#mqtt-startnextpendingjobexecution
             *
             * Subscribe to StartNextPendingJobExecutionAccepted messages
             *
             * Once subscribed, `handler` is invoked each time a message matching
             * the `topic` is received. It is possible for such messages to arrive before
             * the SUBACK is received.
             *
             * @param request Subscription request configuration
             * @param qos Maximum requested QoS that server may use when sending messages to the client.
             *            The server may grant a lower QoS in the SUBACK
             * @param handler callback function to invoke with messages received on the subscription topic
             * @param onSubAck callback function invoked on receipt of the SUBACK from the server
             *
             * @return true if the subscribe was successfully queued, false if there was an error doing so
             */
            bool SubscribeToStartNextPendingJobExecutionAccepted(
                const Aws::Iotjobs::StartNextPendingJobExecutionSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToStartNextPendingJobExecutionAcceptedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribes to the rejected topic for the StartNextPendingJobExecution operation
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/jobs-api.html#mqtt-startnextpendingjobexecution
             *
             * Subscribe to StartNextPendingJobExecutionRejected messages
             *
             * Once subscribed, `handler` is invoked each time a message matching
             * the `topic` is received. It is possible for such messages to arrive before
             * the SUBACK is received.
             *
             * @param request Subscription request configuration
             * @param qos Maximum requested QoS that server may use when sending messages to the client.
             *            The server may grant a lower QoS in the SUBACK
             * @param handler callback function to invoke with messages received on the subscription topic
             * @param onSubAck callback function invoked on receipt of the SUBACK from the server
             *
             * @return true if the subscribe was successfully queued, false if there was an error doing so
             */
            bool SubscribeToStartNextPendingJobExecutionRejected(
                const Aws::Iotjobs::StartNextPendingJobExecutionSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToStartNextPendingJobExecutionRejectedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribes to the accepted topic for the UpdateJobExecution operation
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/jobs-api.html#mqtt-updatejobexecution
             *
             * Subscribe to UpdateJobExecutionAccepted messages
             *
             * Once subscribed, `handler` is invoked each time a message matching
             * the `topic` is received. It is possible for such messages to arrive before
             * the SUBACK is received.
             *
             * @param request Subscription request configuration
             * @param qos Maximum requested QoS that server may use when sending messages to the client.
             *            The server may grant a lower QoS in the SUBACK
             * @param handler callback function to invoke with messages received on the subscription topic
             * @param onSubAck callback function invoked on receipt of the SUBACK from the server
             *
             * @return true if the subscribe was successfully queued, false if there was an error doing so
             */
            bool SubscribeToUpdateJobExecutionAccepted(
                const Aws::Iotjobs::UpdateJobExecutionSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToUpdateJobExecutionAcceptedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribes to the rejected topic for the UpdateJobExecution operation
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/jobs-api.html#mqtt-updatejobexecution
             *
             * Subscribe to UpdateJobExecutionRejected messages
             *
             * Once subscribed, `handler` is invoked each time a message matching
             * the `topic` is received. It is possible for such messages to arrive before
             * the SUBACK is received.
             *
             * @param request Subscription request configuration
             * @param qos Maximum requested QoS that server may use when sending messages to the client.
             *            The server may grant a lower QoS in the SUBACK
             * @param handler callback function to invoke with messages received on the subscription topic
             * @param onSubAck callback function invoked on receipt of the SUBACK from the server
             *
             * @return true if the subscribe was successfully queued, false if there was an error doing so
             */
            bool SubscribeToUpdateJobExecutionRejected(
                const Aws::Iotjobs::UpdateJobExecutionSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToUpdateJobExecutionRejectedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Gets detailed information about a job execution.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/jobs-api.html#mqtt-describejobexecution
             *
             * Publish a DescribeJobExecution message.
             * If the device is offline, the PUBLISH packet will be sent once the connection resumes.
             *
             * @param request Message to be serialized and sent
             * @param qos Quality of Service for delivering this message
             * @param onPubAck callback when the publish "completes" (see below)
             *
             * @return true if the message was successfully queued for publication, false if there was an error
             *
             * * For QoS 0, completes as soon as the packet is sent.
             * * For QoS 1, completes when PUBACK is received.
             * * QoS 2 is not supported by AWS IoT.
             */
            bool PublishDescribeJobExecution(
                const Aws::Iotjobs::DescribeJobExecutionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnPublishComplete &onPubAck);

            /**
             * Gets the list of all jobs for a thing that are not in a terminal state.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/jobs-api.html#mqtt-getpendingjobexecutions
             *
             * Publish a GetPendingJobExecutions message.
             * If the device is offline, the PUBLISH packet will be sent once the connection resumes.
             *
             * @param request Message to be serialized and sent
             * @param qos Quality of Service for delivering this message
             * @param onPubAck callback when the publish "completes" (see below)
             *
             * @return true if the message was successfully queued for publication, false if there was an error
             *
             * * For QoS 0, completes as soon as the packet is sent.
             * * For QoS 1, completes when PUBACK is received.
             * * QoS 2 is not supported by AWS IoT.
             */
            bool PublishGetPendingJobExecutions(
                const Aws::Iotjobs::GetPendingJobExecutionsRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnPublishComplete &onPubAck);

            /**
             * Gets and starts the next pending job execution for a thing (status IN_PROGRESS or QUEUED).
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/jobs-api.html#mqtt-startnextpendingjobexecution
             *
             * Publish a StartNextPendingJobExecution message.
             * If the device is offline, the PUBLISH packet will be sent once the connection resumes.
             *
             * @param request Message to be serialized and sent
             * @param qos Quality of Service for delivering this message
             * @param onPubAck callback when the publish "completes" (see below)
             *
             * @return true if the message was successfully queued for publication, false if there was an error
             *
             * * For QoS 0, completes as soon as the packet is sent.
             * * For QoS 1, completes when PUBACK is received.
             * * QoS 2 is not supported by AWS IoT.
             */
            bool PublishStartNextPendingJobExecution(
                const Aws::Iotjobs::StartNextPendingJobExecutionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnPublishComplete &onPubAck);

            /**
             * Updates the status of a job execution. You can optionally create a step timer by setting a value for the
             * stepTimeoutInMinutes property. If you don't update the value of this property by running
             * UpdateJobExecution again, the job execution times out when the step timer expires.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/jobs-api.html#mqtt-updatejobexecution
             *
             * Publish a UpdateJobExecution message.
             * If the device is offline, the PUBLISH packet will be sent once the connection resumes.
             *
             * @param request Message to be serialized and sent
             * @param qos Quality of Service for delivering this message
             * @param onPubAck callback when the publish "completes" (see below)
             *
             * @return true if the message was successfully queued for publication, false if there was an error
             *
             * * For QoS 0, completes as soon as the packet is sent.
             * * For QoS 1, completes when PUBACK is received.
             * * QoS 2 is not supported by AWS IoT.
             */
            bool PublishUpdateJobExecution(
                const Aws::Iotjobs::UpdateJobExecutionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnPublishComplete &onPubAck);

          private:
            std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> m_connection;
        };

    } // namespace Iotjobs

} // namespace Aws
