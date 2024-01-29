# Sample apps for the AWS IoT Device SDK for C++ v2

## MQTT5 Samples
#### MQTT5 is the recommended MQTT Client. It has many benefits over MQTT311 outlined in the [MQTT5 User Guide](../documents/MQTT5_Userguide.md)
* [Mqtt5 Pub-Sub](./mqtt5/mqtt5_pubsub/README.md)
    + [Direct MQTT with X509-based mutual TLS](./mqtt5/mqtt5_pubsub/README.md#direct-mqtt-with-x509-based-mutual-tls)
    + [MQTT over Websockets with Sigv4 authentication](./mqtt5/mqtt5_pubsub/README.md#mqtt-over-websockets-with-sigv4-authentication)
    + [Direct MQTT with Custom Authentication](./mqtt5/mqtt5_pubsub/README.md#direct-mqtt-with-custom-authentication)
    + [MQTT over Websockets with Cognito](./mqtt5/mqtt5_pubsub/README.md#mqtt-over-websockets-with-cognito)
    + [HTTP Proxy](./mqtt5/mqtt5_pubsub/README.md#http-proxy)
* [Mqtt5 Shared Subscription](./mqtt5/mqtt5_shared_subscription/README.md)
* [Mqtt5 Jobs](./jobs/mqtt5_job_execution/README.md)
* [Mqtt5 Shadow](./shadow/mqtt5_shadow_sync/README.md)
* [Mqtt5 Fleet Provisioning](./fleet_provisioning/mqtt5_fleet_provisioning/README.md)
## MQTT311 Samples
* [Basic Pub-Sub](./pub_sub/basic_pub_sub/README.md)
* [Basic Connect](./mqtt/basic_connect/README.md)
* [Websocket Connect](./mqtt/websocket_connect/README.md)
* [PKCS#11 Connect](./mqtt/pkcs11_connect/README.md)
* [PKCS#12 Connect](./mqtt/pkcs12_connect/README.md)
* [x509 Credentials Provider Connect](./mqtt/x509_credentials_provider_connect/README.md)
* [Windows Certificate MQTT Connect](./mqtt/windows_cert_connect/README.md)
* [Custom Authorizer Connect](./mqtt/custom_authorizer_connect/README.md)
* [Cognito Connect](./mqtt/cognito_connect/README.md)
* [Shadow](./shadow/shadow_sync/README.md)
* [Jobs](./jobs/job_execution/README.md)
* [Fleet provisioning](./fleet_provisioning/fleet_provisioning/README.md)
## Other Samples
* [Secure Tunnel](./secure_tunneling/secure_tunnel/README.md)
* [Secure Tunnel Notification](./secure_tunneling/tunnel_notification/README.md)
* [Cycle Pub-Sub](./pub_sub/cycle_pub_sub/README.md)
* [Greengrass discovery](./greengrass/basic_discovery/README.md)
* [Greengrass IPC](./greengrass/ipc/README.md)
* [Mqtt5 Device Defender](./device_defender/mqtt5_basic_report/README.md)
* [Mqtt311 Device Defender](./device_defender/basic_report/README.md)

## Build Instruction

Firstly, build and install aws-iot-devices-sdk-cpp-v2 with following instructions from [Installation](../README.md#Installation).

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

This will compile all the samples at once and place the executables under the `build` directory relative to their file path. To view the commands for a given sample, run the compiled program and pass `--help`. For example, with the MQTT5 PubSub sample:

```sh
./build/mqtt5/mqtt5_pubsub/mqtt5_pubsub --help
```

This will compile all of the samples at once. You can then find the samples in the `aws-iot-device-sdk-cpp-v2/samples/build` folder. For example, the MQTT5 PubSub sample will be located at `aws-iot-device-sdk-cpp-v2/samples/build/mqtt5/mqtt5_pubsub`.

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

### Sample help

All samples will show their options by passing in `--help`. For example:

```sh
./build/mqtt5/mqtt5_pubsub/mqtt5_pubsub --help
```

Which will result in output showing all of the options that can be passed in at the command line, along with descriptions of what each does and whether or not they are optional or not.

### Enable logging in samples

To enable logging in the samples, you can pass in `--verbosity`, and optionally `--log_file`, to the sample:

```sh
./build/basic-pub-sub --verbosity "Trace" --log_file "log.txt"
```

* `--verbosity`: The level of logging shown. Can be `Trace`, `Debug`, `Info`, `Warn`, `Error`, `Fatal` or `None`. Logging will not occur if this is not passed in with `None` or greater logging level.
* `--log_file`: The filepath to store the logs at. This is optional, and if undefined the logs will be printed to `stdout` instead.
