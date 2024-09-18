/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/mqtt/Mqtt5Packets.h>
#include <aws/iot/Mqtt5Client.h>

#include <aws/iotshadow/ErrorResponse.h>
#include <aws/iotshadow/IotShadowClientv2.h>
#include <aws/iotshadow/DeleteNamedShadowRequest.h>
#include <aws/iotshadow/DeleteShadowResponse.h>
#include <aws/iotshadow/GetNamedShadowRequest.h>
#include <aws/iotshadow/GetShadowResponse.h>
#include <aws/iotshadow/UpdateNamedShadowRequest.h>
#include <aws/iotshadow/UpdateShadowResponse.h>

#include <algorithm>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

#include "../../utils/CommandLineUtils.h"

using namespace Aws::Crt;
using namespace Aws::Iotshadow;

static void s_onConnectionSuccess(const Mqtt5::OnConnectionSuccessEventData &eventData) {
    fprintf(
            stdout,
            "Mqtt5 Client connection succeeded!\n");
}

static void s_onConnectionFailure(const Mqtt5::OnConnectionFailureEventData &eventData) {
    fprintf(stdout, "Mqtt5 client connection attempt failed with error: %s.\n", aws_error_debug_str(eventData.errorCode));
}

static void s_onStopped(const Mqtt5::OnStoppedEventData &event) {
    fprintf(stdout, "Protocol client stopped.\n");
}


static Aws::Crt::String s_nibbleNextToken(Aws::Crt::String &input) {
    Aws::Crt::String token;
    Aws::Crt::String remaining;
    auto delimPosition = input.find(' ');
    if (delimPosition != Aws::Crt::String::npos) {
        token = input.substr(0, delimPosition);

        auto untrimmedRemaining = input.substr(delimPosition, Aws::Crt::String::npos);
        auto firstNonSpacePosition = untrimmedRemaining.find_first_not_of(' ');
        if (firstNonSpacePosition != Aws::Crt::String::npos) {
            remaining = untrimmedRemaining.substr(firstNonSpacePosition, Aws::Crt::String::npos);
        } else {
            remaining = "";
        }
    } else {
        token = input;
        remaining = "";
    }

    input = remaining;
    return token;
}

static void s_printHelp() {
    fprintf(stdout, "\nShadow sandbox:\n\n");
    fprintf(stdout, "  quit -- quits the program\n");
    fprintf(stdout, "  start -- starts the protocol client\n");
    fprintf(stdout, "  stop -- stops the protocol client\n");
    fprintf(stdout, "  get <thing-name> <shadow-name> -- gets the state of a named shadow belonging to the specified thing\n");
    fprintf(stdout, "  delete <thing-name> <shadow-name> -- deletes a named shadow belonging to the specified thing\n");
    fprintf(stdout, "  update-desired <thing-name> <shadow-name> <desired-state-JSON> -- updates the desired state of a named shadow belonging to the specified thing\n");
    fprintf(stdout, "  update-reported <thing-name> <shadow-name> <reported-state-JSON> -- updates the reported state a named shadow belonging to the specified thing\n");
}

static void s_onServiceError(const Aws::Iotshadow::ServiceErrorV2<Aws::Iotshadow::ErrorResponse> &serviceError, Aws::Crt::String operationName) {
    fprintf(stdout, "%s failed with error code: %s\n", operationName.c_str(), aws_error_debug_str(serviceError.getErrorCode()));
    if (serviceError.hasModeledError()) {
        const auto &modeledError = serviceError.getModeledError();

        Aws::Crt::JsonObject jsonObject;
        modeledError.SerializeToObject(jsonObject);
        Aws::Crt::String outgoingJson = jsonObject.View().WriteCompact(true);
        fprintf(stdout, "modeled error: %s\n", outgoingJson.c_str());
    }
}

static void s_onGetShadowResult(GetShadowResult &&result) {
    if (result.isSuccess()) {
        const auto &response = result.getResponse();

        Aws::Crt::JsonObject jsonObject;
        response.SerializeToObject(jsonObject);
        Aws::Crt::String outgoingJson = jsonObject.View().WriteCompact(true);
        fprintf(stdout, "get result: %s\n", outgoingJson.c_str());
    } else {
        s_onServiceError(result.getError(), "get");
    }
}

static void s_handleGetNamedShadow(const Aws::Crt::String params, const std::shared_ptr<Aws::Iotshadow::IClientV2> &shadowClient) {
    Aws::Crt::String remaining = params;
    Aws::Crt::String thing = s_nibbleNextToken(remaining);
    Aws::Crt::String shadow = s_nibbleNextToken(remaining);

    if (thing.length() == 0 || shadow.length() == 0) {
        fprintf(stdout, "Invalid arguments to get command!\n\n");
        s_printHelp();
        return;
    }

    Aws::Iotshadow::GetNamedShadowRequest request;
    request.ThingName = thing;
    request.ShadowName = shadow;

    shadowClient->GetNamedShadow(request, [](GetShadowResult &&result){
       s_onGetShadowResult(std::move(result));
    });
}

static void s_onDeleteShadowResult(DeleteShadowResult &&result) {
    if (result.isSuccess()) {
        const auto &response = result.getResponse();

        Aws::Crt::JsonObject jsonObject;
        response.SerializeToObject(jsonObject);
        Aws::Crt::String outgoingJson = jsonObject.View().WriteCompact(true);
        fprintf(stdout, "delete result: %s\n", outgoingJson.c_str());
    } else {
        s_onServiceError(result.getError(), "delete");
    }
}

static void s_handleDeleteNamedShadow(const Aws::Crt::String params, const std::shared_ptr<Aws::Iotshadow::IClientV2> &shadowClient) {
    Aws::Crt::String remaining = params;
    Aws::Crt::String thing = s_nibbleNextToken(remaining);
    Aws::Crt::String shadow = s_nibbleNextToken(remaining);

    if (thing.length() == 0 || shadow.length() == 0) {
        fprintf(stdout, "Invalid arguments to delete command!\n\n");
        s_printHelp();
        return;
    }

    Aws::Iotshadow::DeleteNamedShadowRequest request;
    request.ThingName = thing;
    request.ShadowName = shadow;

    shadowClient->DeleteNamedShadow(request, [](DeleteShadowResult &&result){
        s_onDeleteShadowResult(std::move(result));
    });
}

static void s_onUpdateShadowResult(UpdateShadowResult &&result, Aws::Crt::String operationName) {
    if (result.isSuccess()) {
        const auto &response = result.getResponse();

        Aws::Crt::JsonObject jsonObject;
        response.SerializeToObject(jsonObject);
        Aws::Crt::String outgoingJson = jsonObject.View().WriteCompact(true);
        fprintf(stdout, "%s result: %s\n", operationName.c_str(), outgoingJson.c_str());
    } else {
        s_onServiceError(result.getError(), operationName);
    }
}

static void s_handleUpdateDesiredNamedShadow(const Aws::Crt::String params, const std::shared_ptr<Aws::Iotshadow::IClientV2> &shadowClient) {
    Aws::Crt::String remaining = params;
    Aws::Crt::String thing = s_nibbleNextToken(remaining);
    Aws::Crt::String shadow = s_nibbleNextToken(remaining);

    if (thing.length() == 0 || shadow.length() == 0) {
        fprintf(stdout, "Invalid arguments to update-desired command!\n\n");
        s_printHelp();
        return;
    }

    Aws::Crt::JsonObject stateAsJson(remaining);
    if (!stateAsJson.WasParseSuccessful()) {
        fprintf(stdout, "desired state was not valid JSON!\n\n");
        s_printHelp();
        return;
    }

    Aws::Iotshadow::UpdateNamedShadowRequest request;
    request.ThingName = thing;
    request.ShadowName = shadow;
    request.State = Aws::Iotshadow::ShadowState();
    request.State.value().Desired = stateAsJson;

    shadowClient->UpdateNamedShadow(request, [](UpdateShadowResult &&result){
        s_onUpdateShadowResult(std::move(result), "update-desired");
    });
}

static void s_handleUpdateReportedNamedShadow(const Aws::Crt::String params, const std::shared_ptr<Aws::Iotshadow::IClientV2> &shadowClient) {
    Aws::Crt::String remaining = params;
    Aws::Crt::String thing = s_nibbleNextToken(remaining);
    Aws::Crt::String shadow = s_nibbleNextToken(remaining);

    if (thing.length() == 0 || shadow.length() == 0) {
        fprintf(stdout, "Invalid arguments to update-reported command!\n\n");
        s_printHelp();
        return;
    }

    Aws::Crt::JsonObject stateAsJson(remaining);
    if (!stateAsJson.WasParseSuccessful()) {
        fprintf(stdout, "reported state was not valid JSON!\n\n");
        s_printHelp();
        return;
    }

    Aws::Iotshadow::UpdateNamedShadowRequest request;
    request.ThingName = thing;
    request.ShadowName = shadow;
    request.State = Aws::Iotshadow::ShadowState();
    request.State.value().Reported = stateAsJson;

    shadowClient->UpdateNamedShadow(request, [](UpdateShadowResult &&result){
        s_onUpdateShadowResult(std::move(result), "update-reported");
    });
}

static bool s_handleInput(const Aws::Crt::String &input, const std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> &protocolClient, const std::shared_ptr<Aws::Iotshadow::IClientV2> &shadowClient) {
    Aws::Crt::String remaining = input;
    Aws::Crt::String command = s_nibbleNextToken(remaining);

    if (command == "quit") {
        fprintf(stdout, "Quitting!\n");
        return true;
    } else if (command == "start") {
        fprintf(stdout, "Starting protocol client!\n");
        protocolClient->Start();
    } else if (command == "stop") {
        fprintf(stdout, "Stopping protocol client!\n");
        protocolClient->Stop();
    } else if (command == "get") {
        s_handleGetNamedShadow(remaining, shadowClient);
    } else if (command == "delete") {
        s_handleDeleteNamedShadow(remaining, shadowClient);
    } else if (command == "update-desired") {
        s_handleUpdateDesiredNamedShadow(remaining, shadowClient);
    } else if (command == "update-reported") {
        s_handleUpdateReportedNamedShadow(remaining, shadowClient);
    } else {
        s_printHelp();
    }

    return false;
}

int main(int argc, char *argv[])
{
    /************************ Setup ****************************/

    // Do the global initialization for the API.
    ApiHandle apiHandle;

    Utils::cmdData cmdData = Utils::parseSampleInputBasicConnect(argc, argv, &apiHandle);

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

    if (cmdData.input_port != 0)
    {
        builder->WithPort(static_cast<uint32_t>(cmdData.input_port));
    }

    // Setup lifecycle callbacks
    builder->WithClientConnectionSuccessCallback(s_onConnectionSuccess);
    builder->WithClientConnectionFailureCallback(s_onConnectionFailure);
    builder->WithClientStoppedCallback(s_onStopped);

    // Create Mqtt5Client
    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> client = builder->Build();

    Aws::Iot::RequestResponse::RequestResponseClientOptions requestResponseOptions;
    requestResponseOptions.maxRequestResponseSubscriptions = 4;
    requestResponseOptions.maxStreamingSubscriptions = 10;
    requestResponseOptions.operationTimeoutInSeconds = 30;

    std::shared_ptr<Aws::Iotshadow::IClientV2> shadowClient = Aws::Iotshadow::IClientV2::newFrom5(*client, requestResponseOptions);

    while (true)
    {
        fprintf(stdout, "\nEnter command:\n");

        String input;
        std::getline(std::cin, input);

        if (s_handleInput(input, client, shadowClient))
        {
            fprintf(stdout, "Exiting...");
            break;
        }
    }

    return 0;
}
