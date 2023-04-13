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
    if (message->getServiceId().has_value())
    {
        if (message->getPayload().has_value())
        {
            fprintf(
                stdout,
                "Message received on service id:'" PRInSTR "' with payload:'" PRInSTR "'\n",
                AWS_BYTE_CURSOR_PRI(message->getServiceId().value()),
                AWS_BYTE_CURSOR_PRI(message->getPayload().value()));
        }
        else
        {
            fprintf(
                stdout,
                "Message with service id:'" PRInSTR "' with no payload.\n",
                AWS_BYTE_CURSOR_PRI(message->getServiceId().value()));
        }
        return;
    }
    if (message->getPayload().has_value())
    {
        fprintf(
            stdout,
            "Message received with payload:'" PRInSTR "'\n",
            AWS_BYTE_CURSOR_PRI(message->getPayload().value()));
    }
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
    Utils::cmdData *cmdData,
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
    // Generate secure tunneling endpoint using region
    String region = cmdData->input_signingRegion;
    endpoint->assign("data.tunneling.iot." + region + ".amazonaws.com");

    String tempAccessToken;
    // An access token is required to connect to the secure tunnel service
    if (cmdData->input_accessToken != "")
    {
        tempAccessToken = cmdData->input_accessToken;
    }
    else if (cmdData->input_accessTokenFile != "")
    {
        tempAccessToken = cmdData->input_accessTokenFile;
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
        fprintf(stderr, "--access_token_file or --access_token must be set to connect through a secure tunnel");
        exit(-1);
    }
    accessToken->assign(tempAccessToken);

    String tempClientToken;
    /**
     * A client token is optional as one will be automatically generated if it is absent but it is recommended the
     * customer provides their own so they can reuse it with other secure tunnel clients after the secure tunnel client
     * is terminated.
     */
    if (cmdData->input_clientToken != "")
    {
        tempClientToken = cmdData->input_clientToken;
    }
    else if (cmdData->input_clientTokenFile != "")
    {
        tempClientToken = cmdData->input_clientTokenFile;
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

    if (cmdData->input_ca != "")
    {
        caFile->assign(cmdData->input_ca);
    }

    if (cmdData->input_proxyHost != "" || cmdData->input_proxyPort != 0)
    {
        proxyHost->assign(cmdData->input_proxyHost);
        proxyPort = static_cast<uint16_t>(cmdData->input_proxyPort);
        proxyUserName->assign(cmdData->input_proxy_user_name);
        proxyPassword->assign(cmdData->input_proxy_password);
    }

    // localProxyMode is set to destination by default unless flag is set to source
    if (cmdData->input_localProxyModeSource != "destination")
    {
        localProxyMode = AWS_SECURE_TUNNELING_SOURCE_MODE;
    }
    else
    {
        localProxyMode = AWS_SECURE_TUNNELING_DESTINATION_MODE;
    }

    payloadMessage->assign(cmdData->input_message);
    messageCount = static_cast<uint16_t>(cmdData->input_count);
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
    std::promise<bool> connectionCompletedPromise;
    std::promise<bool> connectionClosedPromise;
    std::promise<bool> clientStoppedPromise;

    // service id storage for use in sample
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

    /*********************** Parse Arguments ***************************/
    Utils::cmdData cmdData = Utils::parseSampleInputSecureTunnel(argc, argv, &apiHandle);
    setupCommandLineValues(
        &cmdData,
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

    // Use a SecureTunnelBuilder to set up and build the secure tunnel client
    SecureTunnelBuilder builder = SecureTunnelBuilder(allocator, accessToken.c_str(), localProxyMode, endpoint.c_str());

    if (caFile.length() > 0)
    {
        builder.WithRootCa(caFile.c_str());
    }

    builder.WithClientToken(clientToken.c_str());

    // Add callbacks using the builder

    builder.WithOnConnectionSuccess([&](SecureTunnel *secureTunnel, const ConnectionSuccessEventData &eventData) {
        if (eventData.connectionData->getServiceId1().has_value())
        {
            // If secure tunnel is using service ids, store one for future use
            aws_byte_buf_clean_up(&m_serviceIdStorage);
            AWS_ZERO_STRUCT(m_serviceIdStorage);
            aws_byte_buf_init_copy_from_cursor(
                &m_serviceIdStorage, allocator, eventData.connectionData->getServiceId1().value());
            m_serviceId = aws_byte_cursor_from_buf(&m_serviceIdStorage);

            fprintf(
                stdout,
                "Secure Tunnel connected with  Service IDs '" PRInSTR "'",
                AWS_BYTE_CURSOR_PRI(eventData.connectionData->getServiceId1().value()));
            if (eventData.connectionData->getServiceId2().has_value())
            {
                fprintf(
                    stdout, ", '" PRInSTR "'", AWS_BYTE_CURSOR_PRI(eventData.connectionData->getServiceId2().value()));
                if (eventData.connectionData->getServiceId3().has_value())
                {
                    fprintf(
                        stdout,
                        ", '" PRInSTR "'",
                        AWS_BYTE_CURSOR_PRI(eventData.connectionData->getServiceId3().value()));
                }
            }
            fprintf(stdout, "\n");

            // Stream Start can only be called from Source Mode
            if (localProxyMode == AWS_SECURE_TUNNELING_SOURCE_MODE)
            {
                fprintf(
                    stdout,
                    "Sending Stream Start request with service id:'" PRInSTR "'\n",
                    AWS_BYTE_CURSOR_PRI(eventData.connectionData->getServiceId1().value()));
                secureTunnel->SendStreamStart(eventData.connectionData->getServiceId1().value());
            }
        }
        else
        {
            fprintf(stdout, "Secure Tunnel is not using Service Ids.\n");

            // Stream Start can only be called from Source Mode
            if (localProxyMode == AWS_SECURE_TUNNELING_SOURCE_MODE)
            {
                fprintf(stdout, "Sending Stream Start request\n");
                secureTunnel->SendStreamStart();
            }
        }

        connectionCompletedPromise.set_value(true);
    });

    builder.WithOnConnectionFailure([&](SecureTunnel *secureTunnel, int errorCode) {
        (void)secureTunnel;
        fprintf(stdout, "Connection attempt failed with error code %d(%s)\n", errorCode, ErrorDebugString(errorCode));
    });

    builder.WithOnConnectionShutdown([&]() {
        fprintf(stdout, "Connection Shutdown\n");
        connectionClosedPromise.set_value(true);
    });

    builder.WithOnMessageReceived([&](SecureTunnel *secureTunnel, const MessageReceivedEventData &eventData) {
        {
            std::shared_ptr<Message> message = eventData.message;

            logMessage(message);
            std::shared_ptr<Message> echoMessage;

            switch (localProxyMode)
            {
                case AWS_SECURE_TUNNELING_DESTINATION_MODE:

                    echoMessage = std::make_shared<Message>(message->getPayload().value());

                    // Echo message on same service id received message came on
                    if (message->getServiceId().has_value())
                    {
                        echoMessage->withServiceId(message->getServiceId().value());
                    }

                    secureTunnel->SendMessage(echoMessage);

                    fprintf(stdout, "Sending Echo Message\n");

                    break;
                case AWS_SECURE_TUNNELING_SOURCE_MODE:

                    break;
            }
        }
    });

    builder.WithOnStreamStarted(
        [&](SecureTunnel *secureTunnel, int errorCode, const StreamStartedEventData &eventData) {
            (void)secureTunnel;
            if (!errorCode)
            {
                std::shared_ptr<StreamStartedData> streamStartedData = eventData.streamStartedData;

                if (streamStartedData->getServiceId().has_value())
                {
                    fprintf(
                        stdout,
                        "Stream started on service id: '" PRInSTR "'\n",
                        AWS_BYTE_CURSOR_PRI(streamStartedData->getServiceId().value()));
                }
                else
                {
                    fprintf(stdout, "Stream started using V1 Protocol");
                }
            }
        });

    builder.WithOnStreamStopped([&](SecureTunnel *secureTunnel, const StreamStoppedEventData &eventData) {
        (void)secureTunnel;
        std::shared_ptr<StreamStoppedData> streamStoppedData = eventData.streamStoppedData;

        if (streamStoppedData->getServiceId().has_value())
        {
            fprintf(
                stdout,
                "Stream stopped on service id: '" PRInSTR "'\n",
                AWS_BYTE_CURSOR_PRI(streamStoppedData->getServiceId().value()));
        }
        else
        {
            fprintf(stdout, "Stream stopped using V1 Protocol");
        }
    });

    builder.WithOnStopped([&](SecureTunnel *secureTunnel) {
        (void)secureTunnel;
        fprintf(stdout, "Secure Tunnel has entered Stopped State\n");
        clientStoppedPromise.set_value(true);
    });

    // ***********************************************************************************************************************
    // Proxy Options
    // ***********************************************************************************************************************
    if (proxyHost.length() > 0)
    {
        auto proxyOptions = Aws::Crt::Http::HttpClientConnectionProxyOptions();
        proxyOptions.HostName = proxyHost.c_str();
        proxyOptions.Port = proxyPort;

        // Set up Proxy Strategy if a user name and password is provided
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

        // Add proxy options to the builder
        builder.WithHttpClientConnectionProxyOptions(proxyOptions);
    }

    // Create Secure Tunnel using the options set with the builder
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

    bool keepRunning = true;
    uint16_t messagesSent = 0;

    if (connectionCompletedPromise.get_future().get())
    {
        std::this_thread::sleep_for(1000ms);

        /**
         * In Destination mode the Secure Tunnel Client will remain open and echo messages that come in.
         * In Source mode the Secure Tunnel Client will send 4 messages and then disconnect and terminate.
         */
        while (keepRunning)
        {
            if (localProxyMode == AWS_SECURE_TUNNELING_SOURCE_MODE)
            {
                messagesSent++;
                String toSend = (std::to_string(messagesSent) + ": " + payloadMessage.c_str()).c_str();

                if (messagesSent <= messageCount)
                {
                    std::shared_ptr<Message> message = std::make_shared<Message>(ByteCursorFromCString(toSend.c_str()));

                    // If the secure tunnel has service ids, we will use one for our messages.
                    if (m_serviceId.has_value())
                    {
                        message->withServiceId(m_serviceId.value());
                    }

                    secureTunnel->SendMessage(message);

                    fprintf(stdout, "Sending Message:\"%s\"\n", toSend.c_str());

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
    // Set the Secure Tunnel Client to desire a stopped state
    if (secureTunnel->Stop() == AWS_OP_ERR)
    {
        fprintf(stderr, "Secure Tunnel Close call failed: %s\n", ErrorDebugString(LastError()));
        exit(-1);
    }

    if (connectionClosedPromise.get_future().get())
    {
        fprintf(stdout, "Secure Tunnel Connection Closed\n");
    }

    // The Secure Tunnel Client at this point will report they are stopped and can be safely removed.
    if (clientStoppedPromise.get_future().get())
    {
        secureTunnel = nullptr;
    }

    fprintf(stdout, "Secure Tunnel Sample Completed\n");

    // Clean Up
    aws_byte_buf_clean_up(&m_serviceIdStorage);

    return 0;
}
