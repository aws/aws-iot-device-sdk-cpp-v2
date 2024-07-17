cmake_minimum_required(VERSION 3.1)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(AwsGetVersion)
aws_get_version(SIMPLE_VERSION FULL_VERSION GIT_HASH)
message(STATUS "AWS SDK C++ ${FULL_VERSION}")

project(aws-iot-device-sdk-cpp-v2
    VERSION ${SIMPLE_VERSION})

include(CTest)

option(BUILD_DEPS "Builds aws common runtime dependencies as part of build. Turn off if you want to control your dependency chain." ON)
option(USE_EXTERNAL_DEPS_SOURCES "Use dependencies provided by add_subdirectory command" OFF)
option(BUILD_SAMPLES "(DEPRECATED) Build samples as part of the build" OFF)

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

# This is required in order to append /lib/cmake to each element in CMAKE_PREFIX_PATH
set(AWS_MODULE_DIR "/${CMAKE_INSTALL_LIBDIR}/cmake")
string(REPLACE ";" "${AWS_MODULE_DIR};" AWS_MODULE_PATH "${CMAKE_PREFIX_PATH}${AWS_MODULE_DIR}")
# Append that generated list to the module search path
list(APPEND CMAKE_MODULE_PATH ${AWS_MODULE_PATH})

if (NOT CMAKE_BUILD_TYPE)
    if (NOT WIN32)
        set(CMAKE_BUILD_TYPE "RelWithDebInfo")
    endif()
endif()

# Tell the CRT the version of the SDK for the metrics string
if (DEFINED FULL_VERSION_METRIC)
    # Remove the "dev+" part if it exists
    string(REPLACE "dev+" "" FULL_VERSION_METRIC ${FULL_VERSION})
    set(AWS_IOT_SDK_VERSION v${FULL_VERSION_METRIC})
else()
    set(AWS_IOT_SDK_VERSION v${SIMPLE_VERSION})
endif()

if (BUILD_DEPS)
    if (USE_EXTERNAL_DEPS_SOURCES)
        message(FATAL_ERROR "USE_EXTERNAL_DEPS_SOURCES option should be used with BUILD_DEPS set to OFF")
    endif()

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
    if (USE_EXTERNAL_DEPS_SOURCES)
        function(validate_deps_sources dep_name)
            if (NOT ${dep_name}_SOURCE_DIR)
                message(FATAL_ERROR "USE_EXTERNAL_DEPS_SOURCES option is set, but ${dep_name} project is not configured."
                    " ${dep_name} must be added using add_subdirectory command (or one of the higher-level commands that"
                    " uses add_subdirectory, like FetchContent)")
            endif()
        endfunction()

        validate_deps_sources(aws-crt-cpp)
        validate_deps_sources(aws-c-iot)
        validate_deps_sources(aws-c-common)

        list(APPEND CMAKE_MODULE_PATH "${aws-c-common_SOURCE_DIR}/cmake")

        set(IN_SOURCE_BUILD ON)
    else()
        set(IN_SOURCE_BUILD OFF)
    endif()

    include(AwsFindPackage)
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
