cmake_minimum_required(VERSION 3.9...3.31)

project(IotSecureTunneling-cpp LANGUAGES CXX VERSION ${SIMPLE_VERSION})

set(GENERATED_ROOT_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(GENERATED_INCLUDE_DIR "${GENERATED_ROOT_DIR}/include")
set(GENERATED_CONFIG_HEADER "${GENERATED_INCLUDE_DIR}/aws/iotsecuretunneling/Config.h")
configure_file(include/aws/iotsecuretunneling/Config.h.in ${GENERATED_CONFIG_HEADER} @ONLY)

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()

file(GLOB AWS_IOTSECURETUNNELING_HEADERS
        "include/aws/iotsecuretunneling/*.h"
        ${GENERATED_CONFIG_HEADER}
)

file(GLOB AWS_IOTSECURETUNNELING_SRC
       "source/*.cpp"
)

file(GLOB AWS_IOTSECURETUNNELING_CPP_SRC
        ${AWS_IOTSECURETUNNELING_SRC}
)

if (WIN32)
    if (MSVC)
        source_group("Header Files\\aws\\iotsecuretunneling\\" FILES ${AWS_IOTSECURETUNNELING_HEADERS})

        source_group("Source Files" FILES ${AWS_IOTSECURETUNNELING_SRC})
    endif ()
endif()

add_library(IotSecureTunneling-cpp ${AWS_IOTSECURETUNNELING_CPP_SRC})

set_target_properties(IotSecureTunneling-cpp PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_C_FLAGS_DEBUGOPT "")

#set warnings
if (MSVC)
    target_compile_options(IotSecureTunneling-cpp PRIVATE /W4 /WX)
else ()
    target_compile_options(IotSecureTunneling-cpp PRIVATE -Wall -Wno-long-long -pedantic -Werror)
endif ()

target_compile_definitions(IotSecureTunneling-cpp PRIVATE $<$<CONFIG:Debug>:DEBUG_BUILD>)

if (BUILD_SHARED_LIBS)
    target_compile_definitions(IotSecureTunneling-cpp PUBLIC "-DAWS_IOTSECURETUNNELING_USE_IMPORT_EXPORT")
    target_compile_definitions(IotSecureTunneling-cpp PRIVATE "-DAWS_IOTSECURETUNNELING_EXPORTS")

    install(TARGETS IotSecureTunneling-cpp
            EXPORT IotSecureTunneling-cpp-targets
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

    install(TARGETS IotSecureTunneling-cpp
            EXPORT IotSecureTunneling-cpp-targets
            LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            NAMELINK_ONLY
            COMPONENT Development)
else()
    install(TARGETS IotSecureTunneling-cpp
            EXPORT IotSecureTunneling-cpp-targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT Development)
endif()

target_include_directories(IotSecureTunneling-cpp PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

if (BUILD_DEPS)
	if (NOT IS_SUBDIRECTORY_INCLUDE)
		aws_use_package(aws-crt-cpp)
        aws_use_package(aws-c-iot)
	endif()
endif()

aws_add_sanitizers(${PROJECT_NAME})

target_link_libraries(IotSecureTunneling-cpp PUBLIC IotDeviceCommon-cpp)

install(FILES ${AWS_IOTSECURETUNNELING_HEADERS} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/aws/iotsecuretunneling/" COMPONENT Development)

if (BUILD_SHARED_LIBS)
    set(TARGET_DIR "shared")
else()
    set(TARGET_DIR "static")
endif()

include(CMakePackageConfigHelpers)
if (DEFINED SIMPLE_VERSION)
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/iotsecuretunneling-cpp-config-version.cmake"
        COMPATIBILITY SameMajorVersion
    )
endif()

install(EXPORT "IotSecureTunneling-cpp-targets"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/IotSecureTunneling-cpp/${TARGET_DIR}"
        NAMESPACE AWS::
        COMPONENT Development)

configure_file("cmake/iotsecuretunneling-cpp-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/iotsecuretunneling-cpp-config.cmake"
        @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/iotsecuretunneling-cpp-config.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/IotSecureTunneling-cpp/"
        COMPONENT Development)
