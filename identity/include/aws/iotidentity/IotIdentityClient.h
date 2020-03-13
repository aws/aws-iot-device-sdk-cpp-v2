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
#include <aws/iotidentity/Exports.h>

#include <aws/crt/Types.h>
#include <aws/crt/StlAllocator.h>

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
                    std::function<void(Aws::Iotidentity::CreateCertificateFromCsrResponse*, int ioErr)>;

    using OnSubscribeToCreateKeysAndCertificateRejectedResponse =
                    std::function<void(Aws::Iotidentity::ErrorResponse*, int ioErr)>;

    using OnSubscribeToRegisterThingAcceptedResponse =
                    std::function<void(Aws::Iotidentity::RegisterThingResponse*, int ioErr)>;

    using OnSubscribeToRegisterThingRejectedResponse =
                    std::function<void(Aws::Iotidentity::ErrorResponse*, int ioErr)>;

    using OnSubscribeToCreateKeysAndCertificateAcceptedResponse =
                    std::function<void(Aws::Iotidentity::CreateKeysAndCertificateResponse*, int ioErr)>;

    using OnSubscribeToCreateCertificateFromCsrRejectedResponse =
                    std::function<void(Aws::Iotidentity::ErrorResponse*, int ioErr)>;

    class AWS_IOTIDENTITY_API IotIdentityClient final
    {
    public:
        IotIdentityClient(const std::shared_ptr<Aws::Crt::Mqtt::MqttConnection>& connection);

        operator bool() const noexcept;
        int GetLastError() const noexcept;

        bool SubscribeToCreateCertificateFromCsrAccepted(const Aws::Iotidentity::CreateCertificateFromCsrSubscriptionRequest& request, Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToCreateCertificateFromCsrAcceptedResponse& handler, const OnSubscribeComplete& onSubAck);

        bool SubscribeToCreateKeysAndCertificateRejected(const Aws::Iotidentity::CreateKeysAndCertificateSubscriptionRequest& request, Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToCreateKeysAndCertificateRejectedResponse& handler, const OnSubscribeComplete& onSubAck);

        bool SubscribeToRegisterThingAccepted(const Aws::Iotidentity::RegisterThingSubscriptionRequest& request, Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToRegisterThingAcceptedResponse& handler, const OnSubscribeComplete& onSubAck);

        bool SubscribeToRegisterThingRejected(const Aws::Iotidentity::RegisterThingSubscriptionRequest& request, Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToRegisterThingRejectedResponse& handler, const OnSubscribeComplete& onSubAck);

        bool SubscribeToCreateKeysAndCertificateAccepted(const Aws::Iotidentity::CreateKeysAndCertificateSubscriptionRequest& request, Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToCreateKeysAndCertificateAcceptedResponse& handler, const OnSubscribeComplete& onSubAck);

        bool SubscribeToCreateCertificateFromCsrRejected(const Aws::Iotidentity::CreateCertificateFromCsrSubscriptionRequest& request, Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToCreateCertificateFromCsrRejectedResponse& handler, const OnSubscribeComplete& onSubAck);


        bool PublishCreateCertificateFromCsr(const Aws::Iotidentity::CreateCertificateFromCsrRequest& request, Aws::Crt::Mqtt::QOS qos, const OnPublishComplete& onPubAck);

        bool PublishCreateKeysAndCertificate(const Aws::Iotidentity::CreateKeysAndCertificateRequest& request, Aws::Crt::Mqtt::QOS qos, const OnPublishComplete& onPubAck);

        bool PublishRegisterThing(const Aws::Iotidentity::RegisterThingRequest& request, Aws::Crt::Mqtt::QOS qos, const OnPublishComplete& onPubAck);

    private:
        std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> m_connection;
    };

}

}

