# AWS IoT Device SDK for C++ v2

The AWS IoT Device SDK for C++ v2 connects your C++ applications and devices to the AWS IoT platform. It handles the complexities of secure communication, authentication, and device management so you can focus on your IoT solution. The SDK makes it easy to use AWS IoT services like Device Shadows, Jobs, Fleet Provisioning, and Commands.

**Supported Platforms**: Linux, Windows 11+, macOS 14+

> **Note**: The SDK is known to work on older platform versions, but we only guarantee compatibility for the platforms listed above.

*__Topics:__*
* [Features](#features)
* [Installation](#installation)
  * [Minimum Requirements](#minimum-requirements)
  * [Building from source](#building-from-source)
* [Getting Started](#getting-started)
* [Samples](samples)
* [MQTT5 User Guide](./documents/MQTT5_Userguide.md)
* [Getting Help](#getting-help)
* [Resources](#resources)

## Features

The primary purpose of the AWS IoT Device SDK for C++ v2 is to simplify the process of connecting devices to AWS IoT Core and interacting with AWS IoT services on various platforms. The SDK provides:

* Integrated service clients for AWS IoT Core services
* Secure device connections to AWS IoT Core using MQTT protocol including MQTT 5.0
* Support for [multiple authentication methods and connection types](./documents/MQTT5_Userguide.md#connecting-to-aws-iot-core)

#### Supported AWS IoT Core services

* The [AWS IoT Device Shadow](https://docs.aws.amazon.com/iot/latest/developerguide/iot-device-shadows.html) service manages device state information in the cloud.
* The [AWS IoT Jobs](https://docs.aws.amazon.com/iot/latest/developerguide/iot-jobs.html) service sends remote operations to connected devices.
* The [AWS IoT fleet provisioning](https://docs.aws.amazon.com/iot/latest/developerguide/provision-wo-cert.html) service generates and delivers device certificates automatically.
* The [AWS IoT Device Management commands](https://docs.aws.amazon.com/iot/latest/developerguide/iot-remote-command.html) service sends instructions from the cloud to connected devices.

## Installation

The recommended way to use the AWS IoT Device SDK for C++ v2 in your project is to build it from source.

### Minimum Requirements

To develop applications with the AWS IoT Device SDK for C++ v2, you need:

* C++ 11 or higher
  * Clang 6+ or GCC 4.8+ or MSVC 2015+
* CMake 3.9+

See [detailed setup instructions](./documents/PREREQUISITES.md) for more information.

#### Supported Architectures

**Linux:**
- manylinux2014-x64, manylinux2014-x86
- al2-x64
- alpine-3.16 (x64, x86, armv6, armv7, arm64)
- raspberry pi bullseye
- ARM64/aarch64, ArmV7, x86_64, x86

**Windows:**
- Win32, x64, ARM64

**macOS:**
- Apple Silicon (M1 and higher)
- Apple Intel Chips (x86_64)

### Building from source

```bash
# Create a workspace directory to hold all the SDK files
mkdir sdk-workspace
cd sdk-workspace

# Clone the repository
git clone --recursive https://github.com/aws/aws-iot-device-sdk-cpp-v2.git

# Make a build directory for the SDK
mkdir aws-iot-device-sdk-cpp-v2-build
cd aws-iot-device-sdk-cpp-v2-build
```

#### macOS and Linux

> [!NOTE]
> **Linux specific**
> The IoT SDK uses [s2n-tls](https://github.com/aws/s2n-tls) for TLS on Linux. However, s2n-tls uses libcrypto (the cryptography library from OpenSSL). To simplify building, s2n-tls and libcrypto source code are included as git submodules and built with the SDK. If your application also loads the system OpenSSL installation (e.g., via libcurl), there may be crashes from using two different libcrypto versions simultaneously. In this case, configure with `-DUSE_OPENSSL=ON` to link against system libcrypto and avoid conflicts.

```bash
# Generate the SDK build files
# -DCMAKE_INSTALL_PREFIX needs to be the absolute/full path to the directory
# -DCMAKE_BUILD_TYPE can be "Release", "RelWithDebInfo", or "Debug"
cmake -DCMAKE_INSTALL_PREFIX="<absolute path to sdk-workspace>" -DCMAKE_BUILD_TYPE="Debug" ../aws-iot-device-sdk-cpp-v2

# Build and install the library
cmake --build . --target install
```

#### Windows

> [!TIP]
> Due to path length limitations in the Windows API, we recommend cloning to a short path like: `C:\dev\iotsdk`

```bash
# Generate the SDK build files
# -DCMAKE_INSTALL_PREFIX needs to be the absolute/full path to the directory
cmake -DCMAKE_INSTALL_PREFIX="<absolute path sdk-workspace dir>" ../aws-iot-device-sdk-cpp-v2

# Build and install the library
# -config can be "Release", "RelWithDebInfo", or "Debug"
cmake --build . --target install --config "Debug"
```

`--config` is only required for multi-configuration build tools (Visual Studio/MSBuild)

## Getting Started

To get started with the AWS IoT Device SDK for C++ v2:

1. **Build the SDK** - See the [Installation](#installation) section for build instructions

2. **Choose your connection method** - The SDK supports multiple authentication methods including X.509 certificates, AWS credentials, and custom authentication. [MQTT5 User Guide connection section](./documents/MQTT5_Userguide.md#connecting-to-aws-iot-core) provides more guidance

3. **Follow a complete example** - Check out the [samples](samples) directory

4. **Learn MQTT5 features** - For advanced usage and configuration options, see the [MQTT5 User Guide](./documents/MQTT5_Userguide.md)

## Samples

Check out the [samples](samples) directory for working code examples that demonstrate:
- [Basic MQTT connection and messaging](./samples/README.md#mqtt5-client-samples)
- [AWS IoT Device Shadow operations](./samples/service_clients/shadow/shadow-sandbox/README.md)
- [AWS IoT Jobs](./samples/service_clients/jobs/jobs-sandbox/README.md)
- AWS IoT Fleet provisioning: [basic](./samples/service_clients/fleet_provisioning/provision-basic/README.md) and [with CSR](./samples/service_clients/fleet_provisioning/provision-csr/README.md)
- [AWS IoT Commands](./samples/service_clients/commands/commands-sandbox/README.md)
- Secure Tunneling: [secure tunnel](./samples/others/secure_tunneling/secure_tunnel/README.md) and [tunnel notification](./samples/others/secure_tunneling/tunnel_notification/README.md)

The samples provide ready-to-run code with detailed setup instructions for each authentication method and use case.

## Getting Help

The best way to interact with our team is through GitHub.
* Open [discussion](https://github.com/aws/aws-iot-device-sdk-cpp-v2/discussions): Share ideas and solutions with the SDK community
* Search [issues](https://github.com/aws/aws-iot-device-sdk-cpp-v2/issues): Find created issues for answers based on a topic
* Create an [issue](https://github.com/aws/aws-iot-device-sdk-cpp-v2/issues/new/choose): New feature request or file a bug

If you have a support plan with [AWS Support](https://aws.amazon.com/premiumsupport/), you can also create a new support case.

#### Mac-Only TLS Behavior

Please note that on Mac, once a private key is used with a certificate, that certificate-key pair is imported into the Mac Keychain.  All subsequent uses of that certificate will use the stored private key and ignore anything passed in programmatically.  Beginning in v1.7.3, when a stored private key from the Keychain is used, the following will be logged at the "info" log level:

```
static: certificate has an existing certificate-key pair that was previously imported into the Keychain.
 Using key from Keychain instead of the one provided.
```

## Resources

Check out our resources for additional guidance too before opening an issue:

* [FAQ](./documents/FAQ.md)
* [AWS IoT Core Developer Guide](https://docs.aws.amazon.com/iot/latest/developerguide/what-is-aws-iot.html)
* [MQTT5 User Guide](./documents/MQTT5_Userguide.md)
* [API Docs](https://aws.github.io/aws-iot-device-sdk-cpp-v2/)
* [AWS IoT Core Documentation](https://docs.aws.amazon.com/iot/)
* [Dev Blog](https://aws.amazon.com/blogs/iot/category/internet-of-things/)
* [Migration Guide from the AWS IoT SDK for C++ v1](./documents/MIGRATION_GUIDE.md)
* [Secure Tunnel User Guide](./documents/Secure_Tunnel_Userguide.md)
* [Contributions Guidelines](./documents/CONTRIBUTING.md)

## License

This library is licensed under the [Apache 2.0 License](./documents/LICENSE).

Latest released version: v1.40.2
