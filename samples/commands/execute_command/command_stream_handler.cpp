/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "command_stream_handler.h"

#include <aws/crt/Api.h>
#include <aws/iotcommands/CommandExecutionEvent.h>
#include <aws/iotcommands/CommandExecutionsSubscriptionRequest.h>
#include <aws/iotcommands/RejectedErrorCode.h>
#include <aws/iotcommands/UpdateCommandExecutionRequest.h>
#include <aws/iotcommands/UpdateCommandExecutionResponse.h>
#include <aws/iotcommands/V2ErrorResponse.h>

#include <cinttypes>
#include <cstdio>
#include <utility>

namespace Aws
{
    namespace IotcommandsSample
    {

        CommandStreamHandler::CommandStreamHandler(std::shared_ptr<Aws::Iotcommands::IClientV2> &&commandClient)
            : m_commandClient(std::move(commandClient))
        {
        }

        bool CommandStreamHandler::openCommandExecutionsStream(
            Aws::Iotcommands::DeviceType deviceType,
            const Aws::Crt::String &deviceId,
            const Aws::Crt::String &payloadFormat)
        {
            static uint64_t nextStreamId = 1;

            Aws::Iotcommands::CommandExecutionsSubscriptionRequest request;
            request.DeviceType = deviceType;
            request.DeviceId = deviceId;

            Aws::Iot::RequestResponse::StreamingOperationOptions<Aws::Iotcommands::CommandExecutionEvent> options;
            options.WithStreamHandler(
                [this, deviceType, deviceId, payloadFormat](Aws::Iotcommands::CommandExecutionEvent &&event)
                {
                    fprintf(
                        stdout,
                        "Received new command for '%s' payload format:\n  execution ID: '%s'\n",
                        payloadFormat.c_str(),
                        event.ExecutionId->c_str());
                    if (event.ContentType)
                    {
                        fprintf(stdout, "  payload format: '%s'\n", event.ContentType->c_str());
                    }
                    if (event.Timeout)
                    {
                        fprintf(stdout, "  execution timeout: %d\n", *event.Timeout);
                    }
                    if (event.Payload)
                    {
                        fprintf(stdout, "  payload size: %lu\n", event.Payload->size());
                    }

                    CommandExecutionContext commandExecution{deviceType, deviceId, std::move(event)};
                    std::lock_guard<std::mutex> lock(m_activeExecutionsMutex);
                    m_activeCommandExecutions.insert(
                        {*commandExecution.event.ExecutionId, std::move(commandExecution)});
                });

            auto streamId = nextStreamId++;
            options.WithSubscriptionStatusEventHandler(
                [streamId](Aws::Iot::RequestResponse::SubscriptionStatusEvent &&event)
                { s_onSubscriptionStatusEvent(streamId, std::move(event)); });

            // NOTE It might be a good idea to track which streams are already open, to prevent opening the same stream
            // multiple times.
            // In a real-world application, `deviceType` and `deviceId` most probably won't be changing. So, to track
            // opened streams, the application needs to check `payloadFormat`.
            // Notice that Aws IoT Commands service distinguishes only JSON and CBOR, all other payload format will be
            // routed to the generic stream.

            std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> operation;
            if (payloadFormat == "json")
            {
                operation = m_commandClient->CreateCommandExecutionsJsonPayloadStream(request, options);
            }
            else if (payloadFormat == "cbor")
            {
                operation = m_commandClient->CreateCommandExecutionsCborPayloadStream(request, options);
            }
            else
            {
                operation = m_commandClient->CreateCommandExecutionsGenericPayloadStream(request, options);
            }

            registerStream(streamId, std::move(operation), deviceType, deviceId, payloadFormat);

            return true;
        }

        bool CommandStreamHandler::updateCommandExecutionStatus(
            const Crt::String &executionId,
            Aws::Iotcommands::CommandExecutionStatus status,
            const Aws::Crt::String &reasonCode,
            const Aws::Crt::String &reasonDescription)
        {
            CommandExecutionContext commandExecutionContext;

            {
                std::lock_guard<std::mutex> lock(m_activeExecutionsMutex);
                auto iter = m_activeCommandExecutions.find(executionId);
                if (iter == m_activeCommandExecutions.end())
                {
                    fprintf(
                        stdout,
                        "Failed to update command execution status: unknown command execution ID '%s'\n",
                        executionId.c_str());
                    return false;
                }

                commandExecutionContext = iter->second;
            }

            Aws::Iotcommands::UpdateCommandExecutionRequest request;
            request.Status = status;
            request.DeviceType = commandExecutionContext.deviceType;
            request.DeviceId = commandExecutionContext.deviceId;
            request.ExecutionId = commandExecutionContext.event.ExecutionId;

            if (!reasonCode.empty())
            {
                request.StatusReason = Aws::Iotcommands::StatusReason();
                request.StatusReason->ReasonCode = reasonCode;
                request.StatusReason->ReasonDescription = reasonDescription;
            }

            fprintf(stdout, "Updating command execution '%s'\n", commandExecutionContext.event.ExecutionId->c_str());

            std::promise<void> updateWaiter;
            m_commandClient->UpdateCommandExecution(
                request,
                [executionId, &updateWaiter](Aws::Iotcommands::UpdateCommandExecutionResult &&result)
                {
                    if (result.IsSuccess())
                    {
                        fprintf(
                            stdout,
                            "Successfully updated execution status for '%s'\n",
                            result.GetResponse().ExecutionId->c_str());
                    }
                    else
                    {
                        fprintf(
                            stdout,
                            "Failed to update execution status: response code %d\n",
                            result.GetError().GetErrorCode());

                        if (result.GetError().HasModeledError())
                        {
                            if (result.GetError().GetModeledError().ExecutionId)
                            {
                                fprintf(
                                    stdout,
                                    "Failed to update execution status: execution ID %s\n",
                                    result.GetError().GetModeledError().ExecutionId->c_str());
                            }
                            if (result.GetError().GetModeledError().ErrorMessage)
                            {
                                fprintf(
                                    stdout,
                                    "Failed to update execution status: error message %s\n",
                                    result.GetError().GetModeledError().ErrorMessage->c_str());
                            }
                            if (result.GetError().GetModeledError().Error)
                            {
                                fprintf(
                                    stdout,
                                    "Failed to update execution status: error code %d (%s)\n",
                                    static_cast<int>(*result.GetError().GetModeledError().Error),
                                    Aws::Iotcommands::RejectedErrorCodeMarshaller::ToString(
                                        *result.GetError().GetModeledError().Error));
                            }
                        }
                    }
                    updateWaiter.set_value();
                });

            updateWaiter.get_future().wait();

            switch (status)
            {
                case Iotcommands::CommandExecutionStatus::SUCCEEDED:
                case Iotcommands::CommandExecutionStatus::FAILED:
                case Iotcommands::CommandExecutionStatus::REJECTED:
                {
                    // NOTE: After the command execution reaches its terminal state, it should be removed from the set
                    // of active command executions. However, for educational purposes, this functionality is disabled,
                    // so it's possible to try to update the same command execution even after a terminal state is set.
                    constexpr bool ENABLE_REMOVING_COMMAND_EXECUTIONS = false;
                    if (ENABLE_REMOVING_COMMAND_EXECUTIONS)
                    {
                        std::lock_guard<std::mutex> lock(m_activeExecutionsMutex);
                        m_activeCommandExecutions.erase(executionId);
                    }
                }
                break;
                case Iotcommands::CommandExecutionStatus::IN_PROGRESS:
                case Iotcommands::CommandExecutionStatus::TIMED_OUT:
                    break;
            }

            return true;
        }

        void CommandStreamHandler::listOpenedStreams()
        {
            fprintf(stdout, "Streams:\n");
            for (const auto &iter : m_streams)
            {
                uint64_t streamId = iter.first;
                const StreamingOperationWrapper &wrapper = iter.second;
                fprintf(
                    stdout,
                    "  %" PRIu64 ": device type '%s', device ID '%s', payload type '%s'\n",
                    streamId,
                    Aws::Iotcommands::DeviceTypeMarshaller::ToString(wrapper.deviceType),
                    wrapper.deviceId.c_str(),
                    wrapper.payloadType.c_str());
            }
        }

        bool CommandStreamHandler::closeStream(uint64_t streamId)
        {
            fprintf(stdout, "Closing stream %" PRIu64 "\n", streamId);
            m_streams.erase(streamId);
            return true;
        }

        void CommandStreamHandler::registerStream(
            uint64_t id,
            std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> &&operation,
            Aws::Iotcommands::DeviceType deviceType,
            const Aws::Crt::String &deviceId,
            const Aws::Crt::String &payloadFormat)
        {
            StreamingOperationWrapper wrapper;
            wrapper.stream = std::move(operation);

            wrapper.deviceType = deviceType;
            wrapper.deviceId = deviceId;
            wrapper.payloadType = payloadFormat;

            m_streams[id] = std::move(wrapper);

            m_streams[id].stream->Open();
        }

        void CommandStreamHandler::s_onSubscriptionStatusEvent(
            uint64_t streamId,
            Aws::Iot::RequestResponse::SubscriptionStatusEvent &&event)
        {
            fprintf(
                stdout,
                "Stream %" PRIu64 ": subscription status event with type %d and status code %s\n",
                streamId,
                event.GetErrorCode(),
                Aws::Crt::ErrorDebugString(event.GetErrorCode()));
        }

    } // namespace IotcommandsSample
} // namespace Aws
