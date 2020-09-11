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
cmake -DCMAKE_PREFIX_PATH="<absolute path sdk-cpp-workspace dir>"  -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

To debug, just change every mentioned `Release` to `Debug`.

The binary executable file will be under build/. for Linux/Mac, and under build/release for Windows.
