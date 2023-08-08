# Frequently Asked Questions

*__Jump To:__*
* [Where should I start](#where-should-i-start)
* [How do I enable logging](#how-do-i-enable-logging)
* [I keep getting AWS_ERROR_MQTT_UNEXPECTED_HANGUP](#i-keep-getting-aws_error_mqtt_unexpected_hangup)
* [Dependencies are bad](#dependencies-are-bad)
* [Detecting connection loss (tldr use keepAliveTimeSecs and pingTimeoutMs)](#connection-loss)
* [How to use a Pre-Built aws-crt-cpp (Most useful for development of this package)](#prebuilt-aws-crt-cpp)
* [I am experiencing deadlocks](#i-am-experiencing-deadlocks)
* [Mac-Only TLS Behavior](#mac-only-tls-behavior)
* [How do debug in VSCode?](#how-do-debug-in-vscode)
* [What certificates do I need?](#what-certificates-do-i-need)
* [I still have more questions about this sdk?](#i-still-have-more-questions-about-this-sdk)

### Where should I start?

If you are just getting started make sure you [install this sdk](https://github.com/aws/aws-iot-device-sdk-cpp-v2#installation) and then build and run the [basic PubSub](https://github.com/aws/aws-iot-device-sdk-cpp-v2/tree/main/samples#basic-mqtt-pub-sub)

### How do I enable logging?

``` c++
ApiHandle apiHandle;
apiHandle.InitializeLogging(Aws::Crt::LogLevel::Error, stderr);
```
You can also enable [CloudWatch logging](https://docs.aws.amazon.com/iot/latest/developerguide/cloud-watch-logs.html) for IoT which will provide you with additional information that is not available on the client side sdk.

### I keep getting AWS_ERROR_MQTT_UNEXPECTED_HANGUP

This could be many different things but it most likely is a policy issue. Start with using a super permissive IAM policy called AWSIOTFullAccess which looks like this:

``` json
{
    "Version": "2012-10-17",
    "Statement": [
        {
            "Effect": "Allow",
            "Action": [
                "iot:*"
            ],
            "Resource": "*"
        }
    ]
}
```

After getting it working make sure to only allow the actions and resources that you need. More info about IoT IAM policies can be found [here](https://docs.aws.amazon.com/iot/latest/developerguide/security_iam_service-with-iam.html).


### Dependencies are bad.
If you get the following Error:
```
CMake Error at CMakeLists.txt:46 (include):
 include could not find load file:
  AwsFindPackage
```
Make sure to use `--recursive` in the git clone command:
`git clone --recursive https://github.com/aws/aws-iot-device-sdk-cpp-v2.git`

If you have already downloaded this repository you can update the submodules with this command:
`git submodule update --init --recursive`


### Detecting connection loss (tldr use keepAliveTimeSecs and pingTimeoutMs) <a name="connection-loss"></a>

There are 3 mechanisms for detecting connection loss:
1. The keepAliveTimeSecs and pingTimeoutMs arguments passed to MqttConnection::Connect(). These control how often the SDK sends a PINGREQ, and how long the SDK will wait for a PINGRESP before assuming the connection is lost. YOU SHOULD USE THIS TO RELIABLY DETECT CONNECTION LOSS.
2. If the OS socket says it's closed, the SDK immediately tries to reconnect. The timing of this is not reliable, it depends on the OS and how the connection is lost, it can take many minutes.
3. The various TcpKeepAlive controls on the MqttClientConnectionConfigBuilder. These control a similar mechanism at the TCP layer, rather than the MQTT layer, but is implemented in the OS and behavior may vary across platforms


### How to use a Pre-Built aws-crt-cpp (Most useful for development of this package) <a name="prebuilt-aws-crt-cpp"></a>

``` sh
mkdir aws-iot-device-sdk-cpp-v2-build
cd aws-iot-device-sdk-cpp-v2-build
cmake -DCMAKE_INSTALL_PREFIX="<absolute path sdk-cpp-workspace dir>"  -DCMAKE_PREFIX_PATH="<absolute path sdk-cpp-workspace dir>" -DBUILD_DEPS=OFF ../aws-iot-device-sdk-cpp-v2
cmake --build . --target install
```

### I am experiencing deadlocks

You MUST NOT perform blocking operations on any callback, or you will cause a deadlock. For example: in the on_publish_received callback, do not send a publish, and then wait for the future to complete within the callback. The Client cannot do work until your callback returns, so the thread will be stuck.

### Mac-Only TLS Behavior

Please note that on Mac, once a private key is used with a certificate, that certificate-key pair is imported into the Mac Keychain.  All subsequent uses of that certificate will use the stored private key and ignore anything passed in programmatically.  Beginning in v1.7.3, when a stored private key from the Keychain is used, the following will be logged at the "info" log level:

```
static: certificate has an existing certificate-key pair that was previously imported into the Keychain.  Using key from Keychain instead of the one provided.
```

### How do debug in VSCode?

Here is an example launch.json file to run the pubsub sample
 ``` json
 {
    // Use IntelliSense to learn about possible attributes.
    // Hover to view descriptions of existing attributes.
    // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [
        {
            "name": "PubSub",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/samples/pub_sub/basic_pub_sub/build/basic-pub-sub",
            "args": [
                "--endpoint", "<account-number>-ats.iot.<region>.amazonaws.com",
                "--ca_file", "<path to root-CA>",
                "--cert", "<path to cert>",
                "--key", "<path to key>",
                "--client-id", "test-client"
            ]
        }
    ]
}
```
### What certificates do I need?

* You can download pre-generated certificates from the AWS console (this is the simplest and is recommended for testing)
* You can also generate your own certificates to fit your specific use case. You can find documentation for that [here](https://docs.aws.amazon.com/iot/latest/developerguide/x509-client-certs.html) and [here](https://iot-device-management.workshop.aws/en/provisioning-options.html)
* Certificates that you will need to run the samples
    * Root CA Certificates
        * Download the root CA certificate file that corresponds to the type of data endpoint and cipher suite you're using (You most likely want Amazon Root CA 1)
        * Generated and provided by Amazon. You can download it [here](https://www.amazontrust.com/repository/) or download it when getting the other certificates from the AWS console
        * When using samples it can look like this: `--ca_file root-CA.crt`
    * Device certificate
        * Intermediate device certificate that is used to generate the key below
        * When using samples it can look like this: `--cert abcde12345-certificate.pem.crt`
    * Key files
        * You should have generated/downloaded private and public keys that will be used to verify that communications are coming from you
        * When using samples you only need the private key and it will look like this: `--key abcde12345-private.pem.key`

### I still have more questions about this sdk?

* [Here](https://docs.aws.amazon.com/iot/latest/developerguide/what-is-aws-iot.html) are the AWS IoT Core docs for more details about IoT Core
* [Here](https://docs.aws.amazon.com/greengrass/v2/developerguide/what-is-iot-greengrass.html) are the AWS IoT Greengrass v2 docs for more details about greengrass
* [Discussion](https://github.com/aws/aws-iot-device-sdk-cpp-v2/discussions) questions are also a great way to ask other questions about this sdk.
* [Open an issue](https://github.com/aws/aws-iot-device-sdk-cpp-v2/issues) if you find a bug or have a feature request
* [Breif MQTT CONCEPT](./MQTT_CONCEPT.md)
* [MQTT5 User Guide](./MQTT5_Userguide.md)
