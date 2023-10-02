#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotsecuretunneling/Exports.h>

#include <aws/crt/StlAllocator.h>
#include <aws/crt/Types.h>

#include <aws/crt/mqtt/Mqtt5Client.h>
#include <aws/crt/mqtt/MqttClient.h>

namespace Aws
{
    namespace Iotsecuretunneling
    {

        class SecureTunnelsNotifyResponse;
        class SubscribeToTunnelsNotifyRequest;

        using OnSubscribeComplete = std::function<void(int ioErr)>;
        using OnPublishComplete = std::function<void(int ioErr)>;

        using OnSubscribeToTunnelsNotifyResponse =
            std::function<void(Aws::Iotsecuretunneling::SecureTunnelsNotifyResponse *, int ioErr)>;

        /**
         * The AWS IoT Device Secure Tunneling service adds bidirectional communication to remote devices over a secure
         * connection that is managed by AWS IoT.
         *
         * https://docs.aws.amazon.com/iot/latest/developerguide/secure-tunneling.html
         *
         */
        class AWS_IOTSECURETUNNELING_API IotSecureTunnelingClient final
        {
          public:
            IotSecureTunnelingClient(const std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> &connection);
            IotSecureTunnelingClient(const std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> &mqtt5Client);

            operator bool() const noexcept;
            int GetLastError() const noexcept;

            /**
             * Subscribes to the secure tunnel notify topic
             *
             * https://docs.aws.amazon.com/iot/latest/developerguide/agent-snippet.html
             *
             * Subscribe to TunnelsNotify messages
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
            bool SubscribeToTunnelsNotify(
                const Aws::Iotsecuretunneling::SubscribeToTunnelsNotifyRequest &request,
                Aws::Crt::Mqtt::QOS qos,
                const OnSubscribeToTunnelsNotifyResponse &handler,
                const OnSubscribeComplete &onSubAck);

          private:
            std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> m_connection;
        };

    } // namespace Iotsecuretunneling

} // namespace Aws
