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
* [FAQ](FAQ.md)
* [Giving Feedback and Contributions](#Giving-Feedback-and-Contributions)

## Installation

### Minimum Requirements
* C++ 11 or higher
* CMake 3.1+
* Clang 3.9+ or GCC 4.8+ or MSVC 2015+

[Step-by-step instructions](./PREREQUISITES.md)


### Build from source

#### Automatically Build and Install AWS Dependencies

Note: `-DCMAKE_INSTALL_PREFIX` can be any path where the project needs to be installed. Taking sdk-cpp-workspace as an example here.

#### MacOS and Linux

``` sh
mkdir sdk-cpp-workspace
cd sdk-cpp-workspace
git clone --recursive https://github.com/aws/aws-iot-device-sdk-cpp-v2.git
mkdir aws-iot-device-sdk-cpp-v2-build
cd aws-iot-device-sdk-cpp-v2-build
cmake -DCMAKE_INSTALL_PREFIX="<absolute path sdk-cpp-workspace dir>" -DCMAKE_BUILD_TYPE="<Release|RelWithDebInfo|Debug>" ../aws-iot-device-sdk-cpp-v2
cmake --build . --target install
```

#### Windows

``` sh
mkdir sdk-cpp-workspace
cd sdk-cpp-workspace
git clone --recursive https://github.com/aws/aws-iot-device-sdk-cpp-v2.git
mkdir aws-iot-device-sdk-cpp-v2-build
cd aws-iot-device-sdk-cpp-v2-build
cmake -DCMAKE_INSTALL_PREFIX="<absolute path sdk-cpp-workspace dir>" ../aws-iot-device-sdk-cpp-v2
cmake --build . --target install --config "<Release|RelWithDebInfo|Debug>"
```

Due to maximum path length limitations in the Windows API, we recommend cloning to a short path like: `C:\dev\iotsdk`

`--config` is only REQUIRED for multi-configuration build tools (VisualStudio/MsBuild being the most common).

## Samples

[Samples README](./samples)

## Getting Help

The best way to interact with our team is through GitHub. You can [open an issue](https://github.com/aws/aws-iot-device-sdk-cpp-v2/issues) and choose from one of our templates for guidance, bug reports, or feature requests. You may also find help on community resources such as [StackOverFlow](https://stackoverflow.com/questions/tagged/aws-iot) with the tag #aws-iot or If you have a support plan with [AWS Support](https://aws.amazon.com/premiumsupport/), you can also create a new support case.

Please make sure to check out our resources too before opening an issue:

* Our [FAQ](FAQ.md)
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

* [Contributions Guidelines](CONTRIBUTING.md)
* Articulate your feature request or upvote existing ones on our [Issues](https://github.com/aws/aws-iot-device-sdk-cpp-v2/issues?q=is%3Aissue+is%3Aopen+label%3Afeature-request) page.
* Submit [Issues](https://github.com/aws/aws-iot-device-sdk-cpp-v2/issues)

## License

This library is licensed under the [Apache 2.0 License](LICENSE).
