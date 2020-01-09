#!/bin/bash

set -e

echo "Using CC=$CC CXX=$CXX"

BUILD_PATH=/tmp/builds
mkdir -p $BUILD_PATH
INSTALL_PATH=$BUILD_PATH/install
mkdir -p $INSTALL_PATH
CMAKE_ARGS="-DCMAKE_PREFIX_PATH=$INSTALL_PATH -DCMAKE_INSTALL_PREFIX=$INSTALL_PATH -DBUILD_DEPS=ON $@"

# If TRAVIS_OS_NAME is OSX, skip this step (will resolve to empty string on CodeBuild)
if [ "$TRAVIS_OS_NAME" != "osx" ]; then
    sudo apt-get install libssl-dev -y
fi

# build SDK
git submodule update --init --recursive
mkdir -p build
pushd build
cmake $CMAKE_ARGS ../
cmake --build . --target install
popd
