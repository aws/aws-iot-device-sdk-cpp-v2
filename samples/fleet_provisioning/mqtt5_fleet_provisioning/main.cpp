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

#include <fstream>

#include "../../utils/CommandLineUtils.h"

using namespace Aws::Crt;
using namespace Aws::Iotidentity;

static std::string getFileData(std::string const &fileName)
{
    std::ifstream ifs(fileName);
    std::string str;
    getline(ifs, str, (char)ifs.eof());
    return str;
}

int main(int argc, char *argv[])
{
    /************************ Setup ****************************/

    //  Do the global initialization for the API
    ApiHandle apiHandle;
    // Variables for the sample
    String csrFile;
    String token;
    RegisterThingResponse registerThingResponse;

    /**
     * cmdData is the arguments/input from the command line placed into a single struct for
     * use in this sample. This handles all of the command line parsing, validating, etc.
     * See the Utils/CommandLineUtils for more information.
     */
    Utils::cmdData cmdData = Utils::parseSampleInputFleetProvisioning(argc, argv, &apiHandle);

    if (cmdData.input_csrPath != "")
    {
        csrFile = getFileData(cmdData.input_csrPath.c_str()).c_str();
    }

    // Create the MQTT5 builder and populate it with data from cmdData.
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(
        cmdData.input_endpoint, cmdData.input_cert.c_str(), cmdData.input_key.c_str());

    // Check if the builder setup correctly.
    if (builder == nullptr)
    {
        printf(
            "Failed to setup mqtt5 client builder with error code %d: %s", LastError(), ErrorDebugString(LastError()));
        return -1;
    }

    // Setup connection options
    std::shared_ptr<Mqtt5::ConnectPacket> connectOptions = std::make_shared<Mqtt5::ConnectPacket>();
    connectOptions->WithClientId(cmdData.input_clientId);
    builder->WithConnectOptions(connectOptions);
    if (cmdData.input_port != 0)
    {
        builder->WithPort(static_cast<uint32_t>(cmdData.input_port));
    }

    std::promise<bool> connectionPromise;
    std::promise<void> stoppedPromise;
    std::promise<void> disconnectPromise;
    std::promise<bool> subscribeSuccess;

    // Setup lifecycle callbacks
    builder->WithClientConnectionSuccessCallback(
        [&connectionPromise](const Mqtt5::OnConnectionSuccessEventData &eventData) {
            fprintf(
                stdout,
                "Mqtt5 Client connection succeed, clientid: %s.\n",
                eventData.negotiatedSettings->getClientId().c_str());
            connectionPromise.set_value(true);
        });
    builder->WithClientConnectionFailureCallback([&connectionPromise](
                                                     const Mqtt5::OnConnectionFailureEventData &eventData) {
        fprintf(stdout, "Mqtt5 Client connection failed with error: %s.\n", aws_error_debug_str(eventData.errorCode));
        connectionPromise.set_value(false);
    });
    builder->WithClientStoppedCallback([&stoppedPromise](const Mqtt5::OnStoppedEventData &) {
        fprintf(stdout, "Mqtt5 Client stopped.\n");
        stoppedPromise.set_value();
    });
    builder->WithClientAttemptingConnectCallback([](const Mqtt5::OnAttemptingConnectEventData &) {
        fprintf(stdout, "Mqtt5 Client attempting connection...\n");
    });
    builder->WithClientDisconnectionCallback([&disconnectPromise](const Mqtt5::OnDisconnectionEventData &eventData) {
        fprintf(stdout, "Mqtt5 Client disconnection with reason: %s.\n", aws_error_debug_str(eventData.errorCode));
        disconnectPromise.set_value();
    });

    // Create Mqtt5Client
    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> client = builder->Build();
    delete builder;
    /************************ Run the sample ****************************/

    fprintf(stdout, "Connecting...\n");
    if (!client->Start())
    {
        fprintf(stderr, "MQTT5 Connection failed to start");
        exit(-1);
    }

    if (!connectionPromise.get_future().get())
    {
        return -1;
    }
    IotIdentityClient identityClient(client);

    /*
     * Use promises to enforce synchronous behavior, so it's easier to follow the workflow.
     */
    std::promise<void> csrPublishPubAckCompletedPromise;
    std::promise<void> csrAcceptedSubAckCompletedPromise;
    std::promise<void> csrRejectedSubAckCompletedPromise;
    std::promise<void> csrAcceptedCompletedPromise;

    std::promise<void> keysPublishPubAckCompletedPromise;
    std::promise<void> keysAcceptedSubAckCompletedPromise;
    std::promise<void> keysRejectedSubAckCompletedPromise;
    std::promise<void> keysAcceptedCompletedPromise;

    std::promise<void> registerPublishPubAckCompletedPromise;
    std::promise<void> registerAcceptedSubAckCompletedPromise;
    std::promise<void> registerRejectedSubAckCompletedPromise;
    std::promise<void> registerAcceptedCompletedPromise;

    auto onCsrPublishPubAck = [&](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error publishing to CreateCertificateFromCsr: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }

        csrPublishPubAckCompletedPromise.set_value();
    };

    auto onCsrAcceptedSubAck = [&](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error subscribing to CreateCertificateFromCsr accepted: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }

        csrAcceptedSubAckCompletedPromise.set_value();
    };

    auto onCsrRejectedSubAck = [&](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error subscribing to CreateCertificateFromCsr rejected: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }
        csrRejectedSubAckCompletedPromise.set_value();
    };

    auto onCsrAccepted = [&](CreateCertificateFromCsrResponse *response, int ioErr) {
        if (ioErr == AWS_OP_SUCCESS)
        {
            fprintf(stdout, "CreateCertificateFromCsrResponse certificateId: %s.\n", response->CertificateId->c_str());
            token = *response->CertificateOwnershipToken;
            csrAcceptedCompletedPromise.set_value();
        }
        else
        {
            fprintf(stderr, "Error on subscription: %s.\n", ErrorDebugString(ioErr));
            exit(-1);
        }
    };

    auto onCsrRejected = [&](ErrorResponse *error, int ioErr) {
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

    auto onKeysPublishPubAck = [&](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error publishing to CreateKeysAndCertificate: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }

        keysPublishPubAckCompletedPromise.set_value();
    };

    auto onKeysAcceptedSubAck = [&](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error subscribing to CreateKeysAndCertificate accepted: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }

        keysAcceptedSubAckCompletedPromise.set_value();
    };

    auto onKeysRejectedSubAck = [&](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error subscribing to CreateKeysAndCertificate rejected: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }
        keysRejectedSubAckCompletedPromise.set_value();
    };

    auto onKeysAccepted = [&](CreateKeysAndCertificateResponse *response, int ioErr) {
        if (ioErr == AWS_OP_SUCCESS)
        {
            fprintf(stdout, "CreateKeysAndCertificateResponse certificateId: %s.\n", response->CertificateId->c_str());
            token = *response->CertificateOwnershipToken;
            keysAcceptedCompletedPromise.set_value();
        }
        else
        {
            fprintf(stderr, "Error on subscription: %s.\n", ErrorDebugString(ioErr));
            exit(-1);
        }
    };

    auto onKeysRejected = [&](ErrorResponse *error, int ioErr) {
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

    auto onRegisterAcceptedSubAck = [&](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error subscribing to RegisterThing accepted: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }

        registerAcceptedSubAckCompletedPromise.set_value();
    };

    auto onRegisterRejectedSubAck = [&](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error subscribing to RegisterThing rejected: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }
        registerRejectedSubAckCompletedPromise.set_value();
    };

    auto onRegisterAccepted = [&](RegisterThingResponse *response, int ioErr) {
        if (ioErr == AWS_OP_SUCCESS)
        {
            fprintf(stdout, "RegisterThingResponse ThingName: %s.\n", response->ThingName->c_str());
            registerAcceptedCompletedPromise.set_value();
        }
        else
        {
            fprintf(stderr, "Error on subscription: %s.\n", ErrorDebugString(ioErr));
            exit(-1);
        }
    };

    auto onRegisterRejected = [&](ErrorResponse *error, int ioErr) {
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

    auto onRegisterPublishPubAck = [&](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error publishing to RegisterThing: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }

        registerPublishPubAckCompletedPromise.set_value();
    };

    if (csrFile.empty())
    {
        // CreateKeysAndCertificate workflow
        fprintf(stdout, "Subscribing to CreateKeysAndCertificate Accepted and Rejected topics\n");
        CreateKeysAndCertificateSubscriptionRequest keySubscriptionRequest;
        identityClient.SubscribeToCreateKeysAndCertificateAccepted(
            keySubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onKeysAccepted, onKeysAcceptedSubAck);
        identityClient.SubscribeToCreateKeysAndCertificateRejected(
            keySubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onKeysRejected, onKeysRejectedSubAck);

        // Wait for the subscriptions to the accept and reject key/certificates topics to be established.
        keysAcceptedSubAckCompletedPromise.get_future().wait();
        keysRejectedSubAckCompletedPromise.get_future().wait();

        // Now, when we subscribed to the keys and certificates topics, we can make a request for a certificate.
        fprintf(stdout, "Publishing to CreateKeysAndCertificate topic\n");
        CreateKeysAndCertificateRequest createKeysAndCertificateRequest;
        identityClient.PublishCreateKeysAndCertificate(
            createKeysAndCertificateRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onKeysPublishPubAck);
        keysPublishPubAckCompletedPromise.get_future().wait();

        // Wait for a certificate token.
        keysAcceptedCompletedPromise.get_future().wait();

        fprintf(stdout, "Subscribing to RegisterThing Accepted and Rejected topics\n");
        RegisterThingSubscriptionRequest registerSubscriptionRequest;
        registerSubscriptionRequest.TemplateName = cmdData.input_templateName;

        identityClient.SubscribeToRegisterThingAccepted(
            registerSubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onRegisterAccepted, onRegisterAcceptedSubAck);

        identityClient.SubscribeToRegisterThingRejected(
            registerSubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onRegisterRejected, onRegisterRejectedSubAck);

        // Wait for the subscriptions to the accept and reject RegisterThing topics to be established.
        registerAcceptedSubAckCompletedPromise.get_future().wait();
        registerRejectedSubAckCompletedPromise.get_future().wait();

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
        registerThingRequest.CertificateOwnershipToken = token;

        identityClient.PublishRegisterThing(registerThingRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onRegisterPublishPubAck);
        registerPublishPubAckCompletedPromise.get_future().wait();

        // Wait for registering a thing to succeed.
        registerAcceptedCompletedPromise.get_future().wait();
    }
    else
    {
        // CreateCertificateFromCsr workflow
        fprintf(stdout, "Subscribing to CreateCertificateFromCsr Accepted and Rejected topics\n");
        CreateCertificateFromCsrSubscriptionRequest csrSubscriptionRequest;
        identityClient.SubscribeToCreateCertificateFromCsrAccepted(
            csrSubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onCsrAccepted, onCsrAcceptedSubAck);

        identityClient.SubscribeToCreateCertificateFromCsrRejected(
            csrSubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onCsrRejected, onCsrRejectedSubAck);

        // Wait for the subscriptions to the accept and reject certificates topics to be established.
        csrAcceptedSubAckCompletedPromise.get_future().wait();
        csrRejectedSubAckCompletedPromise.get_future().wait();

        // Now, when we subscribed to the certificates topics, we can make a request for a certificate.
        fprintf(stdout, "Publishing to CreateCertificateFromCsr topic\n");
        CreateCertificateFromCsrRequest createCertificateFromCsrRequest;
        createCertificateFromCsrRequest.CertificateSigningRequest = csrFile;
        identityClient.PublishCreateCertificateFromCsr(
            createCertificateFromCsrRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onCsrPublishPubAck);
        csrPublishPubAckCompletedPromise.get_future().wait();

        // Wait for a certificate token.
        csrAcceptedCompletedPromise.get_future().wait();

        fprintf(stdout, "Subscribing to RegisterThing Accepted and Rejected topics\n");
        RegisterThingSubscriptionRequest registerSubscriptionRequest;
        registerSubscriptionRequest.TemplateName = cmdData.input_templateName;

        identityClient.SubscribeToRegisterThingAccepted(
            registerSubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onRegisterAccepted, onRegisterAcceptedSubAck);

        identityClient.SubscribeToRegisterThingRejected(
            registerSubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onRegisterRejected, onRegisterRejectedSubAck);

        // Wait for the subscriptions to the accept and reject RegisterThing topics to be established.
        registerAcceptedSubAckCompletedPromise.get_future().wait();
        registerRejectedSubAckCompletedPromise.get_future().wait();

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
        registerThingRequest.CertificateOwnershipToken = token;

        identityClient.PublishRegisterThing(registerThingRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onRegisterPublishPubAck);

        registerPublishPubAckCompletedPromise.get_future().wait();

        // Wait for registering a thing to succeed.
        registerAcceptedCompletedPromise.get_future().wait();
    }

    // Disconnect
    if (client->Stop())
    {
        stoppedPromise.get_future().wait();
    }

    return 0;
}
