include(CMakeFindDependencyMacro)

find_dependency(aws-crt-cpp)

include(${CMAKE_CURRENT_LIST_DIR}/aws-iot-device-sdk-cpp-v2-targets.cmake)
