/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
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
    }
    if (message->getPayload().has_value())
    {
        fprintf(
            stdout,
            "Message received with payload:'" PRInSTR "'\n",
            AWS_BYTE_CURSOR_PRI(message->getPayload().value()));
    }
};

int main(int argc, char *argv[])
{
    /************************ Setup the Lib ****************************/
    /*
     * Do the global initialization for the API.
     */
    ApiHandle apiHandle;

    aws_iotdevice_library_init(Aws::Crt::g_allocator);

    String accessToken;
    String clientToken;
    aws_secure_tunneling_local_proxy_mode localProxyMode;

    String proxyHost;
    uint16_t proxyPort(8080);
    String proxyUserName;
    String proxyPassword;

    /*
     * In a real world application you probably don't want to enforce synchronous behavior
     * but this is a sample console application, so we'll just do that with a condition variable.
     */
    std::promise<bool> connectionCompletedPromise;
    std::promise<bool> connectionClosedPromise;
    std::promise<bool> clientStoppedPromise;

    std::shared_ptr<ConnectionData> connectionData;

    /*********************** Parse Arguments ***************************/
    Utils::CommandLineUtils cmdUtils = Utils::CommandLineUtils();
    cmdUtils.AddCommonProxyCommands();
    cmdUtils.RegisterProgramName("secure_tunnel");
    cmdUtils.RegisterCommand("region", "<str>", "The region of your secure tunnel");
    cmdUtils.RegisterCommand(
        "ca_file", "<path>", "Path to AmazonRootCA1.pem (optional, system trust store used by default).");
    cmdUtils.RegisterCommand(
        "access_token_file", "<path>", "Path to the tunneling access token file (optional if --access_token used).");
    cmdUtils.RegisterCommand("access_token", "<str>", "Tunneling access token (optional if --access_token_file used).");
    cmdUtils.RegisterCommand(
        "local_proxy_mode_source", "<str>", "Use to set local proxy mode to source (optional, default='destination').");
    cmdUtils.RegisterCommand("client_token", "<str>", "Tunneling access token (optional if --client_token_file used).");
    cmdUtils.RegisterCommand("message", "<str>", "Message to send (optional, default='Hello World!').");
    cmdUtils.RegisterCommand(
        "proxy_user_name", "<str>", "User name passed if proxy server requires a user name (optional)");
    cmdUtils.RegisterCommand(
        "proxy_password", "<str>", "Password passed if proxy server requires a password (optional)");
    cmdUtils.AddLoggingCommands();
    const char **const_argv = (const char **)argv;
    cmdUtils.SendArguments(const_argv, const_argv + argc);
    cmdUtils.StartLoggingBasedOnCommand(&apiHandle);

    /*
     * Generate secure tunneling endpoint using region
     */
    String region = cmdUtils.GetCommandRequired("region");
    String endpoint = "data.tunneling.iot." + region + ".amazonaws.com";

    if (!(cmdUtils.HasCommand("access_token_file") || cmdUtils.HasCommand("access_token")))
    {
        cmdUtils.PrintHelp();
        fprintf(stderr, "--access_token_file or --access_token must be set to connect through a secure tunnel");
        exit(-1);
    }

    if (cmdUtils.HasCommand("access_token"))
    {
        accessToken = cmdUtils.GetCommand("access_token");
    }
    else
    {
        accessToken = cmdUtils.GetCommand("access_token_file");

        std::ifstream accessTokenFile(accessToken.c_str());
        if (accessTokenFile.is_open())
        {
            getline(accessTokenFile, accessToken);
            accessTokenFile.close();
        }
        else
        {
            fprintf(stderr, "Failed to open access token file");
            exit(-1);
        }
    }

    if (cmdUtils.HasCommand("client_token"))
    {
        clientToken = cmdUtils.GetCommand("client_token");
    }

    if (cmdUtils.HasCommand("client_token_file"))
    {
        clientToken = cmdUtils.GetCommand("client_token_file");

        std::ifstream clientTokenFile(clientToken.c_str());
        if (clientTokenFile.is_open())
        {
            getline(clientTokenFile, clientToken);
            clientTokenFile.close();
        }
        else
        {
            fprintf(stderr, "Failed to open client token file\n");
            exit(-1);
        }
    }

    if (cmdUtils.HasCommand("proxy_host") || cmdUtils.HasCommand("proxy_port"))
    {
        proxyHost =
            cmdUtils.GetCommandRequired("proxy_host", "--proxy_host must be set to connect through a proxy.").c_str();
        int port = atoi(
            cmdUtils.GetCommandRequired("proxy_port", "--proxy_port must be set to connect through a proxy.").c_str());
        if (port > 0 && port <= UINT16_MAX)
        {
            proxyPort = static_cast<uint16_t>(port);
        }
        proxyUserName = cmdUtils.GetCommandOrDefault("proxy_user_name", "");
        proxyPassword = cmdUtils.GetCommandOrDefault("proxy_password", "");
    }

    String caFile = cmdUtils.GetCommandOrDefault("ca_file", "");

    /*
     * localProxyMode is set to destination by default unless flag is set to source
     */
    if (cmdUtils.HasCommand("local_proxy_mode_source"))
    {
        localProxyMode = AWS_SECURE_TUNNELING_SOURCE_MODE;
    }
    else
    {
        localProxyMode = AWS_SECURE_TUNNELING_DESTINATION_MODE;
    }

    String payloadMessage = cmdUtils.GetCommandOrDefault("message", "Hello World");

    if (apiHandle.GetOrCreateStaticDefaultClientBootstrap()->LastError() != AWS_ERROR_SUCCESS)
    {
        fprintf(
            stderr,
            "ClientBootstrap failed with error %s\n",
            ErrorDebugString(apiHandle.GetOrCreateStaticDefaultClientBootstrap()->LastError()));
        exit(-1);
    }

    SecureTunnelBuilder builder = SecureTunnelBuilder(
        Aws::Crt::g_allocator, SocketOptions(), accessToken.c_str(), localProxyMode, endpoint.c_str());

    builder.WithRootCa(caFile.c_str());

    /* The client token is optional and will be generated automatically for reconnects. */
    builder.WithClientToken(clientToken.c_str());

    /* Add callbacks using the builder */

    builder.WithOnConnectionSuccess([&](SecureTunnel *secureTunnel, const ConnectionSuccessEventData &eventData) {
        connectionData = eventData.connectionData;

        if (connectionData->getServiceId1().has_value())
        {
            fprintf(
                stdout,
                "Secure Tunnel connected with  Service IDs '" PRInSTR "'",
                AWS_BYTE_CURSOR_PRI(connectionData->getServiceId1().value()));
            if (connectionData->getServiceId2().has_value())
            {
                fprintf(stdout, ", '" PRInSTR "'", AWS_BYTE_CURSOR_PRI(connectionData->getServiceId2().value()));
                if (connectionData->getServiceId3().has_value())
                {
                    fprintf(stdout, ", '" PRInSTR "'", AWS_BYTE_CURSOR_PRI(connectionData->getServiceId3().value()));
                }
            }
            fprintf(stdout, "\n");

            /* Stream Start can only be called from Source Mode */
            if (localProxyMode == AWS_SECURE_TUNNELING_SOURCE_MODE)
            {
                fprintf(
                    stdout,
                    "Sending Stream Start request with service id:'" PRInSTR "'\n",
                    AWS_BYTE_CURSOR_PRI(connectionData->getServiceId1().value()));
                secureTunnel->SendStreamStart(connectionData->getServiceId1().value());
            }
        }
        else
        {
            fprintf(stdout, "Secure Tunnel is not using Service Ids.\n");

            /* Stream Start can only be called from Source Mode */
            if (localProxyMode == AWS_SECURE_TUNNELING_SOURCE_MODE)
            {
                fprintf(stdout, "Sending Stream Start request\n");
                secureTunnel->SendStreamStart();
            }
        }

        connectionCompletedPromise.set_value(true);
    });

    builder.WithOnConnectionFailure([&](SecureTunnel *secureTunnel, int errorCode) {
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

                    /* Echo message on same service id received message came on */
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

    builder.WithOnStopped([&](SecureTunnel *secureTunnel) {
        fprintf(stdout, "Secure Tunnel has entered Stopped State\n");
        clientStoppedPromise.set_value(true);
    });

    //***********************************************************************************************************************
    /* Proxy Options */
    //***********************************************************************************************************************
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
                Aws::Crt::Http::HttpProxyStrategy::CreateBasicHttpProxyStrategy(basicAuthConfig, Aws::Crt::g_allocator);
            proxyOptions.AuthType = Aws::Crt::Http::AwsHttpProxyAuthenticationType::Basic;
        }
        else
        {
            proxyOptions.AuthType = Aws::Crt::Http::AwsHttpProxyAuthenticationType::None;
        }

        /* Add proxy options to the builder */
        builder.WithHttpClientConnectionProxyOptions(proxyOptions);
    }

    /* Create Secure Tunnel using the options set with the builder */
    std::shared_ptr<SecureTunnel> secureTunnel = builder.Build();

    if (!secureTunnel)
    {
        fprintf(stderr, "Secure Tunnel Creation failed: %s\n", ErrorDebugString(LastError()));
        exit(-1);
    }

    /* Set the Secure Tunnel to desire a connected state */
    if (secureTunnel->Start())
    {
        fprintf(stderr, "Secure Tunnel Connect call failed: %s\n", ErrorDebugString(LastError()));
        exit(-1);
    }

    int messageCount = 0;
    bool keepRunning = true;

    if (connectionCompletedPromise.get_future().get())
    {
        std::this_thread::sleep_for(1000ms);

        while (keepRunning)
        {
            if (localProxyMode == AWS_SECURE_TUNNELING_SOURCE_MODE)
            {
                messageCount++;
                String toSend = (std::to_string(messageCount) + ": " + payloadMessage.c_str()).c_str();

                if (messageCount < 5)
                {
                    std::shared_ptr<Message> message = std::make_shared<Message>(ByteCursorFromCString(toSend.c_str()));

                    if (connectionData->getServiceId1().has_value())
                    {
                        message->withServiceId(connectionData->getServiceId1().value());
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

    if (secureTunnel->Stop() == AWS_OP_ERR)
    {
        fprintf(stderr, "Secure Tunnel Close call failed: %s\n", ErrorDebugString(LastError()));
        exit(-1);
    }

    if (connectionClosedPromise.get_future().get())
    {
        fprintf(stdout, "Secure Tunnel Connection Closed\n");
    }

    if (clientStoppedPromise.get_future().get())
    {
        secureTunnel = nullptr;
    }

    fprintf(stdout, "Secure Tunnel Sample Completed\n");

    exit(0);
}
