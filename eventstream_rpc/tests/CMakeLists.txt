include(AwsTestHarness)
enable_testing()
include(CTest)

option(AWS_CRT_DISABLE_DEPRECATION_WARNINGS "Set this to silence [[deprecated]] warnings coming from aws-crt-cpp headers" OFF)
if(AWS_CRT_DISABLE_DEPRECATION_WARNINGS)
    add_compile_definitions(AWS_CRT_DISABLE_DEPRECATION_WARNINGS)
endif()

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

# Connectivity tests
add_test_case(EventStreamClientCreateFailureInvalidHost)
add_test_case(EventStreamClientCreateFailureInvalidPort)
add_test_case(EventStreamConnectSuccess)
add_test_case(EventStreamConnectFailureNoAuthHeader)
add_test_case(EventStreamConnectFailureBadAuthHeader)
add_test_case(EchoClientConnectSuccess)
add_test_case(EchoClientDoubleClose)
add_test_case(EchoClientMultiConnectSuccessFail)
add_test_case(EchoClientReconnect)
add_test_case(EchoClientCloseWhileConnecting)
add_test_case(EchoClientConnectWhileClosing)
add_test_case(EchoClientOpenCloseStress)

# Basic non-streaming operation tests
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
add_test_case(EchoClientOperationGetAllProductsSuccess)
add_test_case(EchoClientOperationGetAllCustomersSuccess)
add_test_case(EchoClientOperationCauseServiceErrorSuccess)
add_test_case(EchoClientOperationEchoFailureNeverConnected)
add_test_case(EchoClientOperationEchoFailureDisconnected)

# Non-streaming race condition tests
add_test_case(EchoClientOperationUnactivatedShutdown)
add_test_case(EchoClientOperationUnactivatedClose)
add_test_case(EchoClientOperationUnactivatedCloseDropFuture)
add_test_case(EchoClientOperationActivateActivate)
add_test_case(EchoClientOperationActivateWaitActivate)
add_test_case(EchoClientOperationActivateCloseActivate)
add_test_case(EchoClientOperationActivateClosedActivate)
add_test_case(EchoClientOperationActivateCloseConnection)
add_test_case(EchoClientOperationActivateDoubleCloseContinuation)
add_test_case(EchoClientOperationActivateWaitDoubleCloseContinuation)
add_test_case(EchoClientOperationActivateWaitCloseContinuationWaitCloseContinuation)
add_test_case(EchoClientOperationActivateShutdown)
add_test_case(EchoClientOperationActivateShutdownDropFuture)
add_test_case(EchoClientOperationActivateWaitCloseShutdown)
add_test_case(EchoClientOperationActivateWaitCloseShutdownDropFuture)

# streaming echo messages tests
add_test_case(EchoClientStreamingOperationEchoSuccessString)
add_test_case(EchoClientStreamingOperationEchoSuccessBoolean)
add_test_case(EchoClientStreamingOperationEchoSuccessTime)
add_test_case(EchoClientStreamingOperationEchoSuccessDocument)
add_test_case(EchoClientStreamingOperationEchoSuccessEnum)
add_test_case(EchoClientStreamingOperationEchoSuccessBlob)
add_test_case(EchoClientStreamingOperationEchoSuccessStringList)
add_test_case(EchoClientStreamingOperationEchoSuccessPairList)
add_test_case(EchoClientStreamingOperationEchoSuccessProductMap)

# streaming failure/error tests
add_test_case(EchoClientStreamingOperationCauseServiceError)

# streaming race condition tests
add_test_case(EchoClientStreamingOperationSendCloseOperation)
add_test_case(EchoClientStreamingOperationSendDropOperation)
add_test_case(EchoClientStreamingOperationSendCloseConnection)
add_test_case(EchoClientStreamingOperationUnactivatedSend)
add_test_case(EchoClientOperationStress)

generate_cpp_test_driver(${TEST_BINARY_NAME})
aws_add_sanitizers(${TEST_BINARY_NAME})
target_include_directories(${TEST_BINARY_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
