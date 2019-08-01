cmake_minimum_required(VERSION 3.1)

option(BUILD_DEPS "Builds aws common runtime dependencies as part of build, only do this if you don't want to control your dependency chain." OFF)

if (UNIX AND NOT APPLE)
    include(GNUInstallDirs)
elseif(NOT DEFINED CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR "lib")
endif()

if (BUILD_DEPS)
    set(AWS_DEPS_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/deps CACHE STRING "If BUILD_DEPS is on, aws common runtime dependencies build in this directory.")
    if (DEFINED CMAKE_INSTALL_PREFIX)
        set(AWS_DEPS_INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
    else()
        set(AWS_DEPS_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/deps/install)
    endif()

    if (NOT DEFINED CMAKE_PREFIX_PATH)
        set(CMAKE_PREFIX_PATH ${AWS_DEPS_INSTALL_DIR})
    endif()

    file(MAKE_DIRECTORY ${AWS_DEPS_BUILD_DIR})
    if(TARGET_ARCH STREQUAL "ANDROID")
        execute_process(
                COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR}
                -DTARGET_ARCH=${TARGET_ARCH}
                -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                -DANDROID_NATIVE_API_LEVEL=${ANDROID_NATIVE_API_LEVEL}
                -DANDROID_ABI=${ANDROID_ABI}
                -DANDROID_TOOLCHAIN_NAME=${ANDROID_TOOLCHAIN_NAME}
                -DANDROID_STANDALONE_TOOLCHAIN=${ANDROID_STANDALONE_TOOLCHAIN}
                -DANDROID_STL=${ANDROID_STL}
                -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
                -DCMAKE_INSTALL_PREFIX=${AWS_DEPS_INSTALL_DIR}
                -DCMAKE_PREFIX_PATH=${AWS_DEPS_INSTALL_DIR}
                -DGIT_EXECUTABLE=${GIT_EXECUTABLE}
                -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
                ${CMAKE_CURRENT_SOURCE_DIR}/aws-common-runtime
                WORKING_DIRECTORY ${AWS_DEPS_BUILD_DIR}
        )

    else()
        execute_process(
                COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR}
                -DTARGET_ARCH=${TARGET_ARCH}
                -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
                -DCMAKE_PREFIX_PATH=${AWS_DEPS_INSTALL_DIR}
                -DCMAKE_INSTALL_PREFIX=${AWS_DEPS_INSTALL_DIR}
                -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
                -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${CMAKE_CURRENT_BINARY_DIR}/bin
                -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
                ${CMAKE_CURRENT_SOURCE_DIR}/aws-common-runtime
                WORKING_DIRECTORY ${AWS_DEPS_BUILD_DIR}
                RESULT_VARIABLE BUILD_AWSCRT_EXIT_CODE
        )
    endif()

    if (NOT ${BUILD_AWSCRT_EXIT_CODE} EQUAL 0)
        message(FATAL_ERROR "Failed to configure aws crt libraries.")
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} --build ${AWS_DEPS_BUILD_DIR} --config ${CMAKE_BUILD_TYPE}
            RESULT_VARIABLE BUILD_AWSCRT_EXIT_CODE)

    if (NOT ${BUILD_AWSCRT_EXIT_CODE} EQUAL 0)
        message(FATAL_ERROR "Failed to build aws crt libraries.")
    endif()
    message(STATUS "AWS Common Runtime is installed at: ${AWS_DEPS_INSTALL_DIR}")

    #the following two lines are done in this branch intentionally, don't move it. project() does some magic that
    #we don't want happening until we're done with the above code.
    list(APPEND CMAKE_PREFIX_PATH "${AWS_DEPS_INSTALL_DIR}")

    project(aws-iot-device-sdk-cpp-v2 CXX)

else()
    #the following two lines are done in this branch intentionally, don't move it. project() does some magic that
    #we want happening exactly right now.
    project(aws-iot-device-sdk-cpp-v2 CXX)

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
add_subdirectory(samples/greengrass/basic_discovery)


