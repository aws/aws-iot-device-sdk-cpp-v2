#include <aws/common/byte_buf.h>

#include <aws/crt/Api.h>
#include <aws/crt/UUID.h>
#include <aws/crt/io/HostResolver.h>
#include <aws/crt/io/SocketOptions.h>

#include <aws/http/http.h>

#include <aws/iot/MqttClient.h>

#include <aws/iotdevice/private/secure_tunneling_impl.h>
#include <aws/iotdevice/private/serializer.h>
#include <aws/iotdevice/secure_tunneling.h>

#include <aws/iotdevicecommon/IotDevice.h>

#include <aws/iotsecuretunneling/IotSecureTunnelingClient.h>
#include <aws/iotsecuretunneling/SecureTunnel.h>
#include <aws/iotsecuretunneling/SecureTunnelingNotifyResponse.h>
#include <aws/iotsecuretunneling/SubscribeToTunnelsNotifyRequest.h>

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

using namespace std;
using namespace Aws::Iot;
using namespace Aws::Crt;
using namespace Aws::Crt::Mqtt;
using namespace Aws::Iotsecuretunneling;
using namespace Aws::Crt::Io;
using namespace Aws::Iotdevicecommon;

static void s_printHelp()
{
    fprintf(stdout, "Usage:\n");
    fprintf(
        stdout,
        "secure-tunnel --tunnel_endpoint <endpoint> --cert <path to cert>"
        " --key <path to key> --ca_file <optional: path to custom ca>"
        " --thing_name <thing name> --access_token <path to access token> --localProxyModeSource\n\n");
    fprintf(stdout, "tunnel_endpoint: the endpoint of the mqtt server not including a port\n");
    fprintf(stdout, "cert: path to your client certificate in PEM format\n");
    fprintf(stdout, "key: path to your key in PEM format\n");
    fprintf(
        stdout,
        "ca_file: Optional, if the mqtt server uses a certificate that's not already"
        " in your trust store, set this.\n");
    fprintf(stdout, "\tIt's the path to a CA file in PEM format\n");
    fprintf(stdout, "thing_name: the name of your IOT thing\n");
    fprintf(stdout, "access_token: path to the tunneling access token file\n");
    fprintf(stdout, "localProxyModeSource: Use to set local proxy mode to source. Default is destination\n");
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

// TODO
// Receive a region and generate endpoint from it if you don't want to hardcode the endpoint
// Write comments and document what's going on in this sample

// Instructions:
// Start a client in destination mode and connect to secure tunnel using destination access token first
// Start a client in source mode and it will connect to the destination

// Connect SecureTunnel through Squid and just test that way
//

int main(int argc, char *argv[])
{
    ApiHandle apiHandle;

    String tunnelEndpoint;
    String certificatePath;
    String keyPath;
    String caFile;
    String clientId(String("test-") + Aws::Crt::UUID().ToString());
    String thingName;
    String accessToken;
    aws_secure_tunneling_local_proxy_mode localProxyMode;

    std::shared_ptr<SecureTunnel> mSecureTunnel;

    fprintf(stdout, "\n\n\nParsing Arguments\n\n");
    /*********************** Parse Arguments ***************************/
    if (!(s_cmdOptionExists(argv, argv + argc, "--tunnel_endpoint") && s_cmdOptionExists(argv, argv + argc, "--cert") &&
          s_cmdOptionExists(argv, argv + argc, "--key") && s_cmdOptionExists(argv, argv + argc, "--thing_name")))
    {
        s_printHelp();
        return 0;
    }

    tunnelEndpoint = s_getCmdOption(argv, argv + argc, "--tunnel_endpoint");
    certificatePath = s_getCmdOption(argv, argv + argc, "--cert");
    keyPath = s_getCmdOption(argv, argv + argc, "--key");
    thingName = s_getCmdOption(argv, argv + argc, "--thing_name");

    fprintf(stdout, "tunnelEndpoint: %s\n", tunnelEndpoint.c_str());

    if (s_cmdOptionExists(argv, argv + argc, "--ca_file"))
    {
        caFile = s_getCmdOption(argv, argv + argc, "--ca_file");
        fprintf(stdout, "ca_file: %s\n", caFile.c_str());
    }

    if (s_cmdOptionExists(argv, argv + argc, "--localProxyModeSource"))
    {
        localProxyMode = AWS_SECURE_TUNNELING_SOURCE_MODE;
        fprintf(stdout, "localProxyMode: Source\n");
    }
    else
    {
        localProxyMode = AWS_SECURE_TUNNELING_DESTINATION_MODE;
        fprintf(stdout, "localProxyMode: Destination\n");
    }

    if (s_cmdOptionExists(argv, argv + argc, "--access_token"))
    {
        accessToken = s_getCmdOption(argv, argv + argc, "--access_token");
        fprintf(stdout, "accessToken: %s\n", accessToken.c_str());

        ifstream accessTokenFile(accessToken.c_str());
        if (accessTokenFile.is_open())
        {
            fprintf(stdout, "accessToken was opened\n");
            getline(accessTokenFile, accessToken);
            fprintf(stdout, "access token: %s\n", accessToken.c_str());
            accessTokenFile.close();
        }
        else
        {
            fprintf(stderr, "Access Token file was not openable\n");
            exit(-1);
        }
    }

    fprintf(stdout, "\nArguments Parsed\n\n\n");

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

    auto OnConnectionComplete = [&]()
    {
        switch (localProxyMode)
        {
            case AWS_SECURE_TUNNELING_DESTINATION_MODE:
                fprintf(stdout, "\nI'm Connected in Destination Mode!\n");
                fprintf(stdout, "Time to do some connection complete stuff!\n");
                break;
            case AWS_SECURE_TUNNELING_SOURCE_MODE:
                fprintf(stdout, "\nI'm Connected in Source Mode!\n");
                fprintf(stdout, "Time to do some connection complete stuff!\n");
                fprintf(stdout, "I'm going to try SendStreamStart()!\n");
                mSecureTunnel->SendStreamStart();
                mSecureTunnel->SendData(ByteCursorFromCString("Hello World from Source! 👁️👄👁️"));
                break;
        }
    };

    auto OnConnectionShutdown = [&]()
    {
        fprintf(stdout, "\nI'm Shutdown!\n");
        fprintf(stdout, "Time to do some connection shutdown stuff!\n");
    };

    auto OnSendDataComplete = [&](int error_code)
    {
        switch (localProxyMode)
        {
            case AWS_SECURE_TUNNELING_DESTINATION_MODE:
                fprintf(stdout, "\nSend Data Complete in Destination Mode with error code %d!\n", error_code);
                fprintf(stdout, "Time to do some Send Data Complete stuff!\n");
                break;
            case AWS_SECURE_TUNNELING_SOURCE_MODE:
                fprintf(stdout, "\nSend Data Complete in Source Mode with error code %d!\n", error_code);
                fprintf(stdout, "Time to do some Send Data Complete stuff!\n");
                break;
        }
    };

    auto OnDataReceive = [&](const struct aws_byte_buf &data)
    {
        switch (localProxyMode)
        {
            case AWS_SECURE_TUNNELING_DESTINATION_MODE:
                fprintf(stdout, "\nData Receive Complete in Destination!\n");
                fprintf(stdout, "Time to do some Data Recieve stuff!\n");

                // Receive data here and process it, print it, then send it back to source
                break;
            case AWS_SECURE_TUNNELING_SOURCE_MODE:
                fprintf(stdout, "\nData Receive Complete in Source!\n");
                fprintf(stdout, "Time to do some Data Recieve stuff!\n");
                break;
        }
        string receivedData = std::string((char *)data.buffer, data.len);
        fprintf(stdout, "Received: %s\n", receivedData.c_str());
    };

    /*
     * This only fires in Destination Mode
     */
    auto OnStreamStart = [&]()
    {
        fprintf(stdout, "\nStream Started in Destination Mode!\n");
        fprintf(stdout, "Time to do some Stream Start stuff!\n");
        mSecureTunnel->SendData(ByteCursorFromCString("Hello World from Destination! 👁️👄👁️"));
    };

    auto OnStreamReset = [&]()
    {
        fprintf(stdout, "\nStream Reset!\n");
        fprintf(stdout, "Time to do some Stream Reset stuff!\n");
    };

    auto OnSessionReset = [&]()
    {
        fprintf(stdout, "\nSession Reset!\n");
        fprintf(stdout, "Time to do some Session Reset stuff!\n");
    };

    fprintf(stdout, "I'm gonna make a SecureTunnel!\n");

    mSecureTunnel = SecureTunnelBuilder(
                        Aws::Crt::g_allocator,
                        bootstrap,
                        SocketOptions(),
                        accessToken.c_str(),
                        localProxyMode,
                        tunnelEndpoint.c_str())
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
        fprintf(stderr, "SecureTunnel failed");
        exit(-1);
    }
    fprintf(stdout, "I'm gonna connect!\n");

    int connectReturn = 0;

    connectReturn = mSecureTunnel->Connect();

    fprintf(stdout, "mSecureTunnel->Connect() returned %d\n", connectReturn);

    // fprintf(stdout, "I'm gonna Close()!\n");
    // connectReturn = mSecureTunnel->Close();

    // fprintf(stdout, "mSecureTunnel->Close() returned %d\n", connectReturn);

    // // Hardcode for testing
    // // tunnelEndpoint = "data.tunneling.iot.us-east-1.amazonaws.com";
    // // tunnelEndpoint = "a340ry2giti5y3-ats.iot.us-east-1.amazonaws.com";

    // auto clientConfigBuilder = Aws::Iot::MqttClientConnectionConfigBuilder(certificatePath.c_str(), keyPath.c_str());
    // clientConfigBuilder.WithEndpoint(tunnelEndpoint);

    // if (!caFile.empty())
    // {
    //     clientConfigBuilder.WithCertificateAuthority(caFile.c_str());
    // }
    // auto clientConfig = clientConfigBuilder.Build();

    // if (!clientConfig)
    // {
    //     fprintf(
    //         stderr,
    //         "Client Configuration initialization failed with error %s\n",
    //         ErrorDebugString(clientConfig.LastError()));
    //     exit(-1);
    // }

    // /*
    //  * Now Create a client. This can not throw.
    //  * An instance of a client must outlive its connections.
    //  * It is the users responsibility to make sure of this.
    //  */
    // Aws::Iot::MqttClient mqttClient(bootstrap);

    // /*
    //  * Since no exceptions are used, always check the bool operator
    //  * when an error could have occurred.
    //  */
    // if (!mqttClient)
    // {
    //     fprintf(stderr, "MQTT Client Creation failed with error %s\n", ErrorDebugString(mqttClient.LastError()));
    //     exit(-1);
    // }

    // /*
    //  * Now create a connection object. Note: This type is move only
    //  * and its underlying memory is managed by the client.
    //  */
    // auto connection = mqttClient.NewConnection(clientConfig);

    // if (!*connection)
    // {
    //     fprintf(stderr, "MQTT Connection Creation failed with error %s\n",
    //     ErrorDebugString(connection->LastError())); exit(-1);
    // }

    // /*
    //  * In a real world application you probably don't want to enforce synchronous behavior
    //  * but this is a sample console application, so we'll just do that with a condition variable.
    //  */
    // std::promise<bool> connectionCompletedPromise;
    // std::promise<void> connectionClosedPromise;

    // /*
    //  * This will execute when an mqtt connect has completed or failed.
    //  */
    // auto onConnectionCompleted = [&](Mqtt::MqttConnection &, int errorCode, Mqtt::ReturnCode returnCode, bool)
    // {
    //     if (errorCode)
    //     {
    //         fprintf(stdout, "Connection failed with error %s\n", ErrorDebugString(errorCode));
    //         connectionCompletedPromise.set_value(false);
    //     }
    //     else
    //     {
    //         fprintf(stdout, "Connection completed with return code %d\n", returnCode);
    //         connectionCompletedPromise.set_value(true);
    //     }
    // };

    // /*
    //  * Invoked when a disconnect message has completed.
    //  */
    // auto onDisconnect = [&](Mqtt::MqttConnection & /*conn*/)
    // {
    //     {
    //         fprintf(stdout, "Disconnect completed\n");
    //         connectionClosedPromise.set_value();
    //     }
    // };

    // connection->OnConnectionCompleted = std::move(onConnectionCompleted);
    // connection->OnDisconnect = std::move(onDisconnect);

    // /*
    //  * Actually perform the connect dance.
    //  */
    // fprintf(stdout, "Connecting...\n");
    // if (!connection->Connect(clientId.c_str(), true, 0))
    // {
    //     fprintf(stderr, "MQTT Connection failed with error %s\n", ErrorDebugString(connection->LastError()));
    //     exit(-1);
    // }

    // auto onSubscribeToTunnelsNotifyResponse = [&](Aws::Iotsecuretunneling::SecureTunnelingNotifyResponse *response,
    //                                               int ioErr) -> void
    // {
    //     if (ioErr == 0)
    //     {
    //         fprintf(stdout, "Received MQTT Tunnel Notification\n");

    //         std::string clientAccessToken = response->ClientAccessToken->c_str();
    //         std::string clientMode = response->ClientMode->c_str();
    //         std::string region = response->Region->c_str();

    //         fprintf(
    //             stdout,
    //             "Recv: Token:%s, Mode:%s, Region:%s\n",
    //             clientAccessToken.c_str(),
    //             clientMode.c_str(),
    //             region.c_str());

    //         size_t nServices = response->Services->size();
    //         if (nServices <= 0)
    //         {
    //             fprintf(stdout, "No service requested\n");
    //         }
    //         else
    //         {
    //             std::string service = response->Services->at(0).c_str();
    //             fprintf(stdout, "Requested service=%s\n", service.c_str());

    //             if (nServices > 1)
    //             {
    //                 fprintf(stdout, "Multiple services not supported yet\n");
    //             }
    //         }
    //     }
    //     else
    //     {
    //         fprintf(stderr, "MQTT Connection failed with error %d\n", ioErr);
    //     }
    // };

    // auto OnSubscribeComplete = [&](int ioErr) -> void
    // {
    //     if (ioErr)
    //     {
    //         fprintf(stderr, "MQTT Connection failed with error %d\n", ioErr);
    //     }
    // };

    // if (connectionCompletedPromise.get_future().get())
    // {
    //     SubscribeToTunnelsNotifyRequest request;
    //     request.ThingName = thingName;

    //     IotSecureTunnelingClient client(connection);
    //     client.SubscribeToTunnelsNotify(
    //         request, AWS_MQTT_QOS_AT_LEAST_ONCE, onSubscribeToTunnelsNotifyResponse, OnSubscribeComplete);
    // }

    while (1)
    {
        std::this_thread::sleep_for(500ms);
        continue;
    }
}
