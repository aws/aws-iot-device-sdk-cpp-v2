cmake_minimum_required(VERSION 3.9...3.31)
# note: cxx-17 requires cmake 3.8, cxx-20 requires cmake 3.12
project(fleet-provisioning-csr CXX)

file(GLOB SRC_FILES
       "*.cpp"
       "../../utils/CommandLineUtils.cpp"
       "../../utils/CommandLineUtils.h"
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 14)

#set warnings
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wno-long-long -pedantic -Werror)
endif ()

find_package(aws-crt-cpp REQUIRED)
find_package(IotIdentity-cpp REQUIRED)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

include(AwsSanitizers)
enable_language(C)
aws_add_sanitizers(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE AWS::aws-crt-cpp AWS::IotIdentity-cpp)
