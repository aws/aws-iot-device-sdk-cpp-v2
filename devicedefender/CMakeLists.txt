cmake_minimum_required(VERSION 3.9...3.31)

project(IotDeviceDefender-cpp LANGUAGES CXX VERSION ${SIMPLE_VERSION})

set(GENERATED_ROOT_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(GENERATED_INCLUDE_DIR "${GENERATED_ROOT_DIR}/include")
set(GENERATED_CONFIG_HEADER "${GENERATED_INCLUDE_DIR}/aws/iotdevicedefender/Config.h")
configure_file(include/aws/iotdevicedefender/Config.h.in ${GENERATED_CONFIG_HEADER} @ONLY)

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()

file(GLOB AWS_IOTDEVICEDEFENDER_HEADERS
        "include/aws/iotdevicedefender/*.h"
        ${GENERATED_CONFIG_HEADER}
        )

file(GLOB AWS_IOTDEVICEDEFENDER_SRC
        "source/*.cpp"
        )

file(GLOB AWS_IOTDEVICEDEFENDER_CPP_SRC
        ${AWS_IOTDEVICEDEFENDER_SRC}
        )

if (WIN32)
    if (MSVC)
        source_group("Header Files\\aws\\iotdevicedefender\\" FILES ${AWS_IOTDEVICEDEFENDER_HEADERS})

        source_group("Source Files" FILES ${AWS_IOTDEVICEDEFENDER_SRC})
    endif ()
endif()

add_library(IotDeviceDefender-cpp ${AWS_IOTDEVICEDEFENDER_CPP_SRC})

set_target_properties(IotDeviceDefender-cpp PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_C_FLAGS_DEBUGOPT "")

#set warnings
if (MSVC)
    target_compile_options(IotDeviceDefender-cpp PRIVATE /W4 /WX)
else ()
    target_compile_options(IotDeviceDefender-cpp PRIVATE -Wall -Wno-long-long -pedantic -Werror)
endif ()

target_compile_definitions(IotDeviceDefender-cpp PRIVATE $<$<CONFIG:Debug>:DEBUG_BUILD>)

if (BUILD_SHARED_LIBS)
    target_compile_definitions(IotDeviceDefender-cpp PUBLIC "-DAWS_IOTDEVICEDEFENDER_USE_IMPORT_EXPORT")
    target_compile_definitions(IotDeviceDefender-cpp PRIVATE "-DAWS_IOTDEVICEDEFENDER_EXPORTS")

    install(TARGETS IotDeviceDefender-cpp
            EXPORT IotDeviceDefender-cpp-targets
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

    install(TARGETS IotDeviceDefender-cpp
            EXPORT IotDeviceDefender-cpp-targets
            LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            NAMELINK_ONLY
            COMPONENT Development)
else()
    install(TARGETS IotDeviceDefender-cpp
            EXPORT IotDeviceDefender-cpp-targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT Development)
endif()

target_include_directories(IotDeviceDefender-cpp PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

if (BUILD_DEPS)
	if (NOT IS_SUBDIRECTORY_INCLUDE)
		aws_use_package(aws-crt-cpp)
        aws_use_package(aws-c-iot)
    endif()
endif()

aws_add_sanitizers(IotDeviceDefender-cpp)

target_link_libraries(IotDeviceDefender-cpp PUBLIC IotDeviceCommon-cpp)

install(FILES ${AWS_IOTDEVICEDEFENDER_HEADERS} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/aws/iotdevicedefender/" COMPONENT Development)

if (BUILD_SHARED_LIBS)
    set(TARGET_DIR "shared")
else()
    set(TARGET_DIR "static")
endif()

include(CMakePackageConfigHelpers)
if (DEFINED SIMPLE_VERSION)
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/iotdevicedefender-cpp-config-version.cmake"
        COMPATIBILITY SameMajorVersion
    )
endif()

install(EXPORT "IotDeviceDefender-cpp-targets"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/IotDeviceDefender-cpp/${TARGET_DIR}"
        NAMESPACE AWS::
        COMPONENT Development)

configure_file("cmake/iotdevicedefender-cpp-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/iotdevicedefender-cpp-config.cmake"
        @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/iotdevicedefender-cpp-config.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/IotDeviceDefender-cpp/"
        COMPONENT Development)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()
