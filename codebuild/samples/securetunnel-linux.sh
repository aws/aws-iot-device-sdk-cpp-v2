#!/bin/bash

set -e

env

pushd $CODEBUILD_SRC_DIR/samples/secure_tunneling/secure_tunnel

mkdir _build
cd _build
cmake -DCMAKE_PREFIX_PATH=/tmp/install ..
make -j

echo "creating tunnel and getting variables"

echo $(aws secretsmanager get-secret-value iotsecuretunneling open-tunnel)

read -r a[{1..10}] <<< $(aws secretsmanager get-secret-value iotsecuretunneling open-tunnel)
SOURCETOKEN=${a[7]}
DESTINATIONTOKEN=${a[9]}

echo "SOURCETOKEN"
echo $SOURCETOKEN

echo "DESTINATIONTOKEN"
echo $DESTINATIONTOKEN

echo "got tokens"

echo "Secure Tunnel Destination test"
./secure-tunnel --region us-east-1 --access_token $DESTINATIONTOKEN

echo "Secure Tunnel Source test"
./secure-tunnel --localProxyModeSource --region us-east-1 --access_token $SOURCETOKEN
