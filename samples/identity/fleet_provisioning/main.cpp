/*
 * Copyright 2010-2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */
#include <aws/crt/Api.h>
#include <aws/crt/JsonObject.h>
#include <aws/crt/UUID.h>
#include <aws/crt/io/HostResolver.h>

#include <aws/iot/MqttClient.h>

#include <aws/iotidentity/CreateCertificateFromCsrRequest.h>
#include <aws/iotidentity/CreateCertificateFromCsrResponse.h>
#include <aws/iotidentity/CreateCertificateFromCsrSubscriptionRequest.h>
#include <aws/iotidentity/CreateKeysAndCertificateRequest.h>
#include <aws/iotidentity/CreateKeysAndCertificateResponse.h>
#include <aws/iotidentity/CreateKeysAndCertificateSubscriptionRequest.h>
#include <aws/iotidentity/ErrorResponse.h>
#include <aws/iotidentity/IotIdentityClient.h>
#include <aws/iotidentity/RegisterThingRequest.h>
#include <aws/iotidentity/RegisterThingResponse.h>
#include <aws/iotidentity/RegisterThingSubscriptionRequest.h>

#include <algorithm>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>
#include <string>
#include<iostream>
#include<fstream>
#include<sstream>
#include <streambuf>

using namespace Aws::Crt;
using namespace Aws::Iotidentity;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

static void s_printHelp() {
    fprintf(stdout, "Usage:\n");
    fprintf(
        stdout,
        "fleet-provisioning --endpoint <endpoint> --ca_file <optional: path to custom ca> --cert <path to cert>"
        " --key <path to key> --templateName <template name> --templateParameters <template parameters> --csr <optional: path to csr> \n\n");
    fprintf(stdout, "endpoint: the endpoint of the mqtt server not including a port\n");
    fprintf(stdout, "cert: path to your client certificate in PEM format\n");
    fprintf(stdout, "csr: path to CSR in PEM format\n");
    fprintf(stdout, "key: path to your key in PEM format\n");
    fprintf(
        stdout,
        "ca_file: Optional, if the mqtt server uses a certificate that's not already"
        " in your trust store, set this.\n");
    fprintf(stdout, "\tIt's the path to a CA file in PEM format\n");
    fprintf(stdout, "template_name: the name of your provisioning template\n");
    fprintf(stdout, "template_parameters: template parameters json\n");
}

static bool s_cmdOptionExists(char **begin, char **end, const String &option) {
    return std::find(begin, end, option) != end;
}

static char *s_getCmdOption(char **begin, char **end, const String &option) {
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end) {
        return *itr;
    }
    return 0;
}

static void sleep(int sleeptime) {
    std::cout << "Sleeping for " << sleeptime << " seconds..." << std::endl;
    sleep_until(system_clock::now() + seconds(sleeptime));
}

static std::string getFileData(std::string const& fileName) {
    std::ifstream ifs(fileName);
    std::string str;
    getline(ifs, str, (char) ifs.eof());
    return str;
}

int main(int argc, char *argv[]) {
    /************************ Setup the Lib ****************************/
    /*
     * Do the global initialization for the API.
     */
    ApiHandle apiHandle;

    String endpoint;
    String certificatePath;
    String keyPath;
    String caFile;
    String clientId(Aws::Crt::UUID().ToString());
    String templateName;
    String templateParameters;
    String csrFile;

    String token;
    RegisterThingResponse registerThingResponse;

    /*********************** Parse Arguments ***************************/
    if (!(s_cmdOptionExists(argv, argv + argc, "--endpoint") &&
          s_cmdOptionExists(argv, argv + argc, "--cert") &&
          s_cmdOptionExists(argv, argv + argc, "--key") &&
          s_cmdOptionExists(argv, argv + argc, "--template_name") &&
          s_cmdOptionExists(argv, argv + argc, "--template_parameters"))) {
        s_printHelp();
        return 0;
    }

    endpoint = s_getCmdOption(argv, argv + argc, "--endpoint");
    certificatePath = s_getCmdOption(argv, argv + argc, "--cert");
    keyPath = s_getCmdOption(argv, argv + argc, "--key");
    templateName = s_getCmdOption(argv, argv + argc, "--template_name");
    templateParameters = s_getCmdOption(argv, argv + argc, "--template_parameters");

    if (s_cmdOptionExists(argv, argv + argc, "--ca_file")) {
        caFile = s_getCmdOption(argv, argv + argc, "--ca_file");
    }

    //if CSRFile provided
    if (s_cmdOptionExists(argv, argv + argc, "--csr")) {
        csrFile = getFileData(s_getCmdOption(argv, argv + argc, "--csr")).c_str();
    }

    /********************** Now Setup an Mqtt Client ******************/
    /*
     * You need an event loop group to process IO events.
     * If you only have a few connections, 1 thread is ideal
     */
    Io::EventLoopGroup eventLoopGroup(1);
    if (!eventLoopGroup) {
        fprintf(
            stderr, "Event Loop Group Creation failed with error %s\n", ErrorDebugString(eventLoopGroup.LastError()));
        exit(-1);
    }

    Io::DefaultHostResolver hostResolver(eventLoopGroup, 2, 30);
    Io::ClientBootstrap bootstrap(eventLoopGroup, hostResolver);

    if (!bootstrap) {
        fprintf(stderr, "ClientBootstrap failed with error %s\n", ErrorDebugString(bootstrap.LastError()));
        exit(-1);
    }

    auto clientConfigBuilder = Aws::Iot::MqttClientConnectionConfigBuilder(certificatePath.c_str(), keyPath.c_str());
    clientConfigBuilder.WithEndpoint(endpoint);
    if (!caFile.empty()) {
        clientConfigBuilder.WithCertificateAuthority(caFile.c_str());
    }
    auto clientConfig = clientConfigBuilder.Build();

    if (!clientConfig) {
        fprintf(
            stderr,
            "Client Configuration initialization failed with error %s\n",
            ErrorDebugString(clientConfig.LastError()));
        exit(-1);
    }

    /*
     * Now Create a client. This can not throw.
     * An instance of a client must outlive its connections.
     * It is the users responsibility to make sure of this.
     */
    Aws::Iot::MqttClient mqttClient(bootstrap);

    /*
     * Since no exceptions are used, always check the bool operator
     * when an error could have occured.
     */
    if (!mqttClient) {
        fprintf(stderr, "MQTT Client Creation failed with error %s\n", ErrorDebugString(mqttClient.LastError()));
        exit(-1);
    }
    /*
     * Now create a connection object. Note: This type is move only
     * and its underlying memory is managed by the client.
     */
    auto connection = mqttClient.NewConnection(clientConfig);

    if (!*connection) {
        fprintf(stderr, "MQTT Connection Creation failed with error %s\n", ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    /*
     * In a real world application you probably don't want to enforce synchronous behavior
     * but this is a sample console application, so we'll just do that with a condition variable.
     */
    std::condition_variable conditionVariable;
    std::atomic<bool> connectionSucceeded(false);
    std::atomic<bool> connectionClosed(false);
    std::atomic<bool> connectionCompleted(false);

    /*
     * This will execute when an mqtt connect has completed or failed.
     */
    auto onConnectionCompleted = [&](Mqtt::MqttConnection &, int errorCode, Mqtt::ReturnCode returnCode, bool) {
        if (errorCode) {
            fprintf(stdout, "Connection failed with error %s\n", ErrorDebugString(errorCode));
            connectionSucceeded = false;
        } else {
            fprintf(stdout, "Connection completed with return code %d\n", returnCode);
            connectionSucceeded = true;
        }

        connectionCompleted = true;
        conditionVariable.notify_one();
    };

    /*
     * Invoked when a disconnect message has completed.
     */
    auto onDisconnect = [&](Mqtt::MqttConnection & /*conn*/) {
        {
            fprintf(stdout, "Disconnect completed\n");
            connectionClosed = true;
        }
        conditionVariable.notify_one();
    };

    connection->OnConnectionCompleted = std::move(onConnectionCompleted);
    connection->OnDisconnect = std::move(onDisconnect);

    /*
     * Actually perform the connect dance.
     */
    fprintf(stdout, "Connecting...\n");
    if (!connection->Connect("client_id12335456", true, 0)) {
        fprintf(stderr, "MQTT Connection failed with error %s\n", ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    std::mutex mutex;
    std::unique_lock<std::mutex> uniqueLock(mutex);
    conditionVariable.wait(uniqueLock, [&]() { return connectionSucceeded || connectionClosed; });

    if (connectionSucceeded) {
        IotIdentityClient identityClient(connection);

        std::atomic<bool> csrPublishCompleted(false);
        std::atomic<bool> csrAcceptedCompleted(false);
        std::atomic<bool> csrRejectedCompleted(false);

        std::atomic<bool> keysPublishCompleted(false);
        std::atomic<bool> keysAcceptedCompleted(false);
        std::atomic<bool> keysRejectedCompleted(false);

        std::atomic<bool> registerPublishCompleted(false);
        std::atomic<bool> registerAcceptedCompleted(false);
        std::atomic<bool> registerRejectedCompleted(false);

        auto onCsrPublishSubAck = [&](int ioErr) {
            if (ioErr != AWS_OP_SUCCESS) {
                fprintf(stderr, "Error publishing to CreateCertificateFromCsr: %s\n", ErrorDebugString(ioErr));
                exit(-1);
            }

            csrPublishCompleted = true;
            conditionVariable.notify_one();
        };

        auto onCsrAcceptedSubAck = [&](int ioErr) {
            if (ioErr != AWS_OP_SUCCESS) {
                fprintf(stderr, "Error subscribing to CreateCertificateFromCsr accepted: %s\n", ErrorDebugString(ioErr));
                exit(-1);
            }

            csrAcceptedCompleted = true;
            conditionVariable.notify_one();
        };


        auto onCsrRejectedSubAck = [&](int ioErr) {
            if (ioErr != AWS_OP_SUCCESS) {
                fprintf(stderr, "Error subscribing to CreateCertificateFromCsr rejected: %s\n", ErrorDebugString(ioErr));
            }
            csrRejectedCompleted = true;
            conditionVariable.notify_one();
        };

        auto onCsrAccepted = [&](CreateCertificateFromCsrResponse *response, int ioErr) {
            if (ioErr == AWS_OP_SUCCESS) {
                fprintf(stdout, "CreateCertificateFromCsrResponse certificateId: %s.\n",
                    response->CertificateId->c_str());
                token = *response->CertificateOwnershipToken;
            } else {
                fprintf(stderr, "Error on subscription: %s.\n", ErrorDebugString(ioErr));
                exit(-1);
            }
        };

        auto onCsrRejected = [&](ErrorResponse *error, int ioErr) {
            if (ioErr == AWS_OP_SUCCESS) {
                fprintf(
                    stdout,
                    "CreateCertificateFromCsr failed with statusCode %d, errorMessage %s and errorCode %s.",
                    *error->StatusCode,
                    error->ErrorMessage->c_str(),
                    error->ErrorCode->c_str());
                    exit(-1);
            } else {
                fprintf(stderr, "Error on subscription: %s.\n", ErrorDebugString(ioErr));
                exit(-1);
            }
        };

        auto onKeysPublishSubAck = [&](int ioErr) {
            if (ioErr != AWS_OP_SUCCESS) {
                fprintf(stderr, "Error publishing to CreateKeysAndCertificate: %s\n", ErrorDebugString(ioErr));
                exit(-1);
            }

            keysPublishCompleted = true;
            conditionVariable.notify_one();
        };

        auto onKeysAcceptedSubAck = [&](int ioErr) {
            if (ioErr != AWS_OP_SUCCESS)
            {
                fprintf(stderr, "Error subscribing to CreateKeysAndCertificate accepted: %s\n", ErrorDebugString(ioErr));
                exit(-1);
            }

            keysAcceptedCompleted = true;
            conditionVariable.notify_one();
        };

        auto onKeysRejectedSubAck = [&](int ioErr) {
            if (ioErr != AWS_OP_SUCCESS) {
                fprintf(stderr, "Error subscribing to CreateKeysAndCertificate rejected: %s\n", ErrorDebugString(ioErr));
            }
            keysRejectedCompleted = true;
            conditionVariable.notify_one();
        };

        auto onKeysAccepted = [&](CreateKeysAndCertificateResponse *response, int ioErr) {
            if (ioErr == AWS_OP_SUCCESS) {
                fprintf(stdout, "CreateKeysAndCertificateResponse certificateId: %s.\n",
                    response->CertificateId->c_str());
                token = *response->CertificateOwnershipToken;
            } else {
                fprintf(stderr, "Error on subscription: %s.\n", ErrorDebugString(ioErr));
                exit(-1);
            }
        };

        auto onKeysRejected = [&](ErrorResponse *error, int ioErr) {
            if (ioErr == AWS_OP_SUCCESS) {
                fprintf(
                    stdout,
                    "CreateKeysAndCertificate failed with statusCode %d, errorMessage %s and errorCode %s.",
                    *error->StatusCode,
                    error->ErrorMessage->c_str(),
                    error->ErrorCode->c_str());
                    exit(-1);
            } else {
                fprintf(stderr, "Error on subscription: %s.\n", ErrorDebugString(ioErr));
                exit(-1);
            }
        };

        auto onRegisterAcceptedSubAck = [&](int ioErr) {
            if (ioErr != AWS_OP_SUCCESS) {
                fprintf(stderr, "Error subscribing to RegisterThing accepted: %s\n", ErrorDebugString(ioErr));
                exit(-1);
            }

            registerAcceptedCompleted = true;
            conditionVariable.notify_one();
        };

        auto onRegisterRejectedSubAck = [&](int ioErr) {
            if (ioErr != AWS_OP_SUCCESS) {
                fprintf(stderr, "Error subscribing to RegisterThing rejected: %s\n", ErrorDebugString(ioErr));
            }
            registerRejectedCompleted = true;
            conditionVariable.notify_one();
        };

        auto onRegisterAccepted = [&](RegisterThingResponse *response, int ioErr) {
            if (ioErr == AWS_OP_SUCCESS) {
                fprintf(stdout, "RegisterThingResponse ThingName: %s.\n",
                    response->ThingName->c_str());
            } else {
                fprintf(stderr, "Error on subscription: %s.\n", ErrorDebugString(ioErr));
                exit(-1);
            }
        };

        auto onRegisterRejected = [&](ErrorResponse *error, int ioErr) {
            if (ioErr == AWS_OP_SUCCESS) {
                fprintf(
                    stdout,
                    "RegisterThing failed with statusCode %d, errorMessage %s and errorCode %s.",
                    *error->StatusCode,
                    error->ErrorMessage->c_str(),
                    error->ErrorCode->c_str());
            } else {
                fprintf(stderr, "Error on subscription: %s.\n", ErrorDebugString(ioErr));
                exit(-1);
            }
        };

        auto onRegisterPublishSubAck = [&](int ioErr) {
            if (ioErr != AWS_OP_SUCCESS) {
                fprintf(stderr, "Error publishing to RegisterThing: %s\n", ErrorDebugString(ioErr));
                exit(-1);
            }

            registerPublishCompleted = true;
            conditionVariable.notify_one();
        };

        if (csrFile.empty()) {
            //CreateKeysAndCertificate workflow
            std::cout << "Subscribing to CreateKeysAndCertificate Accepted and Rejected topics" << std::endl;
            CreateKeysAndCertificateSubscriptionRequest keySubscriptionRequest;
            identityClient.SubscribeToCreateKeysAndCertificateAccepted(
                keySubscriptionRequest,
                AWS_MQTT_QOS_AT_LEAST_ONCE,
                onKeysAccepted,
                onKeysAcceptedSubAck);

            identityClient.SubscribeToCreateKeysAndCertificateRejected(
                keySubscriptionRequest,
                AWS_MQTT_QOS_AT_LEAST_ONCE,
                onKeysRejected,
                onKeysRejectedSubAck);

            std::cout << "Publishing to CreateKeysAndCertificate topic" << std::endl;
            CreateKeysAndCertificateRequest createKeysAndCertificateRequest;
            identityClient.PublishCreateKeysAndCertificate(
                createKeysAndCertificateRequest,
                AWS_MQTT_QOS_AT_LEAST_ONCE,
                onKeysPublishSubAck);

            std::cout << "Subscribing to RegisterThing Accepted and Rejected topics" << std::endl;
            RegisterThingSubscriptionRequest registerSubscriptionRequest;
            registerSubscriptionRequest.TemplateName = templateName;

            identityClient.SubscribeToRegisterThingAccepted(
                registerSubscriptionRequest,
                AWS_MQTT_QOS_AT_LEAST_ONCE,
                onRegisterAccepted,
                onRegisterAcceptedSubAck);

            identityClient.SubscribeToRegisterThingRejected(
                        registerSubscriptionRequest,
                        AWS_MQTT_QOS_AT_LEAST_ONCE,
                        onRegisterRejected,
                        onRegisterRejectedSubAck);

            sleep(1);

            std::cout << "Publishing to RegisterThing topic" << std::endl;
            RegisterThingRequest registerThingRequest;
            registerThingRequest.TemplateName = templateName;

            const Aws::Crt::String jsonValue = templateParameters;
            Aws::Crt::JsonObject value(jsonValue);
            Map<String, JsonView> pm = value.View().GetAllObjects();
            Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String> params = Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String>();

            for (const auto &x: pm) {
                params.emplace(x.first, x.second.AsString());
            }

            registerThingRequest.Parameters = params;
            registerThingRequest.CertificateOwnershipToken = token;

            identityClient.PublishRegisterThing(
                        registerThingRequest,
                        AWS_MQTT_QOS_AT_LEAST_ONCE,
                        onRegisterPublishSubAck);
            sleep(1);

            conditionVariable.wait(uniqueLock, [&]() {
                        return keysPublishCompleted.load() &&
                               keysAcceptedCompleted.load() &&
                               keysRejectedCompleted.load() &&
                               registerPublishCompleted.load() &&
                               registerAcceptedCompleted.load() &&
                               registerRejectedCompleted.load();
                    });
        } else {
            //CreateCertificateFromCsr workflow
            std::cout << "Subscribing to CreateCertificateFromCsr Accepted and Rejected topics" << std::endl;
            CreateCertificateFromCsrSubscriptionRequest csrSubscriptionRequest;
            identityClient.SubscribeToCreateCertificateFromCsrAccepted(
                csrSubscriptionRequest,
                AWS_MQTT_QOS_AT_LEAST_ONCE,
                onCsrAccepted,
                onCsrAcceptedSubAck);

            identityClient.SubscribeToCreateCertificateFromCsrRejected(
                csrSubscriptionRequest,
                AWS_MQTT_QOS_AT_LEAST_ONCE,
                onCsrRejected,
                onCsrRejectedSubAck);

            std::cout << "Publishing to CreateCertificateFromCsr topic" << std::endl;
            CreateCertificateFromCsrRequest createCertificateFromCsrRequest;
            createCertificateFromCsrRequest.CertificateSigningRequest = csrFile;
            identityClient.PublishCreateCertificateFromCsr(
                createCertificateFromCsrRequest,
                AWS_MQTT_QOS_AT_LEAST_ONCE,
                onCsrPublishSubAck);

            std::cout << "Subscribing to RegisterThing Accepted and Rejected topics" << std::endl;
            RegisterThingSubscriptionRequest registerSubscriptionRequest;
            registerSubscriptionRequest.TemplateName = templateName;

            identityClient.SubscribeToRegisterThingAccepted(
                registerSubscriptionRequest,
                AWS_MQTT_QOS_AT_LEAST_ONCE,
                onRegisterAccepted,
                onRegisterAcceptedSubAck);

            identityClient.SubscribeToRegisterThingRejected(
                        registerSubscriptionRequest,
                        AWS_MQTT_QOS_AT_LEAST_ONCE,
                        onRegisterRejected,
                        onRegisterRejectedSubAck);

            sleep(2);

            std::cout << "Publishing to RegisterThing topic" << std::endl;
            RegisterThingRequest registerThingRequest;
            registerThingRequest.TemplateName = templateName;

            const Aws::Crt::String jsonValue = templateParameters;
            Aws::Crt::JsonObject value(jsonValue);
            Map<String, JsonView> pm = value.View().GetAllObjects();
            Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String> params = Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String>();

            for (const auto &x: pm) {
                params.emplace(x.first, x.second.AsString());
            }

            registerThingRequest.Parameters = params;
            registerThingRequest.CertificateOwnershipToken = token;

            identityClient.PublishRegisterThing(
                        registerThingRequest,
                        AWS_MQTT_QOS_AT_LEAST_ONCE,
                        onRegisterPublishSubAck);
            sleep(2);

            conditionVariable.wait(uniqueLock, [&]() {
                        return csrPublishCompleted.load() &&
                               csrAcceptedCompleted.load() &&
                               csrRejectedCompleted.load() &&
                               registerPublishCompleted.load() &&
                               registerAcceptedCompleted.load() &&
                               registerRejectedCompleted.load();
                    });
        }

    }

    if (!connectionClosed) {
        /* Disconnect */
        connection->Disconnect();
        conditionVariable.wait(uniqueLock, [&]() { return connectionClosed.load(); });
    }
    return 0;
}
