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

cert=$(aws secretsmanager get-secret-value --secret-id "ci/CodeBuild/cert" --query "SecretString" | cut -f2 -d":" | cut -f2 -d\") && echo -e "$cert" > /tmp/certificate.pem
key=$(aws secretsmanager get-secret-value --secret-id "ci/CodeBuild/key" --query "SecretString" | cut -f2 -d":" | cut -f2 -d\") && echo -e "$key" > /tmp/privatekey.pem
key_p8=$(aws secretsmanager get-secret-value --secret-id "ci/CodeBuild/keyp8" --query "SecretString" | cut -f2 -d":" | cut -f2 -d\") && echo -e "$key_p8" > /tmp/privatekey_p8.pem
