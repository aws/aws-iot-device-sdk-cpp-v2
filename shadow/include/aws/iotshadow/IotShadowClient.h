#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotshadow/Exports.h>

#include <aws/crt/StlAllocator.h>
#include <aws/crt/Types.h>

#include <aws/crt/mqtt/Mqtt5Client.h>
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

        using OnSubscribeToDeleteNamedShadowAcceptedResponse =
            std::function<void(Aws::Iotshadow::DeleteShadowResponse *, int ioErr)>;

        using OnSubscribeToDeleteNamedShadowRejectedResponse =
            std::function<void(Aws::Iotshadow::ErrorResponse *, int ioErr)>;

        using OnSubscribeToDeleteShadowAcceptedResponse =
            std::function<void(Aws::Iotshadow::DeleteShadowResponse *, int ioErr)>;

        using OnSubscribeToDeleteShadowRejectedResponse =
            std::function<void(Aws::Iotshadow::ErrorResponse *, int ioErr)>;

        using OnSubscribeToGetNamedShadowAcceptedResponse =
            std::function<void(Aws::Iotshadow::GetShadowResponse *, int ioErr)>;

        using OnSubscribeToGetNamedShadowRejectedResponse =
            std::function<void(Aws::Iotshadow::ErrorResponse *, int ioErr)>;

        using OnSubscribeToGetShadowAcceptedResponse =
            std::function<void(Aws::Iotshadow::GetShadowResponse *, int ioErr)>;

        using OnSubscribeToGetShadowRejectedResponse = std::function<void(Aws::Iotshadow::ErrorResponse *, int ioErr)>;

        using OnSubscribeToNamedShadowDeltaUpdatedEventsResponse =
            std::function<void(Aws::Iotshadow::ShadowDeltaUpdatedEvent *, int ioErr)>;

        using OnSubscribeToNamedShadowUpdatedEventsResponse =
            std::function<void(Aws::Iotshadow::ShadowUpdatedEvent *, int ioErr)>;

        using OnSubscribeToShadowDeltaUpdatedEventsResponse =
            std::function<void(Aws::Iotshadow::ShadowDeltaUpdatedEvent *, int ioErr)>;

        using OnSubscribeToShadowUpdatedEventsResponse =
            std::function<void(Aws::Iotshadow::ShadowUpdatedEvent *, int ioErr)>;

        using OnSubscribeToUpdateNamedShadowAcceptedResponse =
            std::function<void(Aws::Iotshadow::UpdateShadowResponse *, int ioErr)>;

        using OnSubscribeToUpdateNamedShadowRejectedResponse =
            std::function<void(Aws::Iotshadow::ErrorResponse *, int ioErr)>;

        using OnSubscribeToUpdateShadowAcceptedResponse =
            std::function<void(Aws::Iotshadow::UpdateShadowResponse *, int ioErr)>;

        using OnSubscribeToUpdateShadowRejectedResponse =
            std::function<void(Aws::Iotshadow::ErrorResponse *, int ioErr)>;

        /**
         * The AWS IoT Device Shadow service adds shadows to AWS IoT thing objects. Shadows are a simple data store for
         * device properties and state.  Shadows can make a device’s state available to apps and other services whether
         * the device is connected to AWS IoT or not.
         *
         * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html
         *
         */
        class AWS_IOTSHADOW_API IotShadowClient final
        {
          public:
            IotShadowClient(const std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> &connection);
            IotShadowClient(const std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> &mqtt5Client);

            operator bool() const noexcept;
            int GetLastError() const noexcept;

            /**
             * Subscribes to the accepted topic for the DeleteNamedShadow operation.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#delete-accepted-pub-sub-topic
             *
             * Subscribe to DeleteNamedShadowAccepted messages
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
            bool SubscribeToDeleteNamedShadowAccepted(
                const Aws::Iotshadow::DeleteNamedShadowSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToDeleteNamedShadowAcceptedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribes to the rejected topic for the DeleteNamedShadow operation.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#delete-rejected-pub-sub-topic
             *
             * Subscribe to DeleteNamedShadowRejected messages
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
            bool SubscribeToDeleteNamedShadowRejected(
                const Aws::Iotshadow::DeleteNamedShadowSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToDeleteNamedShadowRejectedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribes to the accepted topic for the DeleteShadow operation
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#delete-accepted-pub-sub-topic
             *
             * Subscribe to DeleteShadowAccepted messages
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
            bool SubscribeToDeleteShadowAccepted(
                const Aws::Iotshadow::DeleteShadowSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToDeleteShadowAcceptedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribes to the rejected topic for the DeleteShadow operation
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#delete-rejected-pub-sub-topic
             *
             * Subscribe to DeleteShadowRejected messages
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
            bool SubscribeToDeleteShadowRejected(
                const Aws::Iotshadow::DeleteShadowSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToDeleteShadowRejectedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribes to the accepted topic for the GetNamedShadow operation.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#get-accepted-pub-sub-topic
             *
             * Subscribe to GetNamedShadowAccepted messages
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
            bool SubscribeToGetNamedShadowAccepted(
                const Aws::Iotshadow::GetNamedShadowSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToGetNamedShadowAcceptedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribes to the rejected topic for the GetNamedShadow operation.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#get-rejected-pub-sub-topic
             *
             * Subscribe to GetNamedShadowRejected messages
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
            bool SubscribeToGetNamedShadowRejected(
                const Aws::Iotshadow::GetNamedShadowSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToGetNamedShadowRejectedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribes to the accepted topic for the GetShadow operation.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#get-accepted-pub-sub-topic
             *
             * Subscribe to GetShadowAccepted messages
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
            bool SubscribeToGetShadowAccepted(
                const Aws::Iotshadow::GetShadowSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToGetShadowAcceptedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribes to the rejected topic for the GetShadow operation.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#get-rejected-pub-sub-topic
             *
             * Subscribe to GetShadowRejected messages
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
            bool SubscribeToGetShadowRejected(
                const Aws::Iotshadow::GetShadowSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToGetShadowRejectedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribe to NamedShadowDelta events for a named shadow of an AWS IoT thing.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#update-delta-pub-sub-topic
             *
             * Subscribe to NamedShadowDeltaUpdatedEvents messages
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
            bool SubscribeToNamedShadowDeltaUpdatedEvents(
                const Aws::Iotshadow::NamedShadowDeltaUpdatedSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToNamedShadowDeltaUpdatedEventsResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribe to ShadowUpdated events for a named shadow of an AWS IoT thing.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#update-documents-pub-sub-topic
             *
             * Subscribe to NamedShadowUpdatedEvents messages
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
            bool SubscribeToNamedShadowUpdatedEvents(
                const Aws::Iotshadow::NamedShadowUpdatedSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToNamedShadowUpdatedEventsResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribe to ShadowDelta events for the (classic) shadow of an AWS IoT thing.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#update-delta-pub-sub-topic
             *
             * Subscribe to ShadowDeltaUpdatedEvents messages
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
            bool SubscribeToShadowDeltaUpdatedEvents(
                const Aws::Iotshadow::ShadowDeltaUpdatedSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToShadowDeltaUpdatedEventsResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribe to ShadowUpdated events for the (classic) shadow of an AWS IoT thing.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#update-documents-pub-sub-topic
             *
             * Subscribe to ShadowUpdatedEvents messages
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
            bool SubscribeToShadowUpdatedEvents(
                const Aws::Iotshadow::ShadowUpdatedSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToShadowUpdatedEventsResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribes to the accepted topic for the UpdateNamedShadow operation
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#update-accepted-pub-sub-topic
             *
             * Subscribe to UpdateNamedShadowAccepted messages
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
            bool SubscribeToUpdateNamedShadowAccepted(
                const Aws::Iotshadow::UpdateNamedShadowSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToUpdateNamedShadowAcceptedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribes to the rejected topic for the UpdateNamedShadow operation
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#update-rejected-pub-sub-topic
             *
             * Subscribe to UpdateNamedShadowRejected messages
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
            bool SubscribeToUpdateNamedShadowRejected(
                const Aws::Iotshadow::UpdateNamedShadowSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToUpdateNamedShadowRejectedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribes to the accepted topic for the UpdateShadow operation
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#update-accepted-pub-sub-topic
             *
             * Subscribe to UpdateShadowAccepted messages
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
            bool SubscribeToUpdateShadowAccepted(
                const Aws::Iotshadow::UpdateShadowSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToUpdateShadowAcceptedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribes to the rejected topic for the UpdateShadow operation
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#update-rejected-pub-sub-topic
             *
             * Subscribe to UpdateShadowRejected messages
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
            bool SubscribeToUpdateShadowRejected(
                const Aws::Iotshadow::UpdateShadowSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToUpdateShadowRejectedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Deletes a named shadow for an AWS IoT thing.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#delete-pub-sub-topic
             *
             * Publish a DeleteNamedShadow message.
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
            bool PublishDeleteNamedShadow(
                const Aws::Iotshadow::DeleteNamedShadowRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnPublishComplete &onPubAck);

            /**
             * Deletes the (classic) shadow for an AWS IoT thing.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#delete-pub-sub-topic
             *
             * Publish a DeleteShadow message.
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
            bool PublishDeleteShadow(
                const Aws::Iotshadow::DeleteShadowRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnPublishComplete &onPubAck);

            /**
             * Gets a named shadow for an AWS IoT thing.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#get-pub-sub-topic
             *
             * Publish a GetNamedShadow message.
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
            bool PublishGetNamedShadow(
                const Aws::Iotshadow::GetNamedShadowRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnPublishComplete &onPubAck);

            /**
             * Gets the (classic) shadow for an AWS IoT thing.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#get-pub-sub-topic
             *
             * Publish a GetShadow message.
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
            bool PublishGetShadow(
                const Aws::Iotshadow::GetShadowRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnPublishComplete &onPubAck);

            /**
             * Update a named shadow for a device.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#update-pub-sub-topic
             *
             * Publish a UpdateNamedShadow message.
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
            bool PublishUpdateNamedShadow(
                const Aws::Iotshadow::UpdateNamedShadowRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnPublishComplete &onPubAck);

            /**
             * Update a device's (classic) shadow.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#update-pub-sub-topic
             *
             * Publish a UpdateShadow message.
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
            bool PublishUpdateShadow(
                const Aws::Iotshadow::UpdateShadowRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnPublishComplete &onPubAck);

          private:
            std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> m_connection;
        };

    } // namespace Iotshadow

} // namespace Aws
