project(shadow-sync CXX)

cmake_minimum_required(VERSION 3.1)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/lib/cmake")

include(AwsFindPackage)

file(GLOB SHADOW_SYNC_SRC
       "*.cpp"
)

add_executable(${PROJECT_NAME} ${SHADOW_SYNC_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES
    LINKER_LANGUAGE CXX
    CXX_STANDARD 11)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

#set warnings
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wno-long-long -pedantic -Werror)
endif ()

find_package(aws-crt-cpp REQUIRED)
find_library (
    SHADOW_LIB
    NAMES IotShadow-cpp
)

target_link_libraries(${PROJECT_NAME} AWS::aws-crt-cpp ${SHADOW_LIB})
