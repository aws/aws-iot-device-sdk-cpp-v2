# This file is generated

cmake_minimum_required(VERSION 3.9...3.31)

project(IotIdentity-cpp LANGUAGES CXX VERSION ${SIMPLE_VERSION})

set(GENERATED_ROOT_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(GENERATED_INCLUDE_DIR "${GENERATED_ROOT_DIR}/include")
set(GENERATED_CONFIG_HEADER "${GENERATED_INCLUDE_DIR}/aws/iotidentity/Config.h")
configure_file(include/aws/iotidentity/Config.h.in ${GENERATED_CONFIG_HEADER} @ONLY)

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()

file(GLOB AWS_IOTIDENTITY_HEADERS
        "include/aws/iotidentity/*.h"
        ${GENERATED_CONFIG_HEADER}
)

file(GLOB AWS_IOTIDENTITY_SRC
       "source/*.cpp"
)

file(GLOB AWS_IOTIDENTITY_CPP_SRC
        ${AWS_IOTIDENTITY_SRC}
)

if (WIN32)
    if (MSVC)
        source_group("Header Files\\aws\\iotidentity\\" FILES ${AWS_IOTIDENTITY_HEADERS})

        source_group("Source Files" FILES ${AWS_IOTIDENTITY_SRC})
    endif ()
endif()

add_library(IotIdentity-cpp ${AWS_IOTIDENTITY_CPP_SRC})

set_target_properties(IotIdentity-cpp PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_C_FLAGS_DEBUGOPT "")

#set warnings
if (MSVC)
    target_compile_options(IotIdentity-cpp PRIVATE /W4 /WX)
else ()
    target_compile_options(IotIdentity-cpp PRIVATE -Wall -Wno-long-long -pedantic -Werror)
endif ()

target_compile_definitions(IotIdentity-cpp PRIVATE $<$<CONFIG:Debug>:DEBUG_BUILD>)

if (BUILD_SHARED_LIBS)
    target_compile_definitions(IotIdentity-cpp PUBLIC "-DAWS_IOTIDENTITY_USE_IMPORT_EXPORT")
    target_compile_definitions(IotIdentity-cpp PRIVATE "-DAWS_IOTIDENTITY_EXPORTS")

    install(TARGETS IotIdentity-cpp
            EXPORT IotIdentity-cpp-targets
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

    install(TARGETS IotIdentity-cpp
            EXPORT IotIdentity-cpp-targets
            LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            NAMELINK_ONLY
            COMPONENT Development)
else()
    install(TARGETS IotIdentity-cpp
            EXPORT IotIdentity-cpp-targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT Development)
endif()

target_include_directories(IotIdentity-cpp PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

if (BUILD_DEPS)
    if (NOT IS_SUBDIRECTORY_INCLUDE)
        aws_use_package(aws-crt-cpp)
    endif()
endif()

aws_add_sanitizers(IotIdentity-cpp)
target_link_libraries(IotIdentity-cpp PUBLIC ${DEP_AWS_LIBS})

install(FILES ${AWS_IOTIDENTITY_HEADERS} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/aws/iotidentity/" COMPONENT Development)

if (BUILD_SHARED_LIBS)
    set(TARGET_DIR "shared")
else()
    set(TARGET_DIR "static")
endif()

include(CMakePackageConfigHelpers)
if (DEFINED SIMPLE_VERSION)
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/iotidentity-cpp-config-version.cmake"
        COMPATIBILITY SameMajorVersion
    )
endif()

install(EXPORT "IotIdentity-cpp-targets"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/IotIdentity-cpp/${TARGET_DIR}"
        NAMESPACE AWS::
        COMPONENT Development)

configure_file("cmake/iotidentity-cpp-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/iotidentity-cpp-config.cmake"
        @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/iotidentity-cpp-config.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/IotIdentity-cpp/"
        COMPONENT Development)

if(NOT CMAKE_CROSSCOMPILING)
    if (BUILD_TESTING AND NOT BYO_CRYPTO)
        add_subdirectory(tests)
    endif()
endif()
