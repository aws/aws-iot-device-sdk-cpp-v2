/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/JsonObject.h>
#include <aws/crt/UUID.h>
#include <aws/crt/io/HostResolver.h>

#include <aws/iot/MqttClient.h>

#include <aws/iotshadow/ErrorResponse.h>
#include <aws/iotshadow/IotShadowClient.h>
#include <aws/iotshadow/ShadowDeltaUpdatedEvent.h>
#include <aws/iotshadow/ShadowDeltaUpdatedSubscriptionRequest.h>
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
#include <string>

#include "../../utils/CommandLineUtils.h"

using namespace Aws::Crt;
using namespace Aws::Iotshadow;

static const char *SHADOW_VALUE_DEFAULT = "off";

static void s_changeShadowValue(
    IotShadowClient &client,
    const String &thingName,
    const String &shadowProperty,
    const String &value)
{
    fprintf(stdout, "Changing local shadow value to %s.\n", value.c_str());

    ShadowState state;
    JsonObject desired;
    JsonObject reported;

    if (value == "null")
    {
        JsonObject nullObject;
        nullObject.AsNull();
        desired.WithObject(shadowProperty, nullObject);
        reported.WithObject(shadowProperty, nullObject);
    }
    else if (value == "clear_shadow")
    {
        desired.AsNull();
        reported.AsNull();
    }
    else
    {
        desired.WithString(shadowProperty, value);
        reported.WithString(shadowProperty, value);
    }
    state.Desired = desired;
    state.Reported = reported;

    UpdateShadowRequest updateShadowRequest;
    Aws::Crt::UUID uuid;
    updateShadowRequest.ClientToken = uuid.ToString();
    updateShadowRequest.ThingName = thingName;
    updateShadowRequest.State = state;

    auto publishCompleted = [thingName, value](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "Failed to update %s shadow state: error %s\n", thingName.c_str(), ErrorDebugString(ioErr));
            return;
        }

        fprintf(stdout, "Successfully updated shadow state for %s, to %s\n", thingName.c_str(), value.c_str());
    };

    client.PublishUpdateShadow(updateShadowRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, std::move(publishCompleted));
}

int main(int argc, char *argv[])
{
    /************************ Setup the Lib ****************************/
    /*
     * Do the global initialization for the API.
     */
    ApiHandle apiHandle;
    String clientId(String("test-") + Aws::Crt::UUID().ToString());
    String currentShadowValue("");

    /*********************** Parse Arguments ***************************/
    Utils::CommandLineUtils cmdUtils = Utils::CommandLineUtils();
    cmdUtils.RegisterProgramName("shadow_sync");
    cmdUtils.AddCommonMQTTCommands();
    cmdUtils.RegisterCommand("key", "<path>", "Path to your key in PEM format.");
    cmdUtils.RegisterCommand("cert", "<path>", "Path to your client certificate in PEM format.");
    cmdUtils.RegisterCommand("thing_name", "<str>", "The name of your IOT thing.");
    cmdUtils.RegisterCommand("shadow_property", "<str>", "The name of the shadow property you want to change.");
    cmdUtils.RegisterCommand("is_ci", "<str>", "If set to 'True' then it will run in CI mode (will publish to shadow automatically).");
    cmdUtils.AddLoggingCommands();
    const char **const_argv = (const char **)argv;
    cmdUtils.SendArguments(const_argv, const_argv + argc);
    cmdUtils.StartLoggingBasedOnCommand(&apiHandle);

    String thingName = cmdUtils.GetCommandRequired("thing_name");
    String shadowProperty = cmdUtils.GetCommandRequired("shadow_property");
    String isCI = cmdUtils.GetCommandOrDefault("is_ci", "");

    /* Get a MQTT client connection from the command parser */
    auto connection = cmdUtils.BuildMQTTConnection();

    /*
     * In a real world application you probably don't want to enforce synchronous behavior
     * but this is a sample console application, so we'll just do that with a condition variable.
     */
    std::promise<bool> connectionCompletedPromise;
    std::promise<void> connectionClosedPromise;

    /*
     * This will execute when a mqtt connect has completed or failed.
     */
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

    /*
     * Invoked when a disconnect message has completed.
     */
    auto onDisconnect = [&](Mqtt::MqttConnection & /*conn*/) {
        {
            fprintf(stdout, "Disconnect completed\n");
            connectionCompletedPromise.set_value(true);
        }
    };

    connection->OnConnectionCompleted = std::move(onConnectionCompleted);
    connection->OnDisconnect = std::move(onDisconnect);

    /*
     * Actually perform the connect dance.
     */
    fprintf(stdout, "Connecting...\n");
    if (!connection->Connect(clientId.c_str(), true, 0))
    {
        fprintf(stderr, "MQTT Connection failed with error %s\n", ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    if (connectionCompletedPromise.get_future().get())
    {
        Aws::Iotshadow::IotShadowClient shadowClient(connection);

        // ==================== Shadow Delta Updates ====================
        // This section is for when a Shadow document updates/changes, whether it is on the server side or client side.

        std::promise<void> subscribeDeltaCompletedPromise;
        std::promise<void> subscribeDeltaAcceptedCompletedPromise;
        std::promise<void> subscribeDeltaRejectedCompletedPromise;

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
                if (event->State && event->State->View().ValueExists(shadowProperty))
                {
                    JsonView objectView = event->State->View().GetJsonObject(shadowProperty);
                    if (objectView.IsNull())
                    {
                        fprintf(
                            stdout,
                            "Delta reports that %s was deleted. Resetting defaults...\n",
                            shadowProperty.c_str());
                        s_changeShadowValue(shadowClient, thingName, shadowProperty, SHADOW_VALUE_DEFAULT);
                    }
                    else
                    {
                        fprintf(
                            stdout,
                            "Delta reports that \"%s\" has a desired value of \"%s\", Changing local value...\n",
                            shadowProperty.c_str(),
                            event->State->View().GetString(shadowProperty).c_str());
                        s_changeShadowValue(
                            shadowClient, thingName, shadowProperty, event->State->View().GetString(shadowProperty));
                    }

                    if (event->ClientToken)
                    {
                        fprintf(stdout, "  ClientToken: %s\n", event->ClientToken->c_str());
                    }
                }
                else
                {
                    fprintf(stdout, "Delta did not report a change in \"%s\".\n", shadowProperty.c_str());
                }
            }
        };

        auto onUpdateShadowAccepted = [&](UpdateShadowResponse *response, int ioErr) {
            if (ioErr != AWS_OP_SUCCESS)
            {
                fprintf(stderr, "Error on subscription: %s.\n", ErrorDebugString(ioErr));
                exit(-1);
            }

            if (response->State->Reported)
            {
                currentShadowValue = response->State->Reported->View().GetString(shadowProperty);
            }
            else
            {
                fprintf(stdout, "Finished clearing shadow properties\n");
                currentShadowValue = "";
            }
            fprintf(stdout, "Enter Desired state of %s:\n", shadowProperty.c_str());
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

        shadowClient.SubscribeToShadowDeltaUpdatedEvents(
            shadowDeltaUpdatedRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onDeltaUpdated, onDeltaUpdatedSubAck);

        UpdateShadowSubscriptionRequest updateShadowSubscriptionRequest;
        updateShadowSubscriptionRequest.ThingName = thingName;

        shadowClient.SubscribeToUpdateShadowAccepted(
            updateShadowSubscriptionRequest,
            AWS_MQTT_QOS_AT_LEAST_ONCE,
            onUpdateShadowAccepted,
            onDeltaUpdatedAcceptedSubAck);

        shadowClient.SubscribeToUpdateShadowRejected(
            updateShadowSubscriptionRequest,
            AWS_MQTT_QOS_AT_LEAST_ONCE,
            onUpdateShadowRejected,
            onDeltaUpdatedRejectedSubAck);

        subscribeDeltaCompletedPromise.get_future().wait();
        subscribeDeltaAcceptedCompletedPromise.get_future().wait();
        subscribeDeltaRejectedCompletedPromise.get_future().wait();

        // ==================== Shadow Value Get ====================
        // This section is to get the initial value of the Shadow document.

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
            if (ioErr != AWS_OP_SUCCESS)
            {
                fprintf(stderr, "Error getting shadow value from document: %s.\n", ErrorDebugString(ioErr));
                exit(-1);
            }
            if (response)
            {
                fprintf(stdout, "Received shadow document.\n");
                if (response->State && response->State->Reported->View().ValueExists(shadowProperty))
                {
                    JsonView objectView = response->State->Reported->View().GetJsonObject(shadowProperty);
                    if (objectView.IsNull())
                    {
                        fprintf(stdout, "Shadow contains \"%s\" but is null.\n", shadowProperty.c_str());
                        currentShadowValue = "";
                    }
                    else
                    {
                        currentShadowValue = response->State->Reported->View().GetString(shadowProperty);
                        fprintf(
                            stdout,
                            "Shadow contains \"%s\". Updating local value to \"%s\"...\n",
                            shadowProperty.c_str(),
                            currentShadowValue.c_str());
                    }
                }
                else
                {
                    fprintf(stdout, "Shadow currently does not contain \"%s\".\n", shadowProperty.c_str());
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

        shadowClient.SubscribeToGetShadowAccepted(
            shadowSubscriptionRequest,
            AWS_MQTT_QOS_AT_LEAST_ONCE,
            onGetShadowAccepted,
            onGetShadowUpdatedAcceptedSubAck);

        shadowClient.SubscribeToGetShadowRejected(
            shadowSubscriptionRequest,
            AWS_MQTT_QOS_AT_LEAST_ONCE,
            onGetShadowRejected,
            onGetShadowUpdatedRejectedSubAck);

        subscribeGetShadowAcceptedCompletedPromise.get_future().wait();
        subscribeGetShadowRejectedCompletedPromise.get_future().wait();

        GetShadowRequest shadowGetRequest;
        shadowGetRequest.ThingName = thingName;

        // Get the current shadow document so we start with the correct value
        shadowClient.PublishGetShadow(shadowGetRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onGetShadowRequestSubAck);

        onGetShadowRequestCompletedPromise.get_future().wait();
        gotInitialShadowPromise.get_future().wait();

        // ==================== Shadow change value input loop ====================
        // This section is to getting user input and changing the shadow value passed to that input.
        // If in CI, then input is automatically passed

        if (isCI != "True") {
            fprintf(stdout, "Enter Desired state of %s:\n", shadowProperty.c_str());
            while (true)
            {
                String input;
                std::cin >> input;

                if (input == "exit" || input == "quit")
                {
                    fprintf(stdout, "Exiting...");
                    break;
                }

                if (input == currentShadowValue)
                {
                    fprintf(stdout, "Shadow is already set to \"%s\"\n", currentShadowValue.c_str());
                    fprintf(stdout, "Enter Desired state of %s:\n", shadowProperty.c_str());
                }
                else
                {
                    s_changeShadowValue(shadowClient, thingName, shadowProperty, input);
                }
            }
        } else {
            int messagesSent = 0;
            while (messagesSent < 5) {
                String input = "Shadow_Value_" + std::to_string(messagesSent);
                s_changeShadowValue(shadowClient, thingName, shadowProperty, input);
                /* Sleep so there is a gap between shadow updates */
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                messagesSent += 1;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

    /* Disconnect */
    if (connection->Disconnect())
    {
        connectionClosedPromise.get_future().wait();
    }

    return 0;
}
