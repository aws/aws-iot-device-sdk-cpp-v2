# Install script for directory: /home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/jobs

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
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libIotJobs-cpp.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libIotJobs-cpp.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libIotJobs-cpp.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/cmake-build-debug/jobs/libIotJobs-cpp.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libIotJobs-cpp.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libIotJobs-cpp.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libIotJobs-cpp.so"
         OLD_RPATH "/home/local/ANT/henso/Source/local-c-install/lib:/usr/local/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libIotJobs-cpp.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/aws/iotjobs" TYPE FILE FILES
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/jobs/include/aws/iotjobs/DescribeJobExecutionRequest.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/jobs/include/aws/iotjobs/DescribeJobExecutionResponse.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/jobs/include/aws/iotjobs/DescribeJobExecutionSubscriptionRequest.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/jobs/include/aws/iotjobs/Exports.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/jobs/include/aws/iotjobs/GetPendingJobExecutionsRequest.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/jobs/include/aws/iotjobs/GetPendingJobExecutionsResponse.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/jobs/include/aws/iotjobs/GetPendingJobExecutionsSubscriptionRequest.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/jobs/include/aws/iotjobs/IotJobsClient.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/jobs/include/aws/iotjobs/JobExecutionData.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/jobs/include/aws/iotjobs/JobExecutionState.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/jobs/include/aws/iotjobs/JobExecutionSummary.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/jobs/include/aws/iotjobs/JobExecutionsChangedEvent.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/jobs/include/aws/iotjobs/JobExecutionsChangedSubscriptionRequest.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/jobs/include/aws/iotjobs/JobStatus.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/jobs/include/aws/iotjobs/NextJobExecutionChangedEvent.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/jobs/include/aws/iotjobs/NextJobExecutionChangedSubscriptionRequest.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/jobs/include/aws/iotjobs/RejectedError.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/jobs/include/aws/iotjobs/RejectedErrorCode.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/jobs/include/aws/iotjobs/StartNextJobExecutionResponse.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/jobs/include/aws/iotjobs/StartNextPendingJobExecutionRequest.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/jobs/include/aws/iotjobs/StartNextPendingJobExecutionSubscriptionRequest.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/jobs/include/aws/iotjobs/UpdateJobExecutionRequest.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/jobs/include/aws/iotjobs/UpdateJobExecutionResponse.h"
    "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/jobs/include/aws/iotjobs/UpdateJobExecutionSubscriptionRequest.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/IotJobs-cpp/cmake/IotJobs-cpp-targets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/IotJobs-cpp/cmake/IotJobs-cpp-targets.cmake"
         "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/cmake-build-debug/jobs/CMakeFiles/Export/lib/IotJobs-cpp/cmake/IotJobs-cpp-targets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/IotJobs-cpp/cmake/IotJobs-cpp-targets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/IotJobs-cpp/cmake/IotJobs-cpp-targets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/IotJobs-cpp/cmake" TYPE FILE FILES "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/cmake-build-debug/jobs/CMakeFiles/Export/lib/IotJobs-cpp/cmake/IotJobs-cpp-targets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/IotJobs-cpp/cmake" TYPE FILE FILES "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/cmake-build-debug/jobs/CMakeFiles/Export/lib/IotJobs-cpp/cmake/IotJobs-cpp-targets-debug.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/IotJobs-cpp/cmake" TYPE FILE FILES "/home/local/ANT/henso/Source/aws-iot-device-sdk-cpp-v2/cmake-build-debug/jobs/IotJobs-cpp-config.cmake")
endif()

