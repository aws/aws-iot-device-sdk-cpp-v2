/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotcommands/IotCommandsClient.h>

#include <aws/iotcommands/CommandExecutionsSubscriptionRequest.h>
#include <aws/iotcommands/UpdateCommandExecutionRequest.h>

namespace Aws
{
    namespace Iotcommands
    {

        IotCommandsClient::IotCommandsClient(const std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> &connection)
            : m_connection(connection)
        {
        }

        IotCommandsClient::IotCommandsClient(const std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> &mqtt5Client)
        {
            m_connection = Aws::Crt::Mqtt::MqttConnection::NewConnectionFromMqtt5Client(mqtt5Client);
        }

        IotCommandsClient::operator bool() const noexcept
        {
            return m_connection && *m_connection;
        }

        int IotCommandsClient::GetLastError() const noexcept
        {
            return aws_last_error();
        }

    } // namespace Iotcommands

} // namespace Aws
