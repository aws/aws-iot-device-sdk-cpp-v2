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

using namespace Aws::Crt;

struct ApplicationContext
{
    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> m_protocolClient;
    Aws::IotcommandsSample::CommandStreamHandler commandStreamHandler;
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
    fprintf(stdout, "  list-streams                lists all open streaming operations\n");
    fprintf(stdout, "  close-stream <stream-id>    closes a streaming operation\n");
    fprintf(stdout, "  quit                        quits the program\n");
}

static void s_handleListStreams(ApplicationContext &context)
{
    context.commandStreamHandler.listOpenedStreams();
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
    context.commandStreamHandler.closeStream(id);
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
    ApiHandle apiHandle;

    Utils::cmdData cmdData = Utils::parseSampleInputCommands(argc, argv, &apiHandle);

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

    auto connectPacket = MakeShared<Mqtt5::ConnectPacket>(DefaultAllocatorImplementation());
    connectPacket->WithClientId(cmdData.input_clientId);

    builder->WithConnectOptions(connectPacket);

    std::promise<bool> connectedWaiter;

    // Setup lifecycle callbacks
    builder->WithClientConnectionSuccessCallback(
        [&connectedWaiter](const Mqtt5::OnConnectionSuccessEventData &)
        {
            fprintf(stdout, "Mqtt5 Client connection succeeded!\n");
            connectedWaiter.set_value(true);
        });
    builder->WithClientConnectionFailureCallback(
        [&connectedWaiter](const Mqtt5::OnConnectionFailureEventData &eventData)
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
        printf("Failed to create mqtt5 client with error code %d: %s", LastError(), ErrorDebugString(LastError()));
        return -1;
    }

    Aws::Iot::RequestResponse::RequestResponseClientOptions requestResponseOptions;
    requestResponseOptions.WithMaxRequestResponseSubscriptions(4);
    requestResponseOptions.WithMaxStreamingSubscriptions(10);
    requestResponseOptions.WithOperationTimeoutInSeconds(30);

    auto commandClient = Aws::Iotcommands::NewClientFrom5(*protocolClient, requestResponseOptions);
    auto commandStreamHandler = Aws::IotcommandsSample::CommandStreamHandler(std::move(commandClient));

    protocolClient->Start();
    auto isConnected = connectedWaiter.get_future().get();
    if (!isConnected)
    {
        return -1;
    }

    ApplicationContext context{std::move(protocolClient), std::move(commandStreamHandler)};

    context.commandStreamHandler.openStream(Aws::Iotcommands::DeviceType::THING, cmdData.input_thingName, "json");
    context.commandStreamHandler.openStream(Aws::Iotcommands::DeviceType::THING, cmdData.input_thingName, "cbor");
    context.commandStreamHandler.openStream(Aws::Iotcommands::DeviceType::THING, cmdData.input_thingName, "generic");

    context.commandStreamHandler.openStream(Aws::Iotcommands::DeviceType::CLIENT, cmdData.input_clientId, "json");
    context.commandStreamHandler.openStream(Aws::Iotcommands::DeviceType::CLIENT, cmdData.input_clientId, "cbor");
    context.commandStreamHandler.openStream(Aws::Iotcommands::DeviceType::CLIENT, cmdData.input_clientId, "generic");

    while (true)
    {
        fprintf(stdout, "\nEnter command:\n");

        String input;
        std::getline(std::cin, input);

        if (s_handleInput(input, context))
        {
            fprintf(stdout, "Exiting...");
            break;
        }
    }

    return 0;
}
