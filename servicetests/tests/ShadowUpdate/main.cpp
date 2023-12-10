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

#include <aws/iotshadow/ErrorResponse.h>
#include <aws/iotshadow/IotShadowClient.h>
#include <aws/iotshadow/NamedShadowUpdatedSubscriptionRequest.h>
#include <aws/iotshadow/ShadowDeltaUpdatedEvent.h>
#include <aws/iotshadow/ShadowDeltaUpdatedSubscriptionRequest.h>
#include <aws/iotshadow/ShadowUpdatedEvent.h>
#include <aws/iotshadow/UpdateNamedShadowRequest.h>

#include <aws/iotshadow/ShadowUpdatedSubscriptionRequest.h>
#include <aws/iotshadow/UpdateShadowRequest.h>
#include <aws/iotshadow/UpdateShadowResponse.h>
#include <aws/iotshadow/UpdateShadowSubscriptionRequest.h>

#include <aws/iotshadow/GetShadowRequest.h>
#include <aws/iotshadow/GetShadowResponse.h>
#include <aws/iotshadow/GetShadowSubscriptionRequest.h>
#include <aws/iotshadow/UpdateShadowSubscriptionRequest.h>

#include <algorithm>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

#include "../../../samples/utils/CommandLineUtils.h"

using namespace Aws::Crt;
using namespace Aws::Iotshadow;

void changeNamedShadowValue(
    String thingName,
    String property,
    String value,
    String shadowName,
    std::shared_ptr<IotShadowClient> shadowClient);

void subscribeNamedShadowUpdatedValue(
    Aws::Crt::String thingName,
    String property,
    String value,
    String shadowName,
    std::shared_ptr<IotShadowClient> shadowClient,
    std::promise<void> &gotResponse);

void changeShadowValue(
    Aws::Crt::String thingName,
    String property,
    String value,
    std::shared_ptr<IotShadowClient> shadowClient);

void subscribeShadowUpdatedValue(
    Aws::Crt::String thingName,
    String property,
    String value,
    std::shared_ptr<IotShadowClient> shadowClient,
    std::promise<void> &gotResponse);

std::shared_ptr<IotShadowClient> build_mqtt3_client(
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
    return std::make_shared<IotShadowClient>(connection);
}
std::shared_ptr<IotShadowClient> build_mqtt5_client(
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
        builder->WithPort(static_cast<uint16_t>(cmdData.input_port));
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
    return std::make_shared<IotShadowClient>(client5);
}

int main(int argc, char *argv[])
{
    /************************ Setup ****************************/
    ApiHandle apiHandle;

    /**
     * cmdData is the arguments/input from the command line placed into a single struct for
     * use in this sample. This handles all of the command line parsing, validating, etc.
     * See the Utils/CommandLineUtils for more information.
     */
    Utils::cmdData cmdData = Utils::parseSampleInputShadow(argc, argv, &apiHandle);
    std::shared_ptr<IotShadowClient> shadowClient = nullptr;

    /**
     * In a real world application you probably don't want to enforce synchronous behavior
     * but this is a sample console application, so we'll just do that with a condition variable.
     */
    std::promise<bool> connectionCompletedPromise;
    std::promise<void> connectionClosedPromise;
    // Create connection
    std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> connection = nullptr;
    // Create Mqtt5Client
    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> client5 = nullptr;

    if (cmdData.input_mqtt_version == 5UL)
    {
        shadowClient = build_mqtt5_client(cmdData, client5, connectionCompletedPromise, connectionClosedPromise);
    }
    else if (cmdData.input_mqtt_version == 3UL)
    {
        shadowClient = build_mqtt3_client(cmdData, connection, connectionCompletedPromise, connectionClosedPromise);
    }
    else
    {
        fprintf(stderr, "MQTT Version not supported\n");
        exit(-1);
    }

    /************************ Run the sample ****************************/
    if (connectionCompletedPromise.get_future().get())
    {
        if (cmdData.input_shadowName.empty())
        {
            std::promise<void> gotResponse;
            subscribeShadowUpdatedValue(
                cmdData.input_thingName,
                cmdData.input_shadowProperty,
                cmdData.input_shadowValue,
                shadowClient,
                gotResponse);

            changeShadowValue(
                cmdData.input_thingName, cmdData.input_shadowProperty, cmdData.input_shadowValue, shadowClient);

            std::future_status status = gotResponse.get_future().wait_for(std::chrono::seconds(5));
            if (status == std::future_status::timeout)
            {
                fprintf(stderr, "Shadow update document timedout\n");
                exit(-1);
            }
        }
        else
        {
            std::promise<void> gotResponse;
            subscribeNamedShadowUpdatedValue(
                cmdData.input_thingName,
                cmdData.input_shadowProperty,
                cmdData.input_shadowValue,
                cmdData.input_shadowName,
                shadowClient,
                gotResponse);

            changeNamedShadowValue(
                cmdData.input_thingName,
                cmdData.input_shadowProperty,
                cmdData.input_shadowValue,
                cmdData.input_shadowName,
                shadowClient);

            std::future_status status = gotResponse.get_future().wait_for(std::chrono::seconds(5));
            if (status == std::future_status::timeout)
            {
                fprintf(stderr, "Shadow update document timedout\n");
                exit(-1);
            }
        }
    }
    /************************ sample ends ****************************/
    /* Closing down */
    // Wait just a little bit to let the console print
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    if (cmdData.input_mqtt_version == 5UL)
    {   // mqtt5
        // Disconnect
        if (client5->Stop() == true)
        {
            connectionClosedPromise.get_future().wait();
        }
    }
    else
    {    // mqtt3
        // Disconnect
        if (connection->Disconnect() == true)
        {
            connectionClosedPromise.get_future().wait();
        }
    }
    return 0;
}

void subscribeShadowUpdatedValue(
    Aws::Crt::String thingName,
    String property,
    String value,
    std::shared_ptr<IotShadowClient> shadowClient,
    std::promise<void> &gotResponse)
{
    std::promise<void> shadowCompletedPromise;
    auto publishCompleted = [&thingName, &value, &shadowCompletedPromise](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(
                stderr,
                "Failed to update %s unnamed shadow state: error %s\n",
                thingName.c_str(),
                ErrorDebugString(ioErr));
            exit(-1);
        }
        fprintf(stdout, "Publish reached the broker shadow state for %s, to %s\n", thingName.c_str(), value.c_str());
        shadowCompletedPromise.set_value();
    };

    /* verify updated shadow */
    auto handler = [&gotResponse, &property] (ShadowUpdatedEvent *event, int ioErr) {
        if (ioErr == AWS_OP_ERR)
        {
            fprintf(stderr, "handler lambda error\n");
            exit(-1);
        }
        if (event)
        {
            String value = event->Previous->State->Reported->View().GetString(property);
            String value2 = event->Previous->State->Desired->View().GetString(property);
            String value3 = event->Current->State->Reported->View().GetString(property);
            String value4 = event->Current->State->Desired->View().GetString(property);

            fprintf(stdout, "previous reported Value 1 %s\n", value.c_str());
            fprintf(stdout, "previous desired  Value 2 %s\n", value2.c_str());

            fprintf(stdout, "current  reported Value 3 %s\n", value3.c_str());
            fprintf(stdout, "current  desired  Value 4 %s\n", value4.c_str());
        }
        gotResponse.set_value();
    };

    /* subscribe to event updates */
    ShadowUpdatedSubscriptionRequest requestUpdate;
    requestUpdate.ThingName = thingName;
    shadowClient->SubscribeToShadowUpdatedEvents(requestUpdate, AWS_MQTT_QOS_AT_LEAST_ONCE, handler, std::move(publishCompleted));
    shadowCompletedPromise.get_future().get();
}

void subscribeNamedShadowUpdatedValue(
    Aws::Crt::String thingName,
    String property,
    String value,
    String shadowName,
    std::shared_ptr<IotShadowClient> shadowClient,
    std::promise<void> &gotResponse)
{
    std::promise<void> shadowCompletedPromise;
    auto publishCompleted = [&thingName, &value, &shadowCompletedPromise](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(
                stderr,
                "Failed to update %s unnamed shadow state: error %s\n",
                thingName.c_str(),
                ErrorDebugString(ioErr));
            exit(-1);
        }
        fprintf(stdout, "Publish reached the broker shadow state for %s, to %s\n", thingName.c_str(), value.c_str());
        shadowCompletedPromise.set_value();
    };

    /* verify updated shadow */
    auto handler = [&gotResponse, &property](ShadowUpdatedEvent *event, int ioErr) {
        if (ioErr == AWS_OP_ERR)
        {
            fprintf(stderr, "handler lambda error\n");
            exit(-1);
        }
        if (event)
        {
            String value = event->Previous->State->Reported->View().GetString(property);
            String value2 = event->Previous->State->Desired->View().GetString(property);
            String value3 = event->Current->State->Reported->View().GetString(property);
            String value4 = event->Current->State->Desired->View().GetString(property);

            fprintf(stdout, "previous reported Value 1 %s\n", value.c_str());
            fprintf(stdout, "previous desired  Value 2 %s\n", value2.c_str());

            fprintf(stdout, "current  reported Value 3 %s\n", value3.c_str());
            fprintf(stdout, "current  desired  Value 4 %s\n", value4.c_str());
        }
        gotResponse.set_value();
    };

    /* subscribe to event updates */
    NamedShadowUpdatedSubscriptionRequest requestUpdate;
    requestUpdate.ThingName = thingName;
    requestUpdate.ShadowName = shadowName;
    shadowClient->SubscribeToNamedShadowUpdatedEvents(requestUpdate, AWS_MQTT_QOS_AT_LEAST_ONCE, handler, std::move(publishCompleted));
    shadowCompletedPromise.get_future().get();
}

void changeShadowValue(
    Aws::Crt::String thingName,
    String property,
    String value,
    std::shared_ptr<IotShadowClient> shadowClient)
{

    /* =============== delta updates =============== */

    /********************** Shadow Delta Updates ********************/
    // This section is for when a Shadow document updates/changes, whether it is on the server side or client side.

    String currentShadowValue("");
    /*
    std::promise<void> subscribeDeltaCompletedPromise;
    std::promise<void> subscribeDeltaAcceptedCompletedPromise;
    std::promise<void> subscribeDeltaRejectedCompletedPromise;
    std::promise<void> gotAnswer;

    auto onDeltaUpdatedSubAck = [&](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error subscribing to shadow delta: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }
        subscribeDeltaCompletedPromise.set_value();
    };

    auto onDeltaUpdatedAcceptedSubAck = [&](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error subscribing to shadow delta accepted: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }
        subscribeDeltaAcceptedCompletedPromise.set_value();
    };

    auto onDeltaUpdatedRejectedSubAck = [&](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error subscribing to shadow delta rejected: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }
        subscribeDeltaRejectedCompletedPromise.set_value();
    };

    auto onDeltaUpdated = [&](ShadowDeltaUpdatedEvent *event, int ioErr) {
        if (ioErr)
        {
            fprintf(stdout, "Error processing shadow delta: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }

        if (event)
        {
            fprintf(stdout, "Received shadow delta event.\n");
            if (event->State && event->State->View().ValueExists(property))
            {
                JsonView objectView = event->State->View().GetJsonObject(property);
                if (objectView.IsNull())
                {
                    fprintf(
                        stdout,
                        "Delta reports that %s was deleted. Resetting defaults...\n",
                        property.c_str());
                    //s_changeShadowValue(
                     //   shadowClient, thingName, property, SHADOW_VALUE_DEFAULT);
                }
                else
                {
                    fprintf(
                        stdout,
                        "Delta reports that \"%s\" has a desired value of \"%s\", Changing local value...\n",
                        property.c_str(),
                        event->State->View().GetString(property).c_str());
               //     s_changeShadowValue(
                //        shadowClient,
                 //       thingName,
                  //      property,
                   //     event->State->View().GetString(property));
                }

                if (event->ClientToken)
                {
                    fprintf(stdout, "  ClientToken: %s\n", event->ClientToken->c_str());
                }
            }
            else
            {
                fprintf(stdout, "Delta did not report a change in \"%s\".\n",
                        property.c_str());
            }
        }
        gotAnswer.set_value();
    };

    auto onUpdateShadowAccepted = [&](UpdateShadowResponse *response, int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error on subscription: %s.\n", ErrorDebugString(ioErr));
            exit(-1);
        }

        if (response->State->Reported)
        {
            currentShadowValue = response->State->Reported->View().GetString(property);
        }
        else
        {
            fprintf(stdout, "Finished clearing shadow properties\n");
            currentShadowValue = "";
        }

        //if (cmdData.input_isCI == false)
       // {
        //    fprintf(stdout, "Enter Desired state of %s:\n", cmdData.input_shadowProperty.c_str());
       // }
    };

    auto onUpdateShadowRejected = [&](ErrorResponse *error, int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error on subscription: %s.\n", ErrorDebugString(ioErr));
            exit(-1);
        }
        fprintf(
            stdout,
            "Update of shadow state failed with message %s and code %d.",
            error->Message->c_str(),
            *error->Code);
    };

    ShadowDeltaUpdatedSubscriptionRequest shadowDeltaUpdatedRequest;
    shadowDeltaUpdatedRequest.ThingName = thingName;

    shadowClient->SubscribeToShadowDeltaUpdatedEvents(
        shadowDeltaUpdatedRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onDeltaUpdated, onDeltaUpdatedSubAck);

    UpdateShadowSubscriptionRequest updateShadowSubscriptionRequest;
    updateShadowSubscriptionRequest.ThingName = thingName;

    shadowClient->SubscribeToUpdateShadowAccepted(
        updateShadowSubscriptionRequest,
        AWS_MQTT_QOS_AT_LEAST_ONCE,
        onUpdateShadowAccepted,
        onDeltaUpdatedAcceptedSubAck);

    shadowClient->SubscribeToUpdateShadowRejected(
        updateShadowSubscriptionRequest,
        AWS_MQTT_QOS_AT_LEAST_ONCE,
        onUpdateShadowRejected,
        onDeltaUpdatedRejectedSubAck);

    subscribeDeltaCompletedPromise.get_future().wait();
    subscribeDeltaAcceptedCompletedPromise.get_future().wait();
    subscribeDeltaRejectedCompletedPromise.get_future().wait();

*/
/* =============== delta updates end =============*/

    /********************** Shadow Value Get ********************/
    // This section is to get the initial value of the Shadow document.
/*

    std::promise<void> subscribeGetShadowAcceptedCompletedPromise;
    std::promise<void> subscribeGetShadowRejectedCompletedPromise;
    std::promise<void> onGetShadowRequestCompletedPromise;
    std::promise<void> gotInitialShadowPromise;

    auto onGetShadowUpdatedAcceptedSubAck = [&](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error subscribing to get shadow document accepted: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }
        subscribeGetShadowAcceptedCompletedPromise.set_value();
    };

    auto onGetShadowUpdatedRejectedSubAck = [&](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error subscribing to get shadow document rejected: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }
        subscribeGetShadowRejectedCompletedPromise.set_value();
    };

    auto onGetShadowRequestSubAck = [&](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error getting shadow document: %s\n", ErrorDebugString(ioErr));
            exit(-1);
        }
        onGetShadowRequestCompletedPromise.set_value();
    };

    auto onGetShadowAccepted = [&](GetShadowResponse *response, int ioErr) {
        printf("============  shadow updated  ===========\n");
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error getting shadow value from document: %s.\n", ErrorDebugString(ioErr));
            exit(-1);
        }
        if (response)
        {
            fprintf(stdout, "Received shadow document.\n");
            if (response->State && response->State->Reported->View().ValueExists(property))
            {
                JsonView objectView = response->State->Reported->View().GetJsonObject(property);
                if (objectView.IsNull())
                {
                    fprintf(stdout, "Shadow contains \"%s\" but is null.\n", property.c_str());
                    currentShadowValue = "";
                }
                else
                {
                    currentShadowValue = response->State->Reported->View().GetString(property);
                    fprintf(
                        stdout,
                        "Shadow contains \"%s\". Updating local value to \"%s\"...\n",
                        property.c_str(),
                        currentShadowValue.c_str());
                }
            }
            else
            {
                fprintf(
                    stdout, "Shadow currently does not contain \"%s\".\n", property.c_str());
                currentShadowValue = "";
            }
            gotInitialShadowPromise.set_value();
        }
    };

    auto onGetShadowRejected = [&](ErrorResponse *error, int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Error on getting shadow document: %s.\n", ErrorDebugString(ioErr));
            exit(-1);
        }
        fprintf(
            stdout,
            "Getting shadow document failed with message %s and code %d.\n",
            error->Message->c_str(),
            *error->Code);
        gotInitialShadowPromise.set_value();
    };

    GetShadowSubscriptionRequest shadowSubscriptionRequest;
    shadowSubscriptionRequest.ThingName = thingName;

    shadowClient->SubscribeToGetShadowAccepted(
        shadowSubscriptionRequest,
        AWS_MQTT_QOS_AT_LEAST_ONCE,
        onGetShadowAccepted,
        onGetShadowUpdatedAcceptedSubAck);

    shadowClient->SubscribeToGetShadowRejected(
        shadowSubscriptionRequest,
        AWS_MQTT_QOS_AT_LEAST_ONCE,
        onGetShadowRejected,
        onGetShadowUpdatedRejectedSubAck);

    subscribeGetShadowAcceptedCompletedPromise.get_future().wait();
    subscribeGetShadowRejectedCompletedPromise.get_future().wait();

    GetShadowRequest shadowGetRequest;
    shadowGetRequest.ThingName = thingName;

    // Get the current shadow document so we start with the correct value
    shadowClient->PublishGetShadow(shadowGetRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onGetShadowRequestSubAck);

    onGetShadowRequestCompletedPromise.get_future().wait();
    gotInitialShadowPromise.get_future().wait();
    */

/********************** Shadow Value Get end ********************/

    std::promise<void> shadowCompletedPromise;

/* publish updates */

    JsonObject desired;
    JsonObject reported;
    ShadowState state;

    JsonObject val;
    val.AsString(value);

    desired.WithObject(property, val);
    reported.WithObject(property, val);

    state.Desired = desired;
    state.Reported = reported;

    UpdateShadowRequest request;
    request.ThingName = thingName;
    request.State = state;
    Aws::Crt::UUID uuid;
    request.ClientToken = uuid.ToString();

    auto publishCompleted = [&thingName, &value, &shadowCompletedPromise](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(
                stderr,
                "Failed to update %s unnamed shadow state: error %s\n",
                thingName.c_str(),
                ErrorDebugString(ioErr));
            exit(-1);
        }
        fprintf(stdout, "Publish reached the broker shadow state for %s, to %s\n", thingName.c_str(), value.c_str());
        shadowCompletedPromise.set_value();
    };
    shadowClient->PublishUpdateShadow(request, AWS_MQTT_QOS_AT_LEAST_ONCE, std::move(publishCompleted));

    shadowCompletedPromise.get_future().get();

    return;
}

void changeNamedShadowValue(
    String thingName,
    String property,
    String value,
    String shadowName,
    std::shared_ptr<IotShadowClient> shadowClient)
{
    printf("change named shadow value ....\n");
    JsonObject desired;
    JsonObject reported;
    ShadowState state;

    JsonObject val;
    val.AsString(value);

    desired.WithObject(property, val);
    reported.WithObject(property, val);

    state.Desired = desired;
    state.Reported = reported;

    UpdateNamedShadowRequest request;
    request.ThingName = thingName;
    request.State = state;
    request.ShadowName = shadowName;
    Aws::Crt::UUID uuid;
    request.ClientToken = uuid.ToString();

    std::promise<void> shadowCompletedPromise;
    auto publishCompleted = [thingName, value, &shadowCompletedPromise](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(
                stderr,
                "Failed to update %s named shadow state: error %s\n",
                thingName.c_str(),
                ErrorDebugString(ioErr));
            exit(-1);
        }

        fprintf(stdout, "Publish reached the broker shadow state for %s, to %s\n", thingName.c_str(), value.c_str());
        shadowCompletedPromise.set_value();
    };
    shadowClient->PublishUpdateNamedShadow(request, AWS_MQTT_QOS_AT_LEAST_ONCE, std::move(publishCompleted));
    shadowCompletedPromise.get_future().get();
}
