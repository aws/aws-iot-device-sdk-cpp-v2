cmake_minimum_required(VERSION 3.1)

if (DEFINED SIMPLE_VERSION)
    set(_sub_project_version ${SIMPLE_VERSION})
else()
    if (EXISTS "../VERSION")
        file(READ "..//VERSION" version_simple)
        string(STRIP ${version_simple} version_simple)
        set(_sub_project_version ${version_simple})
    endif()
endif()

if (DEFINED _sub_project_version)
    message("Discovery version is ${_sub_project_version}")
    project(Discovery-cpp
            VERSION ${_sub_project_version}
            LANGUAGES CXX)
else()
    message("Discovery version is unknown")
    project(Discovery-cpp
        LANGUAGES CXX)
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

file(GLOB AWS_DISCOVERY_HEADERS
        "include/aws/discovery/*.h"
)

file(GLOB AWS_DISCOVERY_SRC
       "source/*.cpp"
)

file(GLOB AWS_DISCOVERY_CPP_SRC
        ${AWS_DISCOVERY_SRC}
)

if (WIN32)
    if (MSVC)
        source_group("Header Files\\aws\\discovery\\" FILES ${AWS_DISCOVERY_HEADERS})

        source_group("Source Files" FILES ${AWS_DISCOVERY_SRC})
    endif ()
endif()

add_library(Discovery-cpp ${AWS_DISCOVERY_CPP_SRC})

set_target_properties(Discovery-cpp PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_C_FLAGS_DEBUGOPT "")

#set warnings
if (MSVC)
    target_compile_options(Discovery-cpp PRIVATE /W4 /WX)
else ()
    target_compile_options(Discovery-cpp PRIVATE -Wall -Wno-long-long -pedantic -Werror)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "" OR CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(Discovery-cpp PRIVATE "-DDEBUG_BUILD")
endif ()

if (BUILD_SHARED_LIBS)
    target_compile_definitions(Discovery-cpp PUBLIC "-DAWS_DISCOVERY_USE_IMPORT_EXPORT")
    target_compile_definitions(Discovery-cpp PRIVATE "-DAWS_DISCOVERY_EXPORTS")

    install(TARGETS Discovery-cpp
            EXPORT Discovery-cpp-targets
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

    install(TARGETS Discovery-cpp
            EXPORT Discovery-cpp-targets
            LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            NAMELINK_ONLY
            COMPONENT Development)
else()
    install(TARGETS Discovery-cpp
            EXPORT Discovery-cpp-targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT Development)
endif()

target_include_directories(Discovery-cpp PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

if (BUILD_DEPS)
	if (NOT IS_SUBDIRECTORY_INCLUDE)
		aws_use_package(aws-crt-cpp)
	endif()
endif()

target_link_libraries(Discovery-cpp ${DEP_AWS_LIBS})

install(FILES ${AWS_DISCOVERY_HEADERS} DESTINATION "include/aws/discovery/" COMPONENT Development)

if (BUILD_SHARED_LIBS)
    set (TARGET_DIR "shared")
else()
    set (TARGET_DIR "static")
endif()

include(CMakePackageConfigHelpers)
if (DEFINED _sub_project_version)
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/discovery-cpp-config-version.cmake"
        COMPATIBILITY SameMajorVersion
    )
endif()

install(EXPORT "Discovery-cpp-targets"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/Discovery-cpp/cmake/${TARGET_DIR}"
        NAMESPACE AWS::
        COMPONENT Development)

configure_file("cmake/discovery-cpp-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/discovery-cpp-config.cmake"
        @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/discovery-cpp-config.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/Discovery-cpp/cmake/"
        COMPONENT Development)
