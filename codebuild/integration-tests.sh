env

pushd $CODEBUILD_SRC_DIR/secure_tunneling/tests

aws iotsecuretunneling open-tunnel --destination-config thingName=,services=ssh --timeout-config maxLifeTimeOutMinutes = 10

# mkdir _build
# cd _build
# cmake -DCMAKE_PREFIX_PATH=/tmp/install ..
# make -j

