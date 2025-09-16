/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/mqtt/Mqtt5Packets.h>
#include <aws/crt/UUID.h>
#include <aws/iot/Mqtt5Client.h>

#include <aws/iotshadow/IotShadowClientV2.h>
#include <aws/iotshadow/DeleteShadowRequest.h>
#include <aws/iotshadow/DeleteShadowResponse.h>
#include <aws/iotshadow/GetShadowRequest.h>
#include <aws/iotshadow/GetShadowResponse.h>
#include <aws/iotshadow/ShadowDeltaUpdatedSubscriptionRequest.h>
#include <aws/iotshadow/ShadowUpdatedSubscriptionRequest.h>
#include <aws/iotshadow/ShadowDeltaUpdatedEvent.h>
#include <aws/iotshadow/ShadowUpdatedEvent.h>
#include <aws/iotshadow/UpdateShadowRequest.h>
#include <aws/iotshadow/UpdateShadowResponse.h>
#include <aws/iotshadow/V2ErrorResponse.h>

#include <algorithm>
#include <condition_variable>
#include <iostream>

#include "../../utils/CommandLineUtils.h"

using namespace Aws::Crt;
using namespace Aws::Iotshadow;

struct ApplicationContext {

    std::shared_ptr<Mqtt5::Mqtt5Client> m_protocolClient;

    std::shared_ptr<IClientV2> m_shadowClient;

    std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> m_shadowDeltaUpdatedStream;
    std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> m_shadowUpdatedStream;

    String m_thingName;
};

static void s_onConnectionFailure(const Mqtt5::OnConnectionFailureEventData &eventData) {
    fprintf(stdout, "Mqtt5 client connection attempt failed with error: %s.\n", aws_error_debug_str(eventData.errorCode));
}

static String s_nibbleNextToken(String &input) {
    String token;
    String remaining;
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
    fprintf(stdout, "  get -- gets the current value of the IoT thing's shadow\n");
    fprintf(stdout, "  delete -- deletes the IoT thing's shadow\n");
    fprintf(stdout, "  update-desired <desired-state-as-JSON> -- updates the desired state of the IoT thing's shadow.  If the shadow does not exist, it will be created.\n");
    fprintf(stdout, "  update-reported <reported-state-as-JSON> -- updates the reported state of the IoT thing's shadow.  If the shadow does not exist, it will be created.\n\n");
}

static void s_onServiceError(const ServiceErrorV2<V2ErrorResponse> &serviceError, String operationName) {
    fprintf(stdout, "%s failed with error code: %s\n", operationName.c_str(), aws_error_debug_str(serviceError.GetErrorCode()));
    if (serviceError.HasModeledError()) {
        const auto &modeledError = serviceError.GetModeledError();

        JsonObject jsonObject;
        modeledError.SerializeToObject(jsonObject);
        String outgoingJson = jsonObject.View().WriteCompact(true);
        fprintf(stdout, "modeled error: %s\n", outgoingJson.c_str());
    }
}

static void s_onGetShadowResult(GetShadowResult &&result) {
    if (result.IsSuccess()) {
        const auto &response = result.GetResponse();

        JsonObject jsonObject;
        response.SerializeToObject(jsonObject);
        String outgoingJson = jsonObject.View().WriteCompact(true);
        fprintf(stdout, "get result: %s\n", outgoingJson.c_str());
    } else {
        const auto &serviceError = result.GetError();
        s_onServiceError(serviceError, "get");
    }
}

static void s_handleGetShadow(const ApplicationContext &context) {
    GetShadowRequest request;
    request.ThingName = context.m_thingName;

    std::promise<bool> getWaiter;
    context.m_shadowClient->GetShadow(request, [&getWaiter](GetShadowResult &&result){
        s_onGetShadowResult(std::move(result));
        getWaiter.set_value(true);
    });

    getWaiter.get_future().wait();
}

static void s_onDeleteShadowResult(DeleteShadowResult &&result) {
    if (result.IsSuccess()) {
        const auto &response = result.GetResponse();

        JsonObject jsonObject;
        response.SerializeToObject(jsonObject);
        String outgoingJson = jsonObject.View().WriteCompact(true);
        fprintf(stdout, "delete result: %s\n", outgoingJson.c_str());
    } else {
        s_onServiceError(result.GetError(), "delete");
    }
}

static void s_handleDeleteShadow(const ApplicationContext &context) {
    DeleteShadowRequest request;
    request.ThingName = context.m_thingName;

    std::promise<bool> deleteWaiter;
    context.m_shadowClient->DeleteShadow(request, [&deleteWaiter](DeleteShadowResult &&result){
        s_onDeleteShadowResult(std::move(result));
        deleteWaiter.set_value(true);
    });

    deleteWaiter.get_future().wait();
}

static void s_onUpdateShadowResult(UpdateShadowResult &&result, String operationName) {
    if (result.IsSuccess()) {
        const auto &response = result.GetResponse();

        JsonObject jsonObject;
        response.SerializeToObject(jsonObject);
        String outgoingJson = jsonObject.View().WriteCompact(true);
        fprintf(stdout, "%s result: %s\n", operationName.c_str(), outgoingJson.c_str());
    } else {
        s_onServiceError(result.GetError(), operationName);
    }
}

static void s_handleUpdateDesiredShadow(const String &params, const ApplicationContext &context) {
    JsonObject stateAsJson(params);
    if (!stateAsJson.WasParseSuccessful()) {
        fprintf(stdout, "desired state was not valid JSON!\n\n");
        s_printHelp();
        return;
    }

    Aws::Iotshadow::UpdateShadowRequest request;
    request.ThingName = context.m_thingName;
    request.State = ShadowState();
    request.State.value().Desired = stateAsJson;

    std::promise<bool> updateWaiter;
    context.m_shadowClient->UpdateShadow(request, [&updateWaiter](UpdateShadowResult &&result){
        s_onUpdateShadowResult(std::move(result), "update-desired");
        updateWaiter.set_value(true);
    });

    updateWaiter.get_future().wait();
}

static void s_handleUpdateReportedShadow(const String &params, const ApplicationContext &context) {
    JsonObject stateAsJson(params);
    if (!stateAsJson.WasParseSuccessful()) {
        fprintf(stdout, "reported state was not valid JSON!\n\n");
        s_printHelp();
        return;
    }

    UpdateShadowRequest request;
    request.ThingName = context.m_thingName;
    request.State = ShadowState();
    request.State.value().Reported = stateAsJson;

    std::promise<bool> updateWaiter;
    context.m_shadowClient->UpdateShadow(request, [&updateWaiter](UpdateShadowResult &&result){
        s_onUpdateShadowResult(std::move(result), "update-reported");
        updateWaiter.set_value(true);
    });

    updateWaiter.get_future().wait();
}

static std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> s_createShadowUpdatedStream(const ApplicationContext &context) {
    std::promise<bool> subscribedWaiter;

    ShadowUpdatedSubscriptionRequest request;
    request.ThingName = context.m_thingName;

    Aws::Iot::RequestResponse::StreamingOperationOptions<Aws::Iotshadow::ShadowUpdatedEvent> options;
    options.WithSubscriptionStatusEventHandler([&subscribedWaiter](Aws::Iot::RequestResponse::SubscriptionStatusEvent &&event) {
        if (event.GetType() == Aws::Iot::RequestResponse::SubscriptionStatusEventType::SubscriptionEstablished) {
            subscribedWaiter.set_value(true);
        }
    });
    options.WithStreamHandler([](Aws::Iotshadow::ShadowUpdatedEvent &&event) {
        Aws::Crt::JsonObject jsonObject;
        event.SerializeToObject(jsonObject);
        Aws::Crt::String json = jsonObject.View().WriteCompact(true);
        fprintf(stdout, "Received ShadowUpdated event:  %s\n", json.c_str());
    });

    auto stream = context.m_shadowClient->CreateShadowUpdatedStream(request, options);
    stream->Open();

    subscribedWaiter.get_future().wait();

    return stream;
}

static std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> s_createShadowDeltaUpdatedStream(const ApplicationContext &context) {
    std::promise<bool> subscribedWaiter;

    ShadowDeltaUpdatedSubscriptionRequest request;
    request.ThingName = context.m_thingName;

    Aws::Iot::RequestResponse::StreamingOperationOptions<Aws::Iotshadow::ShadowDeltaUpdatedEvent> options;
    options.WithSubscriptionStatusEventHandler([&subscribedWaiter](Aws::Iot::RequestResponse::SubscriptionStatusEvent &&event) {
        if (event.GetType() == Aws::Iot::RequestResponse::SubscriptionStatusEventType::SubscriptionEstablished) {
            subscribedWaiter.set_value(true);
        }
    });
    options.WithStreamHandler([](Aws::Iotshadow::ShadowDeltaUpdatedEvent &&event) {
        Aws::Crt::JsonObject jsonObject;
        event.SerializeToObject(jsonObject);
        Aws::Crt::String json = jsonObject.View().WriteCompact(true);
        fprintf(stdout, "Received ShadowDeltaUpdated event:  %s\n", json.c_str());
    });

    auto stream = context.m_shadowClient->CreateShadowDeltaUpdatedStream(request, options);
    stream->Open();

    subscribedWaiter.get_future().wait();

    return stream;
}

static bool s_handleInput(const Aws::Crt::String &input, ApplicationContext &context) {
    Aws::Crt::String remaining = input;
    Aws::Crt::String command = s_nibbleNextToken(remaining);

    if (command == "quit") {
        fprintf(stdout, "Quitting!\n");
        return true;
    } else if (command == "get") {
        s_handleGetShadow(context);
    } else if (command == "delete") {
        s_handleDeleteShadow(context);
    } else if (command == "update-desired") {
        s_handleUpdateDesiredShadow(remaining, context);
    } else if (command == "update-reported") {
        s_handleUpdateReportedShadow(remaining, context);
    } else {
        if (command != "help") {
            fprintf(stdout, "Command not recognized: %s\n", command.c_str());
        }
        s_printHelp();
    }

    return false;
}

int main(int argc, char *argv[])
{
    /************************ Setup ****************************/

    // Do the global initialization for the API.
    ApiHandle apiHandle;

    Utils::cmdData cmdData = Utils::parseSampleInputShadow(argc, argv, &apiHandle);

    ApplicationContext context;
    context.m_thingName = cmdData.input_thingName;

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
    builder->WithClientConnectionSuccessCallback([&connectedWaiter](const Mqtt5::OnConnectionSuccessEventData &){
        fprintf(stdout, "Mqtt5 Client connection succeeded!\n");
        connectedWaiter.set_value(true);
    });
    builder->WithClientConnectionFailureCallback(s_onConnectionFailure);

    auto protocolClient = builder->Build();
    if (!protocolClient) {
        printf(
            "Failed to create mqtt5 client with error code %d: %s", LastError(), ErrorDebugString(LastError()));
        return -1;
    }

    context.m_protocolClient = protocolClient;

    Aws::Iot::RequestResponse::RequestResponseClientOptions requestResponseOptions;
    requestResponseOptions.WithMaxRequestResponseSubscriptions(4);
    requestResponseOptions.WithMaxStreamingSubscriptions(10);
    requestResponseOptions.WithOperationTimeoutInSeconds(30);

    auto shadowClient = Aws::Iotshadow::NewClientFrom5(*context.m_protocolClient, requestResponseOptions);
    if (!shadowClient) {
        printf(
            "Failed to create shadow client with error code %d: %s", LastError(), ErrorDebugString(LastError()));
        return -1;
    }

    context.m_shadowClient = shadowClient;

    fprintf(stdout, "Starting protocol client!\n");
    context.m_protocolClient->Start();

    connectedWaiter.get_future().wait();

    context.m_shadowUpdatedStream = s_createShadowUpdatedStream(context);
    context.m_shadowDeltaUpdatedStream = s_createShadowDeltaUpdatedStream(context);

    while (true)
    {
        fprintf(stdout, "\nEnter command:\n");

        String input;
        std::getline(std::cin, input);

        if (s_handleInput(input, context) || !std::cin)
        {
            fprintf(stdout, "Exiting...");
            break;
        }
    }

    return 0;
}
