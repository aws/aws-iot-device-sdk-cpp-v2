#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/iotcommands/CommandExecutionEvent.h>
#include <aws/iotcommands/CommandExecutionStatus.h>
#include <aws/iotcommands/DeviceType.h>
#include <aws/iotcommands/IotCommandsClientV2.h>

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>

namespace Aws
{
    namespace IotcommandsSample
    {

        /**
         * Sample wrapper around client for the IoT command service.
         */
        class CommandStreamHandler
        {
          public:
            explicit CommandStreamHandler(std::shared_ptr<Aws::Iotcommands::IClientV2> &&commandClient);

            CommandStreamHandler(CommandStreamHandler &&) noexcept = default;
            CommandStreamHandler &operator=(CommandStreamHandler &&) noexcept = default;

            /**
             * TODO Explain why no copy.
             */
            CommandStreamHandler(const CommandStreamHandler &) = delete;
            CommandStreamHandler &operator=(const CommandStreamHandler &) = delete;

            /**
             *
             * @param deviceType
             * @param deviceId
             * @param payloadFormat
             * @return
             */
            bool subscribeToCommandExecutionsStream(
                Aws::Iotcommands::DeviceType deviceType,
                const Aws::Crt::String &deviceId,
                const Aws::Crt::String &payloadFormat);

            bool updateCommandExecutionStatus(
                const Aws::Crt::String &executionId,
                Aws::Iotcommands::CommandExecutionStatus status,
                const Aws::Crt::String &reasonCode,
                const Aws::Crt::String &reasonDescription);

            void listOpenedStreams();

            bool closeStream(uint64_t streamId);

          private:
            struct CommandExecutionContext
            {
                Aws::Iotcommands::DeviceType deviceType;
                Aws::Crt::String deviceId;
                Aws::Iotcommands::CommandExecutionEvent event;
            };

            struct StreamingOperationWrapper
            {
                /**
                 * A streaming operation instance. While this object is alive, the corresponding matching command
                 * executions will be delivered to it.
                 */
                std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> stream;

                /**
                 * Determine if the device should subscribe for commands addressed to an IoT Thing or MQTT client.
                 */
                Aws::Iotcommands::DeviceType deviceType;

                /**
                 * Depending on device type value, this field is either an IoT Thing name or a client ID.
                 */
                Aws::Crt::String deviceId;

                /**
                 * IoT Command uses a designated MQTT topics for JSON and CBOR. For every other payload format or for
                 * the cases when payload format is not specified, a generic topic is used.
                 * TODO Describe this in the service itself.
                 * TODO Provide example?
                 */
                Aws::Crt::String payloadType;
            };

            void registerStream(
                uint64_t id,
                std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> &&operation,
                Aws::Iotcommands::DeviceType deviceType,
                const Aws::Crt::String &deviceId,
                const Aws::Crt::String &payloadFormat);

            static void s_onSubscriptionStatusEvent(
                uint64_t streamId,
                Aws::Iot::RequestResponse::SubscriptionStatusEvent &&event);

            /**
             * Service client for IoT command.
             * All interactions with IoT command are performed via this object.
             */
            std::shared_ptr<Aws::Iotcommands::IClientV2> m_commandClient;

            /**
             * Opened streaming operations.
             */
            std::unordered_map<uint64_t, StreamingOperationWrapper> m_streams;

            /**
             * Active command executions that were received on one of the open streams.
             * On updating a command execution to one of the terminal states, it will be removed from here.
             *
             * @see
             * https://docs.aws.amazon.com/iot/latest/developerguide/iot-remote-command-concepts.html#iot-command-execution-status
             * for more information on IoT command statuses.
             */
            std::unordered_map<Aws::Crt::String, CommandExecutionContext> m_activeCommandExecutions;
        };

    } // namespace IotcommandsSample
} // namespace Aws
