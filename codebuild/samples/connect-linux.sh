#!/bin/bash

set -e

env

pushd $CODEBUILD_SRC_DIR/samples/mqtt/basic_connect

mkdir _build
cd _build
cmake -DCMAKE_PREFIX_PATH=/tmp/install ..
make -j

ENDPOINT=$(aws secretsmanager get-secret-value --secret-id "ci/endpoint" --query "SecretString" | cut -f2 -d":" | sed -e 's/[\\\"\}]//g')

echo "Basic Connect test"
./basic-connect --endpoint $ENDPOINT --key /tmp/privatekey.pem --cert /tmp/certificate.pem

popd

pushd $CODEBUILD_SRC_DIR/samples/mqtt/websocket_connect

mkdir _build
cd _build
cmake -DCMAKE_PREFIX_PATH=/tmp/install ..
make -j

echo "Websocket test"
./websocket-connect --endpoint $ENDPOINT --signing_region us-east-1

popd
