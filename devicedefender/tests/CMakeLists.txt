include(AwsTestHarness)
enable_testing()
include(CTest)

file(GLOB TEST_SRC "*.cpp")
file(GLOB TEST_HDRS "*.h")
file(GLOB TESTS ${TEST_HDRS} ${TEST_SRC})

set(TEST_BINARY_NAME ${PROJECT_NAME}-tests)

aws_use_package(aws-crt-cpp)
aws_use_package(IotDeviceDefender-cpp)

if (UNIX AND NOT APPLE)
    add_net_test_case(DeviceDefenderResourceSafety)
    add_net_test_case(DeviceDefenderFailedTest)
    generate_cpp_test_driver(${TEST_BINARY_NAME})
endif()
