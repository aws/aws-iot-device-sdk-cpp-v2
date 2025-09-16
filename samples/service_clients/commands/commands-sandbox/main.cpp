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
#include <memory>
#include <thread>

struct ApplicationContext
{
    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> m_protocolClient;
    std::shared_ptr<Aws::IotcommandsSample::CommandStreamHandler> commandStreamHandler;
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

/* --------------------------------- ARGUMENT PARSING ----------------------------------------- */
struct CmdArgs
{
    Aws::Crt::String endpoint;
    Aws::Crt::String cert;
    Aws::Crt::String key;
    Aws::Crt::String clientId;
    Aws::Crt::String caFile;
    Aws::Crt::String thingName;
};

void printHelp()
{
    printf("Commands Sandbox Sample\n");
    printf("options:\n");
    printf("  --help        show this help message and exit\n");
    printf("required arguments:\n");
    printf("  --endpoint    IoT endpoint hostname\n");
    printf("  --cert        Path to the certificate file\n");
    printf("  --key         Path to the private key file\n");
    printf("  --thing_name  Thing name\n");
    printf("optional arguments:\n");
    printf("  --client_id   Client ID (default: test-<uuid>)\n");
    printf("  --ca_file     Path to optional CA bundle (PEM)\n");
}

CmdArgs parseArgs(int argc, char *argv[])
{
    CmdArgs args;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--help") == 0)
        {
            printHelp();
            exit(0);
        }
        else if (i < argc - 1)
        {
            if (strcmp(argv[i], "--endpoint") == 0)
            {
                args.endpoint = argv[++i];
            }
            else if (strcmp(argv[i], "--cert") == 0)
            {
                args.cert = argv[++i];
            }
            else if (strcmp(argv[i], "--key") == 0)
            {
                args.key = argv[++i];
            }
            else if (strcmp(argv[i], "--thing_name") == 0)
            {
                args.thingName = argv[++i];
            }
            else if (strcmp(argv[i], "--client_id") == 0)
            {
                args.clientId = argv[++i];
            }
            else if (strcmp(argv[i], "--ca_file") == 0)
            {
                args.caFile = argv[++i];
            }
            else
            {
                fprintf(stderr, "Unknown argument: %s\n", argv[i]);
                printHelp();
                exit(1);
            }
        }
    }
    if (args.endpoint.empty() || args.cert.empty() || args.key.empty() || args.thingName.empty())
    {
        fprintf(stderr, "Error: --endpoint, --cert, --key, and --thing_name are required\n");
        printHelp();
        exit(1);
    }
    if (args.clientId.empty())
    {
        args.clientId = Aws::Crt::String("test-") + Aws::Crt::UUID().ToString();
    }
    return args;
}
/* --------------------------------- ARGUMENT PARSING END ----------------------------------------- */

int main(int argc, char *argv[])
{
    /************************ Setup ****************************/

    // Parse command line arguments
    CmdArgs cmdData = parseArgs(argc, argv);

    // Do the global initialization for the API.
    Aws::Crt::ApiHandle apiHandle;

    // Create the MQTT5 builder and populate it with data from cmdData.
    auto builder = std::unique_ptr<Aws::Iot::Mqtt5ClientBuilder>(
        Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(
            cmdData.endpoint, cmdData.cert.c_str(), cmdData.key.c_str()));
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
    connectPacket->WithClientId(cmdData.clientId);

    // Setup CA file if provided
    if (!cmdData.caFile.empty())
    {
        builder->WithCertificateAuthority(cmdData.caFile.c_str());
    }

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
    auto commandStreamHandler = Aws::Crt::MakeShared<Aws::IotcommandsSample::CommandStreamHandler>(
        Aws::Crt::DefaultAllocatorImplementation(), std::move(commandClient));

    protocolClient->Start();
    auto isConnected = connectedWaiter.get_future().get();
    connectedWaiter = {};

    if (!isConnected)
    {
        return -1;
    }

    ApplicationContext context{
        std::move(protocolClient), std::move(commandStreamHandler), cmdData.thingName, cmdData.clientId};

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
