# AWS IoT SDK for C++ v2

Next generation AWS IoT Client SDK for C++.

This project is in **GENERAL AVAILABILITY**. If you have any issues or feature
requests, please file an issue or pull request.

This SDK is built on the AWS Common Runtime, a collection of libraries
([aws-c-common](https://github.com/awslabs/aws-c-common),
[aws-c-io](https://github.com/awslabs/aws-c-io),
[aws-c-mqtt](https://github.com/awslabs/aws-c-mqtt),
[aws-c-http](https://github.com/awslabs/aws-c-http),
[aws-c-cal](https://github.com/awslabs/aws-c-cal),
[aws-c-auth](https://github.com/awslabs/aws-c-auth),
[s2n](https://github.com/awslabs/s2n)...) written in C to be
cross-platform, high-performance, secure, and reliable. The libraries are bound
to C++ by the [aws-crt-cpp](https://github.com/awslabs/aws-crt-cpp) package.

Integration with AWS IoT Services such as
[Device Shadow](https://docs.aws.amazon.com/iot/latest/developerguide/iot-device-shadows.html)
and [Jobs](https://docs.aws.amazon.com/iot/latest/developerguide/iot-jobs.html)
is provided by code that been generated from a model of the service.

# Installation
## Minimum Requirements
*   C++ 11 or higher
*   CMake 3.1+
*   Clang 3.9+ or GCC 4.4+ or MSVC 2015+

## Build from source
### Automatically Build and Install AWS Dependencies
```
git clone --recursive https://github.com/awslabs/aws-iot-device-sdk-cpp-v2.git
mkdir aws-iot-device-sdk-cpp-v2-build
cd aws-iot-device-sdk-cpp-v2-build
cmake -DCMAKE_INSTALL_PREFIX="<path to where you install>"  -DBUILD_DEPS=ON ../aws-iot-device-sdk-cpp-v2
cmake --build . --target install
```
### Using a Pre-Built aws-crt-cpp (Most useful for development of this package)

```
mkdir aws-iot-device-sdk-cpp-v2-build
cd aws-iot-device-sdk-cpp-v2-build
cmake -DCMAKE_INSTALL_PREFIX="<path to where you install>"  -DCMAKE_PREFIX_PATH="<path to where you install>" -DBUILD_DEPS=OFF ../aws-iot-device-sdk-cpp-v2
cmake --build . --target install
```
# Samples

## Basic MQTT Pub-Sub

This sample uses the
[Message Broker](https://docs.aws.amazon.com/iot/latest/developerguide/iot-message-broker.html)
for AWS IoT to send and receive messages through an MQTT connection.
On startup, the device connects to the server and subscribes to a topic.

The terminal prompts the user for input. Type something and press enter to publish a message to the topic.
Since the sample is subscribed to the same topic, it will also receive the message back from the server.
Type `quit` and press enter to end the sample.

Source: `samples/mqtt/basic_pub_sub`

Your Thing's
[Policy](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html)
must provide privileges for this sample to connect, subscribe, publish,
and receive.

```json
{
    "Effect": "Allow",
    "Action": [
        "iot:Receive",
        "iot:Publish"
    ],
    "Resource": [
        "arn:aws:iot:<your-region>:<your-id>:topic/a/b"
    ],
},
{
    "Effect": "Allow",
    "Action": [
        "iot:Subscribe"
    ],
    "Resource": [
        "arn:aws:iot:<your-region>:<your-id>:topicfilter/a/b"
    ]
}
```

## Raw MQTT Pub-Sub
This sample is similar to the Basic Pub-Sub, but the connection setup is more manual.
This is a starting point for using custom
[Configurable Endpoints](https://docs.aws.amazon.com/iot/latest/developerguide/iot-custom-endpoints-configurable.html).

source: `samples/mqtt/raw_pub_sub`


## Shadow

This sample uses the AWS IoT
[Device Shadow](https://docs.aws.amazon.com/iot/latest/developerguide/iot-device-shadows.html)
Service to keep a property in
sync between device and server. Imagine a light whose color may be changed
through an app, or set by a local user.

Once connected, type a value in the terminal and press Enter to update
the property's "reported" value. The sample also responds when the "desired"
value changes on the server. To observe this, edit the Shadow document in
the AWS Console and set a new "desired" value.

On startup, the sample requests the shadow document to learn the property's
initial state. The sample also subscribes to "delta" events from the server,
which are sent when a property's "desired" value differs from its "reported"
value. When the sample learns of a new desired value, that value is changed
on the device and an update is sent to the server with the new "reported"
value.

Source: `samples/shadow/shadow_sync`

Your Thing's
[Policy](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html)
must provide privileges for this sample to connect, subscribe, publish,
and receive.

<details>
<summary>(see sample policy)</summary>
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
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/shadow/update"
      ]
    },
    {
      "Effect": "Allow",
      "Action": [
        "iot:Receive"
      ],
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/shadow/get/accepted",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/shadow/get/rejected",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/shadow/update/accepted",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/shadow/update/rejected",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/shadow/update/delta"
      ]
    },
    {
      "Effect": "Allow",
      "Action": [
        "iot:Subscribe"
      ],
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/$aws/things/<b>thingname</b>/shadow/get/accepted",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/$aws/things/<b>thingname</b>/shadow/get/rejected",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/$aws/things/<b>thingname</b>/shadow/update/accepted",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/$aws/things/<b>thingname</b>/shadow/update/rejected",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/$aws/things/<b>thingname</b>/shadow/update/delta"
      ]
    },
    {
      "Effect": "Allow",
      "Action": "iot:Connect",
      "Resource": "arn:aws:iot:<b>region</b>:<b>account</b>:client/samples-client-id"
    }
  ]
}
</pre>
</details>

## Jobs

This sample uses the AWS IoT
[Jobs](https://docs.aws.amazon.com/iot/latest/developerguide/iot-jobs.html)
Service to describe jobs to execute.

This sample requires you to create jobs for your device to execute. See
[instructions here](https://docs.aws.amazon.com/iot/latest/developerguide/create-manage-jobs.html).

On startup, the sample describes a job that is pending execution.

Source: `samples/jobs/describe_job_execution`

Your Thing's
[Policy](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html)
must provide privileges for this sample to connect, subscribe, publish,
and receive.
<details>
<summary>(see sample policy)</summary>
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
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/jobs/start-next",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/jobs/*/update"
      ]
    },
    {
      "Effect": "Allow",
      "Action": [
        "iot:Receive"
      ],
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/jobs/notify-next",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/jobs/start-next/accepted",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/jobs/start-next/rejected",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/jobs/*/update/accepted",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/jobs/*/update/rejected"
      ]
    },
    {
      "Effect": "Allow",
      "Action": [
        "iot:Subscribe"
      ],
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/$aws/things/<b>thingname</b>/jobs/notify-next",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/$aws/things/<b>thingname</b>/jobs/start-next/accepted",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/$aws/things/<b>thingname</b>/jobs/start-next/rejected",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/$aws/things/<b>thingname</b>/jobs/*/update/accepted",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/$aws/things/<b>thingname</b>/jobs/*/update/rejected"
      ]
    },
    {
      "Effect": "Allow",
      "Action": "iot:Connect",
      "Resource": "arn:aws:iot:<b>region</b>:<b>account</b>:client/samples-client-id"
    }
  ]
}
</pre>
</details>

## Greengrass Discovery
This sample is intended for direct usage with the Greengrass tutorial found [here](https://docs.aws.amazon.com/greengrass/latest/developerguide/gg-gs.html).

# License

This library is licensed under the Apache 2.0 License.
