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
echo $RESPONSE



read -r a[{1..10}] <<< RESPONSE
SOURCETOKEN= grep -oP '(?<="sourceAccessToken": ")[^"]*' RESPONSE
DESTINATIONTOKEN=grep -oP '(?<="destinationAccessToken": ")[^"]*' RESPONSE

echo "SOURCETOKEN"
echo $SOURCETOKEN

echo "DESTINATIONTOKEN"
echo $DESTINATIONTOKEN

echo "got tokens"

echo "Secure Tunnel Destination test"
./secure-tunnel --region us-east-1 --access_token $DESTINATIONTOKEN

echo "Secure Tunnel Source test"
./secure-tunnel --localProxyModeSource --region us-east-1 --access_token $SOURCETOKEN
