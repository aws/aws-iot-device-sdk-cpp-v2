/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "command_stream_handler.h"

#include <aws/crt/Api.h>
#include <aws/iotcommand/CommandExecutionsEvent.h>
#include <aws/iotcommand/CommandExecutionsSubscriptionRequest.h>
#include <aws/iotcommand/RejectedErrorCode.h>
#include <aws/iotcommand/UpdateCommandExecutionRequest.h>
#include <aws/iotcommand/UpdateCommandExecutionResponse.h>
#include <aws/iotcommand/V2ErrorResponse.h>

#include <cinttypes>
#include <cstdio>
#include <utility>

namespace Aws
{
    namespace IotcommandSample
    {

        CommandStreamHandler::CommandStreamHandler(std::shared_ptr<Aws::Iotcommand::IClientV2> commandClient)
            : m_commandClient(std::move(commandClient))
        {
        }

        bool CommandStreamHandler::openJsonStream(
            Aws::Iotcommand::CommandDeviceType deviceType,
            const Aws::Crt::String &deviceId)
        {
            Aws::Iotcommand::CommandExecutionsSubscriptionRequest request;
            request.DeviceType = deviceType;
            request.DeviceId = deviceId;

            Aws::Iot::RequestResponse::StreamingOperationOptions<Aws::Iotcommand::CommandExecutionsEvent> options;
            options.WithStreamHandler(
                [](Aws::Iotcommand::CommandExecutionsEvent &&event)
                {
                    fprintf(stdout, "Received new command:\n  execution ID: '%s'\n", event.ExecutionId->c_str());
                    if (event.ContentType)
                    {
                        fprintf(stdout, "  payload format: '%s'\n", event.ContentType->c_str());
                    }
                    if (event.Timeout) {
                        fprintf(stdout, "  execution timeout: %d\n", *event.Timeout);
                    }
                    if (event.Payload) {
                        fprintf(stdout, "  payload size: %lu\n", event.Payload->size());
                    }
                });
            auto streamId = m_nextStreamId++;
            options.WithSubscriptionStatusEventHandler(
                [streamId](Aws::Iot::RequestResponse::SubscriptionStatusEvent &&event)
                { s_onSubscriptionStatusEvent(streamId, std::move(event)); });

            auto operation = m_commandClient->CreateCommandExecutionsJsonPayloadStream(request, options);
            registerStream(streamId, std::move(operation), deviceType, deviceId);

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
                    Aws::Iotcommand::CommandDeviceTypeMarshaller::ToString(wrapper.deviceType),
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

        void CommandStreamHandler::commandExecutionHandler(const Aws::Crt::String &executionId)
        {
            std::promise<void> updatePromise;
            Aws::Iotcommand::UpdateCommandExecutionRequest request;
            request.DeviceType = Aws::Iotcommand::CommandDeviceType::THING;
            request.DeviceId = "laptop_test_0001";
            request.ExecutionId = executionId;
            request.Status = Aws::Iotcommand::CommandStatus::SUCCEEDED;
            Aws::Iotcommand::StatusReason statusReason;
            statusReason.ReasonCode = "I-CAN-FAIL-TOO";
            statusReason.ReasonDescription = "But I want to succeed...";
            request.StatusReason = statusReason;
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
                        // ==== LoadFromObject: '{"error":"ResourceNotFound","errorMessage":"The command execution
                        // kokoko was not found.","executionId":"kokoko"}'
                        // ==== LoadFromObject: '{"error":"InvalidStateTransition","errorMessage":"Command execution
                        // status cannot be updated to CREATED.","executionId":"12fa1636-f9a9-442f-a367-e311db5d4e73"}'
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

        void CommandStreamHandler::registerStream(
            uint64_t id,
            std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> &&operation,
            Aws::Iotcommand::CommandDeviceType deviceType,
            Aws::Crt::String deviceId)
        {
            StreamingOperationWrapper wrapper;
            wrapper.stream = std::move(operation);

            wrapper.deviceType = deviceType;
            wrapper.deviceId = std::move(deviceId);

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

    } // namespace IotcommandSample
} // namespace Aws
