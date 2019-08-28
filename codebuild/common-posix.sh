#!/bin/bash

set -e

echo "Using CC=$CC CXX=$CXX"

BUILD_PATH=/tmp/builds
mkdir -p $BUILD_PATH
INSTALL_PATH=$BUILD_PATH/install
mkdir -p $INSTALL_PATH
CMAKE_ARGS="-DCMAKE_PREFIX_PATH=$INSTALL_PATH -DCMAKE_INSTALL_PREFIX=$INSTALL_PATH $@"

# If TRAVIS_OS_NAME is OSX, skip this step (will resolve to empty string on CodeBuild)
if [ "$TRAVIS_OS_NAME" != "osx" ]; then
    sudo apt-get install libssl-dev -y
fi

# build aws-crt-cpp
pushd $BUILD_PATH
git clone --branch v0.5.2 https://github.com/awslabs/aws-crt-cpp.git
cd aws-crt-cpp
cmake $CMAKE_ARGS -DBUILD_DEPS=ON ./
cmake --build . --target install
popd

# build SDK
mkdir -p build
pushd build
cmake $CMAKE_ARGS ../
cmake --build . --target install
popd
