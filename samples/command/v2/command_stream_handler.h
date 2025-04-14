/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <aws/iotcommand/CommandDeviceType.h>
#include <aws/iotcommand/IotCommandClientV2.h>

#include <memory>

namespace Aws
{
    namespace IotcommandSample
    {

        class CommandStreamHandler
        {
          public:
            explicit CommandStreamHandler(std::shared_ptr<Aws::Iotcommand::IClientV2> commandClient);

            bool openJsonStream(Aws::Iotcommand::CommandDeviceType deviceType, const Aws::Crt::String &deviceId);

            void listOpenedStreams();

            bool closeStream(uint64_t streamId);

          private:
            struct StreamingOperationWrapper
            {
                Aws::Iotcommand::CommandDeviceType deviceType;
                /**
                 * Depending on device type value, this field is either a thing name or a client ID.
                 */
                Aws::Crt::String deviceId;
                Aws::Crt::String payloadType;
                std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> stream;
            };

            void commandExecutionHandler(const Aws::Crt::String &executionId);

            void registerStream(
                uint64_t id,
                std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> &&operation,
                Aws::Iotcommand::CommandDeviceType deviceType,
                Aws::Crt::String deviceId);

            static void s_onSubscriptionStatusEvent(
                uint64_t streamId,
                Aws::Iot::RequestResponse::SubscriptionStatusEvent &&event);

            /**
             * Service client for IoT command.
             * All interactions with IoT command are performed via this object.
             */
            std::shared_ptr<Aws::Iotcommand::IClientV2> m_commandClient;

            /**
             * Opened streaming operations.
             */
            std::unordered_map<uint64_t, StreamingOperationWrapper> m_streams;

            uint64_t m_nextStreamId = 1;
        };

    } // namespace IotcommandSample
} // namespace Aws
