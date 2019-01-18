project(shadow-sync CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/lib/cmake")

file(GLOB SHADOW_SYNC_SRC
       "*.cpp"
)

set(SHADOW_SYNC_PROJECT_NAME shadow-sync)
add_executable(${SHADOW_SYNC_PROJECT_NAME} ${SHADOW_SYNC_SRC})
set_target_properties(${SHADOW_SYNC_PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_C_FLAGS_DEBUGOPT "")

#set warnings
if (MSVC)
    target_compile_options(${SHADOW_SYNC_PROJECT_NAME} PRIVATE /W4 /WX)
else ()
    target_compile_options(${SHADOW_SYNC_PROJECT_NAME} PRIVATE -Wall -Wno-long-long -pedantic -Werror)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "" OR CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${SHADOW_SYNC_PROJECT_NAME} PRIVATE "-DDEBUG_BUILD")
endif ()

target_include_directories(${SHADOW_SYNC_PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

target_link_libraries(${SHADOW_SYNC_PROJECT_NAME} IotShadow-cpp)

