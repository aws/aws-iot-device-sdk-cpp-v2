# This file is generated

cmake_minimum_required(VERSION 3.9...3.31)

project(IotCommand-cpp LANGUAGES CXX VERSION ${SIMPLE_VERSION})

set(GENERATED_ROOT_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(GENERATED_INCLUDE_DIR "${GENERATED_ROOT_DIR}/include")
set(GENERATED_CONFIG_HEADER "${GENERATED_INCLUDE_DIR}/aws/iotcommand/Config.h")
configure_file(include/aws/iotcommand/Config.h.in ${GENERATED_CONFIG_HEADER} @ONLY)

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()

file(GLOB AWS_IOTCOMMAND_HEADERS
        "include/aws/iotcommand/*.h"
        ${GENERATED_CONFIG_HEADER}
)

file(GLOB AWS_IOTCOMMAND_SRC
       "source/*.cpp"
)

file(GLOB AWS_IOTCOMMAND_CPP_SRC
        ${AWS_IOTCOMMAND_SRC}
)

if (WIN32)
    if (MSVC)
        source_group("Header Files\\aws\\iotcommand\\" FILES ${AWS_IOTCOMMAND_HEADERS})

        source_group("Source Files" FILES ${AWS_IOTCOMMAND_SRC})
    endif ()
endif()

add_library(IotCommand-cpp ${AWS_IOTCOMMAND_CPP_SRC})

set_target_properties(IotCommand-cpp PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_C_FLAGS_DEBUGOPT "")

#set warnings
if (MSVC)
    target_compile_options(IotCommand-cpp PRIVATE /W4 /WX)
else ()
    target_compile_options(IotCommand-cpp PRIVATE -Wall -Wno-long-long -pedantic -Werror)
endif ()

target_compile_definitions(IotCommand-cpp PRIVATE $<$<CONFIG:Debug>:DEBUG_BUILD>)

if (BUILD_SHARED_LIBS)
    target_compile_definitions(IotCommand-cpp PUBLIC "-DAWS_IOTCOMMAND_USE_IMPORT_EXPORT")
    target_compile_definitions(IotCommand-cpp PRIVATE "-DAWS_IOTCOMMAND_EXPORTS")

    install(TARGETS IotCommand-cpp
            EXPORT IotCommand-cpp-targets
            ARCHIVE
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT Development
            LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            NAMELINK_SKIP
            COMPONENT Runtime
            RUNTIME
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT Runtime)

    install(TARGETS IotCommand-cpp
            EXPORT IotCommand-cpp-targets
            LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            NAMELINK_ONLY
            COMPONENT Development)
else()
    install(TARGETS IotCommand-cpp
            EXPORT IotCommand-cpp-targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT Development)
endif()

target_include_directories(IotCommand-cpp PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

if (BUILD_DEPS)
    if (NOT IS_SUBDIRECTORY_INCLUDE)
        aws_use_package(aws-crt-cpp)
    endif()
endif()

target_link_libraries(IotCommand-cpp ${DEP_AWS_LIBS})

install(FILES ${AWS_IOTCOMMAND_HEADERS} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/aws/iotcommand/" COMPONENT Development)

if (BUILD_SHARED_LIBS)
    set(TARGET_DIR "shared")
else()
    set(TARGET_DIR "static")
endif()

include(CMakePackageConfigHelpers)
if (DEFINED SIMPLE_VERSION)
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/iotcommand-cpp-config-version.cmake"
        COMPATIBILITY SameMajorVersion
    )
endif()

install(EXPORT "IotCommand-cpp-targets"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/IotCommand-cpp/${TARGET_DIR}"
        NAMESPACE AWS::
        COMPONENT Development)

configure_file("cmake/iotcommand-cpp-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/iotcommand-cpp-config.cmake"
        @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/iotcommand-cpp-config.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/IotCommand-cpp/"
        COMPONENT Development)

if(NOT CMAKE_CROSSCOMPILING)
    if (BUILD_TESTING AND NOT BYO_CRYPTO)
        add_subdirectory(tests)
    endif()
endif()
