include(CMakeFindDependencyMacro)

find_dependency(aws-crt-cpp)

include(${CMAKE_CURRENT_LIST_DIR}/IotJobs-cpp-targets.cmake)