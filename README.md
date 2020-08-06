# AWS IoT SDK for C++ v2
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


*__Jump To:__*
* [Installation](#Installation)
* [Samples](samples)
* [Getting Help](#Getting-Help)
* [Giving Feedback and Contributions](#Giving-Feedback-and-Contributions)
* [More Resources](#More-Resources)



## Installation

### Minimum Requirements
*   C++ 11 or higher
*   CMake 3.1+
*   Clang 3.9+ or GCC 4.4+ or MSVC 2015+


### Build from source
#### Automatically Build and Install AWS Dependencies
```sh
mkdir sdk-cpp-workspace
cd sdk-cpp-workspace
git clone --recursive https://github.com/aws/aws-iot-device-sdk-cpp-v2.git
mkdir aws-iot-device-sdk-cpp-v2-build
cd aws-iot-device-sdk-cpp-v2-build
cmake -DCMAKE_INSTALL_PREFIX="<absolute path sdk-cpp-workspace dir>"  -DBUILD_DEPS=ON ../aws-iot-device-sdk-cpp-v2
cmake --build . --target install
```
#### Using a Pre-Built aws-crt-cpp (Most useful for development of this package)

```sh
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

## Samples

[Samples README](samples)



## Getting Help

Use the following sources for information :

*   Check api and developer guides.
*   Check for similar issues already opened.

If you still can’t find a solution to your problem open an [issue](https://github.com/aws/aws-iot-device-sdk-cpp-v2/issues)



## Giving Feedback and Contributions

We need your help in making this SDK great. Please participate in the community and contribute to this effort by submitting issues, participating in discussion forums and submitting pull requests through the following channels.

*   [Contributions Guidelines](CONTRIBUTING.md)
*   Articulate your feature request or upvote existing ones on our [Issues](https://github.com/aws/aws-iot-device-sdk-cpp-v2/issues?q=is%3Aissue+is%3Aopen+label%3Afeature-request) page.
*   Submit [Issues](https://github.com/aws/aws-iot-device-sdk-cpp-v2/issues)



## More Resources

*   [AWS IoT Core Documentation](https://docs.aws.amazon.com/iot/)
*   [Developer Guide](https://docs.aws.amazon.com/iot/latest/developerguide/what-is-aws-iot.html) ([source](https://github.com/awsdocs/aws-iot-docs))
*   [Issues](https://github.com/aws/aws-iot-device-sdk-cpp-v2/issues)
*   [Dev Blog](https://aws.amazon.com/blogs/?awsf.blog-master-iot=category-internet-of-things%23amazon-freertos%7Ccategory-internet-of-things%23aws-greengrass%7Ccategory-internet-of-things%23aws-iot-analytics%7Ccategory-internet-of-things%23aws-iot-button%7Ccategory-internet-of-things%23aws-iot-device-defender%7Ccategory-internet-of-things%23aws-iot-device-management%7Ccategory-internet-of-things%23aws-iot-platform)

Integration with AWS IoT Services such as
[Device Shadow](https://docs.aws.amazon.com/iot/latest/developerguide/iot-device-shadows.html)
and [Jobs](https://docs.aws.amazon.com/iot/latest/developerguide/iot-jobs.html)
is provided by code that been generated from a model of the service.



## License

This library is licensed under the [Apache 2.0 License](LICENSE). 