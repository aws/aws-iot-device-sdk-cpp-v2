cmake_minimum_required(VERSION 3.1)
project(aws-iot-device-sdk-cpp-v2)

include(CTest)

option(BUILD_DEPS "Builds aws common runtime dependencies as part of build, only do this if you don't want to control your dependency chain." OFF)
option(BUILD_SAMPLES "Build samples as part of the build" ON)

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

if (NOT DEFINED CMAKE_BUILD_TYPE)
    if (NOT WIN32)
        set(CMAKE_BUILD_TYPE "RelWithDebInfo")
    endif()
endif()

if (BUILD_DEPS)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/aws-common-runtime/aws-crt-cpp/aws-common-runtime/aws-c-common/cmake")

    include(AwsFindPackage)
    set(IN_SOURCE_BUILD ON)
    set(BUILD_TESTING_PREV ${BUILD_TESTING})
    set(BUILD_TESTING OFF)
    add_subdirectory(aws-common-runtime/aws-crt-cpp)
    set(BUILD_TESTING ${BUILD_TESTING_PREV})
else()
    include(AwsFindPackage)
    set(IN_SOURCE_BUILD OFF)
endif()

add_subdirectory(jobs)
add_subdirectory(shadow)
add_subdirectory(discovery)
add_subdirectory(identity)

if (BUILD_SAMPLES)
    add_subdirectory(samples/identity/fleet_provisioning)
    add_subdirectory(samples/mqtt/basic_pub_sub)
    add_subdirectory(samples/mqtt/raw_pub_sub)
    add_subdirectory(samples/jobs/describe_job_execution)
    add_subdirectory(samples/shadow/shadow_sync)
    add_subdirectory(samples/greengrass/basic_discovery)
endif()
