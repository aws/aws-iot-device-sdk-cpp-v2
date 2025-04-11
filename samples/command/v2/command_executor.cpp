/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "command_executor.h"

#include <aws/iotcommand/CommandExecutionEvent.h>
#include <aws/iotcommand/CommandExecutionsSubscriptionRequest.h>
#include <aws/iotcommand/RejectedErrorCode.h>
#include <aws/iotcommand/UpdateCommandExecutionRequest.h>
#include <aws/iotcommand/UpdateCommandExecutionResponse.h>
#include <aws/iotcommand/V2ErrorResponse.h>

namespace Aws
{
    namespace IotcommandSample
    {
        CommandExecutor::CommandExecutor(std::shared_ptr<Aws::Iotcommand::IClientV2> commandClient)
            : m_commandClient(std::move(commandClient)),
              m_commandStatusUpdater(&CommandExecutor::commandStatusUpdaterThread, this), m_isRunning(true)
        {
        }

        CommandExecutor::~CommandExecutor()
        {
            m_isRunning = false;
            m_commandSignal.notify_one();
            m_commandStatusUpdater.join();
        }

        void CommandExecutor::executeCommand(CommandExecutionContext &&commandExecution)
        {
            {
                std::lock_guard<std::mutex> lock(m_updateMutex);
                m_commandExecutions.push_back(std::move(commandExecution));
            }
            m_commandSignal.notify_one();
        }

        void CommandExecutor::commandStatusUpdaterThread()
        {
            while (m_isRunning)
            {
                {
                    std::unique_lock<std::mutex> lock(m_updateMutex);
                    m_commandSignal.wait(lock, [this]() { return !m_isRunning || !m_commandExecutions.empty(); });
                }

                if (m_commandExecutions.empty())
                {
                    continue;
                }

                CommandExecutionContext commandExecution;

                {
                    std::lock_guard<std::mutex> lock(m_updateMutex);
                    commandExecution = m_commandExecutions.front();
                    m_commandExecutions.pop_front();
                }

                fprintf(stdout, "Updating command execution '%s'\n", commandExecution.event.ExecutionId->c_str());
                updateCommandExecution(std::move(commandExecution));
            }
        }

        void CommandExecutor::updateCommandExecution(CommandExecutionContext &&commandExecutionContext)
        {
            std::promise<void> updatePromise;
            Aws::Iotcommand::UpdateCommandExecutionRequest request;
            request.DeviceType = commandExecutionContext.deviceType;
            request.DeviceId = commandExecutionContext.deviceId;
            request.ExecutionId = commandExecutionContext.event.ExecutionId;

            request.Status = Aws::Iotcommand::CommandStatus::SUCCEEDED;

            m_commandClient->UpdateCommandExecution(
                request,
                [&updatePromise](Aws::Iotcommand::UpdateCommandExecutionResult &&result)
                {
                    if (result.IsSuccess())
                    {
                        fprintf(
                            stdout,
                            "========= Successfully updated execution for ID %s\n",
                            result.GetResponse().ExecutionId->c_str());
                    }
                    else
                    {
                        fprintf(stdout, "========= Error: internal code: %d\n", result.GetError().GetErrorCode());
                        if (result.GetError().HasModeledError())
                        {
                            if (result.GetError().GetModeledError().ErrorMessage)
                            {
                                fprintf(
                                    stdout,
                                    "========= Error: message %s\n",
                                    result.GetError().GetModeledError().ErrorMessage->c_str());
                            }
                            if (result.GetError().GetModeledError().Error)
                            {
                                fprintf(
                                    stdout,
                                    "========= Error: code: %d\n",
                                    static_cast<int>(*result.GetError().GetModeledError().Error));
                                fprintf(
                                    stdout,
                                    "========= Error: code str: %s\n",
                                    Aws::Iotcommand::RejectedErrorCodeMarshaller::ToString(
                                        *result.GetError().GetModeledError().Error));
                            }
                            if (result.GetError().GetModeledError().ExecutionId)
                            {
                                fprintf(
                                    stdout,
                                    "========= Error: execution ID: %s\n",
                                    result.GetError().GetModeledError().ExecutionId->c_str());
                            }
                        }
                    }
                    updatePromise.set_value();
                });

            fprintf(stdout, "==== waiting for update\n");
            updatePromise.get_future().wait();
            fprintf(stdout, "==== updated\n");
        }

    } // namespace IotcommandSample
} // namespace Aws
