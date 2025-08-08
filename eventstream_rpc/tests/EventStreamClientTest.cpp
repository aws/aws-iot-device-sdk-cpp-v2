/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>

#include <awstest/EchoTestRpcClient.h>

#include <aws/testing/aws_test_harness.h>

#if defined(_WIN32)
// aws_test_harness.h includes Windows.h, which is an abomination.
// undef macros with clashing names...
#    undef SetPort
#    undef GetMessage
#endif

#include <aws/common/environment.h>

#include <cinttypes>
#include <thread>

using namespace Aws::Crt;
using namespace Aws::Eventstreamrpc;
using namespace Awstest;

AWS_STATIC_STRING_FROM_LITERAL(s_env_name_echo_server_host, "AWS_TEST_EVENT_STREAM_ECHO_SERVER_HOST");
AWS_STATIC_STRING_FROM_LITERAL(s_env_name_echo_server_port, "AWS_TEST_EVENT_STREAM_ECHO_SERVER_PORT");

struct EventStreamClientTestContext
{
    explicit EventStreamClientTestContext(struct aws_allocator *allocator);
    ~EventStreamClientTestContext() = default;

    bool isValidEnvironment() const;

    std::shared_ptr<Io::EventLoopGroup> elGroup;
    std::shared_ptr<Io::HostResolver> resolver;
    std::shared_ptr<Io::ClientBootstrap> clientBootstrap;

    uint16_t echoServerPort;
    Aws::Crt::String echoServerHost;
};

EventStreamClientTestContext::EventStreamClientTestContext(struct aws_allocator *allocator) : echoServerPort(0)
{
    elGroup = Aws::Crt::MakeShared<Io::EventLoopGroup>(allocator, 0, allocator);
    resolver = Aws::Crt::MakeShared<Io::DefaultHostResolver>(allocator, *elGroup, 8, 30, allocator);
    clientBootstrap = Aws::Crt::MakeShared<Io::ClientBootstrap>(allocator, *elGroup, *resolver, allocator);

    aws_string *host_env_value = nullptr;
    aws_get_environment_value(allocator, s_env_name_echo_server_host, &host_env_value);
    if (host_env_value)
    {
        echoServerHost = aws_string_c_str(host_env_value);
    }

    struct aws_string *port_env_value = nullptr;
    if (!aws_get_environment_value(allocator, s_env_name_echo_server_port, &port_env_value) &&
        port_env_value != nullptr)
    {
        echoServerPort = static_cast<uint16_t>(atoi(aws_string_c_str(port_env_value)));
    }

    aws_string_destroy(host_env_value);
    aws_string_destroy(port_env_value);

    AWS_LOGF_INFO(
        AWS_LS_COMMON_GENERAL, "EventStreamClientTestContext - Host name %d bytes long", (int)echoServerHost.length());
    AWS_LOGF_INFO(AWS_LS_COMMON_GENERAL, "EventStreamClientTestContext - Host name : %s", echoServerHost.c_str());
}

bool EventStreamClientTestContext::isValidEnvironment() const
{
    return !echoServerHost.empty() && echoServerPort > 0;
}

class TestLifecycleHandler : public ConnectionLifecycleHandler
{
  public:
    TestLifecycleHandler()
        : isConnected(false), disconnectCrtErrorCode(AWS_ERROR_SUCCESS),
          disconnectRpcStatusCode(EVENT_STREAM_RPC_SUCCESS)
    {
    }

    void OnConnectCallback() override
    {
        std::lock_guard<std::mutex> lockGuard(semaphoreLock);

        isConnected = true;

        semaphore.notify_one();
    }

    void OnDisconnectCallback(RpcError error) override
    {
        std::lock_guard<std::mutex> lockGuard(semaphoreLock);

        disconnectCrtErrorCode = error.crtError;
        disconnectRpcStatusCode = error.baseStatus;
        isConnected = false;

        semaphore.notify_one();
    }

    void WaitOnCondition(std::function<bool(void)> condition)
    {
        std::unique_lock<std::mutex> semaphoreULock(semaphoreLock);
        semaphore.wait(semaphoreULock, condition);
    }

    void WaitOnDisconnected()
    {
        WaitOnCondition([this]() { return !isConnected; });
    }

  private:
    std::condition_variable semaphore;
    std::mutex semaphoreLock;

    bool isConnected;
    int disconnectCrtErrorCode;
    EventStreamRpcStatusCode disconnectRpcStatusCode;
};

static int s_TestEventStreamConnectSuccess(struct aws_allocator *allocator, void *ctx)
{
    ApiHandle apiHandle(allocator);
    EventStreamClientTestContext testContext(allocator);
    if (!testContext.isValidEnvironment())
    {
        printf("Environment Variables are not set for the test, skipping...");
        return AWS_OP_SKIP;
    }

    {
        MessageAmendment connectionAmendment;
        connectionAmendment.AddHeader(EventStreamHeader(
            Aws::Crt::String("client-name"), Aws::Crt::String("accepted.testy_mc_testerson"), allocator));

        ConnectionConfig connectionConfig;
        connectionConfig.SetHostName(testContext.echoServerHost);
        connectionConfig.SetPort(testContext.echoServerPort);
        connectionConfig.SetConnectAmendment(connectionAmendment);

        TestLifecycleHandler lifecycleHandler;
        ClientConnection connection(allocator, testContext.clientBootstrap->GetUnderlyingHandle());
        auto future = connection.Connect(connectionConfig, &lifecycleHandler);
        EventStreamRpcStatusCode clientStatus = future.get().baseStatus;

        ASSERT_INT_EQUALS(EVENT_STREAM_RPC_SUCCESS, clientStatus);
    }

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(EventStreamConnectSuccess, s_TestEventStreamConnectSuccess);

static int s_TestEventStreamConnectFailureNoAuthHeader(struct aws_allocator *allocator, void *ctx)
{
    ApiHandle apiHandle(allocator);
    EventStreamClientTestContext testContext(allocator);
    if (!testContext.isValidEnvironment())
    {
        printf("Environment Variables are not set for the test, skipping...");
        return AWS_OP_SKIP;
    }

    {
        ConnectionConfig connectionConfig;
        connectionConfig.SetHostName(testContext.echoServerHost);
        connectionConfig.SetPort(testContext.echoServerPort);

        TestLifecycleHandler lifecycleHandler;
        ClientConnection connection(allocator, testContext.clientBootstrap->GetUnderlyingHandle());
        auto future = connection.Connect(connectionConfig, &lifecycleHandler);
        EventStreamRpcStatusCode clientStatus = future.get().baseStatus;

        ASSERT_TRUE(
            clientStatus == EVENT_STREAM_RPC_CRT_ERROR || clientStatus == EVENT_STREAM_RPC_CONNECTION_ACCESS_DENIED ||
            clientStatus == EVENT_STREAM_RPC_CONNECTION_CLOSED);
    }

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(EventStreamConnectFailureNoAuthHeader, s_TestEventStreamConnectFailureNoAuthHeader);

static int s_TestEventStreamConnectFailureBadAuthHeader(struct aws_allocator *allocator, void *ctx)
{
    ApiHandle apiHandle(allocator);
    EventStreamClientTestContext testContext(allocator);
    if (!testContext.isValidEnvironment())
    {
        printf("Environment Variables are not set for the test, skipping...");
        return AWS_OP_SKIP;
    }

    {
        MessageAmendment connectionAmendment;
        connectionAmendment.AddHeader(EventStreamHeader(
            Aws::Crt::String("client-name"), Aws::Crt::String("rejected.testy_mc_testerson"), allocator));

        ConnectionConfig connectionConfig;
        connectionConfig.SetHostName(testContext.echoServerHost);
        connectionConfig.SetPort(testContext.echoServerPort);
        connectionConfig.SetConnectAmendment(connectionAmendment);

        TestLifecycleHandler lifecycleHandler;
        ClientConnection connection(allocator, testContext.clientBootstrap->GetUnderlyingHandle());
        auto future = connection.Connect(connectionConfig, &lifecycleHandler);
        auto clientResult = future.get();
        EventStreamRpcStatusCode clientStatus = clientResult.baseStatus;

        AWS_LOGF_INFO(
            AWS_LS_COMMON_GENERAL,
            "Windows failing test result status: %d, error code: %d",
            (int)clientStatus,
            clientResult.crtError);

        ASSERT_TRUE(
            clientStatus == EVENT_STREAM_RPC_CRT_ERROR || clientStatus == EVENT_STREAM_RPC_CONNECTION_ACCESS_DENIED ||
            clientStatus == EVENT_STREAM_RPC_CONNECTION_CLOSED);
    }

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(EventStreamConnectFailureBadAuthHeader, s_TestEventStreamConnectFailureBadAuthHeader);

static int s_TestEchoClientConnectSuccess(struct aws_allocator *allocator, void *ctx)
{
    ApiHandle apiHandle(allocator);
    EventStreamClientTestContext testContext(allocator);
    if (!testContext.isValidEnvironment())
    {
        printf("Environment Variables are not set for the test, skipping...");
        return AWS_OP_SKIP;
    }

    {
        ConnectionLifecycleHandler lifecycleHandler;
        Awstest::EchoTestRpcClient client(*testContext.clientBootstrap, allocator);
        auto connectedStatus = client.Connect(lifecycleHandler);
        EventStreamRpcStatusCode clientStatus = connectedStatus.get().baseStatus;

        ASSERT_INT_EQUALS(EVENT_STREAM_RPC_SUCCESS, clientStatus);
        client.Close();
    }

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(EchoClientConnectSuccess, s_TestEchoClientConnectSuccess);

static int s_TestEchoClientDoubleClose(struct aws_allocator *allocator, void *ctx)
{
    ApiHandle apiHandle(allocator);
    EventStreamClientTestContext testContext(allocator);
    if (!testContext.isValidEnvironment())
    {
        printf("Environment Variables are not set for the test, skipping...");
        return AWS_OP_SKIP;
    }

    {
        ConnectionLifecycleHandler lifecycleHandler;
        Awstest::EchoTestRpcClient client(*testContext.clientBootstrap, allocator);
        client.Close();
        client.Close();
    }

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(EchoClientDoubleClose, s_TestEchoClientDoubleClose);

static int s_TestEchoClientMultiConnectSuccessFail(struct aws_allocator *allocator, void *ctx)
{
    ApiHandle apiHandle(allocator);
    EventStreamClientTestContext testContext(allocator);
    if (!testContext.isValidEnvironment())
    {
        printf("Environment Variables are not set for the test, skipping...");
        return AWS_OP_SKIP;
    }

    {
        ConnectionLifecycleHandler lifecycleHandler;
        Awstest::EchoTestRpcClient client(*testContext.clientBootstrap, allocator);
        auto connectedStatus = client.Connect(lifecycleHandler);

        auto failedStatus1 = client.Connect(lifecycleHandler);
        ASSERT_INT_EQUALS(EVENT_STREAM_RPC_CONNECTION_ALREADY_ESTABLISHED, failedStatus1.get().baseStatus);

        auto failedStatus2 = client.Connect(lifecycleHandler);
        ASSERT_INT_EQUALS(EVENT_STREAM_RPC_CONNECTION_ALREADY_ESTABLISHED, failedStatus2.get().baseStatus);

        EventStreamRpcStatusCode clientStatus = connectedStatus.get().baseStatus;
        ASSERT_INT_EQUALS(EVENT_STREAM_RPC_SUCCESS, clientStatus);

        client.Close();
    }

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(EchoClientMultiConnectSuccessFail, s_TestEchoClientMultiConnectSuccessFail);

static int s_TestEventStreamClientCreateFailureInvalidHost(struct aws_allocator *allocator, void *ctx)
{
    ApiHandle apiHandle(allocator);

    {
        auto elGroup = Aws::Crt::MakeShared<Io::EventLoopGroup>(allocator, 0, allocator);
        auto resolver = Aws::Crt::MakeShared<Io::DefaultHostResolver>(allocator, *elGroup, 8, 30, allocator);
        auto clientBootstrap = Aws::Crt::MakeShared<Io::ClientBootstrap>(allocator, *elGroup, *resolver, allocator);

        ConnectionLifecycleHandler lifecycleHandler;
        ClientConnection client(allocator, clientBootstrap->GetUnderlyingHandle());

        ConnectionConfig config;
        config.SetPort(8033);

        auto connectFuture = client.Connect(config, &lifecycleHandler);
        ASSERT_INT_EQUALS(EVENT_STREAM_RPC_NULL_PARAMETER, connectFuture.get().baseStatus);
    }

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(EventStreamClientCreateFailureInvalidHost, s_TestEventStreamClientCreateFailureInvalidHost);

static int s_TestEventStreamClientCreateFailureInvalidPort(struct aws_allocator *allocator, void *ctx)
{
    ApiHandle apiHandle(allocator);

    {
        auto elGroup = Aws::Crt::MakeShared<Io::EventLoopGroup>(allocator, 0, allocator);
        auto resolver = Aws::Crt::MakeShared<Io::DefaultHostResolver>(allocator, *elGroup, 8, 30, allocator);
        auto clientBootstrap = Aws::Crt::MakeShared<Io::ClientBootstrap>(allocator, *elGroup, *resolver, allocator);

        ConnectionLifecycleHandler lifecycleHandler;
        ClientConnection client(allocator, clientBootstrap->GetUnderlyingHandle());

        ConnectionConfig config;
        config.SetHostName("localhost");

        auto connectFuture = client.Connect(config, &lifecycleHandler);
        ASSERT_INT_EQUALS(EVENT_STREAM_RPC_NULL_PARAMETER, connectFuture.get().baseStatus);
    }

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(EventStreamClientCreateFailureInvalidPort, s_TestEventStreamClientCreateFailureInvalidPort);

static int s_TestEchoClientReconnect(struct aws_allocator *allocator, void *ctx)
{
    ApiHandle apiHandle(allocator);
    EventStreamClientTestContext testContext(allocator);
    if (!testContext.isValidEnvironment())
    {
        printf("Environment Variables are not set for the test, skipping...");
        return AWS_OP_SKIP;
    }

    {
        TestLifecycleHandler lifecycleHandler;
        Awstest::EchoTestRpcClient client(*testContext.clientBootstrap, allocator);
        auto connectedStatus1 = client.Connect(lifecycleHandler).get().baseStatus;
        ASSERT_INT_EQUALS(EVENT_STREAM_RPC_SUCCESS, connectedStatus1);

        client.Close();
        lifecycleHandler.WaitOnDisconnected();

        auto connectedStatus2 = client.Connect(lifecycleHandler).get().baseStatus;
        ASSERT_INT_EQUALS(EVENT_STREAM_RPC_SUCCESS, connectedStatus2);

        client.Close();
    }

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(EchoClientReconnect, s_TestEchoClientReconnect);

static int s_TestEchoClientCloseWhileConnecting(struct aws_allocator *allocator, void *ctx)
{
    ApiHandle apiHandle(allocator);
    EventStreamClientTestContext testContext(allocator);
    if (!testContext.isValidEnvironment())
    {
        printf("Environment Variables are not set for the test, skipping...");
        return AWS_OP_SKIP;
    }

    {
        TestLifecycleHandler lifecycleHandler;
        Awstest::EchoTestRpcClient client(*testContext.clientBootstrap, allocator);
        auto connectedFuture = client.Connect(lifecycleHandler);
        client.Close();

        // Our primary concern is that
        // we end up in a closed state, the connect future resolves, and nothing explodes in-between.
        auto status = connectedFuture.get().baseStatus;

        // depending on race conditions, we may end up with a connection success (before the close gets triggered)
        // or a connection closed
        ASSERT_TRUE(status == EVENT_STREAM_RPC_CONNECTION_CLOSED || status == EVENT_STREAM_RPC_SUCCESS);
        lifecycleHandler.WaitOnDisconnected();
    }

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(EchoClientCloseWhileConnecting, s_TestEchoClientCloseWhileConnecting);

static int s_TestEchoClientConnectWhileClosing(struct aws_allocator *allocator, void *ctx)
{
    ApiHandle apiHandle(allocator);
    EventStreamClientTestContext testContext(allocator);
    if (!testContext.isValidEnvironment())
    {
        printf("Environment Variables are not set for the test, skipping...");
        return AWS_OP_SKIP;
    }

    {
        TestLifecycleHandler lifecycleHandler;
        Awstest::EchoTestRpcClient client(*testContext.clientBootstrap, allocator);
        auto connectedFuture = client.Connect(lifecycleHandler);
        auto status = connectedFuture.get().baseStatus;
        ASSERT_INT_EQUALS(EVENT_STREAM_RPC_SUCCESS, status);

        client.Close();

        auto reconnectFuture = client.Connect(lifecycleHandler);
        auto reconnectStatus = reconnectFuture.get().baseStatus;

        // depending on race conditions, we may end up with a success (the close completed quickly)
        // or an already established code (the connect arrived in the middle of close)
        ASSERT_TRUE(
            reconnectStatus == EVENT_STREAM_RPC_CONNECTION_ALREADY_ESTABLISHED ||
            reconnectStatus == EVENT_STREAM_RPC_SUCCESS);
        client.Close();

        lifecycleHandler.WaitOnDisconnected();
    }

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(EchoClientConnectWhileClosing, s_TestEchoClientConnectWhileClosing);

static int s_TestEchoClientOpenCloseStress(struct aws_allocator *allocator, void *ctx)
{
    ApiHandle apiHandle(allocator);
    EventStreamClientTestContext testContext(allocator);
    if (!testContext.isValidEnvironment())
    {
        printf("Environment Variables are not set for the test, skipping...");
        return AWS_OP_SKIP;
    }

    {
        TestLifecycleHandler lifecycleHandler;
        Awstest::EchoTestRpcClient client(*testContext.clientBootstrap, allocator);

        for (size_t i = 0; i < 1000; ++i)
        {
            auto connectedFuture = client.Connect(lifecycleHandler);

            if (rand() % 2 == 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 10));
            }

            client.Close();

            auto connectedStatus = connectedFuture.get().baseStatus;
            ASSERT_TRUE(
                connectedStatus == EVENT_STREAM_RPC_CONNECTION_CLOSED || connectedStatus == EVENT_STREAM_RPC_SUCCESS ||
                connectedStatus == EVENT_STREAM_RPC_CONNECTION_ALREADY_ESTABLISHED);

            if (rand() % 2 == 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 10));
            }
        }
    }

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(EchoClientOpenCloseStress, s_TestEchoClientOpenCloseStress);

static void s_onMessageFlush(int errorCode)
{
    (void)errorCode;
}

template <typename T>
static bool s_messageDataMembersAreEqual(
    const Aws::Crt::Optional<T> &expectedValue,
    const Aws::Crt::Optional<T> &actualValue)
{
    if (expectedValue.has_value() != actualValue.has_value())
    {
        return false;
    }

    if (expectedValue.has_value())
    {
        return expectedValue.value() == actualValue.value();
    }

    return true;
}

static bool s_messageDataMembersAreEqual(
    const Aws::Crt::Optional<DateTime> &lhs,
    const Aws::Crt::Optional<DateTime> &rhs)
{
    if (lhs.has_value() != rhs.has_value())
    {
        AWS_LOGF_FATAL(AWS_LS_COMMON_GENERAL, "Timestamp comparison optional has value mismatch");
        return false;
    }

    if (lhs.has_value())
    {
        if (lhs.value() != rhs.value())
        {
            AWS_LOGF_FATAL(
                AWS_LS_COMMON_GENERAL,
                "Timestamp comparison value mismatch: %" PRIu64 " vs %" PRIu64,
                lhs.value().Millis(),
                rhs.value().Millis());
            return false;
        }
    }

    return true;
}

// Specialization for Vector<Pair> since we don't codegen == for Shapes
static bool s_messageDataMembersAreEqual(
    const Aws::Crt::Optional<Aws::Crt::Vector<Pair>> &lhs,
    const Aws::Crt::Optional<Aws::Crt::Vector<Pair>> &rhs)
{
    if (lhs.has_value() != rhs.has_value())
    {
        return false;
    }

    if (!lhs.has_value())
    {
        return true;
    }

    if (lhs.value().size() != rhs.value().size())
    {
        return false;
    }

    for (size_t i = 0; i < lhs.value().size(); ++i)
    {
        const auto &lhs_pair = (lhs.value())[i];
        const auto &rhs_pair = (rhs.value())[i];

        if (lhs_pair.GetKey().has_value() != rhs_pair.GetKey().has_value())
        {
            return false;
        }

        if (lhs_pair.GetValue().has_value() != rhs_pair.GetValue().has_value())
        {
            return false;
        }

        if (lhs_pair.GetKey().value() != rhs_pair.GetKey().value() ||
            lhs_pair.GetValue().value() != rhs_pair.GetValue().value())
        {
            return false;
        }
    }

    return true;
}

// Specialization for Map<String, Product> since we don't codegen == for Shapes
static bool s_messageDataMembersAreEqual(
    const Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, Product>> &lhs,
    const Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, Product>> &rhs)
{
    if (lhs.has_value() != rhs.has_value())
    {
        return false;
    }

    if (!lhs.has_value())
    {
        return true;
    }

    if (lhs.value().size() != rhs.value().size())
    {
        return false;
    }

    for (const auto &lhs_entry : lhs.value())
    {
        const auto &rhs_entry = rhs.value().find(lhs_entry.first);
        if (rhs_entry == rhs.value().end())
        {
            return false;
        }

        const auto &lhs_product = lhs_entry.second;
        const auto &rhs_product = rhs_entry->second;

        if (lhs_product.GetName().has_value() != rhs_product.GetName().has_value())
        {
            return false;
        }

        if (lhs_product.GetPrice().has_value() != rhs_product.GetPrice().has_value())
        {
            return false;
        }

        if (lhs_product.GetName().value() != rhs_product.GetName().value() ||
            lhs_product.GetPrice().value() != rhs_product.GetPrice().value())
        {
            return false;
        }
    }

    return true;
}

static int s_checkMessageDataEquality(const MessageData &expectedData, const MessageData &actualData)
{
    ASSERT_TRUE(s_messageDataMembersAreEqual(expectedData.GetStringMessage(), actualData.GetStringMessage()));
    ASSERT_TRUE(s_messageDataMembersAreEqual(expectedData.GetBooleanMessage(), actualData.GetBooleanMessage()));
    ASSERT_TRUE(s_messageDataMembersAreEqual(expectedData.GetTimeMessage(), actualData.GetTimeMessage()));
    ASSERT_TRUE(s_messageDataMembersAreEqual(expectedData.GetDocumentMessage(), actualData.GetDocumentMessage()));
    ASSERT_TRUE(s_messageDataMembersAreEqual(expectedData.GetEnumMessage(), actualData.GetEnumMessage()));
    ASSERT_TRUE(s_messageDataMembersAreEqual(expectedData.GetBlobMessage(), actualData.GetBlobMessage()));
    ASSERT_TRUE(s_messageDataMembersAreEqual(expectedData.GetStringListMessage(), actualData.GetStringListMessage()));
    ASSERT_TRUE(s_messageDataMembersAreEqual(expectedData.GetKeyValuePairList(), actualData.GetKeyValuePairList()));
    ASSERT_TRUE(s_messageDataMembersAreEqual(expectedData.GetStringToValue(), actualData.GetStringToValue()));

    return AWS_OP_SUCCESS;
}

static int s_DoTestEchoClientOperationEchoSuccess(
    struct aws_allocator *allocator,
    std::function<void(MessageData &)> messageDataBuilder)
{
    ApiHandle apiHandle(allocator);
    EventStreamClientTestContext testContext(allocator);
    if (!testContext.isValidEnvironment())
    {
        printf("Environment Variables are not set for the test, skipping...");
        return AWS_OP_SKIP;
    }

    {
        ConnectionLifecycleHandler lifecycleHandler;
        Awstest::EchoTestRpcClient client(*testContext.clientBootstrap, allocator);
        auto connectedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_SUCCESS);

        auto echoMessage = client.NewEchoMessage();
        EchoMessageRequest echoMessageRequest;
        MessageData messageData;
        messageDataBuilder(messageData);
        echoMessageRequest.SetMessage(messageData);

        auto requestFuture = echoMessage->Activate(echoMessageRequest, s_onMessageFlush);
        requestFuture.wait();
        auto result = echoMessage->GetResult().get();
        ASSERT_TRUE(result);
        auto response = result.GetOperationResponse();
        ASSERT_NOT_NULL(response);

        ASSERT_SUCCESS(s_checkMessageDataEquality(messageData, response->GetMessage().value()));
    }

    return AWS_OP_SUCCESS;
}

static int s_TestEchoClientOperationEchoSuccessString(struct aws_allocator *allocator, void *ctx)
{
    return s_DoTestEchoClientOperationEchoSuccess(
        allocator,
        [](MessageData &messageData)
        {
            Aws::Crt::String value = "Hello World";
            messageData.SetStringMessage(value);
        });
}

AWS_TEST_CASE(EchoClientOperationEchoSuccessString, s_TestEchoClientOperationEchoSuccessString);

static int s_TestEchoClientOperationEchoSuccessBoolean(struct aws_allocator *allocator, void *ctx)
{
    return s_DoTestEchoClientOperationEchoSuccess(
        allocator, [](MessageData &messageData) { messageData.SetBooleanMessage(true); });
}

AWS_TEST_CASE(EchoClientOperationEchoSuccessBoolean, s_TestEchoClientOperationEchoSuccessBoolean);

static int s_TestEchoClientOperationEchoSuccessTime(struct aws_allocator *allocator, void *ctx)
{
    return s_DoTestEchoClientOperationEchoSuccess(
        allocator, [](MessageData &messageData) { messageData.SetTimeMessage(Aws::Crt::DateTime::Now()); });
}

AWS_TEST_CASE(EchoClientOperationEchoSuccessTime, s_TestEchoClientOperationEchoSuccessTime);

static int s_TestEchoClientOperationEchoSuccessDocument(struct aws_allocator *allocator, void *ctx)
{
    return s_DoTestEchoClientOperationEchoSuccess(
        allocator,
        [](MessageData &messageData)
        {
            Aws::Crt::JsonObject subobject;
            subobject.WithString("Hello", "There");
            Aws::Crt::JsonObject document;
            document.WithInt64("Derp", 21);
            document.WithObject("DailyAffirmations", subobject);

            messageData.SetDocumentMessage(document);
        });
}

AWS_TEST_CASE(EchoClientOperationEchoSuccessDocument, s_TestEchoClientOperationEchoSuccessDocument);

static int s_TestEchoClientOperationEchoSuccessEnum(struct aws_allocator *allocator, void *ctx)
{
    return s_DoTestEchoClientOperationEchoSuccess(
        allocator, [](MessageData &messageData) { messageData.SetEnumMessage(FruitEnum::FRUIT_ENUM_PINEAPPLE); });
}

AWS_TEST_CASE(EchoClientOperationEchoSuccessEnum, s_TestEchoClientOperationEchoSuccessEnum);

static int s_TestEchoClientOperationEchoSuccessBlob(struct aws_allocator *allocator, void *ctx)
{
    return s_DoTestEchoClientOperationEchoSuccess(
        allocator,
        [](MessageData &messageData)
        {
            Aws::Crt::Vector<uint8_t> blob = {1, 2, 3, 4, 5};
            messageData.SetBlobMessage(blob);
        });
}

AWS_TEST_CASE(EchoClientOperationEchoSuccessBlob, s_TestEchoClientOperationEchoSuccessBlob);

static int s_TestEchoClientOperationEchoSuccessStringList(struct aws_allocator *allocator, void *ctx)
{
    return s_DoTestEchoClientOperationEchoSuccess(
        allocator,
        [](MessageData &messageData)
        {
            Aws::Crt::Vector<Aws::Crt::String> stringList = {"1", "2", "Toasty", "Mctoaster"};
            messageData.SetStringListMessage(stringList);
        });
}

AWS_TEST_CASE(EchoClientOperationEchoSuccessStringList, s_TestEchoClientOperationEchoSuccessStringList);

static int s_TestEchoClientOperationEchoSuccessPairList(struct aws_allocator *allocator, void *ctx)
{
    return s_DoTestEchoClientOperationEchoSuccess(
        allocator,
        [](MessageData &messageData)
        {
            Pair pair1;
            pair1.SetKey("Uff");
            pair1.SetValue("Dah");

            Pair pair2;
            pair2.SetKey("Hello");
            pair2.SetValue("World");

            Aws::Crt::Vector<Pair> pairList = {pair1, pair2};
            messageData.SetKeyValuePairList(pairList);
        });
}

AWS_TEST_CASE(EchoClientOperationEchoSuccessPairList, s_TestEchoClientOperationEchoSuccessPairList);

static int s_TestEchoClientOperationEchoSuccessProductMap(struct aws_allocator *allocator, void *ctx)
{
    return s_DoTestEchoClientOperationEchoSuccess(
        allocator,
        [](MessageData &messageData)
        {
            Aws::Crt::Map<String, Product> productMap = {};
            Product product1;
            product1.SetName("Derp");
            product1.SetPrice(4.0);

            Product product2;
            product2.SetName("Can Of Derp");
            product2.SetPrice(7.5);

            productMap[product1.GetName().value()] = product1;
            productMap[product2.GetName().value()] = product2;

            messageData.SetStringToValue(productMap);
        });
}

AWS_TEST_CASE(EchoClientOperationEchoSuccessProductMap, s_TestEchoClientOperationEchoSuccessProductMap);

static int s_TestEchoClientOperationEchoSuccessMultiple(struct aws_allocator *allocator, void *ctx)
{
    ApiHandle apiHandle(allocator);
    EventStreamClientTestContext testContext(allocator);
    if (!testContext.isValidEnvironment())
    {
        printf("Environment Variables are not set for the test, skipping...");
        return AWS_OP_SKIP;
    }

    {
        ConnectionLifecycleHandler lifecycleHandler;
        Awstest::EchoTestRpcClient client(*testContext.clientBootstrap, allocator);
        auto connectedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_SUCCESS);

        for (size_t i = 0; i < 5; i++)
        {
            auto echoMessage = client.NewEchoMessage();
            EchoMessageRequest echoMessageRequest;
            MessageData messageData;
            Aws::Crt::StringStream ss;
            ss << "Hello Echo #" << i + 1;
            Aws::Crt::String expectedMessage(ss.str().c_str());
            messageData.SetStringMessage(expectedMessage);
            echoMessageRequest.SetMessage(messageData);

            auto requestFuture = echoMessage->Activate(echoMessageRequest, s_onMessageFlush);
            requestFuture.wait();
            auto result = echoMessage->GetResult().get();
            ASSERT_TRUE(result);
            auto response = result.GetOperationResponse();
            ASSERT_NOT_NULL(response);
            ASSERT_TRUE(response->GetMessage().value().GetStringMessage().value() == expectedMessage);
        }
    }

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(EchoClientOperationEchoSuccessMultiple, s_TestEchoClientOperationEchoSuccessMultiple);

static int s_DoSimpleRequestWhileConnectedTest(
    struct aws_allocator *allocator,
    std::function<int(EventStreamClientTestContext &, EchoTestRpcClient &)> testFunction)
{
    ApiHandle apiHandle(allocator);
    EventStreamClientTestContext testContext(allocator);
    if (!testContext.isValidEnvironment())
    {
        printf("Environment Variables are not set for the test, skipping...");
        return AWS_OP_SKIP;
    }

    {
        ConnectionLifecycleHandler lifecycleHandler;
        Awstest::EchoTestRpcClient client(*testContext.clientBootstrap, allocator);
        auto connectedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_SUCCESS);

        return testFunction(testContext, client);
    }
}

static int s_TestEchoClientOperationGetAllProductsSuccess(struct aws_allocator *allocator, void *ctx)
{
    return s_DoSimpleRequestWhileConnectedTest(
        allocator,
        [](EventStreamClientTestContext &testContext, EchoTestRpcClient &client)
        {
            auto getAllProducts = client.NewGetAllProducts();
            GetAllProductsRequest getAllProductsRequest;

            auto requestFuture = getAllProducts->Activate(getAllProductsRequest, s_onMessageFlush);
            requestFuture.wait();
            auto result = getAllProducts->GetResult().get();
            ASSERT_TRUE(result);
            auto response = result.GetOperationResponse();
            ASSERT_NOT_NULL(response);

            return AWS_OP_SUCCESS;
        });
}

AWS_TEST_CASE(EchoClientOperationGetAllProductsSuccess, s_TestEchoClientOperationGetAllProductsSuccess);

static int s_TestEchoClientOperationGetAllCustomersSuccess(struct aws_allocator *allocator, void *ctx)
{
    return s_DoSimpleRequestWhileConnectedTest(
        allocator,
        [](EventStreamClientTestContext &testContext, EchoTestRpcClient &client)
        {
            auto getAllCustomers = client.NewGetAllCustomers();
            GetAllCustomersRequest getAllCustomersRequest;

            auto requestFuture = getAllCustomers->Activate(getAllCustomersRequest, s_onMessageFlush);
            requestFuture.wait();
            auto result = getAllCustomers->GetResult().get();
            ASSERT_TRUE(result);
            auto response = result.GetOperationResponse();
            ASSERT_NOT_NULL(response);

            return AWS_OP_SUCCESS;
        });
}

AWS_TEST_CASE(EchoClientOperationGetAllCustomersSuccess, s_TestEchoClientOperationGetAllCustomersSuccess);

static int s_TestEchoClientOperationCauseServiceErrorSuccess(struct aws_allocator *allocator, void *ctx)
{
    return s_DoSimpleRequestWhileConnectedTest(
        allocator,
        [](EventStreamClientTestContext &testContext, EchoTestRpcClient &client)
        {
            auto causeServiceError = client.NewCauseServiceError();
            CauseServiceErrorRequest causeServiceErrorRequest;

            auto requestFuture = causeServiceError->Activate(causeServiceErrorRequest, s_onMessageFlush);
            requestFuture.wait();
            auto result = causeServiceError->GetResult().get();
            ASSERT_FALSE(result);
            ASSERT_NOT_NULL(result.GetOperationError());
            auto error = result.GetOperationError();
            auto errorMessage = error->GetMessage();
            ASSERT_TRUE(errorMessage.value() == "Intentionally thrown ServiceError");
            const auto &modelName = error->GetModelName();
            ASSERT_TRUE(modelName == "awstest#ServiceError");

            return AWS_OP_SUCCESS;
        });
}

AWS_TEST_CASE(EchoClientOperationCauseServiceErrorSuccess, s_TestEchoClientOperationCauseServiceErrorSuccess);

static int s_TestEchoClientOperationEchoFailureNeverConnected(struct aws_allocator *allocator, void *ctx)
{
    ApiHandle apiHandle(allocator);
    EventStreamClientTestContext testContext(allocator);
    if (!testContext.isValidEnvironment())
    {
        printf("Environment Variables are not set for the test, skipping...");
        return AWS_OP_SKIP;
    }

    {
        ConnectionLifecycleHandler lifecycleHandler;
        Awstest::EchoTestRpcClient client(*testContext.clientBootstrap, allocator);

        auto echoMessage = client.NewEchoMessage();
        EchoMessageRequest echoMessageRequest;
        MessageData messageData;
        Aws::Crt::String expectedMessage("l33t");
        messageData.SetStringMessage(expectedMessage);
        echoMessageRequest.SetMessage(messageData);

        auto requestFuture = echoMessage->Activate(echoMessageRequest, s_onMessageFlush);
        ASSERT_INT_EQUALS(EVENT_STREAM_RPC_CONNECTION_CLOSED, requestFuture.get().baseStatus);
    }

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(EchoClientOperationEchoFailureNeverConnected, s_TestEchoClientOperationEchoFailureNeverConnected);

static int s_TestEchoClientOperationEchoFailureDisconnected(struct aws_allocator *allocator, void *ctx)
{
    ApiHandle apiHandle(allocator);
    EventStreamClientTestContext testContext(allocator);
    if (!testContext.isValidEnvironment())
    {
        printf("Environment Variables are not set for the test, skipping...");
        return AWS_OP_SKIP;
    }

    {
        ConnectionLifecycleHandler lifecycleHandler;
        Awstest::EchoTestRpcClient client(*testContext.clientBootstrap, allocator);
        auto connectedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_SUCCESS);

        client.Close();

        auto echoMessage = client.NewEchoMessage();
        EchoMessageRequest echoMessageRequest;
        MessageData messageData;
        Aws::Crt::String expectedMessage("l33t");
        messageData.SetStringMessage(expectedMessage);
        echoMessageRequest.SetMessage(messageData);

        auto requestFuture = echoMessage->Activate(echoMessageRequest, s_onMessageFlush);
        auto activateStatus = requestFuture.get().baseStatus;
        ASSERT_TRUE(
            activateStatus == EVENT_STREAM_RPC_CONNECTION_CLOSED || activateStatus == EVENT_STREAM_RPC_CRT_ERROR);
    }

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(EchoClientOperationEchoFailureDisconnected, s_TestEchoClientOperationEchoFailureDisconnected);

static int s_DoSimpleRequestRaceCheckTest(
    struct aws_allocator *allocator,
    std::function<int(EventStreamClientTestContext &, EchoTestRpcClient &)> testFunction)
{
    ApiHandle apiHandle(allocator);
    EventStreamClientTestContext testContext(allocator);
    if (!testContext.isValidEnvironment())
    {
        printf("Environment Variables are not set for the test, skipping...");
        return AWS_OP_SKIP;
    }

    {
        ConnectionLifecycleHandler lifecycleHandler;
        Awstest::EchoTestRpcClient client(*testContext.clientBootstrap, allocator);
        auto connectedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_SUCCESS);

        for (size_t i = 0; i < 1000; ++i)
        {
            ASSERT_SUCCESS(testFunction(testContext, client));
        }

        return AWS_OP_SUCCESS;
    }
}

static int s_TestEchoClientOperationUnactivatedShutdown(struct aws_allocator *allocator, void *ctx)
{
    return s_DoSimpleRequestRaceCheckTest(
        allocator,
        [](EventStreamClientTestContext &testContext, EchoTestRpcClient &client)
        {
            auto echoMessage = client.NewEchoMessage();

            // Drop the operation, invoking shutdown
            return AWS_OP_SUCCESS;
        });
}

AWS_TEST_CASE(EchoClientOperationUnactivatedShutdown, s_TestEchoClientOperationUnactivatedShutdown);

static int s_TestEchoClientOperationUnactivatedClose(struct aws_allocator *allocator, void *ctx)
{
    return s_DoSimpleRequestRaceCheckTest(
        allocator,
        [](EventStreamClientTestContext &testContext, EchoTestRpcClient &client)
        {
            auto echoMessage = client.NewEchoMessage();

            auto closeFuture = echoMessage->Close();
            auto closeStatus = closeFuture.get().baseStatus;
            ASSERT_INT_EQUALS(EVENT_STREAM_RPC_CONTINUATION_CLOSED, closeStatus);

            // Drop the operation, invoking shutdown
            return AWS_OP_SUCCESS;
        });
}

AWS_TEST_CASE(EchoClientOperationUnactivatedClose, s_TestEchoClientOperationUnactivatedClose);

static int s_TestEchoClientOperationUnactivatedCloseDropFuture(struct aws_allocator *allocator, void *ctx)
{
    return s_DoSimpleRequestRaceCheckTest(
        allocator,
        [](EventStreamClientTestContext &testContext, EchoTestRpcClient &client)
        {
            auto echoMessage = client.NewEchoMessage();

            auto closeFuture = echoMessage->Close();

            // Drop the operation and the close future, invoking shutdown
            return AWS_OP_SUCCESS;
        });
}

AWS_TEST_CASE(EchoClientOperationUnactivatedCloseDropFuture, s_TestEchoClientOperationUnactivatedCloseDropFuture);

static int s_TestEchoClientOperationActivateActivate(struct aws_allocator *allocator, void *ctx)
{
    return s_DoSimpleRequestRaceCheckTest(
        allocator,
        [](EventStreamClientTestContext &testContext, EchoTestRpcClient &client)
        {
            auto getAllCustomers = client.NewGetAllCustomers();
            GetAllCustomersRequest getAllCustomersRequest;

            auto requestFuture = getAllCustomers->Activate(getAllCustomersRequest, s_onMessageFlush);
            auto requestFuture2 = getAllCustomers->Activate(getAllCustomersRequest, s_onMessageFlush);
            requestFuture2.wait();
            requestFuture.wait();
            auto result = getAllCustomers->GetResult().get();
            ASSERT_TRUE(result);
            auto response = result.GetOperationResponse();
            ASSERT_NOT_NULL(response);

            auto flush2Result = requestFuture2.get();
            ASSERT_TRUE(
                (flush2Result.baseStatus == EVENT_STREAM_RPC_CRT_ERROR &&
                 flush2Result.crtError == AWS_ERROR_INVALID_STATE) ||
                flush2Result.baseStatus == EVENT_STREAM_RPC_CONTINUATION_CLOSED);

            return AWS_OP_SUCCESS;
        });
}

AWS_TEST_CASE(EchoClientOperationActivateActivate, s_TestEchoClientOperationActivateActivate);

static int s_TestEchoClientOperationActivateWaitActivate(struct aws_allocator *allocator, void *ctx)
{
    return s_DoSimpleRequestRaceCheckTest(
        allocator,
        [](EventStreamClientTestContext &testContext, EchoTestRpcClient &client)
        {
            auto getAllCustomers = client.NewGetAllCustomers();
            GetAllCustomersRequest getAllCustomersRequest;

            auto requestFuture = getAllCustomers->Activate(getAllCustomersRequest, s_onMessageFlush);
            requestFuture.wait();
            auto result = getAllCustomers->GetResult().get();
            ASSERT_TRUE(result);
            auto response = result.GetOperationResponse();
            ASSERT_NOT_NULL(response);

            auto requestFuture2 = getAllCustomers->Activate(getAllCustomersRequest, s_onMessageFlush);
            requestFuture2.wait();

            auto flush2ErrorStatus = requestFuture2.get().baseStatus;
            ASSERT_TRUE(
                flush2ErrorStatus == EVENT_STREAM_RPC_CRT_ERROR ||
                flush2ErrorStatus == EVENT_STREAM_RPC_CONTINUATION_CLOSED);

            return AWS_OP_SUCCESS;
        });
}

AWS_TEST_CASE(EchoClientOperationActivateWaitActivate, s_TestEchoClientOperationActivateWaitActivate);

static int s_validateClosedActivateResult(const RpcError &result)
{
    auto flushErrorStatus = result.baseStatus;
    if (flushErrorStatus == EVENT_STREAM_RPC_CRT_ERROR)
    {
        ASSERT_INT_EQUALS(AWS_ERROR_EVENT_STREAM_RPC_CONNECTION_CLOSED, result.crtError);
    }
    else
    {
        if (flushErrorStatus != EVENT_STREAM_RPC_CONTINUATION_CLOSED && flushErrorStatus != EVENT_STREAM_RPC_SUCCESS)
        {
            return AWS_OP_ERR;
        }
        ASSERT_TRUE(
            flushErrorStatus == EVENT_STREAM_RPC_SUCCESS || flushErrorStatus == EVENT_STREAM_RPC_CONTINUATION_CLOSED);
    }

    return AWS_OP_SUCCESS;
}

static int s_TestEchoClientOperationActivateCloseActivate(struct aws_allocator *allocator, void *ctx)
{
    return s_DoSimpleRequestRaceCheckTest(
        allocator,
        [](EventStreamClientTestContext &testContext, EchoTestRpcClient &client)
        {
            auto getAllCustomers = client.NewGetAllCustomers();
            GetAllCustomersRequest getAllCustomersRequest;

            auto requestFuture = getAllCustomers->Activate(getAllCustomersRequest, s_onMessageFlush);

            auto closeFuture = getAllCustomers->Close();

            requestFuture.wait();
            ASSERT_SUCCESS(s_validateClosedActivateResult(requestFuture.get()));

            auto requestFuture2 = getAllCustomers->Activate(getAllCustomersRequest, s_onMessageFlush);
            requestFuture2.wait();

            s_validateClosedActivateResult(requestFuture2.get());

            closeFuture.wait();

            return AWS_OP_SUCCESS;
        });
}

AWS_TEST_CASE(EchoClientOperationActivateCloseActivate, s_TestEchoClientOperationActivateCloseActivate);

static int s_TestEchoClientOperationActivateClosedActivate(struct aws_allocator *allocator, void *ctx)
{
    return s_DoSimpleRequestRaceCheckTest(
        allocator,
        [](EventStreamClientTestContext &testContext, EchoTestRpcClient &client)
        {
            auto getAllCustomers = client.NewGetAllCustomers();
            GetAllCustomersRequest getAllCustomersRequest;

            auto requestFuture = getAllCustomers->Activate(getAllCustomersRequest, s_onMessageFlush);

            auto closeFuture = getAllCustomers->Close();
            closeFuture.wait();

            requestFuture.wait();
            auto flushErrorStatus = requestFuture.get().baseStatus;
            ASSERT_TRUE(
                flushErrorStatus == EVENT_STREAM_RPC_SUCCESS ||
                flushErrorStatus == EVENT_STREAM_RPC_CONTINUATION_CLOSED);

            auto requestFuture2 = getAllCustomers->Activate(getAllCustomersRequest, s_onMessageFlush);
            requestFuture2.wait();

            auto flush2ErrorStatus = requestFuture2.get().baseStatus;
            ASSERT_INT_EQUALS(EVENT_STREAM_RPC_CONTINUATION_CLOSED, flush2ErrorStatus);

            return AWS_OP_SUCCESS;
        });
}

AWS_TEST_CASE(EchoClientOperationActivateClosedActivate, s_TestEchoClientOperationActivateClosedActivate);

static int s_DoClientScopedRaceCheckTest(
    struct aws_allocator *allocator,
    std::function<int(EventStreamClientTestContext &, EchoTestRpcClient &)> testFunction)
{
    ApiHandle apiHandle(allocator);
    for (size_t i = 0; i < 100; ++i)
    {

        EventStreamClientTestContext testContext(allocator);
        if (!testContext.isValidEnvironment())
        {
            printf("Environment Variables are not set for the test, skipping...");
            return AWS_OP_SKIP;
        }

        {
            ConnectionLifecycleHandler lifecycleHandler;
            Awstest::EchoTestRpcClient client(*testContext.clientBootstrap, allocator);
            auto connectedStatus = client.Connect(lifecycleHandler);
            ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_SUCCESS);

            ASSERT_SUCCESS(testFunction(testContext, client));
        }
    }

    return AWS_OP_SUCCESS;
}

static int s_TestEchoClientOperationActivateCloseConnection(struct aws_allocator *allocator, void *ctx)
{
    return s_DoClientScopedRaceCheckTest(
        allocator,
        [](EventStreamClientTestContext &testContext, EchoTestRpcClient &client)
        {
            auto getAllCustomers = client.NewGetAllCustomers();
            GetAllCustomersRequest getAllCustomersRequest;

            auto requestFuture = getAllCustomers->Activate(getAllCustomersRequest, s_onMessageFlush);

            client.Close();

            requestFuture.wait();
            auto flushResult = requestFuture.get();
            auto flushErrorStatus = flushResult.baseStatus;
            ASSERT_TRUE(
                flushErrorStatus == EVENT_STREAM_RPC_SUCCESS ||
                flushErrorStatus == EVENT_STREAM_RPC_CONTINUATION_CLOSED ||
                flushErrorStatus == EVENT_STREAM_RPC_CONNECTION_CLOSED ||
                (flushErrorStatus == EVENT_STREAM_RPC_CRT_ERROR &&
                 flushResult.crtError == AWS_ERROR_EVENT_STREAM_RPC_CONNECTION_CLOSED));

            auto requestFuture2 = getAllCustomers->Activate(getAllCustomersRequest, s_onMessageFlush);
            requestFuture2.wait();

            auto flush2Result = requestFuture2.get();
            auto flush2ErrorStatus = flush2Result.baseStatus;
            ASSERT_TRUE(
                flush2ErrorStatus == EVENT_STREAM_RPC_CONTINUATION_CLOSED ||
                flush2ErrorStatus == EVENT_STREAM_RPC_CONNECTION_CLOSED ||
                (flush2ErrorStatus == EVENT_STREAM_RPC_CRT_ERROR && flush2Result.crtError == AWS_ERROR_INVALID_STATE));

            return AWS_OP_SUCCESS;
        });
}

AWS_TEST_CASE(EchoClientOperationActivateCloseConnection, s_TestEchoClientOperationActivateCloseConnection);

static int s_TestEchoClientOperationActivateDoubleCloseContinuation(struct aws_allocator *allocator, void *ctx)
{
    return s_DoSimpleRequestRaceCheckTest(
        allocator,
        [](EventStreamClientTestContext &testContext, EchoTestRpcClient &client)
        {
            auto getAllCustomers = client.NewGetAllCustomers();
            GetAllCustomersRequest getAllCustomersRequest;

            auto requestFuture = getAllCustomers->Activate(getAllCustomersRequest, s_onMessageFlush);

            auto closeFuture = getAllCustomers->Close();
            auto closeFuture2 = getAllCustomers->Close();

            requestFuture.wait();
            auto flushErrorStatus = requestFuture.get().baseStatus;
            ASSERT_TRUE(
                flushErrorStatus == EVENT_STREAM_RPC_SUCCESS ||
                flushErrorStatus == EVENT_STREAM_RPC_CONTINUATION_CLOSED);

            closeFuture.wait();
            closeFuture2.wait();

            return AWS_OP_SUCCESS;
        });
}

AWS_TEST_CASE(
    EchoClientOperationActivateDoubleCloseContinuation,
    s_TestEchoClientOperationActivateDoubleCloseContinuation);

static int s_TestEchoClientOperationActivateWaitDoubleCloseContinuation(struct aws_allocator *allocator, void *ctx)
{
    return s_DoSimpleRequestRaceCheckTest(
        allocator,
        [](EventStreamClientTestContext &testContext, EchoTestRpcClient &client)
        {
            auto getAllCustomers = client.NewGetAllCustomers();
            GetAllCustomersRequest getAllCustomersRequest;

            auto requestFuture = getAllCustomers->Activate(getAllCustomersRequest, s_onMessageFlush);
            requestFuture.wait();

            auto closeFuture = getAllCustomers->Close();
            auto closeFuture2 = getAllCustomers->Close();

            auto flushErrorStatus = requestFuture.get().baseStatus;
            ASSERT_TRUE(
                flushErrorStatus == EVENT_STREAM_RPC_SUCCESS ||
                flushErrorStatus == EVENT_STREAM_RPC_CONTINUATION_CLOSED);

            closeFuture.wait();
            closeFuture2.wait();

            return AWS_OP_SUCCESS;
        });
}

AWS_TEST_CASE(
    EchoClientOperationActivateWaitDoubleCloseContinuation,
    s_TestEchoClientOperationActivateWaitDoubleCloseContinuation);

static int s_TestEchoClientOperationActivateWaitCloseContinuationWaitCloseContinuation(
    struct aws_allocator *allocator,
    void *ctx)
{
    return s_DoSimpleRequestRaceCheckTest(
        allocator,
        [](EventStreamClientTestContext &testContext, EchoTestRpcClient &client)
        {
            auto getAllCustomers = client.NewGetAllCustomers();
            GetAllCustomersRequest getAllCustomersRequest;

            auto requestFuture = getAllCustomers->Activate(getAllCustomersRequest, s_onMessageFlush);
            auto flushErrorStatus = requestFuture.get().baseStatus;
            ASSERT_TRUE(
                flushErrorStatus == EVENT_STREAM_RPC_SUCCESS ||
                flushErrorStatus == EVENT_STREAM_RPC_CONTINUATION_CLOSED);

            auto closeFuture = getAllCustomers->Close();
            closeFuture.wait();

            auto closeFuture2 = getAllCustomers->Close();
            closeFuture2.wait();

            return AWS_OP_SUCCESS;
        });
}

AWS_TEST_CASE(
    EchoClientOperationActivateWaitCloseContinuationWaitCloseContinuation,
    s_TestEchoClientOperationActivateWaitCloseContinuationWaitCloseContinuation);

static int s_TestEchoClientOperationActivateShutdown(struct aws_allocator *allocator, void *ctx)
{
    return s_DoSimpleRequestRaceCheckTest(
        allocator,
        [](EventStreamClientTestContext &testContext, EchoTestRpcClient &client)
        {
            auto getAllCustomers = client.NewGetAllCustomers();
            GetAllCustomersRequest getAllCustomersRequest;

            auto requestFuture = getAllCustomers->Activate(getAllCustomersRequest, s_onMessageFlush);
            getAllCustomers = nullptr;
            auto flushErrorStatus = requestFuture.get().baseStatus;
            ASSERT_TRUE(
                flushErrorStatus == EVENT_STREAM_RPC_SUCCESS ||
                flushErrorStatus == EVENT_STREAM_RPC_CONTINUATION_CLOSED);

            return AWS_OP_SUCCESS;
        });
}

AWS_TEST_CASE(EchoClientOperationActivateShutdown, s_TestEchoClientOperationActivateShutdown);

static int s_TestEchoClientOperationActivateShutdownDropFuture(struct aws_allocator *allocator, void *ctx)
{
    return s_DoSimpleRequestRaceCheckTest(
        allocator,
        [](EventStreamClientTestContext &testContext, EchoTestRpcClient &client)
        {
            auto getAllCustomers = client.NewGetAllCustomers();

            GetAllCustomersRequest getAllCustomersRequest;
            getAllCustomers->Activate(getAllCustomersRequest, s_onMessageFlush);

            return AWS_OP_SUCCESS;
        });
}

AWS_TEST_CASE(EchoClientOperationActivateShutdownDropFuture, s_TestEchoClientOperationActivateShutdownDropFuture);

static int s_TestEchoClientOperationActivateWaitCloseShutdown(struct aws_allocator *allocator, void *ctx)
{
    return s_DoSimpleRequestRaceCheckTest(
        allocator,
        [](EventStreamClientTestContext &testContext, EchoTestRpcClient &client)
        {
            auto getAllCustomers = client.NewGetAllCustomers();
            GetAllCustomersRequest getAllCustomersRequest;

            auto requestFuture = getAllCustomers->Activate(getAllCustomersRequest, s_onMessageFlush);
            auto flushErrorStatus = requestFuture.get().baseStatus;
            ASSERT_INT_EQUALS(EVENT_STREAM_RPC_SUCCESS, flushErrorStatus);

            auto closeFuture = getAllCustomers->Close();
            getAllCustomers = nullptr;

            closeFuture.wait();

            return AWS_OP_SUCCESS;
        });
}

AWS_TEST_CASE(EchoClientOperationActivateWaitCloseShutdown, s_TestEchoClientOperationActivateWaitCloseShutdown);

static int s_TestEchoClientOperationActivateWaitCloseShutdownDropFuture(struct aws_allocator *allocator, void *ctx)
{
    return s_DoSimpleRequestRaceCheckTest(
        allocator,
        [](EventStreamClientTestContext &testContext, EchoTestRpcClient &client)
        {
            auto getAllCustomers = client.NewGetAllCustomers();
            GetAllCustomersRequest getAllCustomersRequest;

            auto requestFuture = getAllCustomers->Activate(getAllCustomersRequest, s_onMessageFlush);
            auto flushErrorStatus = requestFuture.get().baseStatus;
            ASSERT_INT_EQUALS(EVENT_STREAM_RPC_SUCCESS, flushErrorStatus);

            getAllCustomers->Close();

            return AWS_OP_SUCCESS;
        });
}

AWS_TEST_CASE(
    EchoClientOperationActivateWaitCloseShutdownDropFuture,
    s_TestEchoClientOperationActivateWaitCloseShutdownDropFuture);

class EchoStreamMessagesTestHandler : public EchoStreamMessagesStreamHandler
{
  public:
    EchoStreamMessagesTestHandler(Aws::Crt::Allocator *allocator, bool closeOnError)
        : m_allocator(allocator), m_closeOnError(closeOnError)
    {
    }

    void OnStreamEvent(EchoStreamingMessage *response) override
    {
        std::lock_guard<std::mutex> lock(m_lock);

        m_messages.push_back(Aws::Crt::MakeShared<EchoStreamingMessage>(m_allocator, *response));
        m_signal.notify_all();
    }

    bool OnStreamError(RpcError rpcError) override
    {
        std::lock_guard<std::mutex> lock(m_lock);

        m_rpcErrors.push_back(rpcError);
        m_signal.notify_all();

        return m_closeOnError;
    }

    bool OnStreamError(OperationError *operationError) override
    {
        (void)operationError;

        return m_closeOnError;
    }

    void WaitForMessages(size_t count)
    {
        std::unique_lock<std::mutex> lock(m_lock);
        m_signal.wait(lock, [this, count]() { return m_messages.size() == count; });
    }

    int ValidateMessages(MessageData &expectedData)
    {
        std::lock_guard<std::mutex> lock(m_lock);
        for (size_t i = 0; i < m_messages.size(); i++)
        {
            const auto &message = m_messages[i];
            ASSERT_SUCCESS(s_checkMessageDataEquality(expectedData, message->GetStreamMessage().value()));
        }

        return AWS_OP_SUCCESS;
    }

  private:
    Aws::Crt::Allocator *m_allocator;
    bool m_closeOnError;

    std::mutex m_lock;
    std::condition_variable m_signal;

    Aws::Crt::Vector<std::shared_ptr<EchoStreamingMessage>> m_messages;
    Vector<RpcError> m_rpcErrors;
};

constexpr size_t STREAM_MESSAGE_COUNT = 10;

static int s_DoTestEchoClientStreamingOperationEchoSuccess(
    struct aws_allocator *allocator,
    std::function<void(MessageData &)> messageDataBuilder)
{
    ApiHandle apiHandle(allocator);
    EventStreamClientTestContext testContext(allocator);
    if (!testContext.isValidEnvironment())
    {
        printf("Environment Variables are not set for the test, skipping...");
        return AWS_OP_SKIP;
    }

    MessageData expecectedMessageData;
    messageDataBuilder(expecectedMessageData);

    {
        auto handler = Aws::Crt::MakeShared<EchoStreamMessagesTestHandler>(allocator, allocator, false);
        ConnectionLifecycleHandler lifecycleHandler;
        Awstest::EchoTestRpcClient client(*testContext.clientBootstrap, allocator);
        auto connectedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_SUCCESS);

        auto echoStreamMessages = client.NewEchoStreamMessages(handler);
        EchoStreamingRequest echoStreamingMessageRequest;
        auto activateFuture = echoStreamMessages->Activate(echoStreamingMessageRequest, s_onMessageFlush);
        activateFuture.wait();

        auto result = echoStreamMessages->GetResult().get();
        ASSERT_TRUE(result);

        Aws::Crt::Vector<std::future<RpcError>> streamMessageFutures;
        for (size_t i = 0; i < STREAM_MESSAGE_COUNT; ++i)
        {
            EchoStreamingMessage streamMessage;
            MessageData messageData;
            messageDataBuilder(messageData);
            streamMessage.SetStreamMessage(messageData);

            auto streamFuture = echoStreamMessages->SendStreamMessage(streamMessage, s_onMessageFlush);
            streamMessageFutures.emplace_back(std::move(streamFuture));
        }

        handler->WaitForMessages(STREAM_MESSAGE_COUNT);
        ASSERT_INT_EQUALS(STREAM_MESSAGE_COUNT, streamMessageFutures.size());

        for (size_t i = 0; i < streamMessageFutures.size(); i++)
        {
            auto streamResult = streamMessageFutures[i].get();
            ASSERT_INT_EQUALS(EVENT_STREAM_RPC_SUCCESS, streamResult.baseStatus);
        }

        ASSERT_SUCCESS(handler->ValidateMessages(expecectedMessageData));
    }

    return AWS_OP_SUCCESS;
}

static int s_TestEchoClientStreamingOperationEchoSuccessString(struct aws_allocator *allocator, void *ctx)
{
    return s_DoTestEchoClientStreamingOperationEchoSuccess(
        allocator,
        [](MessageData &messageData)
        {
            Aws::Crt::String value = "Hello World";
            messageData.SetStringMessage(value);
        });
}

AWS_TEST_CASE(EchoClientStreamingOperationEchoSuccessString, s_TestEchoClientStreamingOperationEchoSuccessString);

static int s_TestEchoClientStreamingOperationEchoSuccessBoolean(struct aws_allocator *allocator, void *ctx)
{
    return s_DoTestEchoClientStreamingOperationEchoSuccess(
        allocator, [](MessageData &messageData) { messageData.SetBooleanMessage(true); });
}

AWS_TEST_CASE(EchoClientStreamingOperationEchoSuccessBoolean, s_TestEchoClientStreamingOperationEchoSuccessBoolean);

static int s_TestEchoClientStreamingOperationEchoSuccessTime(struct aws_allocator *allocator, void *ctx)
{
    return s_DoTestEchoClientStreamingOperationEchoSuccess(
        allocator, [](MessageData &messageData) { messageData.SetTimeMessage(Aws::Crt::DateTime::Now()); });
}

AWS_TEST_CASE(EchoClientStreamingOperationEchoSuccessTime, s_TestEchoClientStreamingOperationEchoSuccessTime);

static int s_TestEchoClientStreamingOperationEchoSuccessDocument(struct aws_allocator *allocator, void *ctx)
{
    return s_DoTestEchoClientStreamingOperationEchoSuccess(
        allocator,
        [](MessageData &messageData)
        {
            Aws::Crt::JsonObject subobject;
            subobject.WithString("Hello", "There");
            Aws::Crt::JsonObject document;
            document.WithInt64("Derp", 21);
            document.WithObject("DailyAffirmations", subobject);

            messageData.SetDocumentMessage(document);
        });
}

AWS_TEST_CASE(EchoClientStreamingOperationEchoSuccessDocument, s_TestEchoClientStreamingOperationEchoSuccessDocument);

static int s_TestEchoClientStreamingOperationEchoSuccessEnum(struct aws_allocator *allocator, void *ctx)
{
    return s_DoTestEchoClientStreamingOperationEchoSuccess(
        allocator, [](MessageData &messageData) { messageData.SetEnumMessage(FruitEnum::FRUIT_ENUM_PINEAPPLE); });
}

AWS_TEST_CASE(EchoClientStreamingOperationEchoSuccessEnum, s_TestEchoClientStreamingOperationEchoSuccessEnum);

static int s_TestEchoClientStreamingOperationEchoSuccessBlob(struct aws_allocator *allocator, void *ctx)
{
    return s_DoTestEchoClientStreamingOperationEchoSuccess(
        allocator,
        [](MessageData &messageData)
        {
            Aws::Crt::Vector<uint8_t> blob = {1, 2, 3, 4, 5};
            messageData.SetBlobMessage(blob);
        });
}

AWS_TEST_CASE(EchoClientStreamingOperationEchoSuccessBlob, s_TestEchoClientStreamingOperationEchoSuccessBlob);

static int s_TestEchoClientStreamingOperationEchoSuccessStringList(struct aws_allocator *allocator, void *ctx)
{
    return s_DoTestEchoClientStreamingOperationEchoSuccess(
        allocator,
        [](MessageData &messageData)
        {
            Aws::Crt::Vector<Aws::Crt::String> stringList = {"1", "2", "Toasty", "Mctoaster"};
            messageData.SetStringListMessage(stringList);
        });
}

AWS_TEST_CASE(
    EchoClientStreamingOperationEchoSuccessStringList,
    s_TestEchoClientStreamingOperationEchoSuccessStringList);

static int s_TestEchoClientStreamingOperationEchoSuccessPairList(struct aws_allocator *allocator, void *ctx)
{
    return s_DoTestEchoClientStreamingOperationEchoSuccess(
        allocator,
        [](MessageData &messageData)
        {
            Pair pair1;
            pair1.SetKey("Uff");
            pair1.SetValue("Dah");

            Pair pair2;
            pair2.SetKey("Hello");
            pair2.SetValue("World");

            Aws::Crt::Vector<Pair> pairList = {pair1, pair2};
            messageData.SetKeyValuePairList(pairList);
        });
}

AWS_TEST_CASE(EchoClientStreamingOperationEchoSuccessPairList, s_TestEchoClientStreamingOperationEchoSuccessPairList);

static int s_TestEchoClientStreamingOperationEchoSuccessProductMap(struct aws_allocator *allocator, void *ctx)
{
    return s_DoTestEchoClientStreamingOperationEchoSuccess(
        allocator,
        [](MessageData &messageData)
        {
            Aws::Crt::Map<String, Product> productMap = {};
            Product product1;
            product1.SetName("Derp");
            product1.SetPrice(4.0);

            Product product2;
            product2.SetName("Can Of Derp");
            product2.SetPrice(7.5);

            productMap[product1.GetName().value()] = product1;
            productMap[product2.GetName().value()] = product2;

            messageData.SetStringToValue(productMap);
        });
}

AWS_TEST_CASE(
    EchoClientStreamingOperationEchoSuccessProductMap,
    s_TestEchoClientStreamingOperationEchoSuccessProductMap);

class CauseServiceErrorTestHandler : public CauseStreamServiceToErrorStreamHandler
{
  public:
    CauseServiceErrorTestHandler(Aws::Crt::Allocator *allocator, bool closeOnError)
        : m_allocator(allocator), m_closeOnError(closeOnError)
    {
    }

    void OnStreamEvent(EchoStreamingMessage *response) override { (void)response; }

    bool OnStreamError(RpcError rpcError) override
    {
        (void)rpcError;

        return m_closeOnError;
    }

    bool OnStreamError(ServiceError *serviceError) override
    {
        std::lock_guard<std::mutex> lock(m_lock);

        m_serviceErrors.push_back(Aws::Crt::MakeShared<ServiceError>(m_allocator, *serviceError));
        m_signal.notify_all();

        return m_closeOnError;
    }

    bool OnStreamError(OperationError *operationError) override
    {
        (void)operationError;

        return m_closeOnError;
    }

    void WaitForErrors(size_t count)
    {
        std::unique_lock<std::mutex> lock(m_lock);
        m_signal.wait(lock, [this, count]() { return m_serviceErrors.size() == count; });
    }

    int ValidateErrors(ServiceError &expectedError)
    {
        std::lock_guard<std::mutex> lock(m_lock);
        for (size_t i = 0; i < m_serviceErrors.size(); i++)
        {
            const auto &error = m_serviceErrors[i];

            auto errorMessage = error->GetMessage();
            auto expectedErrorMessage = expectedError.GetMessage();
            auto errorValue = error->GetValue();
            auto expectedErrorValue = expectedError.GetValue();

            ASSERT_TRUE(errorMessage.has_value() == expectedErrorMessage.has_value());
            ASSERT_TRUE(errorValue.has_value() == expectedErrorValue.has_value());

            if (expectedErrorMessage.has_value())
            {
                ASSERT_TRUE(expectedErrorMessage.value() == errorMessage.value());
            }

            if (expectedErrorValue.has_value())
            {
                ASSERT_TRUE(expectedErrorValue.value() == errorValue.value());
            }
        }

        return AWS_OP_SUCCESS;
    }

  private:
    Aws::Crt::Allocator *m_allocator;
    bool m_closeOnError;

    std::mutex m_lock;
    std::condition_variable m_signal;

    Aws::Crt::Vector<std::shared_ptr<ServiceError>> m_serviceErrors;
};

static int s_TestEchoClientStreamingOperationCauseServiceError(struct aws_allocator *allocator, void *ctx)
{
    ApiHandle apiHandle(allocator);
    EventStreamClientTestContext testContext(allocator);
    if (!testContext.isValidEnvironment())
    {
        printf("Environment Variables are not set for the test, skipping...");
        return AWS_OP_SKIP;
    }

    {
        auto handler = Aws::Crt::MakeShared<CauseServiceErrorTestHandler>(allocator, allocator, false);
        ConnectionLifecycleHandler lifecycleHandler;
        Awstest::EchoTestRpcClient client(*testContext.clientBootstrap, allocator);
        auto connectedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_SUCCESS);

        auto causeStreamServiceError = client.NewCauseStreamServiceToError(handler);
        EchoStreamingRequest request;
        auto activateFuture = causeStreamServiceError->Activate(request, s_onMessageFlush);
        activateFuture.wait();

        auto result = causeStreamServiceError->GetResult().get();
        ASSERT_TRUE(result);

        EchoStreamingMessage streamMessage;
        MessageData messageData;
        Aws::Crt::String value = "Hello World";
        messageData.SetStringMessage(value);
        streamMessage.SetStreamMessage(messageData);

        auto streamFuture = causeStreamServiceError->SendStreamMessage(streamMessage, s_onMessageFlush);

        handler->WaitForErrors(1);

        auto streamResult = streamFuture.get();
        ASSERT_INT_EQUALS(EVENT_STREAM_RPC_SUCCESS, streamResult.baseStatus);

        ServiceError expectedError;
        expectedError.SetMessage("Intentionally caused ServiceError on stream");

        ASSERT_SUCCESS(handler->ValidateErrors(expectedError));
    }

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(EchoClientStreamingOperationCauseServiceError, s_TestEchoClientStreamingOperationCauseServiceError);

static int s_TestEchoClientStreamingOperationSendCloseOperation(struct aws_allocator *allocator, void *ctx)
{
    return s_DoSimpleRequestRaceCheckTest(
        allocator,
        [allocator](EventStreamClientTestContext &testContext, EchoTestRpcClient &client)
        {
            auto handler = Aws::Crt::MakeShared<EchoStreamMessagesTestHandler>(allocator, allocator, false);
            auto echoStreamMessages = client.NewEchoStreamMessages(handler);
            EchoStreamingRequest echoStreamingMessageRequest;
            auto activateFuture = echoStreamMessages->Activate(echoStreamingMessageRequest, s_onMessageFlush);
            activateFuture.wait();

            auto result = echoStreamMessages->GetResult().get();
            ASSERT_TRUE(result);

            EchoStreamingMessage streamMessage;
            MessageData messageData;
            Aws::Crt::String value = "Hello World";
            messageData.SetStringMessage(value);

            streamMessage.SetStreamMessage(messageData);

            auto streamFuture = echoStreamMessages->SendStreamMessage(streamMessage, s_onMessageFlush);

            auto closeFuture = echoStreamMessages->Close();
            closeFuture.wait();

            auto streamResultStatus = streamFuture.get().baseStatus;
            ASSERT_TRUE(
                streamResultStatus == EVENT_STREAM_RPC_SUCCESS ||
                streamResultStatus == EVENT_STREAM_RPC_CONTINUATION_CLOSED);

            return AWS_OP_SUCCESS;
        });
}

AWS_TEST_CASE(EchoClientStreamingOperationSendCloseOperation, s_TestEchoClientStreamingOperationSendCloseOperation);

static int s_TestEchoClientStreamingOperationSendDropOperation(struct aws_allocator *allocator, void *ctx)
{

    return s_DoSimpleRequestRaceCheckTest(
        allocator,
        [allocator](EventStreamClientTestContext &testContext, EchoTestRpcClient &client)
        {
            auto handler = Aws::Crt::MakeShared<EchoStreamMessagesTestHandler>(allocator, allocator, false);
            auto echoStreamMessages = client.NewEchoStreamMessages(handler);
            EchoStreamingRequest echoStreamingMessageRequest;
            auto activateFuture = echoStreamMessages->Activate(echoStreamingMessageRequest, s_onMessageFlush);
            activateFuture.wait();

            auto result = echoStreamMessages->GetResult().get();
            ASSERT_TRUE(result);

            EchoStreamingMessage streamMessage;
            MessageData messageData;
            Aws::Crt::String value = "Hello World";
            messageData.SetStringMessage(value);

            streamMessage.SetStreamMessage(messageData);

            auto streamFuture = echoStreamMessages->SendStreamMessage(streamMessage, s_onMessageFlush);

            echoStreamMessages = nullptr;

            auto streamResultStatus = streamFuture.get().baseStatus;
            ASSERT_TRUE(
                streamResultStatus == EVENT_STREAM_RPC_SUCCESS ||
                streamResultStatus == EVENT_STREAM_RPC_CONTINUATION_CLOSED);

            return AWS_OP_SUCCESS;
        });
}

AWS_TEST_CASE(EchoClientStreamingOperationSendDropOperation, s_TestEchoClientStreamingOperationSendDropOperation);

static int s_TestEchoClientStreamingOperationSendCloseConnection(struct aws_allocator *allocator, void *ctx)
{
    return s_DoClientScopedRaceCheckTest(
        allocator,
        [allocator](EventStreamClientTestContext &testContext, EchoTestRpcClient &client)
        {
            auto handler = Aws::Crt::MakeShared<EchoStreamMessagesTestHandler>(allocator, allocator, false);
            auto echoStreamMessages = client.NewEchoStreamMessages(handler);
            EchoStreamingRequest echoStreamingMessageRequest;
            auto activateFuture = echoStreamMessages->Activate(echoStreamingMessageRequest, s_onMessageFlush);
            activateFuture.wait();

            auto result = echoStreamMessages->GetResult().get();
            ASSERT_TRUE(result);

            EchoStreamingMessage streamMessage;
            MessageData messageData;
            Aws::Crt::String value = "Hello World";
            messageData.SetStringMessage(value);

            streamMessage.SetStreamMessage(messageData);

            auto streamFuture = echoStreamMessages->SendStreamMessage(streamMessage, s_onMessageFlush);

            client.Close();

            auto streamResult = streamFuture.get();
            auto streamResultStatus = streamResult.baseStatus;
            ASSERT_TRUE(
                streamResultStatus == EVENT_STREAM_RPC_SUCCESS ||
                streamResultStatus == EVENT_STREAM_RPC_CONTINUATION_CLOSED ||
                streamResultStatus == EVENT_STREAM_RPC_CONNECTION_CLOSED ||
                (streamResultStatus == EVENT_STREAM_RPC_CRT_ERROR &&
                 streamResult.crtError == AWS_ERROR_EVENT_STREAM_RPC_CONNECTION_CLOSED));

            return AWS_OP_SUCCESS;
        });
}

AWS_TEST_CASE(EchoClientStreamingOperationSendCloseConnection, s_TestEchoClientStreamingOperationSendCloseConnection);

static int s_TestEchoClientStreamingOperationUnactivatedSend(struct aws_allocator *allocator, void *ctx)
{

    return s_DoSimpleRequestRaceCheckTest(
        allocator,
        [allocator](EventStreamClientTestContext &testContext, EchoTestRpcClient &client)
        {
            auto handler = Aws::Crt::MakeShared<EchoStreamMessagesTestHandler>(allocator, allocator, false);
            auto echoStreamMessages = client.NewEchoStreamMessages(handler);

            EchoStreamingMessage streamMessage;
            MessageData messageData;
            Aws::Crt::String value = "Hello World";
            messageData.SetStringMessage(value);

            streamMessage.SetStreamMessage(messageData);

            auto streamFuture = echoStreamMessages->SendStreamMessage(streamMessage, s_onMessageFlush);

            auto streamResultStatus = streamFuture.get().baseStatus;
            ASSERT_INT_EQUALS(EVENT_STREAM_RPC_CONTINUATION_NOT_YET_OPENED, streamResultStatus);

            return AWS_OP_SUCCESS;
        });
}

AWS_TEST_CASE(EchoClientStreamingOperationUnactivatedSend, s_TestEchoClientStreamingOperationUnactivatedSend);

class EchoStressContext
{
  public:
    ~EchoStressContext()
    {
        m_client = nullptr;
        m_stream = nullptr;
    }

    std::shared_ptr<EchoTestRpcClient> m_client;
    std::shared_ptr<TestLifecycleHandler> m_lifecycleHandler;
    std::shared_ptr<EchoStreamMessagesOperation> m_stream;
    std::shared_ptr<EchoStreamMessagesTestHandler> m_streamHandler;
    Aws::Crt::Vector<std::shared_ptr<TestLifecycleHandler>> m_oldLifecycleHandlers;

    Aws::Crt::Allocator *m_allocator;
};

using StressAction = std::function<void(const EventStreamClientTestContext &, EchoStressContext &)>;

struct WeightedAction
{
    StressAction m_action;
    size_t m_weight;
};

class EchoStressActionDistribution
{
  public:
    EchoStressActionDistribution();

    void AddAction(StressAction &&action, size_t weight);

    void ApplyRandomAction(const EventStreamClientTestContext &testContext, EchoStressContext &stressContext);

  private:
    Aws::Crt::Vector<WeightedAction> m_actions;

    size_t m_totalWeight;
};

EchoStressActionDistribution::EchoStressActionDistribution() : m_actions(), m_totalWeight(0) {}

void EchoStressActionDistribution::AddAction(StressAction &&action, size_t weight)
{
    if (weight == 0)
    {
        return;
    }

    m_actions.emplace_back(std::move(WeightedAction{std::move(action), weight}));
    m_totalWeight += weight;
}

void EchoStressActionDistribution::ApplyRandomAction(
    const EventStreamClientTestContext &testContext,
    EchoStressContext &stressContext)
{
    if (m_totalWeight == 0 || m_actions.empty())
    {
        return;
    }

    size_t accumulator = std::rand() % m_totalWeight;
    for (size_t i = 0; i < m_actions.size(); ++i)
    {
        const auto &action = m_actions[i];

        if (accumulator < action.m_weight)
        {
            action.m_action(testContext, stressContext);
            break;
        }

        accumulator -= action.m_weight;
    }
}

static void s_CreateClientIfNeeded(const EventStreamClientTestContext &testContext, EchoStressContext &stressContext)
{
    if (stressContext.m_client != nullptr)
    {
        return;
    }

    stressContext.m_lifecycleHandler = Aws::Crt::MakeShared<TestLifecycleHandler>(stressContext.m_allocator);
    stressContext.m_client = Aws::Crt::MakeShared<EchoTestRpcClient>(
        stressContext.m_allocator, *testContext.clientBootstrap, stressContext.m_allocator);
}

static void s_BeginClientAction(const EventStreamClientTestContext &testContext, EchoStressContext &stressContext)
{
    AWS_LOGF_INFO(AWS_LS_COMMON_GENERAL, "EchoStressTest - BeginClient");
    s_CreateClientIfNeeded(testContext, stressContext);

    auto connectFuture = stressContext.m_client->Connect(*stressContext.m_lifecycleHandler);

    if (std::rand() % 2)
    {
        AWS_LOGF_INFO(AWS_LS_COMMON_GENERAL, "EchoStressTest - BeginClient - waiting on connect future");
        connectFuture.wait();
    }
}

static void s_EndClientAction(const EventStreamClientTestContext &testContext, EchoStressContext &stressContext)
{
    (void)testContext;

    AWS_LOGF_INFO(AWS_LS_COMMON_GENERAL, "EchoStressTest - EndClient");
    if (stressContext.m_client == nullptr)
    {
        return;
    }

    if (std::rand() % 2)
    {
        AWS_LOGF_INFO(AWS_LS_COMMON_GENERAL, "EchoStressTest - EndClient - Closing");
        stressContext.m_client->Close();
        if (std::rand() % 2)
        {
            AWS_LOGF_INFO(AWS_LS_COMMON_GENERAL, "EchoStressTest - EndClient - Waiting On Close Completion");
            stressContext.m_lifecycleHandler->WaitOnDisconnected();
        }
    }
    else
    {
        AWS_LOGF_INFO(AWS_LS_COMMON_GENERAL, "EchoStressTest - EndClient - Dropping");
        stressContext.m_oldLifecycleHandlers.push_back(stressContext.m_lifecycleHandler);
        stressContext.m_lifecycleHandler = nullptr;
        stressContext.m_client = nullptr;
    }
}

static void s_RequestResponseAction(const EventStreamClientTestContext &testContext, EchoStressContext &stressContext)
{
    (void)testContext;

    AWS_LOGF_INFO(AWS_LS_COMMON_GENERAL, "EchoStressTest - RequestResponse");
    s_CreateClientIfNeeded(testContext, stressContext);

    auto echoMessage = stressContext.m_client->NewEchoMessage();

    EchoMessageRequest echoMessageRequest;
    MessageData messageData;
    messageData.SetStringMessage("HelloWorld");
    echoMessageRequest.SetMessage(messageData);

    auto requestFuture = echoMessage->Activate(echoMessageRequest, s_onMessageFlush);

    if (std::rand() % 2)
    {
        AWS_LOGF_INFO(AWS_LS_COMMON_GENERAL, "EchoStressTest - RequestResponse - waiting on flush future");
        auto flushResult = requestFuture.get();

        if (flushResult.baseStatus == EVENT_STREAM_RPC_SUCCESS)
        {
            if (std::rand() % 2)
            {
                AWS_LOGF_INFO(AWS_LS_COMMON_GENERAL, "EchoStressTest - RequestResponse - waiting on result");
                auto result = echoMessage->GetResult().get();
            }
        }
    }
}

static void s_BeginStreamAction(const EventStreamClientTestContext &testContext, EchoStressContext &stressContext)
{
    AWS_LOGF_INFO(AWS_LS_COMMON_GENERAL, "EchoStressTest - BeginStream");
    s_CreateClientIfNeeded(testContext, stressContext);

    stressContext.m_stream = nullptr;
    stressContext.m_streamHandler = nullptr;

    stressContext.m_streamHandler = Aws::Crt::MakeShared<EchoStreamMessagesTestHandler>(
        stressContext.m_allocator, stressContext.m_allocator, false);
    auto echoStreamMessages = stressContext.m_client->NewEchoStreamMessages(stressContext.m_streamHandler);

    EchoStreamingRequest echoStreamingMessageRequest;
    auto activateFuture = echoStreamMessages->Activate(echoStreamingMessageRequest, s_onMessageFlush);

    if (std::rand() % 2)
    {
        AWS_LOGF_INFO(AWS_LS_COMMON_GENERAL, "EchoStressTest - BeginStream - waiting on Activate flush future");
        auto flushResult = activateFuture.get();
        if (flushResult.baseStatus == EVENT_STREAM_RPC_SUCCESS)
        {
            if (std::rand() % 2)
            {
                AWS_LOGF_INFO(AWS_LS_COMMON_GENERAL, "EchoStressTest - BeginStream - waiting on Activate result");
                auto result = echoStreamMessages->GetResult().get();
            }
        }
    }
}

static void s_SendStreamAction(const EventStreamClientTestContext &testContext, EchoStressContext &stressContext)
{
    (void)testContext;

    if (stressContext.m_stream == nullptr)
    {
        AWS_LOGF_INFO(AWS_LS_COMMON_GENERAL, "EchoStressTest - SendStream - nothing to do");
        return;
    }

    AWS_LOGF_INFO(AWS_LS_COMMON_GENERAL, "EchoStressTest - SendStream");
    EchoStreamingMessage streamMessage;
    MessageData messageData;
    Aws::Crt::String value = "Hello World";
    messageData.SetStringMessage(value);

    streamMessage.SetStreamMessage(messageData);

    auto streamFuture = stressContext.m_stream->SendStreamMessage(streamMessage, s_onMessageFlush);
    if (std::rand() % 2)
    {
        AWS_LOGF_INFO(AWS_LS_COMMON_GENERAL, "EchoStressTest - SendStream - waiting on flush future");
        streamFuture.get();
    }
}

static void s_EndStreamAction(const EventStreamClientTestContext &testContext, EchoStressContext &stressContext)
{
    (void)testContext;

    if (stressContext.m_stream == nullptr)
    {
        AWS_LOGF_INFO(AWS_LS_COMMON_GENERAL, "EchoStressTest - EndStream - nothing to do");
        return;
    }

    if (std::rand() % 2)
    {
        AWS_LOGF_INFO(AWS_LS_COMMON_GENERAL, "EchoStressTest - EndStream - Closing");
        auto closeFuture = stressContext.m_stream->Close();
        if (std::rand() % 2)
        {
            AWS_LOGF_INFO(AWS_LS_COMMON_GENERAL, "EchoStressTest - EndStream - Waiting on Close future");
            closeFuture.wait();
        }
    }
    else
    {
        AWS_LOGF_INFO(AWS_LS_COMMON_GENERAL, "EchoStressTest - EndStream - Dropping");
        stressContext.m_stream = nullptr;
        stressContext.m_streamHandler = nullptr;
    }
}

constexpr size_t STRESS_TEST_STEP_COUNT = 50000;

static int s_TestEchoClientOperationStress(struct aws_allocator *allocator, void *ctx)
{
    (void)ctx;

    ApiHandle apiHandle(allocator);

    {
        EchoStressContext stressContext;
        stressContext.m_allocator = allocator;

        EventStreamClientTestContext testContext(allocator);
        if (!testContext.isValidEnvironment())
        {
            printf("Environment Variables are not set for the test, skipping...");
            return AWS_OP_SKIP;
        }

        EchoStressActionDistribution actionDistribution;
        actionDistribution.AddAction(s_BeginClientAction, 3);
        actionDistribution.AddAction(s_EndClientAction, 1);
        actionDistribution.AddAction(s_RequestResponseAction, 4);
        actionDistribution.AddAction(s_BeginStreamAction, 2);
        actionDistribution.AddAction(s_SendStreamAction, 4);
        actionDistribution.AddAction(s_EndStreamAction, 1);

        for (size_t i = 0; i < STRESS_TEST_STEP_COUNT; ++i)
        {
            actionDistribution.ApplyRandomAction(testContext, stressContext);
        }
    }

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(EchoClientOperationStress, s_TestEchoClientOperationStress);
