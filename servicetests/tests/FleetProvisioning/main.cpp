/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/JsonObject.h>
#include <aws/crt/UUID.h>
#include <aws/crt/io/HostResolver.h>

#include <aws/crt/mqtt/Mqtt5Packets.h>
#include <aws/iot/Mqtt5Client.h>
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
#include <aws/mqtt/mqtt.h>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <future>
#include <iostream>
#include <mutex>
#include <sstream>
#include <streambuf>
#include <string>
#include <thread>

#include "../../../samples/utils/CommandLineUtils.h"

using namespace Aws::Crt;
using namespace Aws::Iotidentity;

static std::promise<void> gotResponse;

static std::string getFileData(std::string const &fileName)
{
    std::ifstream ifs(fileName);
    std::string str;
    getline(ifs, str, (char)ifs.eof());
    return str;
}

std::shared_ptr<IotIdentityClient> build_mqtt3_client(
    Utils::cmdData &cmdData,
    std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> &connection,
    std::promise<bool> &connectionCompletedPromise,
    std::promise<void> &connectionClosedPromise)
{
    Aws::Iot::MqttClientConnectionConfigBuilder clientConfigBuilder;
    // Create the MQTT builder and populate it with data from cmdData.
    clientConfigBuilder =
        Aws::Iot::MqttClientConnectionConfigBuilder(cmdData.input_cert.c_str(), cmdData.input_key.c_str());
    clientConfigBuilder.WithEndpoint(cmdData.input_endpoint);
    if (cmdData.input_ca != "")
    {
        clientConfigBuilder.WithCertificateAuthority(cmdData.input_ca.c_str());
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

    Aws::Iot::MqttClient client3 = Aws::Iot::MqttClient();
    connection = client3.NewConnection(clientConfig);
    if (!*connection)
    {
        fprintf(
            stderr,
            "MQTT Connection Creation failed with error %s\n",
            Aws::Crt::ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    // Invoked when a MQTT connect has completed or failed
    auto onConnectionCompleted = [&](Mqtt::MqttConnection &, int errorCode, Mqtt::ReturnCode returnCode, bool) {
        if (errorCode)
        {
            fprintf(stdout, "Connection failed with error %s\n", ErrorDebugString(errorCode));
            connectionCompletedPromise.set_value(false);
        }
        else
        {
            fprintf(stdout, "Connection completed with return code %d\n", returnCode);
            connectionCompletedPromise.set_value(true);
        }
    };

    // Invoked when a disconnect has been completed
    auto onDisconnect = [&](Mqtt::MqttConnection & /*conn*/) {
        {
            fprintf(stdout, "Disconnect completed\n");
            connectionClosedPromise.set_value();
        }
    };

    connection->OnConnectionCompleted = std::move(onConnectionCompleted);
    connection->OnDisconnect = std::move(onDisconnect);

    if (!connection->Connect(cmdData.input_clientId.c_str(), true, 0))
    {
        fprintf(stderr, "MQTT Connection failed with error %s\n", ErrorDebugString(connection->LastError()));
        exit(-1);
    }
    return std::make_shared<IotIdentityClient>(connection);
}

std::shared_ptr<IotIdentityClient> build_mqtt5_client(
    Utils::cmdData &cmdData,
    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> &client5,
    std::promise<bool> &connectionCompletedPromise,
    std::promise<void> &connectionClosedPromise)
{
    std::shared_ptr<Aws::Iot::Mqtt5ClientBuilder> builder(
        Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(
            cmdData.input_endpoint, cmdData.input_cert.c_str(), cmdData.input_key.c_str()));

    // Check if the builder setup correctly.
    if (builder == nullptr)
    {
        printf(
            "Failed to setup mqtt5 client builder with error code %d: %s", LastError(), ErrorDebugString(LastError()));
        return nullptr;
    }
    // Create the MQTT5 builder and populate it with data from cmdData.
    // Setup connection options
    std::shared_ptr<Mqtt5::ConnectPacket> connectOptions = std::make_shared<Mqtt5::ConnectPacket>();
    connectOptions->WithClientId(cmdData.input_clientId);
    builder->WithConnectOptions(connectOptions);
    if (cmdData.input_port != 0)
    {
        builder->WithPort(static_cast<uint32_t>(cmdData.input_port));
    }
    // Setup lifecycle callbacks
    builder->WithClientConnectionSuccessCallback(
        [&connectionCompletedPromise](const Mqtt5::OnConnectionSuccessEventData &eventData) {
            fprintf(
                stdout,
                "Mqtt5 Client connection succeed, clientid: %s.\n",
                eventData.negotiatedSettings->getClientId().c_str());
            connectionCompletedPromise.set_value(true);
        });
    builder->WithClientConnectionFailureCallback([&connectionCompletedPromise](
                                                     const Mqtt5::OnConnectionFailureEventData &eventData) {
        fprintf(stdout, "Mqtt5 Client connection failed with error: %s.\n", aws_error_debug_str(eventData.errorCode));
        connectionCompletedPromise.set_value(false);
    });
    builder->WithClientStoppedCallback([&connectionClosedPromise](const Mqtt5::OnStoppedEventData &) {
        fprintf(stdout, "Mqtt5 Client stopped.\n");
        connectionClosedPromise.set_value();
    });

    client5 = builder->Build();
    if (client5 == nullptr)
    {
        fprintf(
            stdout, "Failed to Init Mqtt5Client with error code %d: %s.\n", LastError(), ErrorDebugString(LastError()));
        exit(-1);
    }

    if (!client5->Start())
    {
        fprintf(stderr, "MQTT5 Connection failed to start");
        exit(-1);
    }
    return std::make_shared<IotIdentityClient>(client5);
}

void SubscribeToRegisterThing(String input_templateName, std::shared_ptr<IotIdentityClient> iotIdentityClient)
{
    std::promise<void> onSubAckPromise;
    std::promise<void> registerRejectedCompletedPromise;
    std::promise<void> registerAcceptedCompletedPromise;

    auto onRegisterThingAccepted = [&](RegisterThingResponse *response, int ioErr) {
        if (ioErr)
        {
            fprintf(stderr, "Error: onSuback callback error %d\n", ioErr);
            exit(-1);
        }
        if (response)
        {
            fprintf(stdout, "Register thing: %s\n", response->ThingName.value().c_str());
        }
    };

    RegisterThingSubscriptionRequest registerThingSubscriptionRequest;
    registerThingSubscriptionRequest.TemplateName = input_templateName;

    auto onRegisterAcceptedSubAck = [&](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error subscribing to RegisterThing accepted: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }
        registerAcceptedCompletedPromise.set_value();
    };

    iotIdentityClient->SubscribeToRegisterThingAccepted(
        registerThingSubscriptionRequest,
        AWS_MQTT_QOS_AT_LEAST_ONCE,
        onRegisterThingAccepted,
        onRegisterAcceptedSubAck);

    auto rejectedHandler = [&](ErrorResponse *error, int ioErr) {
        if (ioErr)
        {
            fprintf(stderr, "Error: rejectedHandler callback error %d\n", ioErr);
            exit(-1);
        }
        else
        {
            fprintf(
                stdout,
                "RegisterThing failed with statusCode %d, errorMessage %s and errorCode %s.",
                *error->StatusCode,
                error->ErrorMessage->c_str(),
                error->ErrorCode->c_str());
        }
    };
    auto onRegisterRejectedSubAck = [&](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error subscribing to RegisterThing rejected: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }
        registerRejectedCompletedPromise.set_value();
    };
    iotIdentityClient->SubscribeToRegisterThingRejected(
        registerThingSubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, rejectedHandler, onRegisterRejectedSubAck);

    registerAcceptedCompletedPromise.get_future().wait();
    registerRejectedCompletedPromise.get_future().wait();
}

void createKeysAndCertificateWorkflow(
    String &input_templateName,
    String &input_templateParameters,
    std::shared_ptr<IotIdentityClient> iotIdentityClient)
{
    std::promise<void> onSubAckPromise;

    std::promise<void> keysPublishCompletedPromise;
    std::promise<void> keysAcceptedCompletedPromise;
    std::promise<void> keysRejectedCompletedPromise;

    std::promise<void> registerPublishCompletedPromise;
    std::promise<void> registerAcceptedCompletedPromise;

    CreateKeysAndCertificateResponse *createKeysAndCertificateResponse = nullptr;
    String token;

    auto acceptedHandler = [&createKeysAndCertificateResponse,
                            &token](CreateKeysAndCertificateResponse *response, int ioErr) {
        fprintf(stderr, "acceptedhandler is called\n");
        if (ioErr)
        {
            fprintf(stderr, "Error: acceptedHandler callback error %d\n", ioErr);
            exit(-1);
        }
        if (response)
        {
            if (createKeysAndCertificateResponse == nullptr)
            {
                createKeysAndCertificateResponse = response;
                token = *response->CertificateOwnershipToken;
            }
        }
        gotResponse.set_value();
    };

    auto onKeysAcceptedSubAck = [&](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error subscribing to CreateKeysAndCertificate accepted: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }

        keysAcceptedCompletedPromise.set_value();
    };

    CreateKeysAndCertificateSubscriptionRequest createKeysAndCertificateSubscriptionRequest;
    iotIdentityClient->SubscribeToCreateKeysAndCertificateAccepted(
        createKeysAndCertificateSubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, acceptedHandler, onKeysAcceptedSubAck);

    auto rejectedHandler = [&](Aws::Iotidentity::ErrorResponse *error, int ioErr) {
        if (ioErr == AWS_OP_ERR)
        {
            fprintf(stderr, "Error: rejectedHandler callback error %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }
        if (error)
        {
            fprintf(
                stdout,
                "CreateKeysAndCertificate failed with statusCode %d, errorMessage %s and errorCode %s.",
                *error->StatusCode,
                error->ErrorMessage->c_str(),
                error->ErrorCode->c_str());
        }
        exit(-1);
    };

    auto onKeysRejectedSubAck = [&](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error subscribing to CreateKeysAndCertificate rejected: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }
        keysRejectedCompletedPromise.set_value();
    };
    iotIdentityClient->SubscribeToCreateKeysAndCertificateRejected(
        createKeysAndCertificateSubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, rejectedHandler, onKeysRejectedSubAck);

    auto onKeysPublishSubAck = [&](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error publishing to CreateKeysAndCertificate: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }

        keysPublishCompletedPromise.set_value();
    };

    CreateKeysAndCertificateRequest createKeysAndCertificateRequest;
    iotIdentityClient->PublishCreateKeysAndCertificate(
        createKeysAndCertificateRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onKeysPublishSubAck);

    SubscribeToRegisterThing(input_templateName, iotIdentityClient);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Verify the response is good
    if (createKeysAndCertificateResponse == nullptr)
    {
        fprintf(stderr, "Error: 1 createKeysAndCertificateResponse is null\n");
        exit(-1);
    }
    // reset gotResponse future
    gotResponse = std::promise<void>();

    RegisterThingRequest registerThingRequest;
    registerThingRequest.TemplateName = input_templateName;

    if (!input_templateParameters.empty())
    {
        const Aws::Crt::String jsonValue = input_templateParameters;
        Aws::Crt::JsonObject value(jsonValue);
        Map<String, JsonView> pm = value.View().GetAllObjects();
        Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String> params = Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String>();

        for (const auto &x : pm)
        {
            params.emplace(x.first, x.second.AsString());
        }
        registerThingRequest.Parameters = params;
    }
    registerThingRequest.CertificateOwnershipToken = token;

    auto onRegisterPublishSubAck = [&](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error publishing to RegisterThing: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }

        registerPublishCompletedPromise.set_value();
    };
    iotIdentityClient->PublishRegisterThing(registerThingRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onRegisterPublishSubAck);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    keysPublishCompletedPromise.get_future().wait();
    keysAcceptedCompletedPromise.get_future().wait();
    keysRejectedCompletedPromise.get_future().wait();
    registerPublishCompletedPromise.get_future().wait();
}

void createCertificateFromCsrWorkflow(
    String input_templateName,
    String input_templateParameters,
    String input_csrPath,
    std::shared_ptr<IotIdentityClient> iotIdentityClient)
{
    std::promise<void> keysPublishCompletedPromise;
    std::promise<void> keysAcceptedCompletedPromise;
    std::promise<void> keysRejectedCompletedPromise;

    std::promise<void> registerPublishCompletedPromise;
    std::promise<void> registerAcceptedCompletedPromise;
    CreateCertificateFromCsrResponse *createCertificateFromCsrResponse = nullptr;
    String token;

    gotResponse = std::promise<void>();

    auto onCreateCertificateFromCsrResponseAccepted = [&createCertificateFromCsrResponse,
                                                       &token](CreateCertificateFromCsrResponse *response, int ioErr) {
        if (ioErr)
        {
            fprintf(stderr, "Error: onCreateCertificateFromCsrResponseAccepted callback error %d\n", ioErr);
            exit(-1);
        }
        if (response != nullptr)
        {
            if (createCertificateFromCsrResponse == nullptr)
            {
                createCertificateFromCsrResponse = response;
                token = *response->CertificateOwnershipToken;
            }
        }
        gotResponse.set_value();
    };

    auto onSubAck = [&](int ioErr) {
        if (ioErr)
        {
            fprintf(stderr, "Error: onSubAck callback error %d\n", ioErr);
            exit(-1);
        }
        keysAcceptedCompletedPromise.set_value();
    };

    CreateCertificateFromCsrSubscriptionRequest createCertificateFromCsrSubscriptionRequest;
    iotIdentityClient->SubscribeToCreateCertificateFromCsrAccepted(
        createCertificateFromCsrSubscriptionRequest,
        AWS_MQTT_QOS_AT_LEAST_ONCE,
        onCreateCertificateFromCsrResponseAccepted,
        onSubAck);

    auto onRejectedCsr = [&](ErrorResponse *response, int ioErr) {
        (void)response;
        if (ioErr)
        {
            fprintf(stderr, "Error: onSubAck callback error %d\n", ioErr);
            exit(-1);
        }
        exit(1);
    };

    auto onKeysRejectedSubAck = [&](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error subscribing to CreateKeysAndCertificate rejected: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }
        keysRejectedCompletedPromise.set_value();
    };

    iotIdentityClient->SubscribeToCreateCertificateFromCsrRejected(
        createCertificateFromCsrSubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onRejectedCsr, onKeysRejectedSubAck);

    auto onKeysPublishSubAck = [&](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error publishing to CreateKeysAndCertificate: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }
        keysPublishCompletedPromise.set_value();
    };

    std::string csrContents = getFileData(input_csrPath.c_str());
    CreateCertificateFromCsrRequest createCertificateFromCsrRequest;
    createCertificateFromCsrRequest.CertificateSigningRequest = csrContents.c_str();

    iotIdentityClient->PublishCreateCertificateFromCsr(
        createCertificateFromCsrRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onKeysPublishSubAck);

    // Subscribes to the register thing accepted and rejected topics
    SubscribeToRegisterThing(input_templateName, iotIdentityClient);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Verify the response is good
    if (createCertificateFromCsrResponse == nullptr)
    {
        fprintf(stderr, "Error: 1 createCertificateFromCsrResponse is null\n");
        exit(-1);
    }
    // reset gotResponse future
    gotResponse = std::promise<void>();

    RegisterThingRequest registerThingRequest;
    registerThingRequest.CertificateOwnershipToken = token;
    registerThingRequest.TemplateName = input_templateName;

    if (!input_templateParameters.empty())
    {
        const Aws::Crt::String jsonValue = input_templateParameters;
        Aws::Crt::JsonObject value(jsonValue);
        Map<String, JsonView> pm = value.View().GetAllObjects();
        Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String> params = Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String>();

        for (const auto &x : pm)
        {
            params.emplace(x.first, x.second.AsString());
        }
        registerThingRequest.Parameters = params;
    }

    auto onRegisterPublishSubAck = [&](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error publishing to RegisterThing: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }

        registerPublishCompletedPromise.set_value();
    };
    iotIdentityClient->PublishRegisterThing(registerThingRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onRegisterPublishSubAck);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    keysPublishCompletedPromise.get_future().wait();
    keysAcceptedCompletedPromise.get_future().wait();
    keysRejectedCompletedPromise.get_future().wait();
    registerPublishCompletedPromise.get_future().wait();
}

int main(int argc, char *argv[])
{
    // Do the global initialization for the API
    ApiHandle apiHandle;

    /**
     * cmdData is the arguments/input from the command line placed into a single struct for
     * use in this sample. This handles all of the command line parsing, validating, etc.
     * See the Utils/CommandLineUtils for more information.
     */
    Utils::cmdData cmdData = Utils::parseSampleInputFleetProvisioning(argc, argv, &apiHandle);

    /**
     * In a real world application you probably don't want to enforce synchronous behavior
     * but this is a sample console application, so we'll just do that with a condition variable.
     */
    std::promise<bool> connectionCompletedPromise;
    std::promise<void> connectionClosedPromise;
    std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> connection = nullptr;
    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> client5 = nullptr;
    std::shared_ptr<IotIdentityClient> iotIdentityClient = nullptr;

    if (cmdData.input_mqtt_version == 5UL)
    {
        iotIdentityClient = build_mqtt5_client(cmdData, client5, connectionCompletedPromise, connectionClosedPromise);
    }
    else if (cmdData.input_mqtt_version == 3UL)
    {
        iotIdentityClient =
            build_mqtt3_client(cmdData, connection, connectionCompletedPromise, connectionClosedPromise);
    }
    else
    {
        fprintf(stderr, "MQTT Version not supported\n");
        exit(-1);
    }
    if (connectionCompletedPromise.get_future().get())
    {
        if (cmdData.input_csrPath.empty())
        {
            createKeysAndCertificateWorkflow(
                cmdData.input_templateName, cmdData.input_templateParameters, iotIdentityClient);
        }
        else
        {
            createCertificateFromCsrWorkflow(
                cmdData.input_templateName, cmdData.input_templateParameters, cmdData.input_csrPath, iotIdentityClient);
        }
    }
    // Wait just a little bit to let the console print
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    if (cmdData.input_mqtt_version == 5UL)
    {
        // Disconnect
        if (client5->Stop() == true)
        {
            connectionClosedPromise.get_future().wait();
        }
    }
    else
    { // mqtt3

        // Disconnect
        if (connection->Disconnect() == true)
        {
            connectionClosedPromise.get_future().wait();
        }
    }
    return 0;
}
