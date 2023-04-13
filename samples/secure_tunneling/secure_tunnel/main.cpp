/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/common/byte_buf.h>
#include <aws/crt/Api.h>
#include <aws/crt/http/HttpProxyStrategy.h>
#include <aws/iot/MqttClient.h>
#include <aws/iotdevicecommon/IotDevice.h>
#include <aws/iotsecuretunneling/SecureTunnel.h>

#include <fstream>
#include <thread>

#include "../../utils/CommandLineUtils.h"

using namespace Aws::Crt;
using namespace Aws::Iotsecuretunneling;
using namespace Aws::Crt::Io;
using namespace std::chrono_literals;

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

void setupCommandLineUtils(Utils::CommandLineUtils *cmdUtils, int argc, char *argv[])
{
    cmdUtils->AddCommonProxyCommands();
    cmdUtils->RegisterProgramName("secure_tunnel");
    cmdUtils->RegisterCommand("region", "<str>", "The region of your secure tunnel");
    cmdUtils->RegisterCommand(
        "ca_file", "<path>", "Path to AmazonRootCA1.pem (optional, system trust store used by default).");
    cmdUtils->RegisterCommand(
        "access_token_file", "<path>", "Path to the tunneling access token file (optional if --access_token used).");
    cmdUtils->RegisterCommand(
        "access_token", "<str>", "Tunneling access token (optional if --access_token_file used).");
    cmdUtils->RegisterCommand(
        "local_proxy_mode_source", "<str>", "Use to set local proxy mode to source (optional, default='destination').");
    cmdUtils->RegisterCommand(
        "client_token", "<str>", "Tunneling access token (optional if --client_token_file used).");
    cmdUtils->RegisterCommand("message", "<str>", "Message to send (optional, default='Hello World!').");
    cmdUtils->RegisterCommand(
        "proxy_user_name", "<str>", "User name passed if proxy server requires a user name (optional)");
    cmdUtils->RegisterCommand(
        "proxy_password", "<str>", "Password passed if proxy server requires a password (optional)");
    cmdUtils->RegisterCommand("count", "<int>", "Number of messages to send before completing (optional, default='5')");
    cmdUtils->AddLoggingCommands();
    const char **const_argv = (const char **)argv;
    cmdUtils->SendArguments(const_argv, const_argv + argc);
}

void setupCommandLineValues(
    Utils::CommandLineUtils *cmdUtils,
    String *endpoint,
    String *accessToken,
    String *clientToken,
    String *caFile,
    String *proxyHost,
    String *proxyUserName,
    String *proxyPassword,
    uint16_t &proxyPort,
    uint16_t &messageCount,
    aws_secure_tunneling_local_proxy_mode &localProxyMode,
    String *payloadMessage)
{
    /* Generate secure tunneling endpoint using region */
    String region = cmdUtils->GetCommandRequired("region");
    endpoint->assign("data.tunneling.iot." + region + ".amazonaws.com");

    String tempAccessToken;
    /* An access token is required to connect to the secure tunnel service */
    if (cmdUtils->HasCommand("access_token"))
    {
        tempAccessToken = cmdUtils->GetCommand("access_token");
    }
    else if (cmdUtils->HasCommand("access_token_file"))
    {
        tempAccessToken = cmdUtils->GetCommand("access_token_file");

        std::ifstream accessTokenFile(tempAccessToken.c_str());
        if (accessTokenFile.is_open())
        {
            getline(accessTokenFile, tempAccessToken);
            accessTokenFile.close();
        }
        else
        {
            fprintf(stderr, "Failed to open access token file");
            exit(-1);
        }
    }
    else
    {
        cmdUtils->PrintHelp();
        fprintf(stderr, "--access_token_file or --access_token must be set to connect through a secure tunnel");
        exit(-1);
    }
    accessToken->assign(tempAccessToken);

    String tempClientToken;
    /*
     * A client token is optional as one will be automatically generated if it is absent but it is recommended the
     * customer provides their own so they can reuse it with other secure tunnel clients after the secure tunnel client
     * is terminated.
     * */
    if (cmdUtils->HasCommand("client_token"))
    {
        tempClientToken = cmdUtils->GetCommand("client_token");
    }

    if (cmdUtils->HasCommand("client_token_file"))
    {
        tempClientToken = cmdUtils->GetCommand("client_token_file");

        std::ifstream clientTokenFile(tempClientToken.c_str());
        if (clientTokenFile.is_open())
        {
            getline(clientTokenFile, tempClientToken);
            clientTokenFile.close();
        }
        else
        {
            fprintf(stderr, "Failed to open client token file\n");
            exit(-1);
        }
    }

    clientToken->assign(tempClientToken);

    caFile->assign(cmdUtils->GetCommandOrDefault("ca_file", ""));

    if (cmdUtils->HasCommand("proxy_host") || cmdUtils->HasCommand("proxy_port"))
    {
        proxyHost->assign(
            cmdUtils->GetCommandRequired("proxy_host", "--proxy_host must be set to connect through a proxy.").c_str());
        int port = atoi(
            cmdUtils->GetCommandRequired("proxy_port", "--proxy_port must be set to connect through a proxy.").c_str());
        if (port > 0 && port <= UINT16_MAX)
        {
            proxyPort = static_cast<uint16_t>(port);
        }
        proxyUserName->assign(cmdUtils->GetCommandOrDefault("proxy_user_name", ""));
        proxyPassword->assign(cmdUtils->GetCommandOrDefault("proxy_password", ""));
    }

    /*
     * localProxyMode is set to destination by default unless flag is set to source
     */
    if (cmdUtils->HasCommand("local_proxy_mode_source"))
    {
        localProxyMode = AWS_SECURE_TUNNELING_SOURCE_MODE;
    }
    else
    {
        localProxyMode = AWS_SECURE_TUNNELING_DESTINATION_MODE;
    }

    payloadMessage->assign(cmdUtils->GetCommandOrDefault("message", "Hello World"));

    int count = atoi(cmdUtils->GetCommandOrDefault("count", "5").c_str());
    messageCount = static_cast<uint16_t>(count);
}

int main(int argc, char *argv[])
{
    struct aws_allocator *allocator = aws_default_allocator();
    /************************ Setup the Lib ****************************/
    /*
     * Do the global initialization for the API and aws-c-iot.
     */
    ApiHandle apiHandle;
    aws_iotdevice_library_init(allocator);

    /*
     * In a real world application you probably don't want to enforce synchronous behavior
     * but this is a sample console application, so we'll just do that with a condition variable.
     */
    std::promise<bool> clientStoppedPromise;

    /* service id storage for use in sample */
    Aws::Crt::ByteBuf m_serviceIdStorage;
    AWS_ZERO_STRUCT(m_serviceIdStorage);
    Aws::Crt::Optional<Aws::Crt::ByteCursor> m_serviceId;

    String endpoint;
    String accessToken;
    String clientToken;
    String caFile;
    String proxyHost;
    uint16_t proxyPort(8080);
    String proxyUserName;
    String proxyPassword;
    aws_secure_tunneling_local_proxy_mode localProxyMode;
    String payloadMessage;
    uint16_t messageCount(5);

    /* Connection Id is used for Simultaneous HTTP Connections (Protocl V3) */
    uint32_t connectionId = 1;
    bool keepRunning = true;
    uint16_t messagesSent = 0;

    /*********************** Parse Arguments ***************************/
    Utils::CommandLineUtils cmdUtils = Utils::CommandLineUtils();
    setupCommandLineUtils(&cmdUtils, argc, argv);
    cmdUtils.StartLoggingBasedOnCommand(&apiHandle);
    setupCommandLineValues(
        &cmdUtils,
        &endpoint,
        &accessToken,
        &clientToken,
        &caFile,
        &proxyHost,
        &proxyUserName,
        &proxyPassword,
        proxyPort,
        messageCount,
        localProxyMode,
        &payloadMessage);

    if (apiHandle.GetOrCreateStaticDefaultClientBootstrap()->LastError() != AWS_ERROR_SUCCESS)
    {
        fprintf(
            stderr,
            "ClientBootstrap failed with error %s\n",
            ErrorDebugString(apiHandle.GetOrCreateStaticDefaultClientBootstrap()->LastError()));
        exit(-1);
    }

    /* Use a SecureTunnelBuilder to set up and build the secure tunnel client */
    SecureTunnelBuilder builder = SecureTunnelBuilder(allocator, accessToken.c_str(), localProxyMode, endpoint.c_str());

    if (caFile.length() > 0)
    {
        builder.WithRootCa(caFile.c_str());
    }

    /* Proxy Options */
    if (proxyHost.length() > 0)
    {
        auto proxyOptions = Aws::Crt::Http::HttpClientConnectionProxyOptions();
        proxyOptions.HostName = proxyHost.c_str();
        proxyOptions.Port = proxyPort;

        /* Set up Proxy Strategy if a user name and password is provided */
        if (proxyUserName.length() > 0 || proxyPassword.length() > 0)
        {
            fprintf(stdout, "Creating proxy strategy\n");
            Aws::Crt::Http::HttpProxyStrategyBasicAuthConfig basicAuthConfig;
            basicAuthConfig.ConnectionType = Aws::Crt::Http::AwsHttpProxyConnectionType::Tunneling;
            basicAuthConfig.Username = proxyUserName.c_str();
            basicAuthConfig.Password = proxyPassword.c_str();
            proxyOptions.ProxyStrategy =
                Aws::Crt::Http::HttpProxyStrategy::CreateBasicHttpProxyStrategy(basicAuthConfig, allocator);
            proxyOptions.AuthType = Aws::Crt::Http::AwsHttpProxyAuthenticationType::Basic;
        }
        else
        {
            proxyOptions.AuthType = Aws::Crt::Http::AwsHttpProxyAuthenticationType::None;
        }

        /* Add proxy options to the builder */
        builder.WithHttpClientConnectionProxyOptions(proxyOptions);
    }

    builder.WithClientToken(clientToken.c_str());

    /* Add callbacks using the builder */
    builder.WithOnMessageReceived([&](SecureTunnel *secureTunnel, const MessageReceivedEventData &eventData) {
        {
            std::shared_ptr<Message> message = eventData.message;

            logMessage(message);

            /* Send an echo message back to the Source Device */
            if (localProxyMode == AWS_SECURE_TUNNELING_DESTINATION_MODE)
            {
                std::shared_ptr<Message> echoMessage = std::make_shared<Message>(message->getPayload().value());

                /* Echo message on same service id received message arrived on */
                if (message->getServiceId().has_value())
                {
                    echoMessage->withServiceId(message->getServiceId().value());
                }

                /* Echo message on the same connection id received message arrived on */
                if (message->getConnectionId() > 0)
                {
                    echoMessage->withConnectionId(message->getConnectionId());
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

    /* Set the Secure Tunnel Client to desire a connected state */
    if (secureTunnel->Start())
    {
        fprintf(stderr, "Secure Tunnel Connect call failed: %s\n", ErrorDebugString(LastError()));
        exit(-1);
    }

    /*
     * In Destination mode the Secure Tunnel Client will remain open and echo messages that come in.
     * In Source mode the Secure Tunnel Client will send 4 messages and then disconnect and terminate.
     */
    while (keepRunning)
    {
        std::this_thread::sleep_for(2000ms);
        if (localProxyMode == AWS_SECURE_TUNNELING_SOURCE_MODE)
        {
            messagesSent++;
            String toSend = (std::to_string(messagesSent) + ": " + payloadMessage.c_str()).c_str();

            if (messagesSent <= messageCount)
            {

                std::shared_ptr<Message> message = std::make_shared<Message>(ByteCursorFromCString(toSend.c_str()));

                /* If the secure tunnel has service ids, we will use one for our messages. */
                if (m_serviceId.has_value())
                {
                    message->withServiceId(m_serviceId.value());
                }

                message->withConnectionId(connectionId);

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

                    std::this_thread::sleep_for(2000ms);
                }
                else
                {
                    keepRunning = false;
                }
            }
        }
    }

    std::this_thread::sleep_for(3000ms);

    fprintf(stdout, "Closing Connection\n");
    /* Set the Secure Tunnel Client to desire a stopped state */
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

    /* Clean Up */
    aws_byte_buf_clean_up(&m_serviceIdStorage);

    return 0;
}
