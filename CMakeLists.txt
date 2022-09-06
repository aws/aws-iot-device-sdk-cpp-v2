cmake_minimum_required(VERSION 3.1)
project(aws-iot-device-sdk-cpp-v2)

include(CTest)

option(BUILD_DEPS "Builds aws common runtime dependencies as part of build. Turn off if you want to control your dependency chain." ON)
option(BUILD_SAMPLES "Build samples as part of the build" OFF)

if (DEFINED CMAKE_PREFIX_PATH)
    file(TO_CMAKE_PATH "${CMAKE_PREFIX_PATH}" CMAKE_PREFIX_PATH)
endif()

if (DEFINED CMAKE_INSTALL_PREFIX)
    file(TO_CMAKE_PATH "${CMAKE_INSTALL_PREFIX}" CMAKE_INSTALL_PREFIX)
endif()

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()

if (UNIX AND NOT APPLE)
    include(GNUInstallDirs)
elseif(NOT DEFINED CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR "lib")
endif()

if (${CMAKE_INSTALL_LIBDIR} STREQUAL "lib64")
    set(FIND_LIBRARY_USE_LIB64_PATHS true)
endif()

if (NOT DEFINED AWS_SDK_CPP_VERSION)
    set(AWS_SDK_CPP_VERSION "v1.18.6")
    # Try to determine the current git tag and use that as the version
    execute_process(COMMAND git describe --tags
        RESULT_VARIABLE GIT_EXIT_CODE
        OUTPUT_VARIABLE GIT_TAG
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if (NOT GIT_EXIT_CODE)
        set(AWS_SDK_CPP_VERSION ${GIT_TAG})
    endif()
endif()
message(STATUS "AWS SDK C++ ${AWS_SDK_CPP_VERSION}")

# This is required in order to append /lib/cmake to each element in CMAKE_PREFIX_PATH
set(AWS_MODULE_DIR "/${CMAKE_INSTALL_LIBDIR}/cmake")
string(REPLACE ";" "${AWS_MODULE_DIR};" AWS_MODULE_PATH "${CMAKE_PREFIX_PATH}${AWS_MODULE_DIR}")
# Append that generated list to the module search path
list(APPEND CMAKE_MODULE_PATH ${AWS_MODULE_PATH})

include(CMakePackageConfigHelpers)
STRING(REPLACE "v" "" AWS_SDK_CPP_VERSION_WITHOUT_V ${AWS_SDK_CPP_VERSION})
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
        VERSION ${AWS_SDK_CPP_VERSION_WITHOUT_V}
        COMPATIBILITY ExactVersion
)

if (NOT CMAKE_BUILD_TYPE)
    if (NOT WIN32)
        set(CMAKE_BUILD_TYPE "RelWithDebInfo")
    endif()
endif()

if (BUILD_DEPS)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/crt/aws-crt-cpp/crt/aws-c-common/cmake")

    include(AwsFindPackage)
    set(IN_SOURCE_BUILD ON)
    set(BUILD_TESTING_PREV ${BUILD_TESTING})
    set(BUILD_TESTING OFF)
    add_subdirectory(crt/aws-crt-cpp)
    if (NOT BYO_CRYPTO)
        # TODO: get this working with BYO_CRYPTO
        add_subdirectory(crt/aws-c-iot)
    endif ()
    set(BUILD_TESTING ${BUILD_TESTING_PREV})
else()
    include(AwsFindPackage)
    set(IN_SOURCE_BUILD OFF)
endif()

aws_use_package(aws-crt-cpp)

add_subdirectory(jobs)
add_subdirectory(shadow)
add_subdirectory(discovery)
add_subdirectory(identity)
add_subdirectory(eventstream_rpc)
add_subdirectory(greengrass_ipc)
if (NOT BYO_CRYPTO)
    # TODO: get these working with BYO_CRYPTO
    add_subdirectory(iotdevicecommon)
    add_subdirectory(devicedefender)
    add_subdirectory(secure_tunneling)
endif ()

if (BUILD_SAMPLES)
    message(WARNING "BUILD_SAMPLES has been deprecated. Please build each sample separately.")
endif()
