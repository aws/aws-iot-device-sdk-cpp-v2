/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotcommand/IotCommandClient.h>

#include <aws/iotcommand/CommandExecutionsSubscriptionRequest.h>
#include <aws/iotcommand/UpdateCommandExecutionRequest.h>

namespace Aws
{
    namespace Iotcommand
    {

        IotCommandClient::IotCommandClient(const std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> &connection)
            : m_connection(connection)
        {
        }

        IotCommandClient::IotCommandClient(const std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> &mqtt5Client)
        {
            m_connection = Aws::Crt::Mqtt::MqttConnection::NewConnectionFromMqtt5Client(mqtt5Client);
        }

        IotCommandClient::operator bool() const noexcept
        {
            return m_connection && *m_connection;
        }

        int IotCommandClient::GetLastError() const noexcept
        {
            return aws_last_error();
        }

    } // namespace Iotcommand

} // namespace Aws
