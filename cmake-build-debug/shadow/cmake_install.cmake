# Install script for directory: /home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/shadow

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/local/ANT/henso/Source/local-c-install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xRuntimex" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libIotShadow-cpp.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libIotShadow-cpp.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libIotShadow-cpp.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/cmake-build-debug/shadow/libIotShadow-cpp.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libIotShadow-cpp.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libIotShadow-cpp.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libIotShadow-cpp.so"
         OLD_RPATH "/home/local/ANT/henso/Source/local-c-install/lib:/usr/local/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libIotShadow-cpp.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/aws/iotshadow" TYPE FILE FILES
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/shadow/include/aws/iotshadow/DeleteShadowRequest.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/shadow/include/aws/iotshadow/DeleteShadowResponse.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/shadow/include/aws/iotshadow/DeleteShadowSubscriptionRequest.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/shadow/include/aws/iotshadow/ErrorResponse.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/shadow/include/aws/iotshadow/Exports.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/shadow/include/aws/iotshadow/GetShadowRequest.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/shadow/include/aws/iotshadow/GetShadowResponse.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/shadow/include/aws/iotshadow/GetShadowSubscriptionRequest.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/shadow/include/aws/iotshadow/IotShadowClient.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/shadow/include/aws/iotshadow/ShadowDeltaUpdatedEvent.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/shadow/include/aws/iotshadow/ShadowDeltaUpdatedSubscriptionRequest.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/shadow/include/aws/iotshadow/ShadowMetadata.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/shadow/include/aws/iotshadow/ShadowState.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/shadow/include/aws/iotshadow/ShadowStateWithDelta.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/shadow/include/aws/iotshadow/ShadowUpdatedEvent.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/shadow/include/aws/iotshadow/ShadowUpdatedSnapshot.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/shadow/include/aws/iotshadow/ShadowUpdatedSubscriptionRequest.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/shadow/include/aws/iotshadow/UpdateShadowRequest.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/shadow/include/aws/iotshadow/UpdateShadowResponse.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/shadow/include/aws/iotshadow/UpdateShadowSubscriptionRequest.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/IotShadow-cpp/cmake/IotShadow-cpp-targets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/IotShadow-cpp/cmake/IotShadow-cpp-targets.cmake"
         "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/cmake-build-debug/shadow/CMakeFiles/Export/lib/IotShadow-cpp/cmake/IotShadow-cpp-targets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/IotShadow-cpp/cmake/IotShadow-cpp-targets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/IotShadow-cpp/cmake/IotShadow-cpp-targets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/IotShadow-cpp/cmake" TYPE FILE FILES "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/cmake-build-debug/shadow/CMakeFiles/Export/lib/IotShadow-cpp/cmake/IotShadow-cpp-targets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/IotShadow-cpp/cmake" TYPE FILE FILES "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/cmake-build-debug/shadow/CMakeFiles/Export/lib/IotShadow-cpp/cmake/IotShadow-cpp-targets-debug.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/IotShadow-cpp/cmake" TYPE FILE FILES "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/cmake-build-debug/shadow/IotShadow-cpp-config.cmake")
endif()

