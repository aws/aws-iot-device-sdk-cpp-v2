# MQTT5 Custom Authorizer Signed PubSub

[**Return to main sample list**](../../README.md)

*__Jump To:__*
* [Introduction](#introduction)
* [Requirements](#requirements)
* [How To Build](#how-to-build)
* [How To Run](#how-to-run)
* [Additional Information](#additional-information)

## Introduction
The Custom Authorizer Signed sample illustrate how to connect to the [AWS IoT Message Broker](https://docs.aws.amazon.com/iot/latest/developerguide/iot-message-broker.html) with the MQTT5 Client by authenticating with a signed [Custom Authorizer Lambda Function](https://docs.aws.amazon.com/iot/latest/developerguide/custom-auth-tutorial.html)

You can read more about MQTT5 for the CPP IoT Device SDK V2 in the [MQTT5 user guide](../../../documents/MQTT5_Userguide.md).

## Requirements

You will need to setup your Custom Authorizer so the Lambda function returns a policy document. See [this page on the documentation](https://docs.aws.amazon.com/iot/latest/developerguide/config-custom-auth.html) for more details and example return result. You can customize this lambda function as needed for your application to provide your own security measures based on the needs of your application.

The policy [Policy](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html) provided by your Custom Authorizer Lambda must provide iot connect, subscribe, publish, and receive privileges for this sample to run successfully.

Below is a sample policy that provides the necessary privileges.

<details>
<summary>(see sample policy)</summary>
<pre>
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Effect": "Allow",
      "Action": [
        "iot:Publish",
        "iot:Receive"
      ],
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/test/topic"
      ]
    },
    {
      "Effect": "Allow",
      "Action": [
        "iot:Subscribe"
      ],
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/test/topic"
      ]
    },
    {
      "Effect": "Allow",
      "Action": [
        "iot:Connect"
      ],
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:client/mqtt5-sample-*"
      ]
    }
  ]
}
</pre>

Replace with the following with the data from your AWS account:
* `<region>`: The AWS IoT Core region where you created your AWS IoT Core thing you wish to use with this sample. For example `us-east-1`.
* `<account>`: Your AWS IoT Core account ID. This is the set of numbers in the top right next to your AWS account name when using the AWS IoT Core website.

Note that in a real application, you may want to avoid the use of wildcards in your ClientID or use them selectively. Please follow best practices when working with AWS on production applications using the SDK. Also, for the purposes of this sample, please make sure your policy allows a client ID of `mqtt5-sample-*` to connect or use `--client_id <client ID here>` to send the client ID your policy supports.

</details>

## How to build

To build the sample, change directory into the sample's folder and run the cmake commands. The sample executable will be built into the `samples/mqtt/mqtt5_custom_auth_signed/build` folder.
```sh
cd samples/mqtt/mqtt5_custom_auth_signed/
# If you followed the SDK build instruction, you would use the path to `sdk-workspace` folder for `CMAKE_PREFIX_PATH` here
cmake -B build -S . -DCMAKE_PREFIX_PATH="<absolute path sdk-workspace dir>" -DCMAKE_BUILD_TYPE="Debug" .
cmake --build build --config "Debug"
```

## How to run

To run this sample, navigate to the build directory where the executable was created:

```sh
# From samples/mqtt/mqtt5_custom_auth_signed/, go to the build directory
cd build
# For a signed custom authorizer
./mqtt5_custom_auth_signed \
    --endpoint <AWS IoT endpoint> \
    --authorizer_name <The name of the custom authorizer to connect to invoke> \
    --auth_token_key_name <Authorizer token key name> \
    --auth_token_key_value <Authorizer token key value> \
    --auth_signature <Custom authorizer signature> \
    --auth_username <The name to send when connecting through the custom authorizer> \
    --auth_password <The password to send when connecting through a custom authorizer>
```

If you would like to see what optional arguments are available, use the `--help` argument:
```sh
./mqtt5_custom_auth_signed --help
```

will result in the following output:
```
MQTT5 Signed Custom Authorizer Sample
options:
  --help        show this help message and exit
required arguments:
  --endpoint    IoT endpoint hostname
  --authorizer_name    The name of the custom authorizer to connect to invoke
  --auth_signature     Custom authorizer signature
  --auth_token_key_name    Authorizer token key name
  --auth_token_key_value   Authorizer token key value
  --auth_username      The name to send when connecting through the custom authorizer
  --auth_password      The password to send when connecting through a custom authorizer
optional arguments:
  --client_id   Client ID (default: mqtt5-sample-<uuid>)
  --topic       Topic (default: test/topic)
  --message     Message payload (default: Hello from mqtt5 sample)
  --count       Messages to publish (0 = infinite) (default: 5)
```
The sample will not run without the required arguments and will notify you of missing arguments.

## Additional Information
Additional help with the MQTT5 Client can be found in the [MQTT5 Userguide](../../../documents/MQTT5_Userguide.md). This guide will provide more details on MQTT5 [operations](../../../documents/MQTT5_Userguide.md#client-operations), [lifecycle events](../../../documents/MQTT5_Userguide.md#client-lifecycle-management), [connection methods](../../../documents/MQTT5_Userguide.md#connecting-to-aws-iot-core), and other useful information.
