#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/iotidentity/Exports.h>

#include <aws/crt/StlAllocator.h>
#include <aws/crt/Types.h>

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

        using OnSubscribeToCreateKeysAndCertificateRejectedResponse =
            std::function<void(Aws::Iotidentity::ErrorResponse *, int ioErr)>;

        using OnSubscribeToRegisterThingAcceptedResponse =
            std::function<void(Aws::Iotidentity::RegisterThingResponse *, int ioErr)>;

        using OnSubscribeToRegisterThingRejectedResponse =
            std::function<void(Aws::Iotidentity::ErrorResponse *, int ioErr)>;

        using OnSubscribeToCreateKeysAndCertificateAcceptedResponse =
            std::function<void(Aws::Iotidentity::CreateKeysAndCertificateResponse *, int ioErr)>;

        using OnSubscribeToCreateCertificateFromCsrRejectedResponse =
            std::function<void(Aws::Iotidentity::ErrorResponse *, int ioErr)>;

        class AWS_IOTIDENTITY_API IotIdentityClient final
        {
          public:
            IotIdentityClient(const std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> &connection);

            operator bool() const noexcept;
            int GetLastError() const noexcept;

            bool SubscribeToCreateCertificateFromCsrAccepted(
                const Aws::Iotidentity::CreateCertificateFromCsrSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToCreateCertificateFromCsrAcceptedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToCreateKeysAndCertificateRejected(
                const Aws::Iotidentity::CreateKeysAndCertificateSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToCreateKeysAndCertificateRejectedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToRegisterThingAccepted(
                const Aws::Iotidentity::RegisterThingSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToRegisterThingAcceptedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToRegisterThingRejected(
                const Aws::Iotidentity::RegisterThingSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToRegisterThingRejectedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToCreateKeysAndCertificateAccepted(
                const Aws::Iotidentity::CreateKeysAndCertificateSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToCreateKeysAndCertificateAcceptedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool SubscribeToCreateCertificateFromCsrRejected(
                const Aws::Iotidentity::CreateCertificateFromCsrSubscriptionRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToCreateCertificateFromCsrRejectedResponse &handler,
                const OnSubscribeComplete &onSubAck);

            bool PublishCreateCertificateFromCsr(
                const Aws::Iotidentity::CreateCertificateFromCsrRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnPublishComplete &onPubAck);

            bool PublishCreateKeysAndCertificate(
                const Aws::Iotidentity::CreateKeysAndCertificateRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnPublishComplete &onPubAck);

            bool PublishRegisterThing(
                const Aws::Iotidentity::RegisterThingRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnPublishComplete &onPubAck);

          private:
            std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> m_connection;
        };

    } // namespace Iotidentity

} // namespace Aws
