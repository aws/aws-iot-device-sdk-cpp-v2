set -e

env

pushd $CODEBUILD_SRC_DIR/secure_tunneling/tests

mkdir _build
cd _build
cmake -DCMAKE_PREFIX_PATH=/tmp/install ..
make -j

printf "Currently set env variables\n"
printenv

tunnel_info=$(aws iotsecuretunneling open-tunnel --destination-config services=ssh,ssh2,ssh3 --timeout-config maxLifetimeTimeoutMinutes=10) && echo -e "$tunnel_info" > /tmp/tunnel_info.pem
source_access_token=$(sed '4!d' /tmp/tunnel_info.pem | cut -d'"' -f4) && echo -e "$source_access_token" > /tmp/source_access_token.pem
destination_access_token=$(sed '5!d' /tmp/tunnel_info.pem | cut -d'"' -f4) && echo -e "$destination_access_token" > /tmp/destination_access_token.pem

cat /tmp/tunnel_info.pem
cat /tmp/source_access_token.pem
cat /tmp/destination_access_token.pem

echo "Secure Tunnel Test"
./secure_tunnel_test

# mkdir _build
# cd _build
# cmake -DCMAKE_PREFIX_PATH=/tmp/install ..
# make -j
