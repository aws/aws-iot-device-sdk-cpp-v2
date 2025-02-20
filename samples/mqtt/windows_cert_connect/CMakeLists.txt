cmake_minimum_required(VERSION 3.9...3.31)
# note: cxx-17 requires cmake 3.8, cxx-20 requires cmake 3.12
project(windows-cert-connect CXX)

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

find_package(aws-crt-cpp REQUIRED)

if (ENABLE_SANITIZERS AND UNIX AND NOT APPLE)
    include(GNUInstallDirs)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_PREFIX_PATH}/${CMAKE_INSTALL_LIBDIR}/cmake")
    include(AwsSanitizers)
    enable_language(C)
    aws_add_sanitizers(${PROJECT_NAME})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE AWS::aws-crt-cpp)
