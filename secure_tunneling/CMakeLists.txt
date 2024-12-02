cmake_minimum_required(VERSION 3.9)

project(IotSecureTunneling-cpp LANGUAGES CXX VERSION ${SIMPLE_VERSION})

set(RUNTIME_DIRECTORY bin)

if (UNIX AND NOT APPLE)
    include(GNUInstallDirs)
elseif(NOT DEFINED CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR "lib")

    if (${CMAKE_INSTALL_LIBDIR} STREQUAL "lib64")
        set(FIND_LIBRARY_USE_LIB64_PATHS true)
    endif()
endif()

set(GENERATED_ROOT_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(GENERATED_INCLUDE_DIR "${GENERATED_ROOT_DIR}/include")
set(GENERATED_CONFIG_HEADER "${GENERATED_INCLUDE_DIR}/aws/iotsecuretunneling/Config.h")
configure_file(include/aws/iotsecuretunneling/Config.h.in ${GENERATED_CONFIG_HEADER} @ONLY)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_PREFIX_PATH}/${CMAKE_INSTALL_LIBDIR}/cmake")

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

add_library(${PROJECT_NAME} ${AWS_IOTSECURETUNNELING_CPP_SRC})

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
    target_compile_definitions(${PROJECT_NAME} PUBLIC "-DAWS_IOTSECURETUNNELING_USE_IMPORT_EXPORT")
    target_compile_definitions(${PROJECT_NAME} PRIVATE "-DAWS_IOTSECURETUNNELING_EXPORTS")

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

target_link_libraries(${PROJECT_NAME} PUBLIC IotDeviceCommon-cpp)

install(FILES ${AWS_IOTSECURETUNNELING_HEADERS} DESTINATION "include/aws/iotsecuretunneling/" COMPONENT Development)

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

install(EXPORT "${PROJECT_NAME}-targets"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}/cmake/${TARGET_DIR}"
        NAMESPACE AWS::
        COMPONENT Development)

configure_file("cmake/iotsecuretunneling-cpp-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/iotsecuretunneling-cpp-config.cmake"
        @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/iotsecuretunneling-cpp-config.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}/cmake/"
        COMPONENT Development)
