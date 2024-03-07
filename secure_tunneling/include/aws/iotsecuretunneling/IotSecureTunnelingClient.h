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
#include <aws/iotsecuretunneling/Exports.h>

#include <aws/crt/StlAllocator.h>
#include <aws/crt/Types.h>

#include <aws/crt/mqtt/MqttClient.h>

namespace Aws
{
    namespace Iotsecuretunneling
    {

        class SecureTunnelingNotifyResponse;
        class SubscribeToTunnelsNotifyRequest;

        using OnSubscribeComplete = std::function<void(int ioErr)>;
        using OnPublishComplete = std::function<void(int ioErr)>;

        using OnSubscribeToTunnelsNotifyResponse =
            std::function<void(Aws::Iotsecuretunneling::SecureTunnelingNotifyResponse *, int ioErr)>;

        class AWS_IOTSECURETUNNELING_API IotSecureTunnelingClient final
        {
          public:
            IotSecureTunnelingClient(const std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> &connection);

            operator bool() const noexcept;
            int GetLastError() const noexcept;

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
