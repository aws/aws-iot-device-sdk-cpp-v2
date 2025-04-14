/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <aws/iotcommands/CommandExecutionEvent.h>
#include <aws/iotcommands/IotCommandsClientV2.h>
#include <aws/iotcommands/UpdateCommandExecutionRequest.h>

#include <deque>
#include <random>
#include <thread>

#include "command_execution_context.h"

namespace Aws
{
    namespace IotcommandSample
    {

        /**
         * Sample class for processing received command executions and updating their statuses in IoT Core.
         */
        class CommandExecutor
        {
          public:
            /**
             * Constructor.
             *
             * Command client instance is needed because all interactions with IoT command service are performed via
             * an Aws::Iotcommand::IClientV2 instance.
             *
             * @param commandClient Instance of the client for the IoT command service.
             */
            explicit CommandExecutor(std::shared_ptr<Aws::Iotcommands::IClientV2> commandClient);

            ~CommandExecutor();

            /**
             * Enqueue a given command execution for processing.
             * @param commandExecution Command execution data.
             */
            void enqueueCommandForExecution(CommandExecutionContext &&commandExecution);

          private:
            std::pair<Aws::Iotcommands::CommandExecutionStatus, Aws::Crt::Optional<Aws::Iotcommands::StatusReason>>
                validateCommand(const CommandExecutionContext &commandExecution);

            std::pair<Aws::Iotcommands::CommandExecutionStatus, Aws::Crt::Optional<Aws::Iotcommands::StatusReason>>
                processCommand(const CommandExecutionContext &commandExecution);

            void commandStatusUpdaterThread();

            void updateCommandExecutionStatus(const Aws::Iotcommands::UpdateCommandExecutionRequest &request);

            /**
             * Service client for IoT command.
             * All interactions with IoT command are performed via this object.
             */
            std::shared_ptr<Aws::Iotcommands::IClientV2> m_commandClient;

            std::thread m_commandStatusUpdater;
            std::atomic_bool m_isRunning;
            std::mutex m_updateMutex;
            std::condition_variable m_commandSignal;
            std::deque<CommandExecutionContext> m_commandExecutions;
            std::random_device m_rd;
            std::mt19937 m_gen;
        };

    } // namespace IotcommandSample
} // namespace Aws
