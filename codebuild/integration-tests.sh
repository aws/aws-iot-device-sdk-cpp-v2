set -e

env

pushd $CODEBUILD_SRC_DIR/secure_tunneling/tests

printenv

aws iotsecuretunneling open-tunnel --destination-config services=ssh,ssh2,ssh3 --timeout-config maxLifetimeTimeoutMinutes=10



# mkdir _build
# cd _build
# cmake -DCMAKE_PREFIX_PATH=/tmp/install ..
# make -j

