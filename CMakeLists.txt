cmake_minimum_required(VERSION 3.1)
project(aws-iot-device-sdk-cpp-v2 CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/lib/cmake")

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()

file(GLOB AWS_IOTSDK_HEADERS
        "include/aws/iotsdk/*.h"
)

file(GLOB AWS_IOTSDK_JOBS_HEADERS
        "include/aws/iotsdk/jobs/*.h"
)

file(GLOB AWS_IOTSDK_CPP_HEADERS
        ${AWS_IOTSDK_HEADERS}
        ${AWS_IOTSDK_JOBS_HEADERS})

file(GLOB AWS_IOTSDK_SRC 
       "source/*.cpp"
)

file (GLOB AWS_IOTSDK_JOBS_SRC
        "source/jobs/*.cpp"
)

file(GLOB AWS_IOTSDK_CPP_SRC
        ${AWS_IOTSDK_SRC}
        ${AWS_IOTSDK_JOBS_SRC}
)

if (WIN32)
    if (MSVC)
        source_group("Header Files\\aws\\iotsdk\\" FILES ${AWS_IOTSDK_HEADERS})
        source_group("Header Files\\aws\\iotsdk\\jobs" FILES ${AWS_IOTSDK_JOBS_HEADERS})

        source_group("Source Files" FILES ${AWS_IOTSDK_SRC})
        source_group("Source Files\\external" FILES ${AWS_IOTSDK_EXTERNAL_SRC})
        source_group("Source Files\\jobs" FILES ${AWS_IOTSDK_JOBS_SRC})
    endif ()
endif()

add_library(${CMAKE_PROJECT_NAME} ${AWS_IOTSDK_CPP_SRC})

if (BUILD_SHARED_LIBS AND WIN32)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC -DAWS_IOTSDK_USE_IMPORT_EXPORT)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE -DAWS_IOTSDK_EXPORTS)
endif()

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_C_FLAGS_DEBUGOPT "")

#set warnings
if (MSVC)
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /W4 /WX)
else ()
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall -Wno-long-long -pedantic -Werror)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "" OR CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE "-DDEBUG_BUILD")
endif ()

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

find_package(aws-crt-cpp REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} AWS::aws-crt-cpp)

install(FILES ${AWS_IOTSDK_HEADERS} DESTINATION "include/aws/iotsdk")

install(
        TARGETS ${CMAKE_PROJECT_NAME}
        EXPORT ${CMAKE_PROJECT_NAME}-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

install(EXPORT "${CMAKE_PROJECT_NAME}-targets"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_PROJECT_NAME}/cmake/"
        NAMESPACE AWS::)

configure_file("cmake/${CMAKE_PROJECT_NAME}-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}-config.cmake"
        @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}-config.cmake"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_PROJECT_NAME}/cmake/")

add_subdirectory(samples/jobs/describe_job_execution)
