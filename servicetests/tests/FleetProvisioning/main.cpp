/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/crt/Api.h>
#include <aws/crt/JsonObject.h>
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

#include <fstream>

#include "../../../samples/utils/CommandLineUtils.h"

using namespace Aws::Crt;
using namespace Aws::Iotidentity;

static String getFileData(const String &fileName)
{
    std::ifstream ifs(fileName.c_str());
    std::string str;
    getline(ifs, str, (char)ifs.eof());
    return str.c_str();
}

/**
 * Auxiliary structure for holding data used by MQTT3 connection.
 */
struct Mqtt3ConnectionContext
{
    std::promise<bool> connectionCompletedPromise;
    std::promise<void> connectionClosedPromise;
};

/**
 * Auxiliary structure for holding data used by MQTT5 connection.
 */
struct Mqtt5ClientContext
{
    std::promise<bool> connectionPromise;
    std::promise<void> stoppedPromise;
    std::promise<void> disconnectPromise;
    std::promise<bool> subscribeSuccess;
};

/**
 * Auxiliary structure for holding data used when creating a certificate.
 */
struct CreateCertificateContext
{
    std::promise<void> pubAckPromise;
    std::promise<void> acceptedSubAckPromise;
    std::promise<void> rejectedSubAckPromise;
    std::promise<String> tokenPromise;
};

/**
 * Auxiliary structure for holding data used when registering a thing.
 */
struct RegisterThingContext
{
    std::promise<void> pubAckPromise;
    std::promise<void> acceptedSubAckPromise;
    std::promise<void> rejectedSubAckPromise;
    std::promise<void> thingCreatedPromise;
};

/**
 * Create MQTT5 client.
 */
std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> createMqtt5Client(const Utils::cmdData &cmdData, Mqtt5ClientContext &ctx)
{
    // Create the MQTT5 builder and populate it with data from cmdData.
    auto builder = std::unique_ptr<Aws::Iot::Mqtt5ClientBuilder>(
        Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(
            cmdData.input_endpoint, cmdData.input_cert.c_str(), cmdData.input_key.c_str()));

    // Check if the builder setup correctly.
    if (builder == nullptr)
    {
        printf(
            "Failed to setup mqtt5 client builder with error code %d: %s", LastError(), ErrorDebugString(LastError()));
        return nullptr;
    }

    // Setup connection options
    std::shared_ptr<Mqtt5::ConnectPacket> connectOptions =
        Aws::Crt::MakeShared<Mqtt5::ConnectPacket>(Aws::Crt::DefaultAllocatorImplementation());
    connectOptions->WithClientId(cmdData.input_clientId);
    builder->WithConnectOptions(connectOptions);
    if (cmdData.input_port != 0)
    {
        builder->WithPort(static_cast<uint32_t>(cmdData.input_port));
    }

    // Setup lifecycle callbacks
    builder->WithClientConnectionSuccessCallback([&ctx](const Mqtt5::OnConnectionSuccessEventData &eventData) {
        fprintf(
            stdout,
            "Mqtt5 Client connection succeed, clientid: %s.\n",
            eventData.negotiatedSettings->getClientId().c_str());
        ctx.connectionPromise.set_value(true);
    });
    builder->WithClientConnectionFailureCallback([&ctx](const Mqtt5::OnConnectionFailureEventData &eventData) {
        fprintf(stdout, "Mqtt5 Client connection failed with error: %s.\n", aws_error_debug_str(eventData.errorCode));
        ctx.connectionPromise.set_value(false);
    });
    builder->WithClientStoppedCallback([&ctx](const Mqtt5::OnStoppedEventData &) {
        fprintf(stdout, "Mqtt5 Client stopped.\n");
        ctx.stoppedPromise.set_value();
    });
    builder->WithClientAttemptingConnectCallback([](const Mqtt5::OnAttemptingConnectEventData &) {
        fprintf(stdout, "Mqtt5 Client attempting connection...\n");
    });
    builder->WithClientDisconnectionCallback([&ctx](const Mqtt5::OnDisconnectionEventData &eventData) {
        fprintf(stdout, "Mqtt5 Client disconnection with reason: %s.\n", aws_error_debug_str(eventData.errorCode));
        ctx.disconnectPromise.set_value();
    });

    // Create Mqtt5Client
    auto client = builder->Build();

    fprintf(stdout, "Connecting...\n");
    if (!client->Start())
    {
        fprintf(stderr, "MQTT5 Connection failed to start");
        return nullptr;
    }

    if (!ctx.connectionPromise.get_future().get())
    {
        return nullptr;
    }

    return client;
}

/**
 * Create MQTT3 connection.
 */
std::shared_ptr<Mqtt::MqttConnection> createMqtt3Connection(const Utils::cmdData &cmdData, Mqtt3ConnectionContext &ctx)
{
    /**
     * In a real world application you probably don't want to enforce synchronous behavior
     * but this is a sample console application, so we'll just do that with a promise.
     */

    // Invoked when a MQTT connect has completed or failed
    auto onConnectionCompleted = [&ctx](Mqtt::MqttConnection &, int errorCode, Mqtt::ReturnCode returnCode, bool) {
        if (errorCode)
        {
            fprintf(stdout, "Connection failed with error %s\n", ErrorDebugString(errorCode));
            ctx.connectionCompletedPromise.set_value(false);
        }
        else
        {
            fprintf(stdout, "Connection completed with return code %d\n", returnCode);
            ctx.connectionCompletedPromise.set_value(true);
        }
    };

    // Invoked when a disconnect has been completed
    auto onDisconnect = [&ctx](Mqtt::MqttConnection & /*conn*/) {
        {
            fprintf(stdout, "Disconnect completed\n");
            ctx.connectionClosedPromise.set_value();
        }
    };

    // Create the MQTT builder and populate it with data from cmdData.
    auto clientConfigBuilder =
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
        return nullptr;
    }
    Aws::Iot::MqttClient client = Aws::Iot::MqttClient();
    auto connection = client.NewConnection(clientConfig);
    if (!*connection)
    {
        fprintf(
            stderr,
            "MQTT Connection Creation failed with error %s\n",
            Aws::Crt::ErrorDebugString(connection->LastError()));
        return nullptr;
    }

    connection->OnConnectionCompleted = std::move(onConnectionCompleted);
    connection->OnDisconnect = std::move(onDisconnect);

    fprintf(stdout, "Connecting...\n");
    if (!connection->Connect(cmdData.input_clientId.c_str(), true, 0))
    {
        fprintf(stderr, "MQTT Connection failed with error %s\n", ErrorDebugString(connection->LastError()));
        return nullptr;
    }

    if (!ctx.connectionCompletedPromise.get_future().get())
    {
        return nullptr;
    }

    return connection;
}

/**
 * Keys-and-Certificate workflow.
 *
 * @note Subscriptions created here will be active even after the function completes. So, all variables accessed in the
 * callbacks must be alive for the whole duration of the identityClient's lifetime. An instance of
 * CreateCertificateContext is used to store variables used by the callbacks.
 */
void createKeysAndCertificate(const std::shared_ptr<IotIdentityClient> &identityClient, CreateCertificateContext &ctx)
{
    auto onKeysPublishPubAck = [&ctx](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error publishing to CreateKeysAndCertificate: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }
        ctx.pubAckPromise.set_value();
    };

    auto onKeysAcceptedSubAck = [&ctx](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error subscribing to CreateKeysAndCertificate accepted: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }
        ctx.acceptedSubAckPromise.set_value();
    };

    auto onKeysRejectedSubAck = [&ctx](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error subscribing to CreateKeysAndCertificate rejected: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }
        ctx.rejectedSubAckPromise.set_value();
    };

    auto onKeysAccepted = [&ctx](CreateKeysAndCertificateResponse *response, int ioErr) {
        if (ioErr == AWS_OP_SUCCESS)
        {
            fprintf(stdout, "CreateKeysAndCertificateResponse certificateId: %s.\n", response->CertificateId->c_str());
            ctx.tokenPromise.set_value(*response->CertificateOwnershipToken);
        }
        else
        {
            fprintf(stderr, "Error on subscription: %s.\n", ErrorDebugString(ioErr));
            exit(-1);
        }
    };

    auto onKeysRejected = [](ErrorResponse *error, int ioErr) {
        if (ioErr == AWS_OP_SUCCESS)
        {
            fprintf(
                stdout,
                "CreateKeysAndCertificate failed with statusCode %d, errorMessage %s and errorCode %s.",
                *error->StatusCode,
                error->ErrorMessage->c_str(),
                error->ErrorCode->c_str());
            exit(-1);
        }
        else
        {
            fprintf(stderr, "Error on subscription: %s.\n", ErrorDebugString(ioErr));
            exit(-1);
        }
    };

    fprintf(stdout, "Subscribing to CreateKeysAndCertificate Accepted and Rejected topics\n");
    CreateKeysAndCertificateSubscriptionRequest keySubscriptionRequest;
    identityClient->SubscribeToCreateKeysAndCertificateAccepted(
        keySubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onKeysAccepted, onKeysAcceptedSubAck);
    identityClient->SubscribeToCreateKeysAndCertificateRejected(
        keySubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onKeysRejected, onKeysRejectedSubAck);

    // Wait for the subscriptions to the accept and reject keys-and-certificate topics to be established.
    ctx.acceptedSubAckPromise.get_future().wait();
    ctx.rejectedSubAckPromise.get_future().wait();

    // Now, when we subscribed to the keys and certificate topics, we can make a request for a certificate.
    fprintf(stdout, "Publishing to CreateKeysAndCertificate topic\n");
    CreateKeysAndCertificateRequest createKeysAndCertificateRequest;
    identityClient->PublishCreateKeysAndCertificate(
        createKeysAndCertificateRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onKeysPublishPubAck);
    ctx.pubAckPromise.get_future().wait();
}

/**
 * Certificate-from-CSR workflow.
 *
 * @note Subscriptions created here will be active even after the function completes. So, all variables accessed in the
 * callbacks must be alive for the whole duration of the identityClient's lifetime. An instance of
 * CreateCertificateContext is used to store variables used by the callbacks.
 */
void createCertificateFromCsr(
    const std::shared_ptr<IotIdentityClient> &identityClient,
    CreateCertificateContext &ctx,
    const String &csrFile)
{
    auto onCsrPublishPubAck = [&ctx](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error publishing to CreateCertificateFromCsr: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }
        ctx.pubAckPromise.set_value();
    };

    auto onCsrAcceptedSubAck = [&ctx](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error subscribing to CreateCertificateFromCsr accepted: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }
        ctx.acceptedSubAckPromise.set_value();
    };

    auto onCsrRejectedSubAck = [&ctx](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error subscribing to CreateCertificateFromCsr rejected: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }
        ctx.rejectedSubAckPromise.set_value();
    };

    auto onCsrAccepted = [&ctx](CreateCertificateFromCsrResponse *response, int ioErr) {
        if (ioErr == AWS_OP_SUCCESS)
        {
            fprintf(stdout, "CreateCertificateFromCsrResponse certificateId: %s.\n", response->CertificateId->c_str());
            ctx.tokenPromise.set_value(*response->CertificateOwnershipToken);
        }
        else
        {
            fprintf(stderr, "Error on subscription: %s.\n", ErrorDebugString(ioErr));
            exit(-1);
        }
    };

    auto onCsrRejected = [](ErrorResponse *error, int ioErr) {
        if (ioErr == AWS_OP_SUCCESS)
        {
            fprintf(
                stdout,
                "CreateCertificateFromCsr failed with statusCode %d, errorMessage %s and errorCode %s.",
                *error->StatusCode,
                error->ErrorMessage->c_str(),
                error->ErrorCode->c_str());
            exit(-1);
        }
        else
        {
            fprintf(stderr, "Error on subscription: %s.\n", ErrorDebugString(ioErr));
            exit(-1);
        }
    };

    // CreateCertificateFromCsr workflow
    fprintf(stdout, "Subscribing to CreateCertificateFromCsr Accepted and Rejected topics\n");
    CreateCertificateFromCsrSubscriptionRequest csrSubscriptionRequest;
    identityClient->SubscribeToCreateCertificateFromCsrAccepted(
        csrSubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onCsrAccepted, onCsrAcceptedSubAck);

    identityClient->SubscribeToCreateCertificateFromCsrRejected(
        csrSubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onCsrRejected, onCsrRejectedSubAck);

    // Wait for the subscriptions to the accept and reject certificates topics to be established.
    ctx.acceptedSubAckPromise.get_future().wait();
    ctx.rejectedSubAckPromise.get_future().wait();

    // Now, when we subscribed to the certificates topics, we can make a request for a certificate.
    fprintf(stdout, "Publishing to CreateCertificateFromCsr topic\n");
    CreateCertificateFromCsrRequest createCertificateFromCsrRequest;
    createCertificateFromCsrRequest.CertificateSigningRequest = csrFile;
    identityClient->PublishCreateCertificateFromCsr(
        createCertificateFromCsrRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onCsrPublishPubAck);
    ctx.pubAckPromise.get_future().wait();
}

/**
 * Provision an AWS IoT thing using a pre-defined template.
 */
void registerThing(
    const std::shared_ptr<IotIdentityClient> &identityClient,
    RegisterThingContext &ctx,
    const Utils::cmdData &cmdData,
    const String &token)
{
    auto onRegisterAcceptedSubAck = [&ctx](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error subscribing to RegisterThing accepted: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }
        ctx.acceptedSubAckPromise.set_value();
    };

    auto onRegisterRejectedSubAck = [&ctx](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error subscribing to RegisterThing rejected: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }
        ctx.rejectedSubAckPromise.set_value();
    };

    auto onRegisterAccepted = [&ctx](RegisterThingResponse *response, int ioErr) {
        if (ioErr == AWS_OP_SUCCESS)
        {
            fprintf(stdout, "RegisterThingResponse ThingName: %s.\n", response->ThingName->c_str());
            ctx.thingCreatedPromise.set_value();
        }
        else
        {
            fprintf(stderr, "Error on subscription: %s.\n", ErrorDebugString(ioErr));
            exit(-1);
        }
    };

    auto onRegisterRejected = [](ErrorResponse *error, int ioErr) {
        if (ioErr == AWS_OP_SUCCESS)
        {
            fprintf(
                stdout,
                "RegisterThing failed with statusCode %d, errorMessage %s and errorCode %s.",
                *error->StatusCode,
                error->ErrorMessage->c_str(),
                error->ErrorCode->c_str());
        }
        else
        {
            fprintf(stderr, "Error on subscription: %s.\n", ErrorDebugString(ioErr));
            exit(-1);
        }
    };

    auto onRegisterPublishPubAck = [&ctx](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error publishing to RegisterThing: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }
        ctx.pubAckPromise.set_value();
    };

    fprintf(stdout, "Subscribing to RegisterThing Accepted and Rejected topics\n");
    RegisterThingSubscriptionRequest registerSubscriptionRequest;
    registerSubscriptionRequest.TemplateName = cmdData.input_templateName;

    identityClient->SubscribeToRegisterThingAccepted(
        registerSubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onRegisterAccepted, onRegisterAcceptedSubAck);

    identityClient->SubscribeToRegisterThingRejected(
        registerSubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onRegisterRejected, onRegisterRejectedSubAck);

    // Wait for the subscriptions to the accept and reject RegisterThing topics to be established.
    ctx.acceptedSubAckPromise.get_future().wait();
    ctx.rejectedSubAckPromise.get_future().wait();

    fprintf(stdout, "Publishing to RegisterThing topic\n");
    RegisterThingRequest registerThingRequest;
    registerThingRequest.TemplateName = cmdData.input_templateName;

    const Aws::Crt::String jsonValue = cmdData.input_templateParameters;
    Aws::Crt::JsonObject value(jsonValue);
    Map<String, JsonView> pm = value.View().GetAllObjects();
    Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String> params = Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String>();

    for (const auto &x : pm)
    {
        params.emplace(x.first, x.second.AsString());
    }

    registerThingRequest.Parameters = params;
    // NOTE: In a real application creating multiple certificates you'll probably need to protect token var with
    // a critical section. This sample makes only one request for a certificate, so no data race is possible.
    registerThingRequest.CertificateOwnershipToken = token;

    identityClient->PublishRegisterThing(registerThingRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onRegisterPublishPubAck);
    ctx.pubAckPromise.get_future().wait();

    // Wait for registering a thing to succeed.
    ctx.thingCreatedPromise.get_future().wait();
}

int main(int argc, char *argv[])
{
    /************************ Setup ****************************/

    // Do the global initialization for the API
    ApiHandle apiHandle;

    /**
     * cmdData is the arguments/input from the command line placed into a single struct for
     * use in this sample. This handles all of the command line parsing, validating, etc.
     * See the Utils/CommandLineUtils for more information.
     */
    Utils::cmdData cmdData = Utils::parseSampleInputFleetProvisioning(argc, argv, &apiHandle);

    Mqtt5ClientContext mqtt5ClientContext;
    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> mqtt5Client;

    Mqtt3ConnectionContext mqtt3ConnectionContext;
    std::shared_ptr<Mqtt::MqttConnection> mqtt3Connection;

    std::shared_ptr<IotIdentityClient> identityClient;

    if (cmdData.input_mqtt_version == 5UL)
    {
        mqtt5Client = createMqtt5Client(cmdData, mqtt5ClientContext);
        if (!mqtt5Client)
        {
            fprintf(stderr, "MQTT5 Connection failed to start");
            return -1;
        }
        identityClient =
            Aws::Crt::MakeShared<IotIdentityClient>(Aws::Crt::DefaultAllocatorImplementation(), mqtt5Client);
    }
    else if (cmdData.input_mqtt_version == 3UL)
    {
        mqtt3Connection = createMqtt3Connection(cmdData, mqtt3ConnectionContext);
        if (!mqtt3Connection)
        {
            fprintf(stderr, "MQTT3 Connection failed to start");
            return -1;
        }
        identityClient =
            Aws::Crt::MakeShared<IotIdentityClient>(Aws::Crt::DefaultAllocatorImplementation(), mqtt3Connection);
    }
    else
    {
        fprintf(stderr, "MQTT Version not supported\n");
        return -1;
    }

    /************************ Run the sample ****************************/

    // Create fleet provisioning client.

    // Create certificate.
    CreateCertificateContext certificateContext;
    if (cmdData.input_csrPath != "")
    {
        auto csrFile = getFileData(cmdData.input_csrPath);
        createCertificateFromCsr(identityClient, certificateContext, csrFile);
    }
    else
    {
        createKeysAndCertificate(identityClient, certificateContext);
    }

    // Wait for a certificate token to be obtained.
    auto token = certificateContext.tokenPromise.get_future().get();

    // After certificate is obtained, it's time to register a thing.
    RegisterThingContext registerThingContext;
    registerThing(identityClient, registerThingContext, cmdData, token);

    // Disconnect
    if (cmdData.input_mqtt_version == 5UL)
    {
        if (mqtt5Client->Stop())
        {
            mqtt5ClientContext.stoppedPromise.get_future().wait();
        }
    }
    else if (cmdData.input_mqtt_version == 3UL)
    {
        if (mqtt3Connection->Disconnect())
        {
            mqtt3ConnectionContext.connectionClosedPromise.get_future().wait();
        }
    }

    return 0;
}
