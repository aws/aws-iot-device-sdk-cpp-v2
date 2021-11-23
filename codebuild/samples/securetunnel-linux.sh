#!/bin/bash
echo "bin/bash"
set -e
echo "-e set"
env
echo "env"
pushd $CODEBUILD_SRC_DIR/samples/secure_tunneling/secure_tunnel
echo "pushd"
mkdir _build
cd _build
cmake -DCMAKE_PREFIX_PATH=/tmp/install ..
make -j
echo "make-j"
REGION=$(aws configure get region)
echo "REGION set"
read -r a[{1..10}] <<< $(aws iotsecuretunneling open-tunnel)
echo "TUNNEL CREATED"
SOURCETOKEN=${a[7]}
DESTINATIONTOKEN=${a[9]}
echo "Tokens set"
echo "Secure Tunnel Destination test"
./secure-tunnel --region $REGION --access_token $DESTINATIONTOKEN

echo "Secure Tunnel Source test"
./secure-tunnel --region $REGION --access_token $SOURCETOKEN --localProxyModeSource
