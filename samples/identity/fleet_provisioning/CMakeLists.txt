project(fleet-provisioning CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/lib/cmake")

file(GLOB IDENTITY_SRC
       "*.cpp"
)

set(IDENTITY_PROJECT_NAME fleet-provisioning)
add_executable(${IDENTITY_PROJECT_NAME} ${IDENTITY_SRC})
set_target_properties(${IDENTITY_PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_C_FLAGS_DEBUGOPT "")

#set warnings
if (MSVC)
    target_compile_options(${IDENTITY_PROJECT_NAME} PRIVATE /W4 /WX)
else ()
    target_compile_options(${IDENTITY_PROJECT_NAME} PRIVATE -Wall -Wno-long-long -pedantic -Werror)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "" OR CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${IDENTITY_PROJECT_NAME} PRIVATE "-DDEBUG_BUILD")
endif ()

target_include_directories(${IDENTITY_PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

target_link_libraries(${IDENTITY_PROJECT_NAME} IotIdentity-cpp)

