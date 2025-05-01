/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/UUID.h>
#include <aws/crt/mqtt/Mqtt5Packets.h>
#include <aws/iot/Mqtt5Client.h>

#include "command_stream_handler.h"

#include <algorithm>
#include <cinttypes>
#include <condition_variable>
#include <iostream>
#include <thread>

#include "../../utils/CommandLineUtils.h"

struct ApplicationContext
{
    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> m_protocolClient;
    std::unique_ptr<Aws::IotcommandsSample::CommandStreamHandler> commandStreamHandler;
    Aws::Crt::String thingName;
    Aws::Crt::String clientId;
};

static Aws::Crt::String s_nibbleNextToken(Aws::Crt::String &input)
{
    Aws::Crt::String token;
    Aws::Crt::String remaining;
    auto delimPosition = input.find(' ');
    if (delimPosition != Aws::Crt::String::npos)
    {
        token = input.substr(0, delimPosition);

        auto untrimmedRemaining = input.substr(delimPosition, Aws::Crt::String::npos);
        auto firstNonSpacePosition = untrimmedRemaining.find_first_not_of(' ');
        if (firstNonSpacePosition != Aws::Crt::String::npos)
        {
            remaining = untrimmedRemaining.substr(firstNonSpacePosition, Aws::Crt::String::npos);
        }
        else
        {
            remaining = "";
        }
    }
    else
    {
        token = input;
        remaining = "";
    }

    input = remaining;
    return token;
}

static void s_printHelp()
{
    fprintf(stdout, "\nIoT Commands sample:\n\n");
    fprintf(stdout, "  open-thing-stream <payload-format>\n");
    fprintf(stdout, "                   subscribe to a stream of command executions with a specified payload format\n");
    fprintf(stdout, "                   targeting the IoT Thing set on the application startup\n");
    fprintf(stdout, "  open-client-stream <payload-format>\n");
    fprintf(stdout, "                   subscribe to a stream of command executions with a specified payload format\n");
    fprintf(stdout, "                   targeting the MQTT client ID set on the application startup\n");
    fprintf(stdout, "  update-command-execution <execution-id> <status> [<reason-code>] [<reason-description>]\n");
    fprintf(stdout, "                   update status for specified command execution ID;\n");
    fprintf(stdout, "                   <status> can be one of the following:\n");
    fprintf(stdout, "                       IN_PROGRESS, SUCCEEDED, REJECTED, FAILED, TIMED_OUT\n");
    fprintf(stdout, "                   <reason-code> and <reason-description> may be optionally provided for\n");
    fprintf(stdout, "                       the REJECTED, FAILED, or TIMED_OUT statuses\n");
    fprintf(stdout, "  list-streams     list all open streaming operations\n");
    fprintf(stdout, "  close-stream <stream-id>\n");
    fprintf(stdout, "                   close a specified stream;\n");
    fprintf(stdout, "                   <stream-id> is internal ID that can be found with 'list-streams'\n");
    fprintf(stdout, "  quit             quit the sample application\n");
}

static void s_handleOpenStream(
    Aws::Iotcommands::DeviceType deviceType,
    const Aws::Crt::String &params,
    ApplicationContext &context)
{
    Aws::Crt::String remaining = params;
    Aws::Crt::String payloadFormat = s_nibbleNextToken(remaining);

    if (payloadFormat.empty())
    {
        fprintf(stdout, "Invalid arguments to open-*-stream command!\n\n");
        s_printHelp();
        return;
    }

    switch (deviceType)
    {
        case Aws::Iotcommands::DeviceType::THING:
            context.commandStreamHandler->openCommandExecutionsStream(deviceType, context.thingName, payloadFormat);
            break;
        case Aws::Iotcommands::DeviceType::CLIENT:
            context.commandStreamHandler->openCommandExecutionsStream(deviceType, context.clientId, payloadFormat);
            break;
    }
}

static void s_handleUpdateCommandExecution(const Aws::Crt::String &params, ApplicationContext &context)
{
    Aws::Crt::String paramCopy = params;
    Aws::Crt::String commandExecutionId = s_nibbleNextToken(paramCopy);
    Aws::Crt::String statusStr = s_nibbleNextToken(paramCopy);

    if (commandExecutionId.empty() || statusStr.empty())
    {
        fprintf(stdout, "Invalid arguments to update-command-execution command!\n\n");
        s_printHelp();
        return;
    }

    // NOTE: For debug builds, invalid values will cause abort here.
    Aws::Iotcommands::CommandExecutionStatus status =
        Aws::Iotcommands::CommandExecutionStatusMarshaller::FromString(statusStr);

    Aws::Crt::String reasonCode = s_nibbleNextToken(paramCopy);
    Aws::Crt::String reasonDescription = paramCopy;

    context.commandStreamHandler->updateCommandExecutionStatus(
        commandExecutionId, status, reasonCode, reasonDescription);
}

static void s_handleListStreams(ApplicationContext &context)
{
    context.commandStreamHandler->listOpenedStreams();
}

static void s_handleCloseStream(const Aws::Crt::String &params, ApplicationContext &context)
{
    Aws::Crt::String remaining = params;
    Aws::Crt::String streamId = s_nibbleNextToken(remaining);

    if (streamId.length() == 0)
    {
        fprintf(stdout, "Invalid arguments to close-stream command!\n\n");
        s_printHelp();
        return;
    }

    uint64_t id = std::stoull(streamId.c_str());
    context.commandStreamHandler->closeStream(id);
}

static bool s_handleInput(const Aws::Crt::String &input, ApplicationContext &context)
{
    Aws::Crt::String remaining = input;
    Aws::Crt::String command = s_nibbleNextToken(remaining);

    if (command == "quit")
    {
        fprintf(stdout, "Quitting!\n");
        return true;
    }
    else if (command == "open-thing-stream")
    {
        s_handleOpenStream(Aws::Iotcommands::DeviceType::THING, remaining, context);
    }
    else if (command == "open-client-stream")
    {
        s_handleOpenStream(Aws::Iotcommands::DeviceType::CLIENT, remaining, context);
    }
    else if (command == "update-command-execution")
    {
        s_handleUpdateCommandExecution(remaining, context);
    }
    else if (command == "list-streams")
    {
        s_handleListStreams(context);
    }
    else if (command == "close-stream")
    {
        s_handleCloseStream(remaining, context);
    }
    else
    {
        s_printHelp();
    }

    return false;
}

int main(int argc, char *argv[])
{
    /************************ Setup ****************************/

    // Do the global initialization for the API.
    Aws::Crt::ApiHandle apiHandle;

    Utils::cmdData cmdData = Utils::parseSampleInputCommands(argc, argv, &apiHandle);

    // Create the MQTT5 builder and populate it with data from cmdData.
    auto builder = std::unique_ptr<Aws::Iot::Mqtt5ClientBuilder>(
        Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(
            cmdData.input_endpoint, cmdData.input_cert.c_str(), cmdData.input_key.c_str()));
    // Check if the builder setup correctly.
    if (builder == nullptr)
    {
        printf(
            "Failed to setup mqtt5 client builder with error code %d: %s",
            Aws::Crt::LastError(),
            Aws::Crt::ErrorDebugString(Aws::Crt::LastError()));
        return -1;
    }

    auto connectPacket =
        Aws::Crt::MakeShared<Aws::Crt::Mqtt5::ConnectPacket>(Aws::Crt::DefaultAllocatorImplementation());
    connectPacket->WithClientId(cmdData.input_clientId);

    builder->WithConnectOptions(connectPacket);

    std::promise<bool> connectedWaiter;

    // Setup lifecycle callbacks
    builder->WithClientConnectionSuccessCallback(
        [&connectedWaiter](const Aws::Crt::Mqtt5::OnConnectionSuccessEventData &)
        {
            fprintf(stdout, "Mqtt5 Client connection succeeded!\n");
            connectedWaiter.set_value(true);
        });
    builder->WithClientConnectionFailureCallback(
        [&connectedWaiter](const Aws::Crt::Mqtt5::OnConnectionFailureEventData &eventData)
        {
            fprintf(
                stdout,
                "Mqtt5 client connection attempt failed with error: %s.\n",
                aws_error_debug_str(eventData.errorCode));
            connectedWaiter.set_value(false);
        });

    auto protocolClient = builder->Build();
    if (!protocolClient)
    {
        printf(
            "Failed to create mqtt5 client with error code %d: %s",
            Aws::Crt::LastError(),
            Aws::Crt::ErrorDebugString(Aws::Crt::LastError()));
        return -1;
    }

    Aws::Iot::RequestResponse::RequestResponseClientOptions requestResponseOptions;
    requestResponseOptions.WithMaxRequestResponseSubscriptions(4);
    requestResponseOptions.WithMaxStreamingSubscriptions(10);
    requestResponseOptions.WithOperationTimeoutInSeconds(30);

    auto commandClient = Aws::Iotcommands::NewClientFrom5(*protocolClient, requestResponseOptions);
    auto commandStreamHandler =
        std::make_unique<Aws::IotcommandsSample::CommandStreamHandler>(std::move(commandClient));

    protocolClient->Start();
    auto isConnected = connectedWaiter.get_future().get();
    connectedWaiter = {};

    if (!isConnected)
    {
        return -1;
    }

    ApplicationContext context{
        std::move(protocolClient), std::move(commandStreamHandler), cmdData.input_thingName, cmdData.input_clientId};

    while (true)
    {
        fprintf(stdout, "\nEnter command:\n> ");

        Aws::Crt::String input;
        std::getline(std::cin, input);

        if (s_handleInput(input, context) || !std::cin)
        {
            fprintf(stdout, "Exiting...");
            break;
        }
    }

    return 0;
}
