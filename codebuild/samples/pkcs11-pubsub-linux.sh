#!/bin/bash

set -e
set -o pipefail

ENDPOINT=$(aws secretsmanager get-secret-value --secret-id "unit-test/endpoint" --query "SecretString" | cut -f2 -d":" | sed -e 's/[\\\"\}]//g')

# from hereon commands are echoed. don't leak secrets
set -x

echo "--- Configuring SoftHSM2 for PKCS#11 test ---"
softhsm2-util --version

# set tokendir, since SoftHSM2's default tokendir might be invalid on this machine
export SOFTHSM2_CONF=/tmp/softhsm2.conf
echo "directories.tokendir = /tmp/tokens" > $SOFTHSM2_CONF

# create token
softhsm2-util --init-token --free --label my-token --pin 0000 --so-pin 0000

# add private key to token (must be in PKCS#8 format)
openssl pkcs8 -topk8 -in /tmp/privatekey.pem -out /tmp/privatekey.p8.pem -nocrypt
softhsm2-util --import /tmp/privatekey.p8.pem --token my-token --label my-label --id BEEFCAFE --pin 0000

# build and run sample
pushd $CODEBUILD_SRC_DIR/samples/mqtt/pkcs11_pub_sub

echo "--- Build PKCS#11 sample ---"
mkdir _build
cd _build
cmake -DCMAKE_PREFIX_PATH=/tmp/install ..
make -j

echo "--- Run PKCS#11 sample ---"
./pkcs11-pub-sub \
    --endpoint $ENDPOINT \
    --cert /tmp/certificate.pem \
    --pkcs11_lib libsofthsm2.so \
    --pin 0000 \
    --token_label my-token \
    --key_label my-key

popd
