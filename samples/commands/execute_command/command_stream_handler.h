/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <aws/iotcommands/DeviceType.h>
#include <aws/iotcommands/IotCommandsClientV2.h>

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>

#include "command_executor.h"

namespace Aws
{
    namespace IotcommandSample
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
            bool openStream(
                Aws::Iotcommands::DeviceType deviceType,
                const Aws::Crt::String &deviceId,
                const Aws::Crt::String &payloadFormat);

            void listOpenedStreams();

            bool closeStream(uint64_t streamId);

          private:
            struct StreamingOperation
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
                 * the cases when payload format is unknown, a generic topic is used.
                 * TODO Describe this in the service itself.
                 * TODO Provide example?
                 */
                Aws::Crt::String payloadType;
            };

            void registerStream(
                uint64_t id,
                std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> &&operation,
                Aws::Iotcommands::DeviceType deviceType,
                Aws::Crt::String deviceId);

            static void s_onSubscriptionStatusEvent(
                uint64_t streamId,
                Aws::Iot::RequestResponse::SubscriptionStatusEvent &&event);

            /**
             * Service client for IoT command.
             * All interactions with IoT command are performed via this object.
             */
            std::shared_ptr<Aws::Iotcommands::IClientV2> m_commandClient;

            /**
             * TODO
             */
            std::shared_ptr<CommandExecutor> m_commandExecutor;

            /**
             * Opened streaming operations.
             */
            std::unordered_map<uint64_t, StreamingOperation> m_streams;
        };

    } // namespace IotcommandSample
} // namespace Aws
