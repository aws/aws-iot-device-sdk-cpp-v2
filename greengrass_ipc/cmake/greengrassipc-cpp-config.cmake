include(CMakeFindDependencyMacro)

find_dependency(aws-crt-cpp)
find_dependency(EventstreamRpc-cpp)

macro(aws_load_targets type)
    include(${CMAKE_CURRENT_LIST_DIR}/${type}/@PROJECT_NAME@-targets.cmake)
endmacro()

# Allow static or shared lib to be used.
# If both are installed, choose based on BUILD_SHARED_LIBS.
if (BUILD_SHARED_LIBS)
    if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/shared")
        aws_load_targets(shared)
    else()
        aws_load_targets(static)
    endif()
else()
    if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/static")
        aws_load_targets(static)
    else()
        aws_load_targets(shared)
    endif()
endif()
