INSTALL_PATH=C:/Iot/workspace

cmake -Bbuild/deviceadvisor/tests/mqtt_connect -Hdeviceadvisor/tests/mqtt_connect -DCMAKE_PREFIX_PATH=$INSTALL_PATH -DCMAKE_BUILD_TYPE=RelWithDebInfo 
cmake --build build/deviceadvisor/tests/mqtt_connect --config RelWithDebInfo 
cmake -Bbuild/deviceadvisor/tests/mqtt_publish -Hdeviceadvisor/tests/mqtt_publish -DCMAKE_PREFIX_PATH=$INSTALL_PATH -DCMAKE_BUILD_TYPE=RelWithDebInfo 
cmake --build build/deviceadvisor/tests/mqtt_publish --config RelWithDebInfo 
cmake -Bbuild/deviceadvisor/tests/mqtt_subscribe -Hdeviceadvisor/tests/mqtt_subscribe -DCMAKE_PREFIX_PATH=$INSTALL_PATH -DCMAKE_BUILD_TYPE=RelWithDebInfo 
cmake --build build/deviceadvisor/tests/mqtt_subscribe --config RelWithDebInfo 
cmake -Bbuild/deviceadvisor/tests/shadow_update -Hdeviceadvisor/tests/shadow_update -DCMAKE_PREFIX_PATH=$INSTALL_PATH -DCMAKE_BUILD_TYPE=RelWithDebInfo 
cmake --build build/deviceadvisor/tests/shadow_update --config RelWithDebInfo 