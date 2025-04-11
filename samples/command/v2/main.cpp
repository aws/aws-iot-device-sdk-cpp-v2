/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/mqtt/Mqtt5Packets.h>
#include <aws/iot/Mqtt5Client.h>

#include <aws/iotcommand/CommandExecutionsEvent.h>
#include <aws/iotcommand/CommandExecutionsSubscriptionRequest.h>
#include <aws/iotcommand/IotCommandClientV2.h>
#include <aws/iotcommand/RejectedErrorCode.h>
#include <aws/iotcommand/UpdateCommandExecutionRequest.h>
#include <aws/iotcommand/UpdateCommandExecutionResponse.h>
#include <aws/iotcommand/V2ErrorResponse.h>

#include <algorithm>
#include <cinttypes>
#include <condition_variable>
#include <iostream>
#include <thread>

#include "../../utils/CommandLineUtils.h"

using namespace Aws::Crt;
using namespace Aws::Iotcommand;

struct StreamingOperationWrapper
{
    Aws::Crt::String m_thingName;

    Aws::Crt::String m_commandName;

    Aws::Crt::String m_type;

    std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> m_stream;
};

struct ApplicationContext
{

    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> m_protocolClient;

    std::shared_ptr<Aws::Iotcommand::IClientV2> m_commandClient;

    uint64_t m_nextStreamId;

    std::unordered_map<uint64_t, StreamingOperationWrapper> m_streams;
};

static void s_onConnectionSuccess(const Mqtt5::OnConnectionSuccessEventData &eventData)
{
    fprintf(stdout, "Mqtt5 Client connection succeeded!\n");
}

static void s_onConnectionFailure(const Mqtt5::OnConnectionFailureEventData &eventData)
{
    fprintf(
        stdout, "Mqtt5 client connection attempt failed with error: %s.\n", aws_error_debug_str(eventData.errorCode));
}

static void s_onStopped(const Mqtt5::OnStoppedEventData &event)
{
    fprintf(stdout, "Protocol client stopped.\n");
}

static void s_onSubscriptionStatusEvent(uint64_t id, Aws::Iot::RequestResponse::SubscriptionStatusEvent &&event)
{
    fprintf(
        stdout,
        "Stream %" PRIu64 ": subscription status event with type %d and error %s\n",
        id,
        event.GetErrorCode(),
        Aws::Crt::ErrorDebugString(event.GetErrorCode()));
}

void updateExecution(const Aws::Crt::String &executionId) {}

int main(int argc, char *argv[])
{
    /************************ Setup ****************************/

    // Do the global initialization for the API.
    ApiHandle apiHandle;
    apiHandle.InitializeLogging(Aws::Crt::LogLevel::Debug, stderr);

    Utils::cmdData cmdData = Utils::parseSampleInputBasicConnect(argc, argv, &apiHandle);

    // Create the MQTT5 builder and populate it with data from cmdData.
    auto builder = std::unique_ptr<Aws::Iot::Mqtt5ClientBuilder>(
        Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(
            cmdData.input_endpoint, cmdData.input_cert.c_str(), cmdData.input_key.c_str()));
    // Check if the builder setup correctly.
    if (builder == nullptr)
    {
        printf(
            "Failed to setup mqtt5 client builder with error code %d: %s", LastError(), ErrorDebugString(LastError()));
        return -1;
    }

    // Setup lifecycle callbacks
    builder->WithClientConnectionSuccessCallback(s_onConnectionSuccess);
    builder->WithClientConnectionFailureCallback(s_onConnectionFailure);
    builder->WithClientStoppedCallback(s_onStopped);

    Aws::Iot::RequestResponse::RequestResponseClientOptions requestResponseOptions;
    requestResponseOptions.WithMaxRequestResponseSubscriptions(4);
    requestResponseOptions.WithMaxStreamingSubscriptions(10);
    requestResponseOptions.WithOperationTimeoutInSeconds(30);

    ApplicationContext context;
    context.m_protocolClient = builder->Build();
    context.m_commandClient = Aws::Iotcommand::NewClientFrom5(*context.m_protocolClient, requestResponseOptions);
    context.m_nextStreamId = 1;

    context.m_protocolClient->Start();

    Aws::Crt::String executionId;

    {
        std::promise<void> executionIdPromise;
        Aws::Iotcommand::CommandExecutionsSubscriptionRequest request;
        request.DeviceType = Aws::Iotcommand::CommandDeviceType::THING;
        request.DeviceId = "laptop_test_0001";

        Aws::Iot::RequestResponse::StreamingOperationOptions<Aws::Iotcommand::CommandExecutionsEvent> options;
        options.WithStreamHandler(
            [&executionIdPromise, &executionId](CommandExecutionsEvent &&event)
            {
                fprintf(stdout, "==== Got new event %s\n", event.ExecutionId->c_str());
                executionId = *event.ExecutionId;
                executionIdPromise.set_value();
            });
        uint32_t streamId = 8;
        options.WithSubscriptionStatusEventHandler(
            [streamId](Aws::Iot::RequestResponse::SubscriptionStatusEvent &&event)
            { s_onSubscriptionStatusEvent(streamId, std::move(event)); });

        auto operation = context.m_commandClient->CreateCommandExecutionsStream(request, options);
        operation->Open();

        fprintf(stdout, "==== waiting for a command\n");
        executionIdPromise.get_future().wait();
        fprintf(stdout, "==== received new command\n");
    }

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
        context.m_commandClient->UpdateCommandExecution(
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
                    // ==== LoadFromObject: '{"error":"ResourceNotFound","errorMessage":"The command execution kokoko
                    // was not found.","executionId":"kokoko"}'
                    // ==== LoadFromObject: '{"error":"InvalidStateTransition","errorMessage":"Command execution status
                    // cannot be updated to CREATED.","executionId":"12fa1636-f9a9-442f-a367-e311db5d4e73"}'
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
                                RejectedErrorCodeMarshaller::ToString(*result.GetError().GetModeledError().Error));
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

    return 0;
}
