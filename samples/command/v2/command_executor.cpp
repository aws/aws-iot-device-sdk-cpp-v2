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

#include <random>

namespace Aws
{
    namespace IotcommandSample
    {
        CommandExecutor::CommandExecutor(std::shared_ptr<Aws::Iotcommand::IClientV2> commandClient)
            : m_commandClient(std::move(commandClient)),
              m_commandStatusUpdater(&CommandExecutor::commandStatusUpdaterThread, this), m_isRunning(true),
              m_gen(m_rd())
        {
        }

        CommandExecutor::~CommandExecutor()
        {
            {
                std::lock_guard<std::mutex> lock(m_updateMutex);
                m_isRunning = false;
            }
            m_commandSignal.notify_one();
            m_commandStatusUpdater.join();
        }

        void CommandExecutor::enqueueCommandForExecution(CommandExecutionContext &&commandExecution)
        {
            {
                std::lock_guard<std::mutex> lock(m_updateMutex);
                m_commandExecutions.push_back(std::move(commandExecution));
            }
            m_commandSignal.notify_one();
        }

        std::pair<Aws::Iotcommand::CommandStatus, Aws::Crt::Optional<Aws::Iotcommand::StatusReason>> CommandExecutor::
            validateCommand(const CommandExecutionContext &commandExecution)
        {
            (void)commandExecution;

            std::uniform_int_distribution<> successDistrib(0, 3);
            bool isSuccess = (successDistrib(m_gen) > 0);
            if (!isSuccess)
            {
                Aws::Iotcommand::StatusReason statusReason;
                statusReason.ReasonCode = "VALIDATION_FAILED";
                statusReason.ReasonDescription = "A longer explanation: it's pure random";
                fprintf(
                    stdout,
                    "[%s] Failed to validate command: %s (%s)\n",
                    commandExecution.event.ExecutionId->c_str(),
                    statusReason.ReasonCode->c_str(),
                    statusReason.ReasonDescription->c_str());
                return {Aws::Iotcommand::CommandStatus::REJECTED, std::move(statusReason)};
            }

            return {Aws::Iotcommand::CommandStatus::SUCCEEDED, {}};
        }

        std::pair<Aws::Iotcommand::CommandStatus, Aws::Crt::Optional<Aws::Iotcommand::StatusReason>> CommandExecutor::
            processCommand(const CommandExecutionContext &commandExecution)
        {
            (void)commandExecution;

            std::uniform_int_distribution<> successDistrib(0, 3);
            bool isSuccess = (successDistrib(m_gen) > 0);
            if (!isSuccess)
            {
                Aws::Iotcommand::StatusReason statusReason;
                statusReason.ReasonCode = "EXECUTION_FAILED";
                statusReason.ReasonDescription = "A longer explanation: it's still pure random";
                fprintf(
                    stdout,
                    "[%s] Failed to process command: %s (%s)\n",
                    commandExecution.event.ExecutionId->c_str(),
                    statusReason.ReasonCode->c_str(),
                    statusReason.ReasonDescription->c_str());
                return {Aws::Iotcommand::CommandStatus::FAILED, std::move(statusReason)};
            }

            // Some random work that takes 1 to max_seconds_to_sleep seconds.
            int max_seconds_to_sleep = 20;
            std::uniform_int_distribution<> sleepDistrib(1, max_seconds_to_sleep);
            int seconds_to_sleep = sleepDistrib(m_gen);
            fprintf(
                stdout,
                "[%s] Processing command... it'll take %d seconds\n",
                commandExecution.event.ExecutionId->c_str(),
                seconds_to_sleep);
            std::this_thread::sleep_for(std::chrono::seconds(seconds_to_sleep));

            return {Aws::Iotcommand::CommandStatus::SUCCEEDED, {}};
        }

        void CommandExecutor::commandStatusUpdaterThread()
        {
            while (m_isRunning)
            {
                {
                    std::unique_lock<std::mutex> lock(m_updateMutex);
                    m_commandSignal.wait(lock, [this]() { return !m_isRunning.load() || !m_commandExecutions.empty(); });
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

                Aws::Iotcommand::UpdateCommandExecutionRequest request;

                request.Status = Aws::Iotcommand::CommandStatus::SUCCEEDED;
                request.DeviceType = commandExecution.deviceType;
                request.DeviceId = commandExecution.deviceId;
                request.ExecutionId = commandExecution.event.ExecutionId;

                fprintf(stdout, "[%s] Validating command\n", commandExecution.event.ExecutionId->c_str());
                std::tie(request.Status, request.StatusReason) = validateCommand(commandExecution);
                if (*request.Status == Aws::Iotcommand::CommandStatus::SUCCEEDED)
                {
                    fprintf(stdout, "[%s] Executing command\n", commandExecution.event.ExecutionId->c_str());
                    std::tie(request.Status, request.StatusReason) = processCommand(commandExecution);
                }

                fprintf(stdout, "[%s] Updating command execution\n", commandExecution.event.ExecutionId->c_str());
                updateCommandExecutionStatus(request);
            }
        }

        void CommandExecutor::updateCommandExecutionStatus(
            const Aws::Iotcommand::UpdateCommandExecutionRequest &request)
        {
            m_commandClient->UpdateCommandExecution(
                request,
                [](Aws::Iotcommand::UpdateCommandExecutionResult &&result)
                {
                    if (result.IsSuccess())
                    {
                        fprintf(
                            stdout,
                            "[%s] Successfully updated execution status\n",
                            result.GetResponse().ExecutionId->c_str());
                    }
                    else
                    {
                        fprintf(
                            stdout,
                            "[%s] Failed to update execution status: response code %d\n",
                            result.GetResponse().ExecutionId->c_str(),
                            result.GetError().GetErrorCode());

                        if (result.GetError().HasModeledError())
                        {
                            if (result.GetError().GetModeledError().ErrorMessage)
                            {
                                fprintf(
                                    stdout,
                                    "[%s] Failed to update execution status: error message %s\n",
                                    result.GetResponse().ExecutionId->c_str(),
                                    result.GetError().GetModeledError().ErrorMessage->c_str());
                            }
                            if (result.GetError().GetModeledError().Error)
                            {
                                fprintf(
                                    stdout,
                                    "[%s] Failed to update execution status: error code %d (%s)\n",
                                    result.GetResponse().ExecutionId->c_str(),
                                    static_cast<int>(*result.GetError().GetModeledError().Error),
                                    Aws::Iotcommand::RejectedErrorCodeMarshaller::ToString(
                                        *result.GetError().GetModeledError().Error));
                            }

                            if (result.GetError().GetModeledError().ExecutionId)
                            {
                                fprintf(
                                    stdout,
                                    "[%s] Failed to update execution status: execution ID %s\n",
                                    result.GetResponse().ExecutionId->c_str(),
                                    result.GetError().GetModeledError().ExecutionId->c_str());
                            }
                        }
                    }
                });
        }

    } // namespace IotcommandSample
} // namespace Aws
