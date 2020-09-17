if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()

cmake_minimum_required(VERSION 3.1)

project(shadow-sync CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/lib/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

file(GLOB SHADOW_SYNC_SRC
       "*.cpp"
)

set(SHADOW_SYNC_PROJECT_NAME shadow-sync)
add_executable(${SHADOW_SYNC_PROJECT_NAME} ${SHADOW_SYNC_SRC})
set_target_properties(${SHADOW_SYNC_PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

#set warnings
if (MSVC)
    target_compile_options(${SHADOW_SYNC_PROJECT_NAME} PRIVATE /W4 /WX)
else ()
    target_compile_options(${SHADOW_SYNC_PROJECT_NAME} PRIVATE -Wall -Wno-long-long -pedantic -Werror)
endif ()

target_include_directories(${SHADOW_SYNC_PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

find_package(aws-crt-cpp REQUIRED)

find_library (
    SHADOW_LIB
    NAMES IotShadow-cpp
)
set(DEP_AWS_LIBS ${DEP_AWS_LIBS} AWS::aws-crt-cpp)

target_link_libraries(${SHADOW_SYNC_PROJECT_NAME} ${DEP_AWS_LIBS} ${SHADOW_LIB})
