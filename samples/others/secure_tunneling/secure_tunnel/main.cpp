/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/common/byte_buf.h>
#include <aws/crt/Api.h>
#include <aws/crt/http/HttpProxyStrategy.h>
#include <aws/iotdevicecommon/IotDevice.h>
#include <aws/iotsecuretunneling/SecureTunnel.h>

#include <fstream>
#include <thread>


using namespace Aws::Crt;
using namespace Aws::Iotsecuretunneling;
using namespace Aws::Crt::Io;

void logMessage(std::shared_ptr<Message> message)
{

    fprintf(stdout, "Message received");
    if (message->getServiceId().has_value())
    {
        fprintf(stdout, " on service id:'" PRInSTR "'", AWS_BYTE_CURSOR_PRI(message->getServiceId().value()));
    }

    if (message->getConnectionId() > 0)
    {
        fprintf(stdout, " with connection id: (%d)", message->getConnectionId());
    }

    if (message->getPayload().has_value())
    {
        fprintf(stdout, " with payload: '" PRInSTR "'", AWS_BYTE_CURSOR_PRI(message->getPayload().value()));
    }

    fprintf(stdout, "\n");
}

void logConnectionData(const ConnectionSuccessEventData &eventData)
{
    if (eventData.connectionData->getServiceId1().has_value())
    {
        fprintf(
            stdout,
            "Secure Tunnel connected with Service IDs '" PRInSTR "'",
            AWS_BYTE_CURSOR_PRI(eventData.connectionData->getServiceId1().value()));

        if (eventData.connectionData->getServiceId2().has_value())
        {
            fprintf(stdout, ", '" PRInSTR "'", AWS_BYTE_CURSOR_PRI(eventData.connectionData->getServiceId2().value()));
            if (eventData.connectionData->getServiceId3().has_value())
            {
                fprintf(
                    stdout, ", '" PRInSTR "'", AWS_BYTE_CURSOR_PRI(eventData.connectionData->getServiceId3().value()));
            }
        }
        fprintf(stdout, "\n");
    }
    else
    {
        fprintf(stdout, "Secure Tunnel connected with no Service IDs available\n");
    }
}

void logStreamStartData(const StreamStartedEventData &eventData)
{
    fprintf(stdout, "Stream started");

    if (eventData.streamStartedData->getServiceId().has_value())
    {
        fprintf(
            stdout,
            " on service id: '" PRInSTR,
            AWS_BYTE_CURSOR_PRI(eventData.streamStartedData->getServiceId().value()));
    }

    if (eventData.streamStartedData->getConnectionId() > 0)
    {
        fprintf(stdout, " with Connection Id: (%d)", eventData.streamStartedData->getConnectionId());
    }

    fprintf(stdout, "\n");
}

void logStreamStoppedData(const StreamStoppedEventData &eventData)
{
    fprintf(stdout, "Stream");

    if (eventData.streamStoppedData->getServiceId().has_value())
    {
        fprintf(
            stdout,
            " on service id: '" PRInSTR,
            AWS_BYTE_CURSOR_PRI(eventData.streamStoppedData->getServiceId().value()));
    }

    fprintf(stdout, " stopped\n");
}

void logConnectionStartedData(const ConnectionStartedEventData &eventData)
{
    fprintf(stdout, "Connection started");

    if (eventData.connectionStartedData->getServiceId().has_value())
    {
        fprintf(
            stdout,
            " on service id: '" PRInSTR,
            AWS_BYTE_CURSOR_PRI(eventData.connectionStartedData->getServiceId().value()));
    }

    if (eventData.connectionStartedData->getConnectionId() > 0)
    {
        fprintf(stdout, " with Connection Id: (%d)", eventData.connectionStartedData->getConnectionId());
    }

    fprintf(stdout, "\n");
}

int main(int argc, char *argv[])
{
    struct aws_allocator *allocator = aws_default_allocator();
    /************************ Setup the Lib ****************************/

    // Do the global initialization for the API and aws-c-iot.
    ApiHandle apiHandle;
    aws_iotdevice_library_init(allocator);

    /**
     * In a real world application you probably don't want to enforce synchronous behavior
     * but this is a sample console application, so we'll just do that with a condition variable.
     */
    std::promise<void> clientConnectedPromise;
    std::promise<bool> clientStoppedPromise;

    // service id storage for use in sample
    Aws::Crt::ByteBuf m_serviceIdStorage;
    AWS_ZERO_STRUCT(m_serviceIdStorage);
    Aws::Crt::Optional<Aws::Crt::ByteCursor> m_serviceId;

    aws_secure_tunneling_local_proxy_mode localProxyMode;

    /* Connection Id is used for Simultaneous HTTP Connections (Protocl V3) */
    uint32_t connectionId = 1;
    bool keepRunning = true;
    uint16_t messagesSent = 0;

    /* --------------------------------- ARGUMENT PARSING ----------------------------------------- */
    struct CmdArgs
    {
        String accessToken;
        String endpoint;
        String caFile;
        String clientToken;
        String localProxyModeSource = "destination";
        String proxyHost;
        String proxyUserName;
        String proxyPassword;
        String message = "Hello World";
        uint32_t proxyPort = 0;
        uint32_t count = 4;
    };

    auto printHelp = []() {
        printf("Secure Tunnel Sample\n");
        printf("options:\n");
        printf("  --help        show this help message and exit\n");
        printf("required arguments:\n");
        printf("  --access_token    Access token for secure tunnel\n");
        printf("  --endpoint        Secure tunnel endpoint\n");
        printf("optional arguments:\n");
        printf("  --ca_file         Path to optional CA bundle (PEM)\n");
        printf("  --client_token    Client token\n");
        printf("  --local_proxy_mode_source  Local proxy mode (default: destination)\n");
        printf("  --proxy_host      HTTP proxy host\n");
        printf("  --proxy_port      HTTP proxy port\n");
        printf("  --proxy_username  HTTP proxy username\n");
        printf("  --proxy_password  HTTP proxy password\n");
        printf("  --message         Message to send (default: Hello World)\n");
        printf("  --count           Number of messages to send (default: 4)\n");
    };

    auto parseArgs = [&](int argc, char *argv[]) -> CmdArgs {
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
                if (strcmp(argv[i], "--access_token") == 0)
                {
                    args.accessToken = argv[++i];
                }
                else if (strcmp(argv[i], "--endpoint") == 0)
                {
                    args.endpoint = argv[++i];
                }
                else if (strcmp(argv[i], "--ca_file") == 0)
                {
                    args.caFile = argv[++i];
                }
                else if (strcmp(argv[i], "--client_token") == 0)
                {
                    args.clientToken = argv[++i];
                }
                else if (strcmp(argv[i], "--local_proxy_mode_source") == 0)
                {
                    args.localProxyModeSource = argv[++i];
                }
                else if (strcmp(argv[i], "--proxy_host") == 0)
                {
                    args.proxyHost = argv[++i];
                }
                else if (strcmp(argv[i], "--proxy_username") == 0)
                {
                    args.proxyUserName = argv[++i];
                }
                else if (strcmp(argv[i], "--proxy_password") == 0)
                {
                    args.proxyPassword = argv[++i];
                }
                else if (strcmp(argv[i], "--message") == 0)
                {
                    args.message = argv[++i];
                }
                else if (strcmp(argv[i], "--proxy_port") == 0)
                {
                    args.proxyPort = atoi(argv[++i]);
                }
                else if (strcmp(argv[i], "--count") == 0)
                {
                    args.count = atoi(argv[++i]);
                }
                else
                {
                    fprintf(stderr, "Unknown argument: %s\n", argv[i]);
                    printHelp();
                    exit(1);
                }
            }
        }
        if (args.accessToken.empty() || args.endpoint.empty())
        {
            fprintf(stderr, "Error: --access_token and --endpoint are required\n");
            printHelp();
            exit(1);
        }
        return args;
    };

    /*********************** Parse Arguments ***************************/
    CmdArgs cmdData = parseArgs(argc, argv);
    // localProxyMode is set to destination by default unless flag is set to source
    if (cmdData.localProxyModeSource != "destination")
    {
        localProxyMode = AWS_SECURE_TUNNELING_SOURCE_MODE;
    }
    else
    {
        localProxyMode = AWS_SECURE_TUNNELING_DESTINATION_MODE;
    }

    if (apiHandle.GetOrCreateStaticDefaultClientBootstrap()->LastError() != AWS_ERROR_SUCCESS)
    {
        fprintf(
            stderr,
            "ClientBootstrap failed with error %s\n",
            ErrorDebugString(apiHandle.GetOrCreateStaticDefaultClientBootstrap()->LastError()));
        exit(-1);
    }

    // Use a SecureTunnelBuilder to set up and build the secure tunnel client
    SecureTunnelBuilder builder = SecureTunnelBuilder(
        allocator, cmdData.accessToken.c_str(), localProxyMode, cmdData.endpoint.c_str());

    if (!cmdData.caFile.empty())
    {
        builder.WithRootCa(cmdData.caFile.c_str());
    }

    /* Proxy Options */
    if (!cmdData.proxyHost.empty())
    {
        auto proxyOptions = Aws::Crt::Http::HttpClientConnectionProxyOptions();
        proxyOptions.HostName = cmdData.proxyHost;
        proxyOptions.Port = cmdData.proxyPort;

        // Set up Proxy Strategy if a user name and password is provided
        if (!cmdData.proxyUserName.empty() || !cmdData.proxyPassword.empty())
        {
            fprintf(stdout, "Creating proxy strategy\n");
            Aws::Crt::Http::HttpProxyStrategyBasicAuthConfig basicAuthConfig;
            basicAuthConfig.ConnectionType = Aws::Crt::Http::AwsHttpProxyConnectionType::Tunneling;
            basicAuthConfig.Username = cmdData.proxyUserName.c_str();
            basicAuthConfig.Password = cmdData.proxyPassword.c_str();
            proxyOptions.ProxyStrategy =
                Aws::Crt::Http::HttpProxyStrategy::CreateBasicHttpProxyStrategy(basicAuthConfig, allocator);
            proxyOptions.AuthType = Aws::Crt::Http::AwsHttpProxyAuthenticationType::Basic;
        }
        else
        {
            proxyOptions.AuthType = Aws::Crt::Http::AwsHttpProxyAuthenticationType::None;
        }

        // Add proxy options to the builder
        builder.WithHttpClientConnectionProxyOptions(proxyOptions);
    }

    if (!cmdData.clientToken.empty())
    {
        builder.WithClientToken(cmdData.clientToken.c_str());
    }

    /* Add callbacks using the builder */
    builder.WithOnMessageReceived([&](SecureTunnel *secureTunnel, const MessageReceivedEventData &eventData) {
        {
            std::shared_ptr<Message> message = eventData.message;

            logMessage(message);

            /* Send an echo message back to the Source Device */
            if (localProxyMode == AWS_SECURE_TUNNELING_DESTINATION_MODE)
            {
                std::shared_ptr<Message> echoMessage =
                    Aws::Crt::MakeShared<Message>(allocator, message->getPayload().value());

                /* Echo message on same service id received message arrived on */
                if (message->getServiceId().has_value())
                {
                    echoMessage->WithServiceId(message->getServiceId().value());
                }

                /* Echo message on the same connection id received message arrived on */
                if (message->getConnectionId() > 0)
                {
                    echoMessage->WithConnectionId(message->getConnectionId());
                }

                secureTunnel->SendMessage(echoMessage);

                fprintf(stdout, "Sending Echo Message\n");
            }
        }
    });

    builder.WithOnSendMessageComplete(
        [&](SecureTunnel *secureTunnel, int errorCode, const SendMessageCompleteEventData &eventData) {
            (void)secureTunnel;
            (void)eventData;

            if (!errorCode)
            {
                fprintf(
                    stdout,
                    "Message of type '" PRInSTR "' sent successfully\n",
                    AWS_BYTE_CURSOR_PRI(eventData.sendMessageCompleteData->getMessageType()));
            }
            else
            {
                fprintf(stdout, "Send Message failed with error code %d(%s)\n", errorCode, ErrorDebugString(errorCode));
            }
        });

    builder.WithOnConnectionSuccess([&](SecureTunnel *secureTunnel, const ConnectionSuccessEventData &eventData) {
        logConnectionData(eventData);

        /* Stream Start can only be called from Source Mode */
        if (localProxyMode == AWS_SECURE_TUNNELING_SOURCE_MODE)
        {
            /* Use a Multiplexing (Service Id) if available on this Secure Tunnel */
            if (eventData.connectionData->getServiceId1().has_value())
            {
                /* Store the service id for future use */
                aws_byte_buf_clean_up(&m_serviceIdStorage);
                AWS_ZERO_STRUCT(m_serviceIdStorage);
                aws_byte_buf_init_copy_from_cursor(
                    &m_serviceIdStorage, allocator, eventData.connectionData->getServiceId1().value());
                m_serviceId = aws_byte_cursor_from_buf(&m_serviceIdStorage);

                fprintf(
                    stdout,
                    "Sending Stream Start request on Service Id:'" PRInSTR "' with Connection Id: (%d)\n",
                    AWS_BYTE_CURSOR_PRI(eventData.connectionData->getServiceId1().value()),
                    connectionId);

                secureTunnel->SendStreamStart(eventData.connectionData->getServiceId1().value(), connectionId);
            }
            else
            {
                fprintf(stdout, "Sending Stream Start request\n");
                secureTunnel->SendStreamStart();
            }
            clientConnectedPromise.set_value();
        }
    });

    builder.WithOnStreamStarted(
        [&](SecureTunnel *secureTunnel, int errorCode, const StreamStartedEventData &eventData) {
            (void)secureTunnel;
            if (!errorCode)
            {
                logStreamStartData(eventData);
            }
            else
            {
                fprintf(stdout, "Stream Start failed with error code %d(%s)\n", errorCode, ErrorDebugString(errorCode));
            }
        });

    builder.WithOnConnectionStarted([&](SecureTunnel *secureTunnel,
                                        int errorCode,
                                        const ConnectionStartedEventData &eventData) {
        (void)secureTunnel;
        if (!errorCode)
        {
            logConnectionStartedData(eventData);
        }
        else
        {
            fprintf(stdout, "Connection Start failed with error code %d(%s)\n", errorCode, ErrorDebugString(errorCode));
        }
    });

    builder.WithOnStreamStopped([&](SecureTunnel *secureTunnel, const StreamStoppedEventData &eventData) {
        (void)secureTunnel;

        logStreamStoppedData(eventData);
    });

    builder.WithOnConnectionShutdown([&]() { fprintf(stdout, "Connection Shutdown\n"); });

    builder.WithOnStopped([&](SecureTunnel *secureTunnel) {
        (void)secureTunnel;
        fprintf(stdout, "Secure Tunnel has entered Stopped State\n");
        clientStoppedPromise.set_value(true);
    });

    /* Create Secure Tunnel using the options set with the builder */
    std::shared_ptr<SecureTunnel> secureTunnel = builder.Build();

    if (!secureTunnel)
    {
        fprintf(stderr, "Secure Tunnel Creation failed: %s\n", ErrorDebugString(LastError()));
        exit(-1);
    }

    // Set the Secure Tunnel Client to desire a connected state
    if (secureTunnel->Start())
    {
        fprintf(stderr, "Secure Tunnel Connect call failed: %s\n", ErrorDebugString(LastError()));
        exit(-1);
    }

    clientConnectedPromise.get_future().wait_for(std::chrono::seconds(5));

    /*
     * In Destination mode the Secure Tunnel Client will remain open and echo messages that come in.
     * In Source mode the Secure Tunnel Client will send 4 messages and then disconnect and terminate.
     */
    while (keepRunning)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        if (localProxyMode == AWS_SECURE_TUNNELING_SOURCE_MODE)
        {
            uint16_t messageCount = static_cast<uint16_t>(cmdData.count);
            messagesSent++;
            String toSend = (std::to_string(messagesSent) + ": " + cmdData.message.c_str()).c_str();

            if (messagesSent <= messageCount)
            {

                std::shared_ptr<Message> message =
                    Aws::Crt::MakeShared<Message>(allocator, ByteCursorFromCString(toSend.c_str()));

                /* If the secure tunnel has service ids, we will use one for our messages. */
                if (m_serviceId.has_value())
                {
                    message->WithServiceId(m_serviceId.value());
                }

                message->WithConnectionId(connectionId);

                secureTunnel->SendMessage(message);

                fprintf(stdout, "Sending Message:\"%s\"\n", toSend.c_str());
            }
            else
            {
                /*
                 * Start a new Simultaneous HTTP Connection using Connection Id 2 and send/receive messages on new
                 * established stream on the same service id.
                 */
                if (connectionId == 1 && m_serviceId.has_value())
                {
                    messagesSent = 0;
                    connectionId = 2;
                    secureTunnel->SendConnectionStart(m_serviceId.value(), connectionId);

                    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                }
                else
                {
                    keepRunning = false;
                }
            }
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    fprintf(stdout, "Closing Connection\n");
    // Set the Secure Tunnel Client to desire a stopped state
    if (secureTunnel->Stop() == AWS_OP_ERR)
    {
        fprintf(stderr, "Secure Tunnel Stop call failed: %s\n", ErrorDebugString(LastError()));
        exit(-1);
    }

    /* The Secure Tunnel Client at this point will report they are stopped and can be safely removed. */
    if (clientStoppedPromise.get_future().get())
    {
        secureTunnel = nullptr;
    }

    fprintf(stdout, "Secure Tunnel Sample Completed\n");

    // Clean Up
    aws_byte_buf_clean_up(&m_serviceIdStorage);

    return 0;
}
