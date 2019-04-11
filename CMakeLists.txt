cmake_minimum_required(VERSION 3.1)
project(aws-iot-device-sdk-cpp-v2 CXX)

if (UNIX AND NOT APPLE)
    include(GNUInstallDirs)
elseif(NOT DEFINED CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR "lib")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_PREFIX_PATH}/${CMAKE_INSTALL_LIBDIR}/cmake")

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()

set(IS_SUBDIRECTORY_INCLUDE true)
find_package(aws-crt-cpp REQUIRED)

add_subdirectory(jobs)
add_subdirectory(shadow)
add_subdirectory(discovery)
add_subdirectory(samples/jobs/describe_job_execution)
add_subdirectory(samples/shadow/shadow_sync)

