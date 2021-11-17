/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
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
        "secure-tunnel\n--region <region>\n"
        "--ca_file <optional: path to custom ca>\n"
        "--access_token_file <path to access token> or "
        "--access_token <access token>\n"
        "--localProxyModeSource <optional: sets to Source Mode>\n"
        "--message <optional: message to send>\n\n");
    fprintf(stdout, "region: the region of your iot thing and secure tunnel\n");
    fprintf(
        stdout,
        "ca_file: Optional, if the mqtt server uses a certificate that's not already"
        " in your trust store, set this.\n");
    fprintf(stdout, "\tIt's the path to a CA file in PEM format\n");
    fprintf(stdout, "access_token_file: path to the tunneling access token file\n");
    fprintf(stdout, "access_token: tunneling access token\n");
    fprintf(stdout, "localProxyModeSource: Use to set local proxy mode to source. Default is destination\n");
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

    String region;
    String endpoint;
    String caFile;
    String accessToken;
    String message = "Hello World";
    string receivedData = "";
    aws_secure_tunneling_local_proxy_mode localProxyMode;
    std::shared_ptr<SecureTunnel> mSecureTunnel;

    /*********************** Parse Arguments ***************************/
    if (!s_cmdOptionExists(argv, argv + argc, "--region"))
    {
        fprintf(stderr, "--region must be set to connect through a secure tunnel");
        s_printHelp();
        exit(-1);
    }

    if (!(s_cmdOptionExists(argv, argv + argc, "--access_token_file") ||
          s_cmdOptionExists(argv, argv + argc, "--access_token")))
    {
        fprintf(stderr, "--access_token_file or --access_token must be set to connect through a secure tunnel");
        s_printHelp();
        exit(-1);
    }

    /*
     * Generate secure tunneling endpoint using region
     */
    region = s_getCmdOption(argv, argv + argc, "--region");
    endpoint = "data.tunneling.iot." + region + ".amazonaws.com";

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

    if (s_cmdOptionExists(argv, argv + argc, "--message"))
    {
        message = s_getCmdOption(argv, argv + argc, "--message");
    }

    Io::EventLoopGroup eventLoopGroup(1);
    if (!eventLoopGroup)
    {
        fprintf(
            stderr, "Event Loop Group Creation failed with error %s\n", ErrorDebugString(eventLoopGroup.LastError()));
        exit(-1);
    }

    Io::DefaultHostResolver defaultHostResolver(eventLoopGroup, 2, 30);
    Io::ClientBootstrap bootstrap(eventLoopGroup, defaultHostResolver);

    if (!bootstrap)
    {
        fprintf(stderr, "ClientBootstrap failed with error %s\n", ErrorDebugString(bootstrap.LastError()));
        exit(-1);
    }

    /*********************** Callbacks ***************************/
    auto OnConnectionComplete = [&]() {
        switch (localProxyMode)
        {
            case AWS_SECURE_TUNNELING_DESTINATION_MODE:
                fprintf(stdout, "Connection Complete in Destination Mode\n");
                break;
            case AWS_SECURE_TUNNELING_SOURCE_MODE:
                fprintf(stdout, "Connection Complete in Source Mode\n");
                fprintf(stdout, "Sending Stream Start request and message\n");
                mSecureTunnel->SendStreamStart();
                // mSecureTunnel->SendData(ByteCursorFromCString(message.c_str()));
                break;
        }
    };

    auto OnConnectionShutdown = [&]() { fprintf(stdout, "Connection Shutdown\n"); };

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
                    fprintf(stderr, "Send Data Failed with error code %d\n", error_code);
                }

                break;
            case AWS_SECURE_TUNNELING_SOURCE_MODE:
                if (!error_code)
                {
                    fprintf(stdout, "Send Data Complete in Source Mode\n");
                }
                else
                {
                    fprintf(stderr, "Send Data Failed with error code %d\n", error_code);
                }
                break;
        }
    };

    auto OnDataReceive = [&](const struct aws_byte_buf &data) {
        receivedData = std::string((char *)data.buffer, data.len);
        string returnMessage = "Echo: " + receivedData;

        fprintf(stdout, "Received: %s\n", receivedData.c_str());

        switch (localProxyMode)
        {
            case AWS_SECURE_TUNNELING_DESTINATION_MODE:
                fprintf(stdout, "Data Receive Complete in Destination\n");
                mSecureTunnel->SendData(ByteCursorFromCString(returnMessage.c_str()));
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

    /*********************** Secure Tunnel Setup ***************************/

    /*
     * Create a new SecureTunnel using the SecureTunnelBuilder
     */
    mSecureTunnel =
        SecureTunnelBuilder(
            Aws::Crt::g_allocator, bootstrap, SocketOptions(), accessToken.c_str(), localProxyMode, endpoint.c_str())
            .WithRootCa(caFile.c_str())
            .WithOnConnectionComplete(OnConnectionComplete)
            .WithOnConnectionShutdown(OnConnectionShutdown)
            .WithOnSendDataComplete(OnSendDataComplete)
            .WithOnDataReceive(OnDataReceive)
            .WithOnStreamStart(OnStreamStart)
            .WithOnStreamReset(OnStreamReset)
            .WithOnSessionReset(OnSessionReset)
            .Build();

    if (!mSecureTunnel)
    {
        fprintf(stderr, "Secure Tunnel Creation failed");
        exit(-1);
    }

    mSecureTunnel->Connect();
    int messageCount = 0;

    while (1)
    {
        std::this_thread::sleep_for(3000ms);
        if (localProxyMode == AWS_SECURE_TUNNELING_SOURCE_MODE)
        {
            string toSend = to_string(messageCount) + ": " + message.c_str();
            if (!mSecureTunnel->SendData(ByteCursorFromCString(toSend.c_str())))
            {
                messageCount++;
                if (messageCount > 5)
                {
                    fprintf(stdout, "Closing Connection\n");
                    mSecureTunnel->Close();
                }
            }
            else
            {
                fprintf(stdout, "Sample Complete");
                exit(0);
            }
        }
    }
}
