cmake_minimum_required(VERSION 3.9...3.31)

# note: cxx-17 requires cmake 3.8, cxx-20 requires cmake 3.12
project(mqtt5-basic-report CXX)

option(AWS_CRT_DISABLE_DEPRECATION_WARNINGS "Set this to silence [[deprecated]] warnings coming from aws-crt-cpp headers" OFF)
if(AWS_CRT_DISABLE_DEPRECATION_WARNINGS)
    add_compile_definitions(AWS_CRT_DISABLE_DEPRECATION_WARNINGS)
endif()

# Device Defender is only supported on Linux
if (UNIX AND NOT APPLE)

    file(GLOB SRC_FILES
            "*.cpp"
    )

    add_executable(${PROJECT_NAME} ${SRC_FILES})

    set_target_properties(${PROJECT_NAME} PROPERTIES
            CXX_STANDARD 14)

    #set warnings
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wno-long-long -pedantic -Werror)

    find_package(aws-crt-cpp REQUIRED)
    find_package(IotDeviceCommon-cpp REQUIRED)
    find_package(IotDeviceDefender-cpp REQUIRED)

    include(AwsSanitizers)
    enable_language(C)
    aws_add_sanitizers(${PROJECT_NAME})

    target_link_libraries(${PROJECT_NAME} PRIVATE AWS::aws-crt-cpp AWS::IotDeviceCommon-cpp AWS::IotDeviceDefender-cpp)

endif()
