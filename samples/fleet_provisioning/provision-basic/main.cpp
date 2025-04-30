/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

/**
 * A sample application demonstrating usage of AWS IoT Fleet provisioning.
 */

#include <aws/crt/Api.h>
#include <aws/crt/JsonObject.h>
#include <aws/crt/UUID.h>
#include <aws/crt/mqtt/Mqtt5Packets.h>
#include <aws/iot/Mqtt5Client.h>

#include <aws/iotidentity/CreateKeysAndCertificateRequest.h>
#include <aws/iotidentity/CreateKeysAndCertificateResponse.h>
#include <aws/iotidentity/IotIdentityClientV2.h>
#include <aws/iotidentity/RegisterThingRequest.h>
#include <aws/iotidentity/RegisterThingResponse.h>
#include <aws/iotidentity/V2ErrorResponse.h>

#include <fstream>

#include "../../utils/CommandLineUtils.h"

using namespace Aws::Crt;
using namespace Aws::Iotidentity;

static void s_onServiceError(const ServiceErrorV2<V2ErrorResponse> &serviceError, String operationName)
{
    fprintf(
        stdout,
        "%s failed with error code: %s\n",
        operationName.c_str(),
        aws_error_debug_str(serviceError.GetErrorCode()));
    if (serviceError.HasModeledError())
    {
        const auto &modeledError = serviceError.GetModeledError();

        JsonObject jsonObject;
        modeledError.SerializeToObject(jsonObject);
        String outgoingJson = jsonObject.View().WriteCompact(true);
        fprintf(stdout, "modeled error: %s\n", outgoingJson.c_str());
    }
}

int main(int argc, char *argv[])
{
    ApiHandle apiHandle;

    /**
     * cmdData is the arguments/input from the command line placed into a single struct for
     * use in this sample. This handles all of the command line parsing, validating, etc.
     * See the Utils/CommandLineUtils for more information.
     */
    Utils::cmdData cmdData = Utils::parseSampleInputFleetProvisioning(argc, argv, &apiHandle);

    // Create the MQTT5 builder and populate it with data from cmdData.
    auto builder = std::unique_ptr<Aws::Iot::Mqtt5ClientBuilder>(
        Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(
            cmdData.input_endpoint, cmdData.input_cert.c_str(), cmdData.input_key.c_str()));
    // Check if the builder setup correctly.
    if (builder == nullptr)
    {
        printf(
            "Failed to setup mqtt5 client builder with error code %d: %s", LastError(), ErrorDebugString(LastError()));
        return -1;
    }

    auto clientId = "test-" + UUID().ToString();
    auto connectPacket = MakeShared<Mqtt5::ConnectPacket>(DefaultAllocatorImplementation());
    connectPacket->WithClientId(clientId);

    builder->WithConnectOptions(connectPacket);

    std::promise<bool> connectedWaiter;

    // Setup lifecycle callbacks
    builder->WithClientConnectionSuccessCallback(
        [&connectedWaiter](const Mqtt5::OnConnectionSuccessEventData &)
        {
            fprintf(stdout, "Mqtt5 Client connection succeeded!\n");
            connectedWaiter.set_value(true);
        });
    builder->WithClientConnectionFailureCallback(
        [](const Mqtt5::OnConnectionFailureEventData &event)
        {
            fprintf(
                stdout,
                "Mqtt5 client connection attempt failed with error: %s.\n",
                aws_error_debug_str(event.errorCode));
        });

    auto protocolClient = builder->Build();
    if (!protocolClient)
    {
        printf("Failed to create mqtt5 client with error code %d: %s", LastError(), ErrorDebugString(LastError()));
        return -1;
    }

    Aws::Iot::RequestResponse::RequestResponseClientOptions requestResponseOptions;
    requestResponseOptions.WithMaxRequestResponseSubscriptions(4);
    requestResponseOptions.WithMaxStreamingSubscriptions(10);
    requestResponseOptions.WithOperationTimeoutInSeconds(30);

    auto identityClient = Aws::Iotidentity::NewClientFrom5(*protocolClient, requestResponseOptions);
    if (!identityClient)
    {
        printf("Failed to create identity client with error code %d: %s", LastError(), ErrorDebugString(LastError()));
        return -1;
    }

    fprintf(stdout, "Starting protocol client!\n");
    protocolClient->Start();

    connectedWaiter.get_future().wait();

    /*
     * Step 1: Invoke the CreateKeysAndCertificate API to get a key pair signed by AmazonRootCA1
     */
    CreateKeysAndCertificateRequest createKeysRequest;

    std::promise<CreateKeysAndCertificateResult> createKeysResultPromise;
    identityClient->CreateKeysAndCertificate(
        createKeysRequest,
        [&createKeysResultPromise](CreateKeysAndCertificateResult &&result)
        { createKeysResultPromise.set_value(std::move(result)); });

    const auto &createKeysResult = createKeysResultPromise.get_future().get();
    if (!createKeysResult.IsSuccess())
    {
        s_onServiceError(createKeysResult.GetError(), "create-keys-and-certificate");
        return -1;
    }

    const auto &createKeysResponse = createKeysResult.GetResponse();

    JsonObject createKeysJsonObject;
    createKeysResponse.SerializeToObject(createKeysJsonObject);
    fprintf(stdout, "create-keys-and-certificate result: %s\n", createKeysJsonObject.View().WriteCompact(true).c_str());

    /*
     * Step 2: Invoke the RegisterThing API using the results of the CreateKeysAndCertificate call.  Until RegisterThing
     * is successfully called, the certificate-and-key pair from Step 1 are useless.
     */
    RegisterThingRequest registerThingRequest;
    registerThingRequest.TemplateName = cmdData.input_templateName;
    registerThingRequest.CertificateOwnershipToken = createKeysResponse.CertificateOwnershipToken;
    if (cmdData.input_templateParameters.length() > 0)
    {
        Map<String, String> finalTemplateParameters;

        JsonObject templateParamsAsJson(cmdData.input_templateParameters);
        Map<String, JsonView> pm = templateParamsAsJson.View().GetAllObjects();
        for (const auto &x : pm)
        {
            finalTemplateParameters.emplace(x.first, x.second.AsString());
        }

        registerThingRequest.Parameters = finalTemplateParameters;
    }

    // A simple `std::promise<RegisterThingResult>` can be used here. But to support old VS compilers requiring
    // a template parameter be default-constructible, we wrap Result into Optional.
    std::promise<Aws::Crt::Optional<RegisterThingResult>> registerThingResultPromise;
    identityClient->RegisterThing(
        registerThingRequest,
        [&registerThingResultPromise](RegisterThingResult &&result)
        {
            registerThingResultPromise.set_value(std::move(result));
        });

    const auto &registerThingResult = registerThingResultPromise.get_future().get().value();
    if (!registerThingResult.IsSuccess())
    {
        s_onServiceError(registerThingResult.GetError(), "register-thing");
        return -1;
    }

    const auto &registerThingResponse = registerThingResult.GetResponse();

    /*
     * The call to RegisterThing was successful.  The certificate-and-key pair inside createKeysResponse is now valid
     * and can be used to connect and interact with AWS IoT Core with a permission policy scope determined by the
     * provisioning template referenced in the RegisterThing API call.
     */
    JsonObject registerThingJsonObject;
    registerThingResponse.SerializeToObject(registerThingJsonObject);
    fprintf(stdout, "register-thing result: %s\n", registerThingJsonObject.View().WriteCompact(true).c_str());

    return 0;
}
