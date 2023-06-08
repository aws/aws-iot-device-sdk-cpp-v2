# AWS IoT Device SDK for C++ v2

This document provides information about the AWS IoT device SDK for C++ V2.

If you have any issues or feature requests, please file an issue or pull request.

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

__Jump To:__

* [Installation](#Installation)
* [Samples](./samples)
* [Getting Help](#Getting-Help)
* [FAQ](./documents/FAQ.md)
* [Giving Feedback and Contributions](#Giving-Feedback-and-Contributions)
* [MQTT5 User Guide](./documents/MQTT5_Userguide.md)
* [Secure Tunnel User Guide](./documents/Secure_Tunnel_Userguide.md)


## Installation

### Minimum Requirements
* C++ 11 or higher
    * Clang 3.9+ or GCC 4.8+ or MSVC 2015+
* CMake 3.1+

[Step-by-step instructions](./documents/PREREQUISITES.md)


### Build from source

#### MacOS and Linux

``` sh
# Create a workspace directory to hold all the SDK files
mkdir sdk-workspace
cd sdk-workspace
# Clone the repository
git clone --recursive https://github.com/aws/aws-iot-device-sdk-cpp-v2.git
# Ensure all submodules are properly updated
cd aws-iot-device-sdk-cpp-v2
git submodule update --init --recursive
cd ..
# Make a build directory for the SDK. Can use any name.
# If working with multiple SDKs, using a SDK-specific name is helpful.
mkdir aws-iot-device-sdk-cpp-v2-build
cd aws-iot-device-sdk-cpp-v2-build
# Generate the SDK build files.
# -DCMAKE_INSTALL_PREFIX needs to be the absolute/full path to the directory.
#     (Example: "/Users/example/sdk-workspace/).
# -DCMAKE_BUILD_TYPE can be "Release", "RelWithDebInfo", or "Debug"
cmake -DCMAKE_INSTALL_PREFIX="<absolute path to sdk-workspace>" -DCMAKE_BUILD_TYPE="Debug" ../aws-iot-device-sdk-cpp-v2
# Build and install the library. Once installed, you can develop with the SDK and run the samples
cmake --build . --target install
```

#### Windows

``` sh
# Create a workspace directory to hold all the SDK files
mkdir sdk-workspace
cd sdk-cpp-workspace
# Clone the repository
git clone --recursive https://github.com/aws/aws-iot-device-sdk-cpp-v2.git
# Ensure all submodules are properly updated
cd aws-iot-device-sdk-cpp-v2
git submodule update --init --recursive
cd ..
# Make a build directory for the SDK. Can use any name.
# If working with multiple SDKs, using a SDK-specific name is helpful.
mkdir aws-iot-device-sdk-cpp-v2-build
cd aws-iot-device-sdk-cpp-v2-build
# Generate the SDK build files.
# -DCMAKE_INSTALL_PREFIX needs to be the absolute/full path to the directory.
#     (Example: "C:/users/example/sdk-workspace/).
cmake -DCMAKE_INSTALL_PREFIX="<absolute path sdk-cpp-workspace dir>" ../aws-iot-device-sdk-cpp-v2
# Build and install the library. Once installed, you can develop with the SDK and run the samples
# -config can be "Release", "RelWithDebInfo", or "Debug"
cmake --build . --target install --config "Debug"
```

**Windows specific notes**:
* Due to maximum path length limitations in the Windows API, we recommend cloning to a short path like: `C:\dev\iotsdk`
* `--config` is only REQUIRED for multi-configuration build tools (VisualStudio/MsBuild being the most common).

**Linux specific notes**:

If your application uses OpenSSL, configure with `-DUSE_OPENSSL=ON`.

The IoT SDK does not use OpenSSL for TLS.
On Apple and Windows, the OS's default TLS library is used.
On Linux, [s2n-tls](https://github.com/aws/s2n-tls) is used.
But s2n-tls uses libcrypto, the cryptography math library bundled with OpenSSL.
To simplify the build process, the source code for s2n-tls and libcrypto are
included as git submodules and built along with the IoT SDK.
But if your application is also loading the system installation of OpenSSL
(i.e. your application uses libcurl which uses libssl which uses libcrypto)
there may be crashes as the application tries to use two different versions of libcrypto at once.

Setting `-DUSE_OPENSSL=ON` will cause the IoT SDK to link against your system's
existing `libcrypto`, instead of building its own copy.

## Samples

[Samples README](./samples)

## Getting Help

The best way to interact with our team is through GitHub. You can [open an issue](https://github.com/aws/aws-iot-device-sdk-cpp-v2/issues) and choose from one of our templates for guidance, bug reports, or feature requests. You may also find help on community resources such as [StackOverFlow](https://stackoverflow.com/questions/tagged/aws-iot) with the tag #aws-iot or If you have a support plan with [AWS Support](https://aws.amazon.com/premiumsupport/), you can also create a new support case.

Please make sure to check out our resources too before opening an issue:

* Our [FAQ](./documents/FAQ.md)
* [API Docs](https://aws.github.io/aws-iot-device-sdk-cpp-v2/)
* Our [Developer Guide](https://docs.aws.amazon.com/iot/latest/developerguide/what-is-aws-iot.html) ([source](https://github.com/awsdocs/aws-iot-docs))
* Check for similar [Issues](https://github.com/aws/aws-iot-device-sdk-cpp-v2/issues)
* [AWS IoT Core Documentation](https://docs.aws.amazon.com/iot/)
* [Dev Blog](https://aws.amazon.com/blogs/?awsf.blog-master-iot=category-internet-of-things%23amazon-freertos%7Ccategory-internet-of-things%23aws-greengrass%7Ccategory-internet-of-things%23aws-iot-analytics%7Ccategory-internet-of-things%23aws-iot-button%7Ccategory-internet-of-things%23aws-iot-device-defender%7Ccategory-internet-of-things%23aws-iot-device-management%7Ccategory-internet-of-things%23aws-iot-platform)
* Integration with AWS IoT Services such as
[Device Shadow](https://docs.aws.amazon.com/iot/latest/developerguide/iot-device-shadows.html)
and [Jobs](https://docs.aws.amazon.com/iot/latest/developerguide/iot-jobs.html)
is provided by code that been generated from a model of the service.

## Giving Feedback and Contributions

We need your help in making this SDK great. Please participate in the community and contribute to this effort by submitting issues, participating in discussion forums and submitting pull requests through the following channels.

* [Contributions Guidelines](./documents/CONTRIBUTING.md)
* Articulate your feature request or upvote existing ones on our [Issues](https://github.com/aws/aws-iot-device-sdk-cpp-v2/issues?q=is%3Aissue+is%3Aopen+label%3Afeature-request) page.
* Create discussion questions [here](https://github.com/aws/aws-iot-device-sdk-cpp-v2/discussions)
* Find a bug open an [issue](https://github.com/aws/aws-iot-device-sdk-cpp-v2/issues)

## License

This library is licensed under the [Apache 2.0 License](./documents/LICENSE).

Latest released version: v1.24.3
