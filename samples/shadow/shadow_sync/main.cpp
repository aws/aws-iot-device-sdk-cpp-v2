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

#include <algorithm>
#include <condition_variable>
#include <iostream>
#include <mutex>

using namespace Aws::Crt;
using namespace Aws::Iotshadow;

static const char *SHADOW_VALUE_DEFAULT = "off";

static void s_printHelp()
{
    fprintf(stdout, "Usage:\n");
    fprintf(
        stdout,
        "shadow-sync --endpoint <endpoint> --cert <path to cert>"
        " --key <path to key> --ca_file <optional: path to custom ca>"
        " --thing_name <thing name> --shadow_property <Name of property in shadow to keep in sync.>\n\n");
    fprintf(stdout, "endpoint: the endpoint of the mqtt server not including a port\n");
    fprintf(stdout, "cert: path to your client certificate in PEM format\n");
    fprintf(stdout, "key: path to your key in PEM format\n");
    fprintf(
        stdout,
        "ca_file: Optional, if the mqtt server uses a certificate that's not already"
        " in your trust store, set this.\n");
    fprintf(stdout, "\tIt's the path to a CA file in PEM format\n");
    fprintf(stdout, "thing_name: the name of your IOT thing\n");
    fprintf(stdout, "shadow_property: The name of the shadow property you want to change.\n");
}

static bool s_cmdOptionExists(char **begin, char **end, const String &option)
{
    return std::find(begin, end, option) != end;
}

static char *s_getCmdOption(char **begin, char **end, const String &option)
{
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

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
            fprintf(stderr, "failed to update %s shadow state: error %s\n", thingName.c_str(), ErrorDebugString(ioErr));
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

    String endpoint;
    String certificatePath;
    String keyPath;
    String caFile;
    String thingName;
    String shadowProperty;
    String clientId(String("test-") + Aws::Crt::UUID().ToString());

    /*********************** Parse Arguments ***************************/
    if (!(s_cmdOptionExists(argv, argv + argc, "--endpoint") && s_cmdOptionExists(argv, argv + argc, "--cert") &&
          s_cmdOptionExists(argv, argv + argc, "--key") && s_cmdOptionExists(argv, argv + argc, "--thing_name") &&
          s_cmdOptionExists(argv, argv + argc, "--shadow_property")))
    {
        s_printHelp();
        return 0;
    }

    endpoint = s_getCmdOption(argv, argv + argc, "--endpoint");
    certificatePath = s_getCmdOption(argv, argv + argc, "--cert");
    keyPath = s_getCmdOption(argv, argv + argc, "--key");
    thingName = s_getCmdOption(argv, argv + argc, "--thing_name");
    shadowProperty = s_getCmdOption(argv, argv + argc, "--shadow_property");

    if (s_cmdOptionExists(argv, argv + argc, "--ca_file"))
    {
        caFile = s_getCmdOption(argv, argv + argc, "--ca_file");
    }

    /********************** Now Setup an Mqtt Client ******************/
    if (apiHandle.GetOrCreateStaticDefaultClientBootstrap()->LastError() != AWS_ERROR_SUCCESS)
    {
        fprintf(
            stderr,
            "ClientBootstrap failed with error %s\n",
            ErrorDebugString(apiHandle.GetOrCreateStaticDefaultClientBootstrap()->LastError()));
        exit(-1);
    }

    auto clientConfigBuilder = Aws::Iot::MqttClientConnectionConfigBuilder(certificatePath.c_str(), keyPath.c_str());
    clientConfigBuilder.WithEndpoint(endpoint);
    if (!caFile.empty())
    {
        clientConfigBuilder.WithCertificateAuthority(caFile.c_str());
    }
    auto clientConfig = clientConfigBuilder.Build();

    if (!clientConfig)
    {
        fprintf(
            stderr,
            "Client Configuration initialization failed with error %s\n",
            ErrorDebugString(clientConfig.LastError()));
        exit(-1);
    }

    /*
     * Now Create a client. This can not throw.
     * An instance of a client must outlive its connections.
     * It is the users responsibility to make sure of this.
     */
    Aws::Iot::MqttClient mqttClient;

    /*
     * Since no exceptions are used, always check the bool operator
     * when an error could have occurred.
     */
    if (!mqttClient)
    {
        fprintf(stderr, "MQTT Client Creation failed with error %s\n", ErrorDebugString(mqttClient.LastError()));
        exit(-1);
    }
    /*
     * Now create a connection object. Note: This type is move only
     * and its underlying memory is managed by the client.
     */
    auto connection = mqttClient.NewConnection(clientConfig);

    if (!*connection)
    {
        fprintf(stderr, "MQTT Connection Creation failed with error %s\n", ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    /*
     * In a real world application you probably don't want to enforce synchronous behavior
     * but this is a sample console application, so we'll just do that with a condition variable.
     */
    std::promise<bool> connectionCompletedPromise;
    std::promise<void> connectionClosedPromise;

    /*
     * This will execute when an mqtt connect has completed or failed.
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
                }
                else
                {
                    fprintf(stdout, "Delta did not report a change in \"%s\".\n", shadowProperty.c_str());
                }
            }

            if (ioErr)
            {
                fprintf(stdout, "Error processing shadow delta: %s\n", ErrorDebugString(ioErr));
                exit(-1);
            }
        };

        auto onUpdateShadowAccepted = [&](UpdateShadowResponse *response, int ioErr) {
            if (ioErr == AWS_OP_SUCCESS)
            {
                if (response->State->Reported)
                {
                    fprintf(
                        stdout,
                        "Finished updating reported shadow value to %s.\n",
                        response->State->Reported->View().GetString(shadowProperty).c_str());
                }
                else
                {
                    fprintf(stdout, "Finished clearing shadow properties\n");
                }
                fprintf(stdout, "Enter desired value:\n");
            }
            else
            {
                fprintf(stderr, "Error on subscription: %s.\n", ErrorDebugString(ioErr));
                exit(-1);
            }
        };

        auto onUpdateShadowRejected = [&](ErrorResponse *error, int ioErr) {
            if (ioErr == AWS_OP_SUCCESS)
            {
                fprintf(
                    stdout,
                    "Update of shadow state failed with message %s and code %d.",
                    error->Message->c_str(),
                    *error->Code);
            }
            else
            {
                fprintf(stderr, "Error on subscription: %s.\n", ErrorDebugString(ioErr));
                exit(-1);
            }
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

        while (true)
        {
            fprintf(stdout, "Enter Desired state of %s:\n", shadowProperty.c_str());
            String input;
            std::cin >> input;

            if (input == "exit" || input == "quit")
            {
                fprintf(stdout, "Exiting...");
                break;
            }

            s_changeShadowValue(shadowClient, thingName, shadowProperty, input);
        }
    }

    /* Disconnect */
    if (connection->Disconnect())
    {
        connectionClosedPromise.get_future().wait();
    }

    return 0;
}
