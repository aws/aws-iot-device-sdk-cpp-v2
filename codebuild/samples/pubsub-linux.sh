#!/bin/bash

set -e

env

pushd $CODEBUILD_SRC_DIR/samples/pub_sub/basic_pub_sub

mkdir _build
cd _build
cmake -DCMAKE_PREFIX_PATH=/tmp/install ..
make -j

ENDPOINT=$(aws secretsmanager get-secret-value --secret-id "ci/endpoint" --query "SecretString" | cut -f2 -d":" | sed -e 's/[\\\"\}]//g')

echo "PubSub test"
./basic-pub-sub --endpoint $ENDPOINT --key /tmp/privatekey.pem --cert /tmp/certificate.pem

popd
