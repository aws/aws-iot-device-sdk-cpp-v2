#!/bin/bash

set -e

env

pushd $CODEBUILD_SRC_DIR/samples/secure_tunneling/secure_tunnel

mkdir _build
cd _build
cmake -DCMAKE_PREFIX_PATH=/tmp/install ..
make -j

echo "make -j"

REGION=$(aws configure get region)

echo "got region"

read -r a[{1..10}] <<< $(aws iotsecuretunneling open-tunnel)
SOURCETOKEN=${a[7]}
DESTINATIONTOKEN=${a[9]}

echo "got tokens"

echo "Secure Tunnel Destination test"
./secure-tunnel --region $REGION --access_token $DESTINATIONTOKEN

echo "Secure Tunnel Source test"
./secure-tunnel --localProxyModeSource --region $REGION --access_token $SOURCETOKEN
