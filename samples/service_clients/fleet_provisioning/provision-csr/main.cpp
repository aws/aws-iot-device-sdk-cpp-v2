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

#include <aws/iotidentity/CreateCertificateFromCsrRequest.h>
#include <aws/iotidentity/CreateCertificateFromCsrResponse.h>
#include <aws/iotidentity/IotIdentityClientV2.h>
#include <aws/iotidentity/RegisterThingRequest.h>
#include <aws/iotidentity/RegisterThingResponse.h>
#include <aws/iotidentity/V2ErrorResponse.h>

#include <fstream>

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

/* --------------------------------- ARGUMENT PARSING ----------------------------------------- */
struct CmdArgs
{
    String endpoint;
    String cert;
    String key;
    String clientId;
    String templateName;
    String templateParameters;
    String csrPath;
};

void printHelp()
{
    printf("Fleet Provisioning CSR Sample\n");
    printf("options:\n");
    printf("  --help        show this help message and exit\n");
    printf("required arguments:\n");
    printf("  --endpoint    IoT endpoint hostname\n");
    printf("  --cert        Path to the certificate file\n");
    printf("  --key         Path to the private key file\n");
    printf("  --template_name  Provisioning template name\n");
    printf("  --csr         Path to CSR in PEM format\n");
    printf("optional arguments:\n");
    printf("  --template_parameters  Template parameters JSON\n");
    printf("  --client_id   Client ID (default: test-<uuid>)\n");
}

CmdArgs parseArgs(int argc, char *argv[])
{
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
            else if (strcmp(argv[i], "--template_name") == 0)
            {
                args.templateName = argv[++i];
            }
            else if (strcmp(argv[i], "--template_parameters") == 0)
            {
                args.templateParameters = argv[++i];
            }
            else if (strcmp(argv[i], "--csr") == 0)
            {
                args.csrPath = argv[++i];
            }
            else if (strcmp(argv[i], "--client_id") == 0)
            {
                args.clientId = argv[++i];
            }

            else
            {
                fprintf(stderr, "Unknown argument: %s\n", argv[i]);
                printHelp();
                exit(1);
            }
        }
    }
    if (args.endpoint.empty() || args.cert.empty() || args.key.empty() || args.templateName.empty() || args.csrPath.empty())
    {
        fprintf(stderr, "Error: --endpoint, --cert, --key, --template_name, and --csr are required\n");
        printHelp();
        exit(1);
    }
    if (args.clientId.empty())
    {
        args.clientId = String("test-") + UUID().ToString();
    }
    return args;
}
/* --------------------------------- ARGUMENT PARSING END ----------------------------------------- */

int main(int argc, char *argv[])
{
    // Parse command line arguments
    CmdArgs cmdData = parseArgs(argc, argv);

    ApiHandle apiHandle;

    // Create the MQTT5 builder and populate it with data from cmdData.
    auto builder = std::unique_ptr<Aws::Iot::Mqtt5ClientBuilder>(
        Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(
            cmdData.endpoint, cmdData.cert.c_str(), cmdData.key.c_str()));
    // Check if the builder setup correctly.
    if (builder == nullptr)
    {
        printf(
            "Failed to setup mqtt5 client builder with error code %d: %s", LastError(), ErrorDebugString(LastError()));
        return -1;
    }

    auto connectPacket = MakeShared<Mqtt5::ConnectPacket>(DefaultAllocatorImplementation());
    connectPacket->WithClientId(cmdData.clientId);

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
        [](const Mqtt5::OnConnectionFailureEventData &event) {
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
     * Read the CSR file
     */
    String csr_data;
    std::ifstream csr_file(cmdData.csrPath.c_str());
    getline(csr_file, csr_data, (char)csr_file.eof());

    /*
     * Step 1: Invoke the CreateKeysAndCertificate API to get a key pair signed by AmazonRootCA1
     */
    CreateCertificateFromCsrRequest createFromCsrRequest;
    createFromCsrRequest.CertificateSigningRequest = csr_data;

    // A simple `std::promise<Result>` can be used here. But to support old VS compilers requiring a template parameter
    // be default-constructible, we wrap Result into Optional.
    std::promise<Aws::Crt::Optional<CreateCertificateFromCsrResult>> createFromCsrResultPromise;
    identityClient->CreateCertificateFromCsr(
        createFromCsrRequest,
        [&createFromCsrResultPromise](CreateCertificateFromCsrResult &&result)
        { createFromCsrResultPromise.set_value(std::move(result)); });

    auto createFromCsrResult = createFromCsrResultPromise.get_future().get().value();
    if (!createFromCsrResult.IsSuccess())
    {
        s_onServiceError(createFromCsrResult.GetError(), "create-certificate-from-csr");
        return -1;
    }

    const auto &creatCertificateFromCsrResponse = createFromCsrResult.GetResponse();

    JsonObject createCertificateFromCsrJsonObject;
    creatCertificateFromCsrResponse.SerializeToObject(createCertificateFromCsrJsonObject);
    fprintf(
        stdout,
        "create-certificate-from-csr result: %s\n",
        createCertificateFromCsrJsonObject.View().WriteCompact(true).c_str());

    /*
     * Step 2: Invoke the RegisterThing API using the results of the CreateKeysAndCertificate call.  Until RegisterThing
     * is successfully called, the certificate-and-key pair from Step 1 are useless.
     */
    RegisterThingRequest registerThingRequest;
    registerThingRequest.TemplateName = cmdData.templateName;
    registerThingRequest.CertificateOwnershipToken = creatCertificateFromCsrResponse.CertificateOwnershipToken;
    if (cmdData.templateParameters.length() > 0)
    {
        Map<String, String> finalTemplateParameters;

        JsonObject templateParamsAsJson(cmdData.templateParameters);
        Map<String, JsonView> pm = templateParamsAsJson.View().GetAllObjects();
        for (const auto &x : pm)
        {
            finalTemplateParameters.emplace(x.first, x.second.AsString());
        }

        registerThingRequest.Parameters = finalTemplateParameters;
    }

    std::promise<Aws::Crt::Optional<RegisterThingResult>> registerThingResultPromise;
    identityClient->RegisterThing(
        registerThingRequest,
        [&registerThingResultPromise](RegisterThingResult &&result)
        { registerThingResultPromise.set_value(std::move(result)); });

    auto registerThingResult = registerThingResultPromise.get_future().get().value();
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
