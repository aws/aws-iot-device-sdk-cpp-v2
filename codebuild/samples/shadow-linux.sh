#!/bin/bash

set -e

env

# v1 MQTT311 shadow sample
pushd $CODEBUILD_SRC_DIR/samples/shadow/deprecated/shadow_sync

mkdir _build
cd _build
cmake -DCMAKE_PREFIX_PATH=/tmp/install ..
make -j

ENDPOINT=$(aws secretsmanager get-secret-value --secret-id "ci/endpoint" --query "SecretString" | cut -f2 -d":" | sed -e 's/[\\\"\}]//g')

echo "Shadow-Sync test"
./shadow-sync --endpoint $ENDPOINT --key /tmp/privatekey.pem --cert /tmp/certificate.pem --thing_name CI_CodeBuild_Thing --is_ci true

popd

# v1 MQTT5 shadow sample
pushd $CODEBUILD_SRC_DIR/samples/shadow/deprecated/mqtt5_shadow_sync

mkdir _build
cd _build
cmake -DCMAKE_PREFIX_PATH=/tmp/install ..
make -j

ENDPOINT=$(aws secretsmanager get-secret-value --secret-id "ci/endpoint" --query "SecretString" | cut -f2 -d":" | sed -e 's/[\\\"\}]//g')

echo "Shadow-Sync test"
./mqtt5-shadow-sync --endpoint $ENDPOINT --key /tmp/privatekey.pem --cert /tmp/certificate.pem --thing_name CI_CodeBuild_Thing --is_ci true


popd

# v2 MQTT5 shadow sample - smaple is interactive so build but don't run

pushd $CODEBUILD_SRC_DIR/samples/shadow/shadow_sync

mkdir _build
cd _build
cmake -DCMAKE_PREFIX_PATH=/tmp/install ..
make -j

popd