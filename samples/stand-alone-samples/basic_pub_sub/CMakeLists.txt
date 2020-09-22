cmake_minimum_required(VERSION 3.1)
project(basic-pub-sub CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/lib/cmake")

file(GLOB PUB_SUB_SRC
       "*.cpp"
)

add_executable(${PROJECT_NAME} ${PUB_SUB_SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES
    LINKER_LANGUAGE CXX
    CXX_STANDARD 11)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

#set warnings
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX /wd4068)
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wno-long-long -pedantic -Werror)
endif ()

find_package(aws-crt-cpp REQUIRED)

target_link_libraries(${PROJECT_NAME} AWS::aws-crt-cpp)
