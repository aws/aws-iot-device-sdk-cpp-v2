/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

/**
 * A sample application demonstrating usage of AWS IoT Fleet provisioning.
 *
 * It's easier to follow a synchronous workflow, when events happen one after another. For that reason, this sample
 * performs all actions, like connecting to a server or registering a thing, in synchronous manner.
 */

#include <aws/crt/Api.h>
#include <aws/crt/JsonObject.h>

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

#include "../../utils/CommandLineUtils.h"

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
 * Auxiliary structure for holding data used by MQTT connection.
 */
struct ConnectionContext
{
    std::promise<bool> connectionCompletedPromise;
    std::promise<void> connectionClosedPromise;
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
 * Create MQTT3 connection.
 */
std::shared_ptr<Mqtt::MqttConnection> createConnection(const Utils::cmdData &cmdData, ConnectionContext &ctx)
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

    connection->OnConnectionCompleted = std::move(onConnectionCompleted);
    connection->OnDisconnect = std::move(onDisconnect);

    return connection;
}

/**
 * Keys-and-Certificate workflow.
 *
 * @note Subscriptions created here will be active even after the function completes. So, all variables accessed in the
 * callbacks must be alive for the whole duration of the identityClient's lifetime. An instance of
 * CreateCertificateContext is used to store variables used by the callbacks.
 */
void createKeysAndCertificate(IotIdentityClient &identityClient, CreateCertificateContext &ctx)
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
    identityClient.SubscribeToCreateKeysAndCertificateAccepted(
        keySubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onKeysAccepted, onKeysAcceptedSubAck);
    identityClient.SubscribeToCreateKeysAndCertificateRejected(
        keySubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onKeysRejected, onKeysRejectedSubAck);

    // Wait for the subscriptions to the accept and reject keys-and-certificate topics to be established.
    ctx.acceptedSubAckPromise.get_future().wait();
    ctx.rejectedSubAckPromise.get_future().wait();

    // Now, when we subscribed to the keys and certificate topics, we can make a request for a certificate.
    fprintf(stdout, "Publishing to CreateKeysAndCertificate topic\n");
    CreateKeysAndCertificateRequest createKeysAndCertificateRequest;
    identityClient.PublishCreateKeysAndCertificate(
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
void createCertificateFromCsr(IotIdentityClient &identityClient, CreateCertificateContext &ctx, const String &csrFile)
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
    identityClient.SubscribeToCreateCertificateFromCsrAccepted(
        csrSubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onCsrAccepted, onCsrAcceptedSubAck);

    identityClient.SubscribeToCreateCertificateFromCsrRejected(
        csrSubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onCsrRejected, onCsrRejectedSubAck);

    // Wait for the subscriptions to the accept and reject certificates topics to be established.
    ctx.acceptedSubAckPromise.get_future().wait();
    ctx.rejectedSubAckPromise.get_future().wait();

    // Now, when we subscribed to the certificates topics, we can make a request for a certificate.
    fprintf(stdout, "Publishing to CreateCertificateFromCsr topic\n");
    CreateCertificateFromCsrRequest createCertificateFromCsrRequest;
    createCertificateFromCsrRequest.CertificateSigningRequest = csrFile;
    identityClient.PublishCreateCertificateFromCsr(
        createCertificateFromCsrRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onCsrPublishPubAck);
    ctx.pubAckPromise.get_future().wait();
}

/**
 * Provision an AWS IoT thing using a pre-defined template.
 */
void registerThing(
    IotIdentityClient &identityClient,
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

    identityClient.SubscribeToRegisterThingAccepted(
        registerSubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onRegisterAccepted, onRegisterAcceptedSubAck);

    identityClient.SubscribeToRegisterThingRejected(
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

    identityClient.PublishRegisterThing(registerThingRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onRegisterPublishPubAck);
    ctx.pubAckPromise.get_future().wait();

    // Wait for registering a thing to succeed.
    ctx.thingCreatedPromise.get_future().wait();
}

int main(int argc, char *argv[])
{
    /************************ Setup ****************************/

    //  Do the global initialization for the API
    ApiHandle apiHandle;

    /**
     * cmdData is the arguments/input from the command line placed into a single struct for
     * use in this sample. This handles all of the command line parsing, validating, etc.
     * See the Utils/CommandLineUtils for more information.
     */
    Utils::cmdData cmdData = Utils::parseSampleInputFleetProvisioning(argc, argv, &apiHandle);

    ConnectionContext connectionContext;
    auto connection = createConnection(cmdData, connectionContext);

    /************************ Run the sample ****************************/

    fprintf(stdout, "Connecting...\n");
    if (!connection->Connect(cmdData.input_clientId.c_str(), true, 0))
    {
        fprintf(stderr, "MQTT Connection failed with error %s\n", ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    if (!connectionContext.connectionCompletedPromise.get_future().get())
    {
        exit(-1);
    }

    // Create fleet provisioning client.
    IotIdentityClient identityClient(connection);

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
    if (connection->Disconnect())
    {
        connectionContext.connectionClosedPromise.get_future().wait();
    }

    return 0;
}
