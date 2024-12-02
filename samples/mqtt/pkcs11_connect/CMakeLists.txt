cmake_minimum_required(VERSION 3.9)
# note: cxx-17 requires cmake 3.8, cxx-20 requires cmake 3.12
project(pkcs11-connect CXX)

file(GLOB SRC_FILES
       "*.cpp"
       "../../utils/CommandLineUtils.cpp"
       "../../utils/CommandLineUtils.h"
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 14)

# set warnings
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX /wd4068)
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wno-long-long -pedantic -Werror)
endif ()

if (UNIX AND NOT APPLE)
    # Hide symbols from libcrypto.a to avoid weird crashes.
    # The issue is that SoftHSM uses libcrypto.so from the system's OpenSSL installation,
    # but aws-crt-cpp is typically built with a statically linked libcrypto.a from its AWS-LC submodule.
    # If the symbols from libcrypto.a aren't hidden then SoftHSM will sometimes
    # call functions from libcrypto.so and sometimes call functions from libcrypto.a,
    # and we'll get weird crashes because the two crypto libraries have different implementations.
    set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--exclude-libs,libcrypto.a")
endif()

find_package(aws-crt-cpp REQUIRED)

if (UNIX AND NOT APPLE)
    include(GNUInstallDirs)
elseif(NOT DEFINED CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR "lib")

    if (${CMAKE_INSTALL_LIBDIR} STREQUAL "lib64")
        set(FIND_LIBRARY_USE_LIB64_PATHS true)
    endif()
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_PREFIX_PATH}/${CMAKE_INSTALL_LIBDIR}/cmake")

include(AwsSanitizers)
enable_language(C)
aws_add_sanitizers(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE AWS::aws-crt-cpp)
