set -e

env

pushd $CODEBUILD_SRC_DIR/secure_tunneling/tests

mkdir _build
cd _build
cmake -DCMAKE_PREFIX_PATH=/tmp/install ..
make -j

tunnel_info=$(aws iotsecuretunneling open-tunnel --destination-config services=ssh,ssh2,ssh3 --timeout-config maxLifetimeTimeoutMinutes=10) && echo -e "$tunnel_info" > /tmp/tunnel_info.pem

export SECTUN_SOURCE_TOKEN=$(sed '4!d' /tmp/tunnel_info.pem | cut -d'"' -f4)
export SECTUN_DESTINATION_TOKEN=$(sed '5!d' /tmp/tunnel_info.pem | cut -d'"' -f4)
export SECTUN_ENDPOINT="data.tunneling.iot.us-east-1.amazonaws.com"

echo "Running Secure Tunnel Test"
./secure_tunnel_test
