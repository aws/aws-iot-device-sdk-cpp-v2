include(AwsTestHarness)
enable_testing()
include(CTest)

file(GLOB TEST_SRC "*.cpp")
file(GLOB TEST_HDRS "*.h")
file(GLOB TESTS ${TEST_HDRS} ${TEST_SRC})

set(TEST_BINARY_NAME ${PROJECT_NAME}-tests)

# Device defender is only supported on Linux
if (UNIX AND NOT APPLE)
    add_net_test_case(DeviceDefenderResourceSafety)
    add_net_test_case(DeviceDefenderFailedTest)
    add_net_test_case(DeviceDefenderCustomMetricSuccess)
    add_net_test_case(DeviceDefenderCustomMetricFail)
    add_net_test_case(Mqtt5DeviceDefenderResourceSafety)
    add_net_test_case(Mqtt5DeviceDefenderFailedTest)
    add_net_test_case(Mqtt5DeviceDefenderCustomMetricSuccess)
    add_net_test_case(Mqtt5DeviceDefenderCustomMetricFail)
    generate_cpp_test_driver(${TEST_BINARY_NAME})
endif()
