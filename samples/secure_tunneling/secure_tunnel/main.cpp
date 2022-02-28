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

using namespace std;
using namespace Aws::Crt;
using namespace Aws::Iotsecuretunneling;
using namespace Aws::Crt::Io;

static void s_printHelp()
{
    fprintf(stdout, "Usage:\n");
    fprintf(
        stdout,
        "secure-tunnel\n"
        "--region                <aws region of secure tunnel>\n"
        "--ca_file               <optional: path to custom ca>\n"
        "--access_token_file     <path to access token> or "
        "--access_token          <access token>\n"
        "--localProxyModeSource  <optional: sets to Source Mode>\n"
        "--proxy_host            <host name of the proxy server>\n"
        "--proxy_port            <port of the proxy server>\n"
        "--proxy_user_name       <optional: user name>\n"
        "--proxy_password        <optional: password>\n"
        "--message               <optional: message to send>\n\n");
    fprintf(stdout, "region: the region of your secure tunnel\n");
    fprintf(
        stdout,
        "ca_file: Optional, if the mqtt server uses a certificate that's not already"
        " in your trust store, set this.\n");
    fprintf(stdout, "\tIt's the path to a CA file in PEM format\n");
    fprintf(stdout, "access_token_file: path to the tunneling access token file\n");
    fprintf(stdout, "access_token: tunneling access token\n");
    fprintf(stdout, "localProxyModeSource: Use to set local proxy mode to source. Default is destination\n");
    fprintf(stdout, "proxy_host: Host name of the proxy server to connect through\n");
    fprintf(stdout, "proxy_port: Port of the proxy server to connect through\n");
    fprintf(stdout, "proxy_user_name: Optional, if proxy server requires a user name\n");
    fprintf(stdout, "proxy_password: Optional, if proxy server requires a password\n");
    fprintf(stdout, "message: message to send. Default: 'Hello World'\n");
}

bool s_cmdOptionExists(char **begin, char **end, const String &option)
{
    return std::find(begin, end, option) != end;
}

char *s_getCmdOption(char **begin, char **end, const String &option)
{
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    ApiHandle apiHandle;

    string region;
    string endpoint;
    string caFile;
    string accessToken;
    string message = "Hello World";
    aws_secure_tunneling_local_proxy_mode localProxyMode;

    string proxyHost;
    uint16_t proxyPort(8080);
    string proxyUserName;
    string proxyPassword;

    /*
     * For internal testing
     */
    bool isTest = s_cmdOptionExists(argv, argv + argc, "--test");
    int expectedMessageCount = 5;

    std::shared_ptr<SecureTunnel> secureTunnel;

    /*********************** Parse Arguments ***************************/
    if (!s_cmdOptionExists(argv, argv + argc, "--region"))
    {
        fprintf(stderr, "--region must be set to connect through a secure tunnel");
        s_printHelp();
        exit(-1);
    }
    /*
     * Generate secure tunneling endpoint using region
     */
    region = s_getCmdOption(argv, argv + argc, "--region");
    endpoint = "data.tunneling.iot." + region + ".amazonaws.com";

    if (!(s_cmdOptionExists(argv, argv + argc, "--access_token_file") ||
          s_cmdOptionExists(argv, argv + argc, "--access_token")))
    {
        fprintf(stderr, "--access_token_file or --access_token must be set to connect through a secure tunnel");
        s_printHelp();
        exit(-1);
    }

    /*
     * Set accessToken either directly or from a file
     */
    if (s_cmdOptionExists(argv, argv + argc, "--access_token"))
    {
        accessToken = s_getCmdOption(argv, argv + argc, "--access_token");
    }
    else if (s_cmdOptionExists(argv, argv + argc, "--access_token_file"))
    {
        accessToken = s_getCmdOption(argv, argv + argc, "--access_token_file");

        ifstream accessTokenFile(accessToken.c_str());
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
    if (s_cmdOptionExists(argv, argv + argc, "--proxy_host") || s_cmdOptionExists(argv, argv + argc, "--proxy_port"))
    {
        if (!s_cmdOptionExists(argv, argv + argc, "--proxy_host"))
        {
            fprintf(stderr, "--proxy_host must be set to connect through a proxy");
            s_printHelp();
            exit(-1);
        }
        proxyHost = s_getCmdOption(argv, argv + argc, "--proxy_host");

        if (!s_cmdOptionExists(argv, argv + argc, "--proxy_port"))
        {
            fprintf(stderr, "--proxy_port must be set to connect through a proxy");
            s_printHelp();
            exit(-1);
        }
        int port = atoi(s_getCmdOption(argv, argv + argc, "--proxy_port"));
        if (port > 0 && port <= UINT16_MAX)
        {
            proxyPort = static_cast<uint16_t>(port);
        }

        if (s_cmdOptionExists(argv, argv + argc, "--proxy_user_name"))
        {
            proxyUserName = s_getCmdOption(argv, argv + argc, "--proxy_user_name");
        }

        if (s_cmdOptionExists(argv, argv + argc, "--proxy_password"))
        {
            proxyPassword = s_getCmdOption(argv, argv + argc, "--proxy_password");
        }
    }

    if (s_cmdOptionExists(argv, argv + argc, "--ca_file"))
    {
        caFile = s_getCmdOption(argv, argv + argc, "--ca_file");
    }

    /*
     * localProxyMode is set to destination by default unless flag is set to source
     */
    if (s_cmdOptionExists(argv, argv + argc, "--localProxyModeSource"))
    {
        localProxyMode = AWS_SECURE_TUNNELING_SOURCE_MODE;
    }
    else
    {
        localProxyMode = AWS_SECURE_TUNNELING_DESTINATION_MODE;
    }

    if (s_cmdOptionExists(argv, argv + argc, "--message"))
    {
        message = s_getCmdOption(argv, argv + argc, "--message");
    }

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
        string receivedData = std::string((char *)data.buffer, data.len);
        string returnMessage = "Echo:" + receivedData;

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
                string toSend = to_string(messageCount) + ": " + message.c_str();

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
