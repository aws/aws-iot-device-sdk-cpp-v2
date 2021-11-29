#!/bin/bash

set -e

env

pushd $CODEBUILD_SRC_DIR/samples/secure_tunneling/secure_tunnel

mkdir _build
cd _build
cmake -DCMAKE_PREFIX_PATH=/tmp/install ..
make -j

echo "aws iotsecuretunneling open-tunnel"
RESPONSE=$(aws iotsecuretunneling open-tunnel)
array=(${RESPONSE//:/ })

echo "Assigning SOURCETOKEN"
SOURCETOKEN="${array[11]}"

echo "SOURCETOKEN"
echo $SOURCETOKEN

echo "Assigning DESTINATIONTOKEN"
DESTINATIONTOKEN="${array[13]}"

echo "DESTINATIONTOKEN"
echo $DESTINATIONTOKEN

echo "got tokens"

echo "Secure Tunnel Destination test"
./secure-tunnel --region us-east-1 --access_token $DESTINATIONTOKEN

echo "Secure Tunnel Source test"
./secure-tunnel --localProxyModeSource --region us-east-1 --access_token $SOURCETOKEN
