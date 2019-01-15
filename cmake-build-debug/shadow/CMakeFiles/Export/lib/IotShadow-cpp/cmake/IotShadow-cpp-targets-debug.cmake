#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "AWS::IotShadow-cpp" for configuration "Debug"
set_property(TARGET AWS::IotShadow-cpp APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(AWS::IotShadow-cpp PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libIotShadow-cpp.so"
  IMPORTED_SONAME_DEBUG "libIotShadow-cpp.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS AWS::IotShadow-cpp )
list(APPEND _IMPORT_CHECK_FILES_FOR_AWS::IotShadow-cpp "${_IMPORT_PREFIX}/lib/libIotShadow-cpp.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
