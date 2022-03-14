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

#include "../../utils/CommandLineUtils.h"

using namespace Aws::Crt;
using namespace Aws::Iotsecuretunneling;
using namespace Aws::Crt::Io;
using namespace std::chrono_literals;

int main(int argc, char *argv[])
{
    ApiHandle apiHandle;

    String region;
    String endpoint;
    String caFile;
    String accessToken;
    String message = "Hello World";
    aws_secure_tunneling_local_proxy_mode localProxyMode;

    String proxyHost;
    uint16_t proxyPort(8080);
    String proxyUserName;
    String proxyPassword;

    std::shared_ptr<SecureTunnel> secureTunnel;

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
    cmdUtils.RegisterCommand("message", "<str>", "Message to send (optional, default='Hello World!').");
    cmdUtils.RegisterCommand("help", "", "Prints this message");
    cmdUtils.RegisterCommand("test", "", "Used to trigger internal testing (optional, ignore unless testing).");
    cmdUtils.RegisterCommand(
        "proxy_user_name", "<str>", "User name passed if proxy server requires a user name (optional)");
    cmdUtils.RegisterCommand(
        "proxy_password", "<str>", "Password passed if proxy server requires a password (optional)");
    const char **const_argv = (const char **)argv;
    cmdUtils.SendArguments(const_argv, const_argv + argc);

    if (cmdUtils.HasCommand("help"))
    {
        cmdUtils.PrintHelp();
        exit(-1);
    }
    /*
     * Generate secure tunneling endpoint using region
     */
    region = cmdUtils.GetCommandRequired("region");
    endpoint = "data.tunneling.iot." + region + ".amazonaws.com";

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

    caFile = cmdUtils.GetCommandOrDefault("ca_file", "");

    /*
     * localProxyMode is set to destination by default unless flag is set to source
     */
    if (cmdUtils.HasCommand("localProxyModeSource"))
    {
        localProxyMode = AWS_SECURE_TUNNELING_SOURCE_MODE;
    }
    else
    {
        localProxyMode = AWS_SECURE_TUNNELING_DESTINATION_MODE;
    }

    message = cmdUtils.GetCommandOrDefault("message", "Hello World");

    /*
     * For internal testing
     */
    bool isTest = cmdUtils.HasCommand("test");
    int expectedMessageCount = 5;

    if (apiHandle.GetOrCreateStaticDefaultClientBootstrap()->LastError() != AWS_ERROR_SUCCESS)
    {
        fprintf(
            stderr,
            "ClientBootstrap failed with error %s\n",
            ErrorDebugString(apiHandle.GetOrCreateStaticDefaultClientBootstrap()->LastError()));
        exit(-1);
    }

    /*
     * In a real world application you probably don't want to enforce synchronous behavior
     * but this is a sample console application, so we'll just do that with a condition variable.
     */
    std::promise<bool> connectionCompletedPromise;
    std::promise<bool> connectionClosedPromise;

    /*********************** Callbacks ***************************/
    auto OnConnectionComplete = [&]() {
        switch (localProxyMode)
        {
            case AWS_SECURE_TUNNELING_DESTINATION_MODE:
                fprintf(stdout, "Connection Complete in Destination Mode\n");
                break;
            case AWS_SECURE_TUNNELING_SOURCE_MODE:
                connectionCompletedPromise.set_value(true);
                fprintf(stdout, "Connection Complete in Source Mode\n");
                fprintf(stdout, "Sending Stream Start request\n");
                secureTunnel->SendStreamStart();
                break;
        }
    };

    auto OnConnectionShutdown = [&]() {
        fprintf(stdout, "Connection Shutdown\n");
        connectionClosedPromise.set_value(true);
    };

    auto OnSendDataComplete = [&](int error_code) {
        switch (localProxyMode)
        {
            case AWS_SECURE_TUNNELING_DESTINATION_MODE:
                if (!error_code)
                {
                    fprintf(stdout, "Send Data Complete in Destination Mode\n");
                }
                else
                {
                    fprintf(stderr, "Send Data Failed: %s\n", ErrorDebugString(error_code));
                }

                break;
            case AWS_SECURE_TUNNELING_SOURCE_MODE:
                if (!error_code)
                {
                    fprintf(stdout, "Send Data Complete in Source Mode\n");
                }
                else
                {
                    fprintf(stderr, "Send Data Failed: %s\n", ErrorDebugString(error_code));
                }
                break;
        }
    };

    auto OnDataReceive = [&](const struct aws_byte_buf &data) {
        String receivedData = String((char *)data.buffer, data.len);
        String returnMessage = "Echo:" + receivedData;

        fprintf(stdout, "Received: \"%s\"\n", receivedData.c_str());

        switch (localProxyMode)
        {
            case AWS_SECURE_TUNNELING_DESTINATION_MODE:
                fprintf(stdout, "Data Receive Complete in Destination\n");
                fprintf(stdout, "Sending response message:\"%s\"\n", returnMessage.c_str());
                secureTunnel->SendData(ByteCursorFromCString(returnMessage.c_str()));
                if (isTest)
                {
                    expectedMessageCount--;
                    if (expectedMessageCount == 0)
                    {
                        exit(0);
                    }
                }
                break;
            case AWS_SECURE_TUNNELING_SOURCE_MODE:
                fprintf(stdout, "Data Receive Complete in Source\n");
                break;
        }
    };

    /*
     * This only fires in Destination Mode
     */
    auto OnStreamStart = [&]() { fprintf(stdout, "Stream Started in Destination Mode\n"); };

    auto OnStreamReset = [&]() { fprintf(stdout, "Stream Reset\n"); };

    auto OnSessionReset = [&]() { fprintf(stdout, "Session Reset\n"); };

    /*********************** Proxy Connection Setup ***************************/
    /*
     * Setup HttpClientCommectionProxyOptions for connecting through a proxy before the Secure Tunnel
     */

    if (proxyHost.length() > 0)
    {
        auto proxyOptions = Aws::Crt::Http::HttpClientConnectionProxyOptions();
        proxyOptions.HostName = proxyHost.c_str();
        proxyOptions.Port = proxyPort;

        /*
         * Set up Proxy Strategy if a user name and password is provided
         */
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

        /*********************** Secure Tunnel Setup ***************************/
        /*
         * Create a new SecureTunnel using the SecureTunnelBuilder
         */
        secureTunnel =
            SecureTunnelBuilder(
                Aws::Crt::g_allocator, SocketOptions(), accessToken.c_str(), localProxyMode, endpoint.c_str())
                .WithRootCa(caFile.c_str())
                .WithHttpClientConnectionProxyOptions(proxyOptions)
                .WithOnConnectionComplete(OnConnectionComplete)
                .WithOnConnectionShutdown(OnConnectionShutdown)
                .WithOnSendDataComplete(OnSendDataComplete)
                .WithOnDataReceive(OnDataReceive)
                .WithOnStreamStart(OnStreamStart)
                .WithOnStreamReset(OnStreamReset)
                .WithOnSessionReset(OnSessionReset)
                .Build();
    }
    else
    {
        /*********************** Secure Tunnel Setup ***************************/
        /*
         * Create a new SecureTunnel using the SecureTunnelBuilder
         */
        secureTunnel =
            SecureTunnelBuilder(
                Aws::Crt::g_allocator, SocketOptions(), accessToken.c_str(), localProxyMode, endpoint.c_str())
                .WithRootCa(caFile.c_str())
                .WithOnConnectionComplete(OnConnectionComplete)
                .WithOnConnectionShutdown(OnConnectionShutdown)
                .WithOnSendDataComplete(OnSendDataComplete)
                .WithOnDataReceive(OnDataReceive)
                .WithOnStreamStart(OnStreamStart)
                .WithOnStreamReset(OnStreamReset)
                .WithOnSessionReset(OnSessionReset)
                .Build();
    }

    if (!secureTunnel)
    {
        fprintf(stderr, "Secure Tunnel Creation failed: %s\n", ErrorDebugString(LastError()));
        exit(-1);
    }

    if (secureTunnel->Connect() == AWS_OP_ERR)
    {
        fprintf(stderr, "Secure Tunnel Connect call failed: %s\n", ErrorDebugString(LastError()));
        exit(-1);
    }
    int messageCount = 0;

    if (connectionCompletedPromise.get_future().get())
    {
        while (1)
        {
            std::this_thread::sleep_for(3000ms);

            if (localProxyMode == AWS_SECURE_TUNNELING_SOURCE_MODE)
            {
                messageCount++;
                String toSend = (std::to_string(messageCount) + ": " + message.c_str()).c_str();

                if (!secureTunnel->SendData(ByteCursorFromCString(toSend.c_str())))
                {
                    fprintf(stdout, "Sending Message:\"%s\"\n", toSend.c_str());
                    if (messageCount >= 5)
                    {
                        fprintf(stdout, "Closing Connection\n");
                        if (secureTunnel->Close() == AWS_OP_ERR)
                        {
                            fprintf(stderr, "Secure Tunnel Close call failed: %s\n", ErrorDebugString(LastError()));
                            exit(-1);
                        }
                    }
                }
                else if (connectionClosedPromise.get_future().get())
                {
                    fprintf(stdout, "Sample Complete");

                    exit(0);
                }
            }
        }
    }
}
