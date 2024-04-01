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

    if (UNIX AND NOT APPLE)
        include(GNUInstallDirs)
    elseif(NOT DEFINED CMAKE_INSTALL_LIBDIR)
        set(CMAKE_INSTALL_LIBDIR "lib")

        if (${CMAKE_INSTALL_LIBDIR} STREQUAL "lib64")
            set(FIND_LIBRARY_USE_LIB64_PATHS true)
        endif()
    endif()

    list(APPEND CMAKE_MODULE_PATH "${CMAKE_PREFIX_PATH}/${CMAKE_INSTALL_LIBDIR}/cmake")

    include(AwsSanitizers)
    enable_language(C)
    aws_add_sanitizers(${PROJECT_NAME})

    target_link_libraries(${PROJECT_NAME} PRIVATE AWS::aws-crt-cpp AWS::IotDeviceCommon-cpp AWS::IotDeviceDefender-cpp)

endif()
