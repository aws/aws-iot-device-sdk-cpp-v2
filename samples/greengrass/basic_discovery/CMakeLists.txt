project(basic-discovery CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/lib/cmake")

file(GLOB BASIC_DISCOVERY_SRC
       "*.cpp"
)

set(BASIC_DISCOVERY_PROJECT_NAME basic-discovery)
add_executable(${BASIC_DISCOVERY_PROJECT_NAME} ${BASIC_DISCOVERY_SRC})
set_target_properties(${BASIC_DISCOVERY_PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_C_FLAGS_DEBUGOPT "")

#set warnings
if (MSVC)
    target_compile_options(${BASIC_DISCOVERY_PROJECT_NAME} PRIVATE /W4 /WX)
else ()
    target_compile_options(${BASIC_DISCOVERY_PROJECT_NAME} PRIVATE -Wall -Wno-long-long -pedantic -Werror)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "" OR CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${BASIC_DISCOVERY_PROJECT_NAME} PRIVATE "-DDEBUG_BUILD")
endif ()

target_include_directories(${BASIC_DISCOVERY_PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

target_link_libraries(${BASIC_DISCOVERY_PROJECT_NAME} Discovery-cpp)
