cmake_minimum_required(VERSION 3.1)

# note: cxx-17 requires cmake 3.8, cxx-20 requires cmake 3.12
project(basic-report CXX)

# Device Defender is only supported on Linux
if (UNIX AND NOT APPLE)

    file(GLOB SRC_FILES
            "*.cpp"
            "../../utils/CommandLineUtils.cpp"
            "../../utils/CommandLineUtils.h"
    )

    add_executable(${PROJECT_NAME} ${SRC_FILES})

    set_target_properties(${PROJECT_NAME} PROPERTIES
            CXX_STANDARD 14)

    #set warnings
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wno-long-long -pedantic -Werror)

    find_package(aws-crt-cpp REQUIRED)
    find_package(IotDeviceCommon-cpp REQUIRED)
    find_package(IotDeviceDefender-cpp REQUIRED)

    target_link_libraries(${PROJECT_NAME} AWS::aws-crt-cpp AWS::IotDeviceCommon-cpp AWS::IotDeviceDefender-cpp)

endif()
