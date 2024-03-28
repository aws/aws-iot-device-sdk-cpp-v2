cmake_minimum_required(VERSION 3.1)

project(IotDeviceDefender-cpp
        LANGUAGES CXX)
if (DEFINED SIMPLE_VERSION)
    message("Device Defender version is ${SIMPLE_VERSION}")
    set(PROJECT_VERSION ${SIMPLE_VERSION})
endif()

set(RUNTIME_DIRECTORY bin)

if (UNIX AND NOT APPLE)
    include(GNUInstallDirs)
elseif(NOT DEFINED CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR "lib")

    if (${CMAKE_INSTALL_LIBDIR} STREQUAL "lib64")
        set(FIND_LIBRARY_USE_LIB64_PATHS true)
    endif()
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_PREFIX_PATH}/${CMAKE_INSTALL_LIBDIR}/cmake")

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()

file(GLOB AWS_IOTDEVICEDEFENDER_HEADERS
        "include/aws/iotdevicedefender/*.h"
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

add_library(${PROJECT_NAME} ${AWS_IOTDEVICEDEFENDER_CPP_SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_C_FLAGS_DEBUGOPT "")

#set warnings
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wno-long-long -pedantic -Werror)
endif ()

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:DEBUG_BUILD>)

if (BUILD_SHARED_LIBS)
    target_compile_definitions(${PROJECT_NAME} PUBLIC "-DAWS_IOTDEVICEDEFENDER_USE_IMPORT_EXPORT")
    target_compile_definitions(${PROJECT_NAME} PRIVATE "-DAWS_IOTDEVICEDEFENDER_EXPORTS")

    install(TARGETS ${PROJECT_NAME}
            EXPORT ${PROJECT_NAME}-targets
            ARCHIVE
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT Development
            LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            NAMELINK_SKIP
            COMPONENT Runtime
            RUNTIME
            DESTINATION ${RUNTIME_DIRECTORY}
            COMPONENT Runtime)

    install(TARGETS ${PROJECT_NAME}
            EXPORT ${PROJECT_NAME}-targets
            LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            NAMELINK_ONLY
            COMPONENT Development)
else()
    install(TARGETS ${PROJECT_NAME}
            EXPORT ${PROJECT_NAME}-targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT Development)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

if (BUILD_DEPS)
	if (NOT IS_SUBDIRECTORY_INCLUDE)
		aws_use_package(aws-crt-cpp)
        aws_use_package(aws-c-iot)
    endif()
endif()

aws_add_sanitizers(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} IotDeviceCommon-cpp)

install(FILES ${AWS_IOTDEVICEDEFENDER_HEADERS} DESTINATION "include/aws/iotdevicedefender/" COMPONENT Development)

if (BUILD_SHARED_LIBS)
    set(TARGET_DIR "shared")
else()
    set(TARGET_DIR "static")
endif()

include(CMakePackageConfigHelpers)
if (DEFINED SIMPLE_VERSION)
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
        COMPATIBILITY SameMajorVersion
    )
endif()

install(EXPORT "${PROJECT_NAME}-targets"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}/cmake/${TARGET_DIR}"
        NAMESPACE AWS::
        COMPONENT Development)

configure_file("cmake/${PROJECT_NAME}-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
        @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}/cmake/"
        COMPONENT Development)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()
