# Frequently Asked Questions

### Where should I start?

If you are just getting started make sure you [install this sdk](https://github.com/aws/aws-iot-device-sdk-cpp-v2#installation) and then build and run the [basic PubSub](https://github.com/aws/aws-iot-device-sdk-cpp-v2/tree/main/samples#basic-mqtt-pub-sub)


### I keep getting AWS_ERROR_MQTT_UNEXPECTED_HANGUP

This could be many different things but it most likely is a policy issue. Start with using a super permissive IAM policy called AWSIOTFullAccess which looks like this:

```
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


### Could NOT find LibCrypto (missing: LibCrypto_LIBRARY)

Add `-DBUILD_SHARED_LIBS=ON` to your first cmake command


### Depenedencies are bad.
If you get the following Error:
```
CMake Error at CMakeLists.txt:46 (include):
 include could not find load file:
  AwsFindPackage
```
Make sure use `--recursive` in the git clone command
`git clone --recursive https://github.com/aws/aws-iot-device-sdk-cpp-v2.git`


### Connection Issues (tldr use keepAliveTimeSecs and pingTimeoutMs)

There are 3 mechanisms for detecting connection loss:
1. The keepAliveTimeSecs and pingTimeoutMs arguments passed to MqttConnection::Connect(). These control how often the SDK sends a PINGREQ, and how long the SDK will wait for a PINGRESP before assuming the connection is lost. YOU SHOULD USE THIS TO RELIABLY DETECT CONNECTION LOSS.
2. If the OS socket says it's closed, the SDK immediately tries to reconnect. The timing of this is not reliable, it depends on the OS and how the connection is lost, it can take many minutes.
3. The various TcpKeepAlive controls on the MqttClientConnectionConfigBuilder. These control a similar mechanism at the TCP layer, rather than the MQTT layer, but is implemented in the OS and behavior may vary across platforms


### How to use a Pre-Built aws-crt-cpp (Most useful for development of this package)

``` sh
mkdir aws-iot-device-sdk-cpp-v2-build
cd aws-iot-device-sdk-cpp-v2-build
cmake -DCMAKE_INSTALL_PREFIX="<absolute path sdk-cpp-workspace dir>"  -DCMAKE_PREFIX_PATH="<absolute path sdk-cpp-workspace dir>" -DBUILD_DEPS=OFF ../aws-iot-device-sdk-cpp-v2
cmake --build . --target install
```


### Mac-Only TLS Behavior

Please note that on Mac, once a private key is used with a certificate, that certificate-key pair is imported into the Mac Keychain.  All subsequent uses of that certificate will use the stored private key and ignore anything passed in programmatically.  Beginning in v1.7.3, when a stored private key from the Keychain is used, the following will be logged at the "info" log level:

```
static: certificate has an existing certificate-key pair that was previously imported into the Keychain.  Using key from Keychain instead of the one provided.
```
