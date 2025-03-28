cmake_minimum_required(VERSION 3.9...3.31)

project(GreengrassIpc-cpp LANGUAGES CXX VERSION ${SIMPLE_VERSION})

set(GENERATED_ROOT_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(GENERATED_INCLUDE_DIR "${GENERATED_ROOT_DIR}/include")
set(GENERATED_CONFIG_HEADER "${GENERATED_INCLUDE_DIR}/aws/greengrass/Config.h")
configure_file(include/aws/greengrass/Config.h.in ${GENERATED_CONFIG_HEADER} @ONLY)

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()

file(GLOB AWS_GREENGRASSIPC_HEADERS
        "include/aws/greengrass/*.h"
        ${GENERATED_CONFIG_HEADER}
)

file(GLOB AWS_GREENGRASSIPC_SRC
       "source/*.cpp"
)

file(GLOB AWS_GREENGRASSIPC_CPP_SRC
        ${AWS_GREENGRASSIPC_SRC}
)

if (WIN32)
    if (MSVC)
        source_group("Header Files\\aws\\greengrass\\" FILES ${AWS_GREENGRASSIPC_HEADERS})

        source_group("Source Files" FILES ${AWS_GREENGRASSIPC_SRC})
    endif ()
endif()

add_library(GreengrassIpc-cpp ${AWS_GREENGRASSIPC_CPP_SRC})
target_link_libraries(GreengrassIpc-cpp PUBLIC EventstreamRpc-cpp)

set_target_properties(GreengrassIpc-cpp PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_C_FLAGS_DEBUGOPT "")

#set warnings
if (MSVC)
    target_compile_options(GreengrassIpc-cpp PRIVATE /W4 /WX)
else ()
    target_compile_options(GreengrassIpc-cpp PRIVATE -Wall -Wno-long-long -pedantic -Werror)
endif ()

if (MSVC)
    # Generated code files can be very big. Set /bigobj to avoid the following error:
    # > fatal error C1128: number of sections exceeded object file format limit: compile with /bigobj
    target_compile_options(GreengrassIpc-cpp PRIVATE /bigobj)
endif ()

target_compile_definitions(GreengrassIpc-cpp PRIVATE $<$<CONFIG:Debug>:DEBUG_BUILD>)

if (BUILD_SHARED_LIBS)
    # The compile definition should match the name in Exports.h
    target_compile_definitions(GreengrassIpc-cpp PUBLIC "-DAWS_GREENGRASSIPC_USE_IMPORT_EXPORT")
    target_compile_definitions(GreengrassIpc-cpp PRIVATE "-DAWS_GREENGRASSIPC_EXPORTS")

    install(TARGETS GreengrassIpc-cpp
            EXPORT GreengrassIpc-cpp-targets
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

    install(TARGETS GreengrassIpc-cpp
            EXPORT GreengrassIpc-cpp-targets
            LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            NAMELINK_ONLY
            COMPONENT Development)
else()
    install(TARGETS GreengrassIpc-cpp
            EXPORT GreengrassIpc-cpp-targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT Development)
endif()

target_include_directories(GreengrassIpc-cpp PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

if (NOT IS_SUBDIRECTORY_INCLUDE)
    aws_use_package(aws-crt-cpp)
endif()

aws_add_sanitizers(GreengrassIpc-cpp)
target_link_libraries(GreengrassIpc-cpp PUBLIC ${DEP_AWS_LIBS})

install(FILES ${AWS_GREENGRASSIPC_HEADERS} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/aws/greengrass/" COMPONENT Development)

if (BUILD_SHARED_LIBS)
    set(TARGET_DIR "shared")
else()
    set(TARGET_DIR "static")
endif()

include(CMakePackageConfigHelpers)
if (DEFINED SIMPLE_VERSION)
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/greengrassipc-cpp-config-version.cmake"
        COMPATIBILITY SameMajorVersion
    )
endif()

install(EXPORT "GreengrassIpc-cpp-targets"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/GreengrassIpc-cpp/${TARGET_DIR}"
        NAMESPACE AWS::
        COMPONENT Development)

configure_file("cmake/greengrassipc-cpp-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/greengrassipc-cpp-config.cmake"
        @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/greengrassipc-cpp-config.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/GreengrassIpc-cpp/"
        COMPONENT Development)
