#!/bin/bash

set -e

env

# build package
cd $CODEBUILD_SRC_DIR

git submodule update --init --recursive
mkdir build
cd build

mkdir -p /tmp/install

cmake -DCMAKE_INSTALL_PREFIX=/tmp/install -DCMAKE_PREFIX_PATH=/tmp/install ..
make -j
make install

cd ..

cert=$(aws secretsmanager get-secret-value --secret-id "unit-test/certificate" --query "SecretString" | cut -f2 -d":" | cut -f2 -d\") && echo -e "$cert" > /tmp/certificate.pem
key=$(aws secretsmanager get-secret-value --secret-id "unit-test/privatekey" --query "SecretString" | cut -f2 -d":" | cut -f2 -d\") && echo -e "$key" > /tmp/privatekey.pem
