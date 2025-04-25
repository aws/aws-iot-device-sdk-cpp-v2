include(AwsTestHarness)
enable_testing()
include(CTest)

file(GLOB AWS_ECHOTESTRPC_HEADERS
        "include/awstest/*.h"
)

file(GLOB AWS_ECHOTESTRPC_SRC
        "EchoTestRpcClient.cpp"
        "EchoTestRpcModel.cpp"
)

if (WIN32)
    if (MSVC)
        source_group("Header Files\\awstest\\" FILES ${AWS_ECHOTESTRPC_HEADERS})
        source_group("Source Files" FILES ${AWS_ECHOTESTRPC_SRC})
    endif ()
endif()

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()

install(FILES ${AWS_ECHOTESTRPC_HEADERS} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/awstest/" COMPONENT Development)

file(GLOB TEST_SRC "*.cpp")
file(GLOB TEST_HDRS "*.h")
file(GLOB TESTS ${TEST_HDRS} ${TEST_SRC})

set(TEST_BINARY_NAME ${PROJECT_NAME}-tests)

add_test_case(EventStreamConnectSuccess)
add_test_case(EventStreamConnectFailureNoAuthHeader)
add_test_case(EventStreamConnectFailureBadAuthHeader)

add_test_case(EchoClientConnectSuccess)
add_test_case(EchoClientDoubleClose)
add_test_case(EchoClientMultiConnectSuccessFail)

add_test_case(EchoClientOperationEchoSuccessString)
add_test_case(EchoClientOperationEchoSuccessBoolean)
add_test_case(EchoClientOperationEchoSuccessTime)
add_test_case(EchoClientOperationEchoSuccessDocument)
add_test_case(EchoClientOperationEchoSuccessEnum)
add_test_case(EchoClientOperationEchoSuccessBlob)
add_test_case(EchoClientOperationEchoSuccessStringList)
add_test_case(EchoClientOperationEchoSuccessPairList)
add_test_case(EchoClientOperationEchoSuccessProductMap)

add_test_case(EchoClientOperationEchoSuccessMultiple)
add_test_case(EchoClientOperationEchoFailureNeverConnected)
add_test_case(EchoClientOperationEchoFailureDisconnected)



#add_test_case(StressTestClient)
generate_cpp_test_driver(${TEST_BINARY_NAME})
aws_add_sanitizers(${TEST_BINARY_NAME})
target_include_directories(${TEST_BINARY_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
