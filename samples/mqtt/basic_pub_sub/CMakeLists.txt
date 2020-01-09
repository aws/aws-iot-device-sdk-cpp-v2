project(basic-pub-sub CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/lib/cmake")

file(GLOB PUB_SUB_SRC
       "*.cpp"
)

set(PUB_SUB_PROJECT_NAME basic-pub-sub)
add_executable(${PUB_SUB_PROJECT_NAME} ${PUB_SUB_SRC})
set_target_properties(${PUB_SUB_PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_C_FLAGS_DEBUGOPT "")

#set warnings
if (MSVC)
    target_compile_options(${PUB_SUB_PROJECT_NAME} PRIVATE /W4 /WX /wd4068)
else ()
    target_compile_options(${PUB_SUB_PROJECT_NAME} PRIVATE -Wall -Wno-long-long -pedantic -Werror)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "" OR CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${PUB_SUB_PROJECT_NAME} PRIVATE "-DDEBUG_BUILD")
endif ()

target_include_directories(${PUB_SUB_PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

aws_use_package(aws-crt-cpp)
target_link_libraries(${PUB_SUB_PROJECT_NAME} ${DEP_AWS_LIBS})
