/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/mqtt/Mqtt5Packets.h>
#include <aws/iot/Mqtt5Client.h>

#include <aws/iotshadow/ErrorResponse.h>
#include <aws/iotshadow/IotShadowClientV2.h>
#include <aws/iotshadow/DeleteNamedShadowRequest.h>
#include <aws/iotshadow/DeleteShadowResponse.h>
#include <aws/iotshadow/GetNamedShadowRequest.h>
#include <aws/iotshadow/GetShadowResponse.h>
#include <aws/iotshadow/NamedShadowDeltaUpdatedSubscriptionRequest.h>
#include <aws/iotshadow/NamedShadowUpdatedSubscriptionRequest.h>
#include <aws/iotshadow/ShadowDeltaUpdatedEvent.h>
#include <aws/iotshadow/ShadowUpdatedEvent.h>
#include <aws/iotshadow/UpdateNamedShadowRequest.h>
#include <aws/iotshadow/UpdateShadowResponse.h>

#include <algorithm>
#include <condition_variable>
#include <cinttypes>
#include <iostream>
#include <thread>

#include "../../utils/CommandLineUtils.h"

using namespace Aws::Crt;
using namespace Aws::Iotshadow;

struct StreamingOperationWrapper {
    Aws::Crt::String m_thingName;

    Aws::Crt::String m_shadowName;

    Aws::Crt::String m_type;

    std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> m_stream;
};

struct ApplicationContext {

    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> m_protocolClient;

    std::shared_ptr<Aws::Iotshadow::IClientV2> m_shadowClient;

    uint64_t m_nextStreamId;

    std::unordered_map<uint64_t, StreamingOperationWrapper> m_streams;
};


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
    fprintf(stdout, "  stop -- stops the protocol client\n\n");
    fprintf(stdout, "  get <thing-name> <shadow-name> -- gets the state of a named shadow belonging to the specified thing\n");
    fprintf(stdout, "  delete <thing-name> <shadow-name> -- deletes a named shadow belonging to the specified thing\n");
    fprintf(stdout, "  update-desired <thing-name> <shadow-name> <desired-state-JSON> -- updates the desired state of a named shadow belonging to the specified thing\n");
    fprintf(stdout, "  update-reported <thing-name> <shadow-name> <reported-state-JSON> -- updates the reported state a named shadow belonging to the specified thing\n\n");
    fprintf(stdout, "  list-streams -- lists all open streaming operations\n");
    fprintf(stdout, "  open-delta-stream <thing-name> <shadow-name> -- opens a new streaming operation that receives delta events about changes to a particular shadow belonging to a thing\n");
    fprintf(stdout, "  open-document-stream <thing-name> <shadow-name> -- opens a new streaming operation that receives document events about changes to a particular shadow belonging to a thing\n");
    fprintf(stdout, "  close-stream <stream-id> -- closes a streaming operation\n");
}

static void s_onServiceError(const Aws::Iotshadow::ServiceErrorV2<Aws::Iotshadow::ErrorResponse> &serviceError, Aws::Crt::String operationName) {
    fprintf(stdout, "%s failed with error code: %s\n", operationName.c_str(), aws_error_debug_str(serviceError.GetErrorCode()));
    if (serviceError.HasModeledError()) {
        const auto &modeledError = serviceError.GetModeledError();

        Aws::Crt::JsonObject jsonObject;
        modeledError.SerializeToObject(jsonObject);
        Aws::Crt::String outgoingJson = jsonObject.View().WriteCompact(true);
        fprintf(stdout, "modeled error: %s\n", outgoingJson.c_str());
    }
}

static void s_onGetShadowResult(GetShadowResult &&result) {
    if (result.IsSuccess()) {
        const auto &response = result.GetResponse();

        Aws::Crt::JsonObject jsonObject;
        response.SerializeToObject(jsonObject);
        Aws::Crt::String outgoingJson = jsonObject.View().WriteCompact(true);
        fprintf(stdout, "get result: %s\n", outgoingJson.c_str());
    } else {
        s_onServiceError(result.GetError(), "get");
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
    if (result.IsSuccess()) {
        const auto &response = result.GetResponse();

        Aws::Crt::JsonObject jsonObject;
        response.SerializeToObject(jsonObject);
        Aws::Crt::String outgoingJson = jsonObject.View().WriteCompact(true);
        fprintf(stdout, "delete result: %s\n", outgoingJson.c_str());
    } else {
        s_onServiceError(result.GetError(), "delete");
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
    if (result.IsSuccess()) {
        const auto &response = result.GetResponse();

        Aws::Crt::JsonObject jsonObject;
        response.SerializeToObject(jsonObject);
        Aws::Crt::String outgoingJson = jsonObject.View().WriteCompact(true);
        fprintf(stdout, "%s result: %s\n", operationName.c_str(), outgoingJson.c_str());
    } else {
        s_onServiceError(result.GetError(), operationName);
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

static void s_handleListStreams(const ApplicationContext &context) {
    fprintf(stdout, "Streams:\n");
    for (const auto &iter : context.m_streams) {
        uint64_t streamId = iter.first;
        const StreamingOperationWrapper &wrapper = iter.second;
        fprintf(stdout, "  %" PRIu64": type '%s', thing '%s', shadow '%s'\n", streamId, wrapper.m_type.c_str(), wrapper.m_thingName.c_str(), wrapper.m_shadowName.c_str());
    }
}

static void s_handleCloseStream(const Aws::Crt::String params, ApplicationContext &context) {
    Aws::Crt::String remaining = params;
    Aws::Crt::String streamId = s_nibbleNextToken(remaining);

    if (streamId.length() == 0) {
        fprintf(stdout, "Invalid arguments to close-stream command!\n\n");
        s_printHelp();
        return;
    }

    uint64_t id = std::stoull(streamId.c_str());
    fprintf(stdout, "Closing stream %" PRIu64 "\n", id);
    context.m_streams.erase(id);
}

static void s_registerStream(ApplicationContext &context, uint64_t id, std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> operation, Aws::Crt::String type, Aws::Crt::String thing, Aws::Crt::String shadow) {
    StreamingOperationWrapper wrapper;
    wrapper.m_stream = operation;
    wrapper.m_type = type;
    wrapper.m_thingName = thing;
    wrapper.m_shadowName = shadow;

    context.m_streams[id] = wrapper;

    operation->Open();
}

static void s_onSubscriptionStatusEvent(uint64_t id, Aws::Iot::RequestResponse::SubscriptionStatusEvent &&event) {
    fprintf(stdout, "Stream %" PRIu64 ": subscription status event with type %d and error %s\n", id, event.GetErrorCode(), Aws::Crt::ErrorDebugString(event.GetErrorCode()));
}

static void s_onShadowDeltaUpdatedEvent(uint64_t id, Aws::Iotshadow::ShadowDeltaUpdatedEvent &&event) {
    fprintf(stdout, "Stream %" PRIu64 ": received shadow delta updated event:\n", id);

    Aws::Crt::JsonObject jsonObject;
    event.SerializeToObject(jsonObject);
    Aws::Crt::String json = jsonObject.View().WriteCompact(true);
    fprintf(stdout, "  %s\n", json.c_str());
}

static void s_handleOpenDeltaStream(const Aws::Crt::String params, ApplicationContext &context) {
    Aws::Crt::String remaining = params;
    Aws::Crt::String thing = s_nibbleNextToken(remaining);
    Aws::Crt::String shadow = s_nibbleNextToken(remaining);

    if (thing.length() == 0 || shadow.length() == 0) {
        fprintf(stdout, "Invalid arguments to open-delta-stream command!\n\n");
        s_printHelp();
        return;
    }

    uint64_t streamId = context.m_nextStreamId++;

    Aws::Iotshadow::NamedShadowDeltaUpdatedSubscriptionRequest request;
    request.ThingName = thing;
    request.ShadowName = shadow;

    Aws::Iot::RequestResponse::StreamingOperationOptions<Aws::Iotshadow::ShadowDeltaUpdatedEvent> options;
    options.WithSubscriptionStatusEventHandler([streamId](Aws::Iot::RequestResponse::SubscriptionStatusEvent &&event) {
        s_onSubscriptionStatusEvent(streamId, std::move(event));
    });
    options.WithStreamHandler([streamId](Aws::Iotshadow::ShadowDeltaUpdatedEvent &&event) {
        s_onShadowDeltaUpdatedEvent(streamId, std::move(event));
    });

    auto operation = context.m_shadowClient->CreateNamedShadowDeltaUpdatedStream(request, options);
    s_registerStream(context, streamId, operation, "Delta", thing, shadow);
}

static void s_onShadowUpdatedEvent(uint64_t id, Aws::Iotshadow::ShadowUpdatedEvent &&event) {
    fprintf(stdout, "Stream %" PRIu64 ": received shadow updated event:\n", id);

    Aws::Crt::JsonObject jsonObject;
    event.SerializeToObject(jsonObject);
    Aws::Crt::String json = jsonObject.View().WriteCompact(true);
    fprintf(stdout, "  %s\n", json.c_str());
}

static void s_handleOpenDocumentStream(const Aws::Crt::String params, ApplicationContext &context) {
    Aws::Crt::String remaining = params;
    Aws::Crt::String thing = s_nibbleNextToken(remaining);
    Aws::Crt::String shadow = s_nibbleNextToken(remaining);

    if (thing.length() == 0 || shadow.length() == 0) {
        fprintf(stdout, "Invalid arguments to open-document-stream command!\n\n");
        s_printHelp();
        return;
    }

    uint64_t streamId = context.m_nextStreamId++;

    Aws::Iotshadow::NamedShadowUpdatedSubscriptionRequest request;
    request.ThingName = thing;
    request.ShadowName = shadow;

    Aws::Iot::RequestResponse::StreamingOperationOptions<Aws::Iotshadow::ShadowUpdatedEvent> options;
    options.WithSubscriptionStatusEventHandler([streamId](Aws::Iot::RequestResponse::SubscriptionStatusEvent &&event) {
        s_onSubscriptionStatusEvent(streamId, std::move(event));
    });
    options.WithStreamHandler([streamId](Aws::Iotshadow::ShadowUpdatedEvent &&event) {
        s_onShadowUpdatedEvent(streamId, std::move(event));
    });

    auto operation = context.m_shadowClient->CreateNamedShadowUpdatedStream(request, options);
    s_registerStream(context, streamId, operation, "Document", thing, shadow);
}

static bool s_handleInput(const Aws::Crt::String &input, ApplicationContext &context) {
    Aws::Crt::String remaining = input;
    Aws::Crt::String command = s_nibbleNextToken(remaining);

    if (command == "quit") {
        fprintf(stdout, "Quitting!\n");
        return true;
    } else if (command == "start") {
        fprintf(stdout, "Starting protocol client!\n");
        context.m_protocolClient->Start();
    } else if (command == "stop") {
        fprintf(stdout, "Stopping protocol client!\n");
        context.m_protocolClient->Stop();
    } else if (command == "get") {
        s_handleGetNamedShadow(remaining, context.m_shadowClient);
    } else if (command == "delete") {
        s_handleDeleteNamedShadow(remaining, context.m_shadowClient);
    } else if (command == "update-desired") {
        s_handleUpdateDesiredNamedShadow(remaining, context.m_shadowClient);
    } else if (command == "update-reported") {
        s_handleUpdateReportedNamedShadow(remaining, context.m_shadowClient);
    } else if (command == "list-streams") {
        s_handleListStreams(context);
    } else if (command == "open-delta-stream") {
        s_handleOpenDeltaStream(remaining, context);
    } else if (command == "open-document-stream") {
        s_handleOpenDocumentStream(remaining, context);
    } else if (command == "close-stream") {
        s_handleCloseStream(remaining, context);
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

    // Setup lifecycle callbacks
    builder->WithClientConnectionSuccessCallback(s_onConnectionSuccess);
    builder->WithClientConnectionFailureCallback(s_onConnectionFailure);
    builder->WithClientStoppedCallback(s_onStopped);

    Aws::Iot::RequestResponse::RequestResponseClientOptions requestResponseOptions;
    requestResponseOptions.WithMaxRequestResponseSubscriptions(4);
    requestResponseOptions.WithMaxStreamingSubscriptions(10);
    requestResponseOptions.WithOperationTimeoutInSeconds(30);

    ApplicationContext context;
    context.m_protocolClient = builder->Build();;
    context.m_shadowClient = Aws::Iotshadow::NewClientFrom5(*context.m_protocolClient, requestResponseOptions);
    context.m_nextStreamId = 1;

    while (true)
    {
        fprintf(stdout, "\nEnter command:\n");

        String input;
        std::getline(std::cin, input);

        if (s_handleInput(input, context))
        {
            fprintf(stdout, "Exiting...");
            break;
        }
    }

    return 0;
}
