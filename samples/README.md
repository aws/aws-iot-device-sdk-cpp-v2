# Sample for the AWS IoT Device SDK v2 for C++
This directory contains sample applications for [aws-iot-device-sdk-cpp-v2](../README.md).

### Table of Contents
* [Samples](#samples)
    * [MQTT5 Client Samples](#mqtt5-client-samples)
    * [Service Client Samples](#service-client-samples)
    * [Greengrass Samples](#greengrass-samples)
* [Instructions](#instructions)
* [Sample Help](#sample-help)
* [Enable Logging](#enable-logging)


## Samples
### MQTT5 Client Samples
##### MQTT5 is the recommended MQTT Client. Additional information and usage instructions can be found in the [MQTT5 User Guide](../documents/MQTT5_Userguide.md). The samples below will create an MQTT5 client, connect using the selected method, subscribe to a topic, publish to the topic, and then disconnect.
| MQTT5 Client Sample | Description |
|--------|-------------|
| [X509-based mutual TLS](./mqtt/mqtt5_x509/README.md) | Demonstrates connecting to AWS IoT Core using X.509 certificates and private keys.
| [Websockets with Sigv4 authentication](./mqtt/mqtt5_aws_websocket/README.md) | Shows how to authenticate over websockets using AWS Signature Version 4 credentials. |
| [AWS Custom Authorizer Lambda Function](./mqtt/mqtt5_custom_auth_signed/README.md) | Examples of connecting with a signed and unsigned Lambda-backed custom authorizer.
| [PKCS11](./mqtt/mqtt5_pkcs11/README.md) | Demonstrates connecting using a hardware security module (HSM) or smartcard with PKCS#11. |
| [Other Connection Methods](../documents/MQTT5_Userguide.md#connecting-to-aws-iot-core) | More connection methods are available for review in the MQTT5 Userguide

### Service Client Samples
##### AWS offers a number of IoT related services using MQTT. The samples below demonstrate how to use the service clients provided by the SDK to interact with those services.
| Service Client Sample | Description |
|--------|-------------|
| [Shadow](./service_clients/shadow/shadow-sandbox/README.md) | Manage and sync device state using the IoT Device Shadow service. |
| [Jobs](./service_clients/jobs/jobs-sandbox/README.md) | Receive and execute remote operations sent from the Jobs service. |
| [Basic Fleet Provisioning](./service_clients/fleet_provisioning/provision-basic/README.md) | Provision a device using the Fleet Provisioning template. |
| [CSR Fleet Provisioning](./service_clients/fleet_provisioning/provision-csr/README.md) | Demonstrates CSR-based device certificate provisioning. |
| [Commands](./service_clients/commands/commands-sandbox/README.md) | Receive and process remote instructions using AWS IoT Device Management commands |


### Greengrass Samples
##### Samples that interact with [AWS Greengrass](https://aws.amazon.com/greengrass/).
| Greengrass Sample | Description |
|--------|-------------|
| [Greengrass Discovery](./greengrass/basic_discovery/README.md) | Discover and connect to a local Greengrass core. |
| [Greengrass IPC](./greengrass/ipc/README.md) | Demonstrates Inter-Process Communication (IPC) with Greengrass components. |

### Others
##### Samples that interact with other AWS IoT Services
| Sample | Description |
|--------|-------------|
| [Device Defender](./others/device_defender/mqtt5_basic_report/README.md) | Monitor the health of your IoT device using AWS IoT Device Defender. |
| [Secure Tunneling](./others/secure_tunneling/secure_tunnel/README.md) | Connect a destination or a source Secure Tunnel Client to an AWS IoT Secure Tunnel endpoint. |
| [Secure Tunneling Notification](./others/secure_tunneling/tunnel_notification/README.md) | Receive a tunnel notification using a Secure Tunnel Client. |



## Instructions

First build and install aws-iot-devices-sdk-cpp-v2 with following instructions from [Installation](../README.md#Installation).

### Build individual sample

Change directory into one of the samples. Under the directory of the sample you wish to build, run the following commands:

``` sh
cmake -B build -S . -DCMAKE_PREFIX_PATH="<absolute path sdk-cpp-workspace dir>" -DCMAKE_BUILD_TYPE="<Release|RelWithDebInfo|Debug>" .
cmake --build build --config "<Release|RelWithDebInfo|Debug>"
```

For CMake versions that do not support the `-B` command, go to the directory of the sample you wish to build and run the following commands:

``` sh
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH="<absolute path sdk-cpp-workspace dir>" -DCMAKE_BUILD_TYPE="<Release|RelWithDebInfo|Debug>" ..
cmake --build . --config "<Release|RelWithDebInfo|Debug>"
```

### Build all samples

Change directory to the `aws-iot-device-sdk-cpp-v2/samples` directory and then run the following commands:

```sh
cmake -B build -S . -DCMAKE_PREFIX_PATH="<absolute path sdk-cpp-workspace dir>" -DCMAKE_BUILD_TYPE="<Release|RelWithDebInfo|Debug>"
cmake --build build --config "<Release|RelWithDebInfo|Debug>"
```

This will compile all of the samples at once. You can then find the samples in the `aws-iot-device-sdk-cpp-v2/samples/build` folder. For example, the MQTT5 PubSub sample will be located at `aws-iot-device-sdk-cpp-v2/samples/build/mqtt/mqtt5_x509`.

For CMake versions that do not support the `-B` command, go to the `aws-iot-device-sdk-cpp-v2/samples` directory and run the following commands:

``` sh
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH="<absolute path sdk-cpp-workspace dir>" -DCMAKE_BUILD_TYPE="<Release|RelWithDebInfo|Debug>" ..
cmake --build . --config "<Release|RelWithDebInfo|Debug>"
```

Note that building all the samples at once is currently only available in the V2 C++ IoT SDK at this time.

### Sample Build Notes

* `-DCMAKE_PREFIX_PATH` needs to be set to the path aws-iot-device-sdk-cpp-v2 installed at. Since [Installation](../README.md#Installation) takes `sdk-cpp-workspace` as an example, this file uses that example too.

* `-DCMAKE_BUILD_TYPE` and `--config` needs to match the `CMAKE_BUILD_TYPE` when aws-iot-device-sdk-cpp-v2 built. `--config` is only REQUIRED for multi-configuration build tools.

## Sample help

All samples will show their options and arguments by passing in `--help`. For example:

```sh
./build/mqtt/mqtt5_x509/mqtt5_x509 --help
```
will result in the following print output:
```
MQTT5 X509 Sample (mTLS)
options:
  --help        show this help message and exit
required arguments:
  --endpoint    IoT endpoint hostname
  --cert        Path to the certificate file to use during mTLS connection establishment
  --key         Path to the private key file to use during mTLS connection establishment
optional arguments:
  --client_id   Client ID (default: mqtt5-sample-<uuid>)
  --topic       Topic (default: test/topic)
  --message     Message payload (default: Hello from mqtt5 sample)
  --count       Messages to publish (0 = infinite) (default: 5)
```
The sample will not run without the required arguments and will notify you of missing arguments.

## Enable logging in samples

Instructions to enable logging are available in the [FAQ](../documents/FAQ.md) under [How do I enable logging](../documents/FAQ.md#how-do-i-enable-logging).