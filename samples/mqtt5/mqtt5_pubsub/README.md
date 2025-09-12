# MQTT5 X509

[**Return to main sample list**](../../README.md)

This sample uses the
[Message Broker](https://docs.aws.amazon.com/iot/latest/developerguide/iot-message-broker.html)
for AWS IoT to send and receive messages through an MQTT connection using MQTT5.

MQTT5 introduces additional features and enhancements that improve the development experience with MQTT. You can read more about MQTT5 in the C++ V2 SDK by checking out the [MQTT5 user guide](../../../documents/MQTT5_Userguide.md).

Your IoT Core Thing's [Policy](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html) must provide privileges for this sample to connect, subscribe, publish, and receive. Below is a sample policy that can be used on your IoT Core Thing that will allow this sample to run as intended.

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

Note that in a real application, you may want to avoid the use of wildcards in your ClientID or use them selectively. Please follow best practices when working with AWS on production applications using the SDK. Also, for the purposes of this sample, please make sure your policy allows a client ID of `test-*` to connect or use `--client_id <client ID here>` to send the client ID your policy supports.

</details>

## How to build
**Build the SDK**

Follow the instruction on https://github.com/aws/aws-iot-device-sdk-cpp-v2/blob/main/README.md#installation

**Build the sample**

Change directory into the samples, and build the sample
```sh
# If you followed the build instruction above, you would use the path to `sdk-workspace` folder for `CMAKE_PREFIX_PATH`
cd samples/mqtt/mqtt5_x509/
cmake -B build -S . -DCMAKE_PREFIX_PATH="<absolute path sdk-workspace dir>" -DCMAKE_BUILD_TYPE="Debug" .
cmake --build build --config "Debug"
```

## How to run

To Run this sample using a direct MQTT connection with a key and certificate, use the following command:

``` sh
./mqtt5_x509  --endpoint <endpoint> --cert <path to the certificate> --key <path to the private key>
```

If you would like to see what optional arguments are available, use the `--help` argument:
```sh
./mqtt5_x509 --help
```
will result in the following output
```
MQTT5 X509 Sample (mTLS)
options:
  --help        show this help message and exit
required arguments:
  --endpoint    IoT endpoint hostname (default: None)
  --cert        Path to the certificate file to use during mTLS connection establishment (default: None)
  --key         Path to the private key file to use during mTLS connection establishment (default: None)
optional arguments:
  --client_id   Client ID (default: mqtt5-sample-<uuid>)
  --ca_file     Path to optional CA bundle (PEM) (default: None)
  --topic       Topic (default: test/topic)
  --message     Message payload (default: Hello from mqtt5 sample)
  --count       Messages to publish (0 = infinite) (default: 5)
```
The sample will not run without the required arguments and will notify you of missing arguments.

## Additional Information
Additional help with the MQTT5 Client can be found in the [MQTT5 Userguide](../../../documents/MQTT5_Userguide.md). This guide will provide more details on MQTT5 [operations](../../../documents/MQTT5_Userguide.md#client-operations), [lifecycle events](../../documents/MQTT5_Userguide.md#client-lifecycle-management), [connection methods](../../../documents/MQTT5_Userguide.md#connecting-to-aws-iot-core), and other useful information.