project(describe-job-execution CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/lib/cmake")

file(GLOB JOBS_SRC
       "*.cpp"
)

set(JOBS_PROJECT_NAME describe-job-execution)
add_executable(${JOBS_PROJECT_NAME} ${JOBS_SRC})
set_target_properties(${JOBS_PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_C_FLAGS_DEBUGOPT "")

#set warnings
if (MSVC)
    target_compile_options(${JOBS_PROJECT_NAME} PRIVATE /W4 /WX)
else ()
    target_compile_options(${JOBS_PROJECT_NAME} PRIVATE -Wall -Wno-long-long -pedantic -Werror)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "" OR CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${JOBS_PROJECT_NAME} PRIVATE "-DDEBUG_BUILD")
endif ()

target_include_directories(${JOBS_PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

target_link_libraries(${JOBS_PROJECT_NAME} IotJobs-cpp)

