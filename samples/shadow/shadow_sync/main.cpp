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
#include <thread>

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
    /************************ Setup ****************************/

    // Do the global initialization for the API.
    ApiHandle apiHandle;

    String currentShadowValue("");

    /**
     * cmdData is the arguments/input from the command line placed into a single struct for
     * use in this sample. This handles all of the command line parsing, validating, etc.
     * See the Utils/CommandLineUtils for more information.
     */
    Utils::cmdData cmdData = Utils::parseSampleInputShadow(argc, argv, &apiHandle);

    // Create the MQTT builder and populate it with data from cmdData.
    auto clientConfigBuilder =
        Aws::Iot::MqttClientConnectionConfigBuilder(cmdData.input_cert.c_str(), cmdData.input_key.c_str());
    clientConfigBuilder.WithEndpoint(cmdData.input_endpoint);
    if (cmdData.input_ca != "")
    {
        clientConfigBuilder.WithCertificateAuthority(cmdData.input_ca.c_str());
    }
    if (cmdData.input_proxyHost != "")
    {
        Aws::Crt::Http::HttpClientConnectionProxyOptions proxyOptions;
        proxyOptions.HostName = cmdData.input_proxyHost;
        proxyOptions.Port = static_cast<uint16_t>(cmdData.input_proxyPort);
        proxyOptions.AuthType = Aws::Crt::Http::AwsHttpProxyAuthenticationType::None;
        clientConfigBuilder.WithHttpProxyOptions(proxyOptions);
    }
    if (cmdData.input_port != 0)
    {
        clientConfigBuilder.WithPortOverride(static_cast<uint16_t>(cmdData.input_port));
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

    /**
     * In a real world application you probably don't want to enforce synchronous behavior
     * but this is a sample console application, so we'll just do that with a condition variable.
     */
    std::promise<bool> connectionCompletedPromise;
    std::promise<void> connectionClosedPromise;

    // Invoked when a MQTT connect has completed or failed
    auto onConnectionCompleted =
        [&](Aws::Crt::Mqtt::MqttConnection &, int errorCode, Aws::Crt::Mqtt::ReturnCode returnCode, bool) {
            if (errorCode)
            {
                fprintf(stdout, "Connection failed with error %s\n", Aws::Crt::ErrorDebugString(errorCode));
                connectionCompletedPromise.set_value(false);
            }
            else
            {
                fprintf(stdout, "Connection completed with return code %d\n", returnCode);
                connectionCompletedPromise.set_value(true);
            }
        };

    // Invoked when a disconnect message has completed.
    auto onDisconnect = [&](Aws::Crt::Mqtt::MqttConnection &) {
        fprintf(stdout, "Disconnect completed\n");
        connectionClosedPromise.set_value();
    };

    // Assign callbacks
    connection->OnConnectionCompleted = std::move(onConnectionCompleted);
    connection->OnDisconnect = std::move(onDisconnect);

    /************************ Run the sample ****************************/

    // Connect
    fprintf(stdout, "Connecting...\n");
    if (!connection->Connect(cmdData.input_clientId.c_str(), true, 0))
    {
        fprintf(stderr, "MQTT Connection failed with error %s\n", ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    if (connectionCompletedPromise.get_future().get())
    {
        Aws::Iotshadow::IotShadowClient shadowClient(connection);

        /********************** Shadow Delta Updates ********************/
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
                if (event->State && event->State->View().ValueExists(cmdData.input_shadowProperty))
                {
                    JsonView objectView = event->State->View().GetJsonObject(cmdData.input_shadowProperty);
                    if (objectView.IsNull())
                    {
                        fprintf(
                            stdout,
                            "Delta reports that %s was deleted. Resetting defaults...\n",
                            cmdData.input_shadowProperty.c_str());
                        s_changeShadowValue(
                            shadowClient, cmdData.input_thingName, cmdData.input_shadowProperty, SHADOW_VALUE_DEFAULT);
                    }
                    else
                    {
                        fprintf(
                            stdout,
                            "Delta reports that \"%s\" has a desired value of \"%s\", Changing local value...\n",
                            cmdData.input_shadowProperty.c_str(),
                            event->State->View().GetString(cmdData.input_shadowProperty).c_str());
                        s_changeShadowValue(
                            shadowClient,
                            cmdData.input_thingName,
                            cmdData.input_shadowProperty,
                            event->State->View().GetString(cmdData.input_shadowProperty));
                    }

                    if (event->ClientToken)
                    {
                        fprintf(stdout, "  ClientToken: %s\n", event->ClientToken->c_str());
                    }
                }
                else
                {
                    fprintf(stdout, "Delta did not report a change in \"%s\".\n", cmdData.input_shadowProperty.c_str());
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
                currentShadowValue = response->State->Reported->View().GetString(cmdData.input_shadowProperty);
            }
            else
            {
                fprintf(stdout, "Finished clearing shadow properties\n");
                currentShadowValue = "";
            }

            if (cmdData.input_isCI == false)
            {
                fprintf(stdout, "Enter Desired state of %s:\n", cmdData.input_shadowProperty.c_str());
            }
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
        shadowDeltaUpdatedRequest.ThingName = cmdData.input_thingName;

        shadowClient.SubscribeToShadowDeltaUpdatedEvents(
            shadowDeltaUpdatedRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onDeltaUpdated, onDeltaUpdatedSubAck);

        UpdateShadowSubscriptionRequest updateShadowSubscriptionRequest;
        updateShadowSubscriptionRequest.ThingName = cmdData.input_thingName;

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

        /********************** Shadow Value Get ********************/
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
                if (response->State && response->State->Reported->View().ValueExists(cmdData.input_shadowProperty))
                {
                    JsonView objectView = response->State->Reported->View().GetJsonObject(cmdData.input_shadowProperty);
                    if (objectView.IsNull())
                    {
                        fprintf(stdout, "Shadow contains \"%s\" but is null.\n", cmdData.input_shadowProperty.c_str());
                        currentShadowValue = "";
                    }
                    else
                    {
                        currentShadowValue = response->State->Reported->View().GetString(cmdData.input_shadowProperty);
                        fprintf(
                            stdout,
                            "Shadow contains \"%s\". Updating local value to \"%s\"...\n",
                            cmdData.input_shadowProperty.c_str(),
                            currentShadowValue.c_str());
                    }
                }
                else
                {
                    fprintf(
                        stdout, "Shadow currently does not contain \"%s\".\n", cmdData.input_shadowProperty.c_str());
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
        shadowSubscriptionRequest.ThingName = cmdData.input_thingName;

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
        shadowGetRequest.ThingName = cmdData.input_thingName;

        // Get the current shadow document so we start with the correct value
        shadowClient.PublishGetShadow(shadowGetRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onGetShadowRequestSubAck);

        onGetShadowRequestCompletedPromise.get_future().wait();
        gotInitialShadowPromise.get_future().wait();

        /********************** Shadow change value input loop ********************/
        /**
         * This section is to getting user input and changing the shadow value passed to that input.
         * If in CI, then input is automatically passed
         */

        if (cmdData.input_isCI == false)
        {
            fprintf(stdout, "Enter Desired state of %s:\n", cmdData.input_shadowProperty.c_str());
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
                    fprintf(stdout, "Enter Desired state of %s:\n", cmdData.input_shadowProperty.c_str());
                }
                else
                {
                    s_changeShadowValue(shadowClient, cmdData.input_thingName, cmdData.input_shadowProperty, input);
                }
            }
        }
        else
        {
            int messagesSent = 0;
            while (messagesSent < 5)
            {
                String input = "Shadow_Value_";
                input.append(std::to_string(messagesSent).c_str());
                s_changeShadowValue(shadowClient, cmdData.input_thingName, cmdData.input_shadowProperty, input);
                // Sleep so there is a gap between shadow updates
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                messagesSent += 1;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

    // Disconnect
    if (connection->Disconnect())
    {
        connectionClosedPromise.get_future().wait();
    }
    return 0;
}
