/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/crt/Api.h>
#include <aws/iot/MqttClient.h>
#include <aws/iotsecuretunneling/IotSecureTunnelingClient.h>
#include <aws/iotsecuretunneling/SecureTunnel.h>
#include <aws/iotsecuretunneling/SecureTunnelingNotifyResponse.h>
#include <aws/iotsecuretunneling/SubscribeToTunnelsNotifyRequest.h>

#include <chrono>
#include <thread>


using namespace std;
using namespace Aws::Crt;
using namespace Aws::Crt::Mqtt;
using namespace Aws::Iotsecuretunneling;

int main(int argc, char *argv[])
{
    /************************ Setup ****************************/

    struct aws_allocator *allocator = aws_default_allocator();
    // Do the global initialization for the API.
    ApiHandle apiHandle;
    aws_iotdevice_library_init(allocator);
    std::shared_ptr<SecureTunnel> secureTunnel;

    /* --------------------------------- ARGUMENT PARSING ----------------------------------------- */
    struct CmdArgs
    {
        String endpoint;
        String cert;
        String key;
        String clientId;
        String caFile;
        String thingName;
    };

    auto printHelp = []() {
        printf("Tunnel Notification Sample\n");
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
            args.clientId = String("test-") + UUID().ToString();
        }
        return args;
    };

    CmdArgs cmdData = parseArgs(argc, argv);

    // Create the MQTT builder and populate it with data from cmdData.
    auto clientConfigBuilder =
        Aws::Iot::MqttClientConnectionConfigBuilder(cmdData.cert.c_str(), cmdData.key.c_str());
    clientConfigBuilder.WithEndpoint(cmdData.endpoint);
    if (!cmdData.caFile.empty())
    {
        clientConfigBuilder.WithCertificateAuthority(cmdData.caFile.c_str());
    }

    // Create the MQTT connection from the MQTT builder
    auto clientConfig = clientConfigBuilder.Build();
    if (!clientConfig)
    {
        fprintf(
            stderr,
            "Client Configuration initialization failed with error %s\n",
            Aws::Crt::ErrorDebugString(clientConfig.LastError()));
        exit(-1);
    }
    Aws::Iot::MqttClient client = Aws::Iot::MqttClient();
    auto connection = client.NewConnection(clientConfig);
    if (!*connection)
    {
        fprintf(
            stderr,
            "MQTT Connection Creation failed with error %s\n",
            Aws::Crt::ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    /**
     * In a real world application you probably don't want to enforce synchronous behavior
     * but this is a sample console application, so we'll just do that with a condition variable.
     */
    std::promise<bool> connectionCompletedPromise;
    std::promise<void> connectionClosedPromise;

    // Invoked when a MQTT connect has completed or failed
    auto onConnectionCompleted =
        [&](Aws::Crt::Mqtt::MqttConnection &, int errorCode, Aws::Crt::Mqtt::ReturnCode returnCode, bool) {
            if (errorCode)
            {
                fprintf(stdout, "Connection failed with error %s\n", Aws::Crt::ErrorDebugString(errorCode));
                connectionCompletedPromise.set_value(false);
            }
            else
            {
                fprintf(stdout, "Connection completed with return code %d\n", returnCode);
                connectionCompletedPromise.set_value(true);
            }
        };

    // Invoked when a disconnect message has completed.
    auto onDisconnect = [&](Aws::Crt::Mqtt::MqttConnection &) {
        fprintf(stdout, "Disconnect completed\n");
        connectionClosedPromise.set_value();
    };

    // Assign callbacks
    connection->OnConnectionCompleted = std::move(onConnectionCompleted);
    connection->OnDisconnect = std::move(onDisconnect);

    /************************ Run the sample ****************************/

    // Connect
    fprintf(stdout, "Connecting...\n");
    if (!connection->Connect(cmdData.clientId.c_str(), true, 0))
    {
        fprintf(stderr, "MQTT Connection failed with error %s\n", ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    auto onSubscribeToTunnelsNotifyResponse = [&](Aws::Iotsecuretunneling::SecureTunnelingNotifyResponse *response,
                                                  int ioErr) -> void {
        if (ioErr == 0)
        {
            fprintf(stdout, "Received MQTT Tunnel Notification\n");

            fprintf(
                stdout,
                "Recv:\n\tToken:%s\n\tMode:%s\n\tRegion:%s\n",
                response->ClientAccessToken->c_str(),
                response->ClientMode->c_str(),
                response->Region->c_str());

            Aws::Crt::String region = response->Region->c_str();
            Aws::Crt::String endpoint = "data.tunneling.iot." + region + ".amazonaws.com";

            size_t nServices = response->Services->size();
            if (nServices <= 0)
            {
                fprintf(stdout, "\tNo Service Ids requested\n");
            }
            else
            {
                for (size_t i = 0; i < nServices; ++i)
                {
                    std::string service = response->Services->at(i).c_str();
                    fprintf(stdout, "\tService Id %zu=%s\n", (i + 1), service.c_str());
                }
            }

            SecureTunnelBuilder builder = SecureTunnelBuilder(
                allocator,
                response->ClientAccessToken->c_str(),
                AWS_SECURE_TUNNELING_DESTINATION_MODE,
                endpoint.c_str());

            builder.WithOnConnectionSuccess(
                [&](SecureTunnel *secureTunnel, const ConnectionSuccessEventData &eventData) {
                    (void)secureTunnel;
                    (void)eventData;
                    fprintf(stdout, "Secure Tunnel connected to Secure Tunnel Service\n");
                });
            secureTunnel = builder.Build();

            if (!secureTunnel)
            {
                fprintf(stderr, "Secure Tunnel Creation failed: %s\n", ErrorDebugString(LastError()));
                exit(-1);
            }
            secureTunnel->Start();
        }
        else
        {
            fprintf(stderr, "MQTT Connection failed with error %d\n", ioErr);
        }
    };

    auto OnSubscribeComplete = [&](int ioErr) -> void {
        if (ioErr)
        {
            fprintf(stderr, "MQTT Connection failed with error %d\n", ioErr);
        }
    };

    if (connectionCompletedPromise.get_future().get())
    {
        SubscribeToTunnelsNotifyRequest request;
        request.ThingName = cmdData.thingName;

        IotSecureTunnelingClient secureClient(connection);
        secureClient.SubscribeToTunnelsNotify(
            request, AWS_MQTT_QOS_AT_LEAST_ONCE, onSubscribeToTunnelsNotifyResponse, OnSubscribeComplete);
    }

    while (1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        continue;
    }
}
