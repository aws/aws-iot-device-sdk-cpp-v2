#!/bin/bash

set -e

env

pushd $CODEBUILD_SRC_DIR/samples/mqtt/custom_authorizer_connect

mkdir _build
cd _build
cmake -DCMAKE_PREFIX_PATH=/tmp/install ..
make -j

ENDPOINT=$(aws secretsmanager get-secret-value --secret-id "ci/endpoint" --query "SecretString" | cut -f2 -d":" | sed -e 's/[\\\"\}]//g')
AUTH_NAME=$(aws secretsmanager get-secret-value --secret-id "ci/CustomAuthorizer/name" --query "SecretString" | cut -f2 -d":" | sed -e 's/[\\\"\}]//g')
AUTH_PASSWORD=$(aws secretsmanager get-secret-value --secret-id "ci/CustomAuthorizer/password" --query "SecretString" | cut -f2 -d":" | sed -e 's/[\\\"\}]//g')

echo "Custom Authorizer Connnect Test"
./custom-authorizer-connect --endpoint $ENDPOINT --custom_auth_authorizer_name $AUTH_NAME --custom_auth_password $AUTH_PASSWORD

popd
