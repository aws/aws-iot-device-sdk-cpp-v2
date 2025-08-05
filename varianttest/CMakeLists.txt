cmake_minimum_required(VERSION 3.9...3.31)

project(VariantTest-cpp LANGUAGES CXX VERSION ${SIMPLE_VERSION})

set(GENERATED_ROOT_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(GENERATED_INCLUDE_DIR "${GENERATED_ROOT_DIR}/include")
set(GENERATED_CONFIG_HEADER "${GENERATED_INCLUDE_DIR}/aws/eventstreamrpc/Config.h")
configure_file(include/aws/varianttest/Config.h.in ${GENERATED_CONFIG_HEADER} @ONLY)

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()

file(GLOB AWS_VARIANTTEST_HEADERS
        "include/aws/varianttest/VariantTest.h"
        ${GENERATED_CONFIG_HEADER}
)

file(GLOB AWS_VARIANTTEST_SRC
       "source/*.cpp"
)

file(GLOB AWS_VARIANTTEST_CPP_SRC
        ${AWS_VARIANTTEST_SRC}
)

if (WIN32)
    if (MSVC)
        source_group("Header Files\\aws\\varianttest\\" FILES ${AWS_VARIANTTEST_HEADERS})

        source_group("Source Files" FILES ${AWS_VARIANTTESTC_SRC})
    endif ()
endif()

add_library(VariantTest-cpp ${AWS_VARIANTTEST_CPP_SRC})

set_target_properties(VariantTest-cpp PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_C_FLAGS_DEBUGOPT "")

#set warnings
if (MSVC)
    target_compile_options(VariantTest-cpp PRIVATE /W4 /WX)
else ()
    target_compile_options(VariantTest-cpp PRIVATE -Wall -Wno-long-long -pedantic -Werror)
endif ()

target_compile_definitions(VariantTest-cpp PRIVATE $<$<CONFIG:Debug>:DEBUG_BUILD>)

if (BUILD_SHARED_LIBS)
    target_compile_definitions(VariantTest-cpp PUBLIC "-DAWS_VARIANTTEST_USE_IMPORT_EXPORT")
    target_compile_definitions(VariantTest-cpp PRIVATE "-DAWS_VARIANTTEST_EXPORTS")

    install(TARGETS VariantTest-cpp
            EXPORT VariantTest-cpp-targets
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

    install(TARGETS VariantTest-cpp
            EXPORT VariantTest-cpp-targets
            LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            NAMELINK_ONLY
            COMPONENT Development)
else()
    install(TARGETS VariantTest-cpp
            EXPORT VariantTest-cpp-targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT Development)
endif()

target_include_directories(VariantTest-cpp PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

if (NOT IS_SUBDIRECTORY_INCLUDE)
    aws_use_package(aws-crt-cpp)
endif()

aws_add_sanitizers(VariantTest-cpp)
target_link_libraries(VariantTest-cpp PUBLIC ${DEP_AWS_LIBS})

install(FILES ${AWS_VARIANTTEST_HEADERS} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/aws/varianttest/" COMPONENT Development)

if (BUILD_SHARED_LIBS)
    set(TARGET_DIR "shared")
else()
    set(TARGET_DIR "static")
endif()

include(CMakePackageConfigHelpers)
if (DEFINED SIMPLE_VERSION)
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/varianttest-cpp-config-version.cmake"
        COMPATIBILITY SameMajorVersion
    )
endif()

install(EXPORT "VariantTest-cpp-targets"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/VariantTest-cpp/${TARGET_DIR}"
        NAMESPACE AWS::
        COMPONENT Development)

configure_file("cmake/varianttest-cpp-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/varianttest-cpp-config.cmake"
        @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/varianttest-cpp-config.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/VariantTest-cpp/"
        COMPONENT Development)

