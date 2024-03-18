#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotidentity/Exports.h>

#include <aws/crt/StlAllocator.h>
#include <aws/crt/Types.h>

#include <aws/crt/mqtt/Mqtt5Client.h>
#include <aws/crt/mqtt/MqttClient.h>

namespace Aws
{
    namespace Iotidentity
    {

        class CreateCertificateFromCsrRequest;
        class CreateCertificateFromCsrResponse;
        class CreateCertificateFromCsrSubscriptionRequest;
        class CreateKeysAndCertificateRequest;
        class CreateKeysAndCertificateResponse;
        class CreateKeysAndCertificateSubscriptionRequest;
        class ErrorResponse;
        class RegisterThingRequest;
        class RegisterThingResponse;
        class RegisterThingSubscriptionRequest;

        using OnSubscribeComplete = std::function<void(int ioErr)>;
        using OnPublishComplete = std::function<void(int ioErr)>;

        using OnSubscribeToCreateCertificateFromCsrAcceptedResponse =
            std::function<void(Aws::Iotidentity::CreateCertificateFromCsrResponse *, int ioErr)>;

        using OnSubscribeToCreateCertificateFromCsrRejectedResponse =
            std::function<void(Aws::Iotidentity::ErrorResponse *, int ioErr)>;

        using OnSubscribeToCreateKeysAndCertificateAcceptedResponse =
            std::function<void(Aws::Iotidentity::CreateKeysAndCertificateResponse *, int ioErr)>;

        using OnSubscribeToCreateKeysAndCertificateRejectedResponse =
            std::function<void(Aws::Iotidentity::ErrorResponse *, int ioErr)>;

        using OnSubscribeToRegisterThingAcceptedResponse =
            std::function<void(Aws::Iotidentity::RegisterThingResponse *, int ioErr)>;

        using OnSubscribeToRegisterThingRejectedResponse =
            std::function<void(Aws::Iotidentity::ErrorResponse *, int ioErr)>;

        /**
         * An AWS IoT service that assists with provisioning a device and installing unique client certificates on it
         *
         * https://docs.aws.amazon.com/iot/latest/developerguide/provision-wo-cert.html
         *
         */
        class AWS_IOTIDENTITY_API IotIdentityClient final
        {
          public:
            IotIdentityClient(const std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> &connection);
            IotIdentityClient(const std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> &mqtt5Client);

            operator bool() const noexcept;
            int GetLastError() const noexcept;

            /**
             * Subscribes to the accepted topic of the CreateCertificateFromCsr operation.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/provision-wo-cert.html#fleet-provision-api
             *
             * Subscribe to CreateCertificateFromCsrAccepted messages
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
            bool SubscribeToCreateCertificateFromCsrAccepted(
                const Aws::Iotidentity::CreateCertificateFromCsrSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToCreateCertificateFromCsrAcceptedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribes to the rejected topic of the CreateCertificateFromCsr operation.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/provision-wo-cert.html#fleet-provision-api
             *
             * Subscribe to CreateCertificateFromCsrRejected messages
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
            bool SubscribeToCreateCertificateFromCsrRejected(
                const Aws::Iotidentity::CreateCertificateFromCsrSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToCreateCertificateFromCsrRejectedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribes to the accepted topic of the CreateKeysAndCertificate operation.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/provision-wo-cert.html#fleet-provision-api
             *
             * Subscribe to CreateKeysAndCertificateAccepted messages
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
            bool SubscribeToCreateKeysAndCertificateAccepted(
                const Aws::Iotidentity::CreateKeysAndCertificateSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToCreateKeysAndCertificateAcceptedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribes to the rejected topic of the CreateKeysAndCertificate operation.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/provision-wo-cert.html#fleet-provision-api
             *
             * Subscribe to CreateKeysAndCertificateRejected messages
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
            bool SubscribeToCreateKeysAndCertificateRejected(
                const Aws::Iotidentity::CreateKeysAndCertificateSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToCreateKeysAndCertificateRejectedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribes to the accepted topic of the RegisterThing operation.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/provision-wo-cert.html#fleet-provision-api
             *
             * Subscribe to RegisterThingAccepted messages
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
            bool SubscribeToRegisterThingAccepted(
                const Aws::Iotidentity::RegisterThingSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToRegisterThingAcceptedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Subscribes to the rejected topic of the RegisterThing operation.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/provision-wo-cert.html#fleet-provision-api
             *
             * Subscribe to RegisterThingRejected messages
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
            bool SubscribeToRegisterThingRejected(
                const Aws::Iotidentity::RegisterThingSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToRegisterThingRejectedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            /**
             * Creates a certificate from a certificate signing request (CSR). AWS IoT provides client certificates that
             * are signed by the Amazon Root certificate authority (CA). The new certificate has a PENDING_ACTIVATION
             * status. When you call RegisterThing to provision a thing with this certificate, the certificate status
             * changes to ACTIVE or INACTIVE as described in the template.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/provision-wo-cert.html#fleet-provision-api
             *
             * Publish a CreateCertificateFromCsr message.
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
            bool PublishCreateCertificateFromCsr(
                const Aws::Iotidentity::CreateCertificateFromCsrRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnPublishComplete &onPubAck);

            /**
             * Creates new keys and a certificate. AWS IoT provides client certificates that are signed by the Amazon
             * Root certificate authority (CA). The new certificate has a PENDING_ACTIVATION status. When you call
             * RegisterThing to provision a thing with this certificate, the certificate status changes to ACTIVE or
             * INACTIVE as described in the template.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/provision-wo-cert.html#fleet-provision-api
             *
             * Publish a CreateKeysAndCertificate message.
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
            bool PublishCreateKeysAndCertificate(
                const Aws::Iotidentity::CreateKeysAndCertificateRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnPublishComplete &onPubAck);

            /**
             * Provisions an AWS IoT thing using a pre-defined template.
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/provision-wo-cert.html#fleet-provision-api
             *
             * Publish a RegisterThing message.
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
            bool PublishRegisterThing(
                const Aws::Iotidentity::RegisterThingRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnPublishComplete &onPubAck);

          private:
            std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> m_connection;
        };

    } // namespace Iotidentity

} // namespace Aws
