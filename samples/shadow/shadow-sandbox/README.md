# Shadow

[**Return to main sample list**](../../README.md)

This is an interactive sample that supports a set of commands that allow you to interact with "classic" (unnamed) shadows of the AWS IoT [Device Shadow](https://docs.aws.amazon.com/iot/latest/developerguide/iot-device-shadows.html) Service.

### Commands
Once connected, the sample supports the following shadow-related commands:

* `get` - gets the current full state of the classic (unnamed) shadow.  This includes both a "desired" state component and a "reported" state component.
* `delete` - deletes the classic (unnamed) shadow completely
* `update-desired <desired-state-json-document>` - applies an update to the classic shadow's desired state component.  Properties in the JSON document set to non-null will be set to new values.  Properties in the JSON document set to null will be removed.
* `update-reported <reported-state-json-document>` - applies an update to the classic shadow's reported state component.  Properties in the JSON document set to non-null will be set to new values.  Properties in the JSON document set to null will be removed.

Two additional commands are supported:
* `help` - prints the set of supported commands
* `quit` - quits the sample application

### Prerequisites
Your IoT Core Thing's [Policy](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html) must provide privileges for this sample to connect, subscribe, publish, and receive. Below is a sample policy that can be used on your IoT Core Thing that will allow this sample to run as intended.

<details>
<summary>Sample Policy</summary>
<pre>
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Effect": "Allow",
      "Action": [
        "iot:Publish"
      ],
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/shadow/get",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/shadow/delete",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/shadow/update"
      ]
    },
    {
      "Effect": "Allow",
      "Action": [
        "iot:Receive"
      ],
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/shadow/get/*",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/shadow/delete/*",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/shadow/update/*"
      ]
    },
    {
      "Effect": "Allow",
      "Action": [
        "iot:Subscribe"
      ],
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/$aws/things/<b>thingname</b>/shadow/get/*",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/$aws/things/<b>thingname</b>/shadow/delete/*",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/$aws/things/<b>thingname</b>/shadow/update/*"
      ]
    },
    {
      "Effect": "Allow",
      "Action": "iot:Connect",
      "Resource": "arn:aws:iot:<b>region</b>:<b>account</b>:client/test-*"
    }
  ]
}
</pre>

Replace with the following with the data from your AWS account:
* `<region>`: The AWS IoT Core region where you created your AWS IoT Core thing you wish to use with this sample. For example `us-east-1`.
* `<account>`: Your AWS IoT Core account ID. This is the set of numbers in the top right next to your AWS account name when using the AWS IoT Core website.
* `<thingname>`: The name of your AWS IoT Core thing you want the device connection to be associated with

Note that in a real application, you may want to avoid the use of wildcards in your ClientID or use them selectively. Please follow best practices when working with AWS on production applications using the SDK. Also, for the purposes of this sample, please make sure your policy allows a client ID of `test-*` to connect or use `--client_id <client ID here>` to send the client ID your policy supports.

</details>

## Walkthrough

Before building and running the sample, you must first build and install the SDK:

``` sh
cd <sdk-root-directory>
mkdir _build
cd _build
cmake -DCMAKE_INSTALL_PREFIX=<sdk_install_path> ..
make && make install
```

Now build the sample:

``` sh
cd samples/shadow/shadow-sandbox
mkdir _build
cd _build
cmake -DCMAKE_PREFIX_PATH=<sdk_install_path> ..
make
```

To run the sample:

``` sh
./shadow-sandbox --endpoint <endpoint> --cert <path to the certificate> --key <path to the private key> --thing_name <thing name>
```

The sample also listens to a pair of event streams related to the classic (unnamed) shadow state of your thing, so in addition to responses, you will occasionally see output from these streaming operations as they receive events from the shadow service.

Once successfully connected, you can issue commands.

### Initialization

Start off by getting the shadow state:

```
get
```

If your thing does have shadow state, you will get its current value, which this sample has no control over.  Let's assume it was initialized
like what is described below:

```
get result: {"clientToken":"<Some UUID>","state":{"reported":{"Color":"green"}},"metadata":{"reported":{"Color":{"timestamp":1730481958}}},"timestamp":1730482166,"version":1}
```

If your thing does not have any shadow state yet, you'll get a ResourceNotFound error:

```
get failed with error code: libaws-c-mqtt: AWS_ERROR_MQTT_REQUEST_RESPONSE_MODELED_SERVICE_ERROR, Request-response operation failed with a modeled service error.
modeled error: {"clientToken":"<Some UUID>","code":404,"message":"No shadow exists with name: '<Thing Name>'"}
```

To create a shadow, you can issue an update call that will initialize the shadow to a starting state:

```
update-reported {"Color":"green"}
```

which will yield output similar to:

```
update-reported result: {"clientToken":"58de9848-e4db-e2ac-9622-d4c1197a5a14","state":{"reported":{"Color":"green"}},"metadata":{"reported":{"Color":{"timestamp":1730481958}}},"timestamp":1730481958,"version":1}

Received ShadowUpdated event:  {"current":{"state":{"reported":{"Color":"green"}},"metadata":{"reported":{"Color":{"timestamp":1730481958}}},"version":1},"timestamp":1730481958}
```

Notice that in addition to receiving a response to the update request, you also receive a `ShadowUpdated` event containing what changed about
the shadow plus additional metadata (version, update timestamps, etc...).  Every time a shadow is updated, this
event is triggered.  If you wish to listen and react to this event, use the `CreateShadowUpdatedStream` API in the shadow client to create a
streaming operation that converts the raw MQTT publish messages into modeled data that the streaming operation invokes a user-supplied callback with.

Issue one more update to get the shadow's reported and desired states in sync:

```
update-desired {"Color":"green"}
```

yielding output similar to:

```
update-desired result: {"clientToken":"7f53091e-16a7-74d9-201b-b2d45a4d1ae7","state":{"desired":{"Color":"green"}},"metadata":{"desired":{"Color":{"timestamp":1730482304}}},"timestamp":1730482304,"version":2}

<ShadowUpdated event omitted>
```

### Changing Properties
A device shadow contains two independent states: reported and desired.  "Reported" represents the device's last-known local state, while
"desired" represents the state that control application(s) would like the device to change to.  In general, each application (whether on the device or running
remotely as a control process) will only update one of these two state components.

Let's walk through the multi-step process to coordinate a change-of-state on the device.  First, a control application needs to update the shadow's desired
state with the change it would like applied:

```
update-desired {"Color":"red"}
```

For our sample, this yields output similar to:

```
update-desired result: {"clientToken":"35624091-7be2-3ab3-c193-bac8f4c4e9c8","state":{"desired":{"Color":"red"}},"metadata":{"desired":{"Color":{"timestamp":1730482794}}},"timestamp":1730482794,"version":3}

Received ShadowUpdated event:  {"previous":{"state":{"desired":{"Color":"green"},"reported":{"Color":"green"}},"metadata":{"desired":{"Color":{"timestamp":1730482304}},"reported":{"Color":{"timestamp":1730481958}}},"version":2},"current":{"state":{"desired":{"Color":"red"},"reported":{"Color":"green"}},"metadata":{"desired":{"Color":{"timestamp":1730482794}},"reported":{"Color":{"timestamp":1730481958}}},"version":3},"timestamp":1730482794}

Received ShadowDeltaUpdated event:  {"state":{"Color":"red"},"metadata":{"Color":{"timestamp":1730482794}},"timestamp":1730482794,"version":3,"clientToken":"35624091-7be2-3ab3-c193-bac8f4c4e9c8"}
```

The key thing to notice here is that in addition to the update response (which only the control application would see) and the ShadowUpdated event,
there is a new event, ShadowDeltaUpdated, which indicates properties on the shadow that are out-of-sync between desired and reported.  All out-of-sync
properties will be included in this event, including properties that became out-of-sync due to a previous update.

Like the ShadowUpdated event, ShadowDeltaUpdated events can be listened to by creating and configuring a streaming operation, this time by using
the `CreateShadowDeltaUpdatedStream` API.  Using the ShadowDeltaUpdated events (rather than ShadowUpdated) lets a device focus on just what has
changed without having to do complex JSON diffs between the desired and reported states of the shadow.

Assuming that the change expressed in the desired state is reasonable, the device should apply it internally and then let the service know it
has done so by updating the reported state of the shadow:

```
update-reported {"Color":"red"}
```

yielding

```
update-reported result: {"clientToken":"5741d710-fe6b-7f4f-ece1-7767498a38c8","state":{"reported":{"Color":"red"}},"metadata":{"reported":{"Color":{"timestamp":1730482948}}},"timestamp":1730482948,"version":4}

Received ShadowUpdated event:  {"previous":{"state":{"desired":{"Color":"red"},"reported":{"Color":"green"}},"metadata":{"desired":{"Color":{"timestamp":1730482794}},"reported":{"Color":{"timestamp":1730481958}}},"version":3},"current":{"state":{"desired":{"Color":"red"},"reported":{"Color":"red"}},"metadata":{"desired":{"Color":{"timestamp":1730482794}},"reported":{"Color":{"timestamp":1730482948}}},"version":4},"timestamp":1730482948}
```

Notice that no ShadowDeltaUpdated event is generated because the reported and desired states are now back in sync.

### Multiple Properties
Not all shadow properties represent device configuration.  To illustrate several more aspects of the Shadow service, let's add a second property to our shadow document,
starting out in sync (output omitted):

```
update-reported {"Status":"Great"}
```

```
update-desired {"Status":"Great"}
```

Notice that shadow updates work by deltas rather than by complete state changes.  Updating the "Status" property to a value had no effect on the shadow's
"Color" property:

```
get
```

yields

```
get result: {"clientToken":"2b689730-8144-c20f-07dd-60bdf4e3f2b7","state":{"desired":{"Color":"red","Status":"Great"},"reported":{"Color":"red","Status":"Great"}},"metadata":{"desired":{"Color":{"timestamp":1730482794},"Status":{"timestamp":1730483069}},"reported":{"Color":{"timestamp":1730482948},"Status":{"timestamp":1730483062}}},"timestamp":1730483086,"version":6}
```

Suppose something goes wrong with the device and its status is no longer "Great"

```
update-reported {"Status":"Awful"}
```

which yields something similar to:

```
update-reported result: {"clientToken":"af54adca-85c9-c4c9-52b0-7349337f57d5","state":{"reported":{"Status":"Awful"}},"metadata":{"reported":{"Status":{"timestamp":1730483858}}},"timestamp":1730483858,"version":7}

Received ShadowUpdated event:  {"previous":{"state":{"desired":{"Color":"red","Status":"Great"},"reported":{"Color":"red","Status":"Great"}},"metadata":{"desired":{"Color":{"timestamp":1730482794},"Status":{"timestamp":1730483069}},"reported":{"Color":{"timestamp":1730482948},"Status":{"timestamp":1730483062}}},"version":6},"current":{"state":{"desired":{"Color":"red","Status":"Great"},"reported":{"Color":"red","Status":"Awful"}},"metadata":{"desired":{"Color":{"timestamp":1730482794},"Status":{"timestamp":1730483069}},"reported":{"Color":{"timestamp":1730482948},"Status":{"timestamp":1730483858}}},"version":7},"timestamp":1730483858}

Received ShadowDeltaUpdated event:  {"state":{"Status":"Great"},"metadata":{"Status":{"timestamp":1730483069}},"timestamp":1730483858,"version":7,"clientToken":"af54adca-85c9-c4c9-52b0-7349337f57d5"}
```

Similar to how updates are delta-based, notice how the ShadowDeltaUpdated event only includes the "Status" property, leaving the "Color" property out because it
is still in sync between desired and reported.

### Removing properties
Properties can be removed from a shadow by setting them to null.  Removing a property completely would require its removal from both the
reported and desired states of the shadow (output omitted):

```
update-reported {"Status":null}
```

```
update-desired {"Status":null}
```

If you now get the shadow state:

```
get
```

its output yields something like

```
get result: {"clientToken":"2dccc7b8-47ad-80c5-299f-3de5d9b553fa","state":{"desired":{"Color":"red"},"reported":{"Color":"red"}},"metadata":{"desired":{"Color":{"timestamp":1730482794}},"reported":{"Color":{"timestamp":1730482948}}},"timestamp":1730483940,"version":9}
```

The Status property has been fully removed from the shadow state.

### Removing a shadow
To remove a shadow, you must invoke the DeleteShadow API (setting the reported and desired
states to null will only clear the states, but not delete the shadow resource itself).

```
delete
```

yields something like

```
delete result: {"clientToken":"495844bb-65c9-58a6-bd16-099fa3021512","timestamp":1730483990,"version":9}
```

Subsequent attempts to get the shadow return an error:

```
get
```

results in something similar to

```
get failed with error code: libaws-c-mqtt: AWS_ERROR_MQTT_REQUEST_RESPONSE_MODELED_SERVICE_ERROR, Request-response operation failed with a modeled service error.
modeled error: {"clientToken":"0228fa77-2bbf-bf91-9915-128668692dbb","code":404,"message":"No shadow exists with name: '<Thing Name>'"}
```