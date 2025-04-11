/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <aws/iotcommand/CommandExecutionEvent.h>
#include <aws/iotcommand/IotCommandClientV2.h>

#include <deque>
#include <thread>

#include "command_execution_context.h"

namespace Aws
{
    namespace IotcommandSample
    {

        class CommandExecutor
        {
          public:
            explicit CommandExecutor(std::shared_ptr<Aws::Iotcommand::IClientV2> commandClient);
            ~CommandExecutor();

            void executeCommand(CommandExecutionContext &&commandExecution);

          private:
            void commandStatusUpdaterThread();

            void updateCommandExecution(CommandExecutionContext &&commandExecution);

            /**
             * Service client for IoT command.
             * All interactions with IoT command are performed via this object.
             */
            std::shared_ptr<Aws::Iotcommand::IClientV2> m_commandClient;

            std::thread m_commandStatusUpdater;
            std::atomic_bool m_isRunning;
            std::mutex m_updateMutex;
            std::condition_variable m_commandSignal;
            std::deque<CommandExecutionContext> m_commandExecutions;
        };

    } // namespace IotcommandSample
} // namespace Aws
