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
    namespace IotcommandSample
    {

        CommandStreamHandler::CommandStreamHandler(std::shared_ptr<Aws::Iotcommands::IClientV2> &&commandClient)
            : m_commandClient(std::move(commandClient))
        {
            m_commandExecutor = std::make_shared<CommandExecutor>(m_commandClient);
        }

        bool CommandStreamHandler::openStream(
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

                    CommandExecutionContext commandExecution{
                        std::move(deviceType), std::move(deviceId), std::move(event)};
                    m_commandExecutor->enqueueCommandForExecution(std::move(commandExecution));
                });

            auto streamId = nextStreamId++;
            options.WithSubscriptionStatusEventHandler(
                [streamId](Aws::Iot::RequestResponse::SubscriptionStatusEvent &&event)
                { s_onSubscriptionStatusEvent(streamId, std::move(event)); });

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

    } // namespace IotcommandSample
} // namespace Aws
