/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotsecuretunneling/IotSecureTunnelingClient.h>

#include <aws/iotsecuretunneling/SecureTunnelsNotifyResponse.h>
#include <aws/iotsecuretunneling/SubscribeToTunnelsNotifyRequest.h>

namespace Aws
{
    namespace Iotsecuretunneling
    {

        IotSecureTunnelingClient::IotSecureTunnelingClient(
            const std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> &connection)
            : m_connection(connection)
        {
        }

        IotSecureTunnelingClient::IotSecureTunnelingClient(
            const std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> &mqtt5Client)
        {
            m_connection = Aws::Crt::Mqtt::MqttConnection::NewConnectionFromMqtt5Client(mqtt5Client);
        }

        IotSecureTunnelingClient::operator bool() const noexcept { return m_connection && *m_connection; }

        int IotSecureTunnelingClient::GetLastError() const noexcept { return aws_last_error(); }

        bool IotSecureTunnelingClient::SubscribeToTunnelsNotify(
            const Aws::Iotsecuretunneling::SubscribeToTunnelsNotifyRequest &request,
            Aws::Crt::Mqtt::QOS qos,
            const OnSubscribeToTunnelsNotifyResponse &handler,
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
                    Aws::Iotsecuretunneling::SecureTunnelsNotifyResponse response(jsonObject);
                    handler(&response, AWS_ERROR_SUCCESS);
                };

            Aws::Crt::StringStream subscribeTopicSStr;
            subscribeTopicSStr << "$aws"
                               << "/"
                               << "things"
                               << "/" << *request.ThingName << "/"
                               << "tunnels"
                               << "/"
                               << "notify";

            return m_connection->Subscribe(
                       subscribeTopicSStr.str().c_str(),
                       qos,
                       std::move(onSubscribePublish),
                       std::move(onSubscribeComplete)) != 0;
        }

    } // namespace Iotsecuretunneling

} // namespace Aws
