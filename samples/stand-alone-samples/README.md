# Stand-alone Samples

## building instruction

Firstly, build the project with the following instruction.

``` sh
mkdir sdk-cpp-workspace
cd sdk-cpp-workspace
git clone --recursive https://github.com/aws/aws-iot-device-sdk-cpp-v2.git
mkdir aws-iot-device-sdk-cpp-v2-build
cd aws-iot-device-sdk-cpp-v2-build
cmake -DCMAKE_INSTALL_PREFIX="<absolute path sdk-cpp-workspace dir>"  -DBUILD_DEPS=ON -DCMAKE_BUILD_TYPE=Release ../aws-iot-device-sdk-cpp-v2
cmake --build . --config Release --target install
```

Then, change directory into stand-alone-samples.

In stand-alone-samples/shadow/shadow_sync/ or stand-alone-samples/basic_pub_sub/:

``` sh
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX="<absolute path sdk-cpp-workspace dir>"  -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```
To debug, just change every mentioned `Release` to `Debug`.

The binary executable file will be under build/. for Linux/Mac, and under build/release for Windows.

cmake -DCMAKE_INSTALL_PREFIX="/home/ANT.AMAZON.COM/dengket/Desktop/project/iot-device-sdks/release-install"  -DBUILD_DEPS=ON -DCMAKE_BUILD_TYPE=Release ../

cmake -DCMAKE_PREFIX_PATH="/home/ANT.AMAZON.COM/dengket/Desktop/project/iot-device-sdks/release-install" -DLibCrypto_INCLUDE_DIR="/usr/include" -DLibCrypto_STATIC_LIBRARY="/usr/lib/x86_64-linux-gnu/libcrypto.a" -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release

cmake -DCMAKE_INSTALL_PREFIX="/home/ANT.AMAZON.COM/dengket/Desktop/project/iot-device-sdks/release-install" -DCMAKE_BUILD_TYPE=Release ../
