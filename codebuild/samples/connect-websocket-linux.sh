#!/bin/bash

set -e

env

pushd $CODEBUILD_SRC_DIR/samples/mqtt/websocket_connect

mkdir _build
cd _build
cmake -DCMAKE_PREFIX_PATH=/tmp/install ..
make -j

ENDPOINT=$(aws secretsmanager get-secret-value --secret-id "unit-test/endpoint" --query "SecretString" | cut -f2 -d":" | sed -e 's/[\\\"\}]//g')

echo "Websocket test"
./websocket-connect --endpoint $ENDPOINT --use_websocket --signing_region us-east-1

popd
