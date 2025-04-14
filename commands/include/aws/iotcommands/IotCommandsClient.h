#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotcommands/Exports.h>

#include <aws/crt/StlAllocator.h>
#include <aws/crt/Types.h>

#include <aws/crt/mqtt/Mqtt5Client.h>
#include <aws/crt/mqtt/MqttClient.h>

namespace Aws
{
    namespace Iotcommands
    {

        class CommandExecutionsSubscriptionRequest;
        class UpdateCommandExecutionRequest;

        using OnSubscribeComplete = std::function<void(int ioErr)>;
        using OnPublishComplete = std::function<void(int ioErr)>;

        /**
         */
        class AWS_IOTCOMMANDS_API IotCommandsClient final
        {
          public:
            IotCommandsClient(const std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> &connection);
            IotCommandsClient(const std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> &mqtt5Client);

            operator bool() const noexcept;
            int GetLastError() const noexcept;

          private:
            std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> m_connection;
        };

    } // namespace Iotcommands

} // namespace Aws
