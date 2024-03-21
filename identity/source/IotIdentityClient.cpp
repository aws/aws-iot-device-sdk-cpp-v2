/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotidentity/IotIdentityClient.h>

#include <aws/iotidentity/CreateCertificateFromCsrRequest.h>
#include <aws/iotidentity/CreateCertificateFromCsrResponse.h>
#include <aws/iotidentity/CreateCertificateFromCsrSubscriptionRequest.h>
#include <aws/iotidentity/CreateKeysAndCertificateRequest.h>
#include <aws/iotidentity/CreateKeysAndCertificateResponse.h>
#include <aws/iotidentity/CreateKeysAndCertificateSubscriptionRequest.h>
#include <aws/iotidentity/ErrorResponse.h>
#include <aws/iotidentity/RegisterThingRequest.h>
#include <aws/iotidentity/RegisterThingResponse.h>
#include <aws/iotidentity/RegisterThingSubscriptionRequest.h>

namespace Aws
{
    namespace Iotidentity
    {

        IotIdentityClient::IotIdentityClient(const std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> &connection)
            : m_connection(connection)
        {
        }

        IotIdentityClient::IotIdentityClient(const std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> &mqtt5Client)
        {
            m_connection = Aws::Crt::Mqtt::MqttConnection::NewConnectionFromMqtt5Client(mqtt5Client);
        }

        IotIdentityClient::operator bool() const noexcept { return m_connection && *m_connection; }

        int IotIdentityClient::GetLastError() const noexcept { return aws_last_error(); }

        bool IotIdentityClient::SubscribeToCreateCertificateFromCsrAccepted(
            const Aws::Iotidentity::CreateCertificateFromCsrSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToCreateCertificateFromCsrAcceptedResponse &handler,
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
                    Aws::Iotidentity::CreateCertificateFromCsrResponse response(jsonObject);
                    handler(&response, AWS_ERROR_SUCCESS);
                };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "certificates"
                               << "/"
                               << "create-from-csr"
                               << "/"
                               << "json"
                               << "/"
                               << "accepted";

            return m_connection->Subscribe(
                       subscribeTopicSStr.str().c_str(),
                       qos,
                       std::move(onSubscribePublish),
                       std::move(onSubscribeComplete)) != 0;
        }

        bool IotIdentityClient::SubscribeToCreateCertificateFromCsrRejected(
            const Aws::Iotidentity::CreateCertificateFromCsrSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToCreateCertificateFromCsrRejectedResponse &handler,
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
                    Aws::Iotidentity::ErrorResponse response(jsonObject);
                    handler(&response, AWS_ERROR_SUCCESS);
                };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "certificates"
                               << "/"
                               << "create-from-csr"
                               << "/"
                               << "json"
                               << "/"
                               << "rejected";

            return m_connection->Subscribe(
                       subscribeTopicSStr.str().c_str(),
                       qos,
                       std::move(onSubscribePublish),
                       std::move(onSubscribeComplete)) != 0;
        }

        bool IotIdentityClient::SubscribeToCreateKeysAndCertificateAccepted(
            const Aws::Iotidentity::CreateKeysAndCertificateSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToCreateKeysAndCertificateAcceptedResponse &handler,
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
                    Aws::Iotidentity::CreateKeysAndCertificateResponse response(jsonObject);
                    handler(&response, AWS_ERROR_SUCCESS);
                };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "certificates"
                               << "/"
                               << "create"
                               << "/"
                               << "json"
                               << "/"
                               << "accepted";

            return m_connection->Subscribe(
                       subscribeTopicSStr.str().c_str(),
                       qos,
                       std::move(onSubscribePublish),
                       std::move(onSubscribeComplete)) != 0;
        }

        bool IotIdentityClient::SubscribeToCreateKeysAndCertificateRejected(
            const Aws::Iotidentity::CreateKeysAndCertificateSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToCreateKeysAndCertificateRejectedResponse &handler,
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
                    Aws::Iotidentity::ErrorResponse response(jsonObject);
                    handler(&response, AWS_ERROR_SUCCESS);
                };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "certificates"
                               << "/"
                               << "create"
                               << "/"
                               << "json"
                               << "/"
                               << "rejected";

            return m_connection->Subscribe(
                       subscribeTopicSStr.str().c_str(),
                       qos,
                       std::move(onSubscribePublish),
                       std::move(onSubscribeComplete)) != 0;
        }

        bool IotIdentityClient::SubscribeToRegisterThingAccepted(
            const Aws::Iotidentity::RegisterThingSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToRegisterThingAcceptedResponse &handler,
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
                    Aws::Iotidentity::RegisterThingResponse response(jsonObject);
                    handler(&response, AWS_ERROR_SUCCESS);
                };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "provisioning-templates"
                               << "/" << *request.TemplateName << "/"
                               << "provision"
                               << "/"
                               << "json"
                               << "/"
                               << "accepted";

            return m_connection->Subscribe(
                       subscribeTopicSStr.str().c_str(),
                       qos,
                       std::move(onSubscribePublish),
                       std::move(onSubscribeComplete)) != 0;
        }

        bool IotIdentityClient::SubscribeToRegisterThingRejected(
            const Aws::Iotidentity::RegisterThingSubscriptionRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToRegisterThingRejectedResponse &handler,
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
                    Aws::Iotidentity::ErrorResponse response(jsonObject);
                    handler(&response, AWS_ERROR_SUCCESS);
                };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "provisioning-templates"
                               << "/" << *request.TemplateName << "/"
                               << "provision"
                               << "/"
                               << "json"
                               << "/"
                               << "rejected";

            return m_connection->Subscribe(
                       subscribeTopicSStr.str().c_str(),
                       qos,
                       std::move(onSubscribePublish),
                       std::move(onSubscribeComplete)) != 0;
        }

        bool IotIdentityClient::PublishCreateCertificateFromCsr(
            const Aws::Iotidentity::CreateCertificateFromCsrRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnPublishComplete &onPubAck)
        {
            Aws::Crt::StringStream publishTopicSStr;
            publishTopicSStr << "$aws"
                             << "/"
                             << "certificates"
                             << "/"
                             << "create-from-csr"
                             << "/"
                             << "json";

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

        bool IotIdentityClient::PublishCreateKeysAndCertificate(
            const Aws::Iotidentity::CreateKeysAndCertificateRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnPublishComplete &onPubAck)
        {
            Aws::Crt::StringStream publishTopicSStr;
            publishTopicSStr << "$aws"
                             << "/"
                             << "certificates"
                             << "/"
                             << "create"
                             << "/"
                             << "json";

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

        bool IotIdentityClient::PublishRegisterThing(
            const Aws::Iotidentity::RegisterThingRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnPublishComplete &onPubAck)
        {
            Aws::Crt::StringStream publishTopicSStr;
            publishTopicSStr << "$aws"
                             << "/"
                             << "provisioning-templates"
                             << "/" << *request.TemplateName << "/"
                             << "provision"
                             << "/"
                             << "json";

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

    } // namespace Iotidentity

} // namespace Aws
