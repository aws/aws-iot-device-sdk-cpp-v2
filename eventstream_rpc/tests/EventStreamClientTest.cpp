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

        semaphore.notify_one();
    }

    void WaitOnCondition(std::function<bool(void)> condition)
    {
        std::unique_lock<std::mutex> semaphoreULock(semaphoreLock);
        semaphore.wait(semaphoreULock, condition);
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
            clientStatus == EVENT_STREAM_RPC_CRT_ERROR || clientStatus == EVENT_STREAM_RPC_CONNECTION_ACCESS_DENIED);
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
        EventStreamRpcStatusCode clientStatus = future.get().baseStatus;

        ASSERT_TRUE(
            clientStatus == EVENT_STREAM_RPC_CRT_ERROR || clientStatus == EVENT_STREAM_RPC_CONNECTION_ACCESS_DENIED);
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

static void s_onMessageFlush(int errorCode)
{
    (void)errorCode;
}

template <typename T>
static bool s_messageDataMembersAreEqual(const Aws::Crt::Optional<T> &expectedValue, const Aws::Crt::Optional<T> &actualValue)
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

        auto result = echoMessage->GetOperationResult().get();
        ASSERT_FALSE(result);

        auto error = result.GetRpcError();
        ASSERT_INT_EQUALS(EVENT_STREAM_RPC_CONNECTION_CLOSED, error.baseStatus);
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
        ASSERT_INT_EQUALS(EVENT_STREAM_RPC_CONNECTION_CLOSED, requestFuture.get().baseStatus);

        auto result = echoMessage->GetOperationResult().get();
        ASSERT_FALSE(result);

        auto error = result.GetRpcError();
        ASSERT_INT_EQUALS(EVENT_STREAM_RPC_CONNECTION_CLOSED, error.baseStatus);
    }

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(EchoClientOperationEchoFailureDisconnected, s_TestEchoClientOperationEchoFailureDisconnected);

#ifdef NEVER

AWS_TEST_CASE_FIXTURE(EchoOperation, s_testSetup, s_TestEchoOperation, s_testTeardown, &s_testContext);
static int s_TestEchoOperation(struct aws_allocator *allocator, void *ctx)
{
    auto *testContext = static_cast<EventStreamClientTestContext *>(ctx);
    if (!s_isEchoserverSetup(*testContext))
    {
        printf("Environment Variables are not set for the test, skip the test");
        return AWS_OP_SKIP;
    }

    ConnectionLifecycleHandler lifecycleHandler;
    Aws::Crt::String expectedMessage("Async I0 FTW");
    EchoMessageRequest echoMessageRequest;
    MessageData messageData;
    messageData.SetStringMessage(expectedMessage);

    /* Perform a regular echo operation. */
    {
        Awstest::EchoTestRpcClient client(*testContext->clientBootstrap, allocator);
        auto connectedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_SUCCESS);
        auto echoMessage = client.NewEchoMessage();
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

    /* Attempt a connection, close it, then try running operations as normal. */
    {
        ConnectionLifecycleHandler lifecycleHandler;
        Awstest::EchoTestRpcClient client(*testContext->clientBootstrap, allocator);
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
        ASSERT_TRUE(requestFuture.get().baseStatus == EVENT_STREAM_RPC_CONNECTION_CLOSED);
        auto result = echoMessage->GetOperationResult().get();
        ASSERT_FALSE(result);
        auto error = result.GetRpcError();
        ASSERT_TRUE(error.baseStatus == EVENT_STREAM_RPC_CONNECTION_CLOSED);
    }

    /* Perform a regular echo operation but one after another without waiting.
     * Only the response from the first operation will be received. */
    {
        Awstest::EchoTestRpcClient client(*testContext->clientBootstrap, allocator);
        auto connectedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_SUCCESS);
        auto echoMessage = client.NewEchoMessage();
        messageData.SetStringMessage(expectedMessage);
        echoMessageRequest.SetMessage(messageData);
        auto requestFuture = echoMessage->Activate(echoMessageRequest, s_onMessageFlush);
        requestFuture = echoMessage->Activate(echoMessageRequest, s_onMessageFlush);
        MessageData differentMessage;
        differentMessage.SetBooleanMessage(true);
        echoMessageRequest.SetMessage(differentMessage);
        requestFuture = echoMessage->Activate(echoMessageRequest, s_onMessageFlush);
        requestFuture.wait();
        auto result = echoMessage->GetResult().get();
        ASSERT_TRUE(result);
        auto response = result.GetOperationResponse();
        ASSERT_NOT_NULL(response);
        ASSERT_TRUE(response->GetMessage().value().GetStringMessage().value() == expectedMessage);
    }

    /* Closing the stream should be idempotent. */
    {
        Awstest::EchoTestRpcClient client(*testContext->clientBootstrap, allocator);

        auto connectedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_SUCCESS);
        auto echoMessage = client.NewEchoMessage();
        messageData.SetStringMessage(expectedMessage);
        echoMessageRequest.SetMessage(messageData);
        auto requestFuture = echoMessage->Activate(echoMessageRequest, s_onMessageFlush);
        requestFuture = echoMessage->Activate(echoMessageRequest, s_onMessageFlush);
        MessageData differentMessage;
        differentMessage.SetBooleanMessage(true);
        echoMessageRequest.SetMessage(differentMessage);
        requestFuture = echoMessage->Activate(echoMessageRequest, s_onMessageFlush);
        requestFuture.wait();
        echoMessage->Close().wait();
        echoMessage->Close().wait();
        echoMessage->Close().wait();
        echoMessage->Close().wait();
    }

    /* Close without waiting on activation or close futures. */
    {
        Awstest::EchoTestRpcClient client(*testContext->clientBootstrap, allocator);

        auto connectedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_SUCCESS);
        auto echoMessage = client.NewEchoMessage();
        messageData.SetStringMessage(expectedMessage);
        echoMessageRequest.SetMessage(messageData);
        auto requestFuture = echoMessage->Activate(echoMessageRequest, s_onMessageFlush);
        requestFuture = echoMessage->Activate(echoMessageRequest, s_onMessageFlush);
        MessageData differentMessage;
        differentMessage.SetBooleanMessage(true);
        echoMessageRequest.SetMessage(differentMessage);
        echoMessage->Activate(echoMessageRequest, s_onMessageFlush);
        echoMessage->Close();
        echoMessage->Close();
        echoMessage->Close();
        echoMessage->Close();
    }

    /* Close without waiting for TERMINATE_STREAM to flush then immediately trying to activate. */
    {
        Awstest::EchoTestRpcClient client(*testContext->clientBootstrap, allocator);

        auto connectedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_SUCCESS);
        auto echoMessage = client.NewEchoMessage();
        messageData.SetStringMessage(expectedMessage);
        echoMessageRequest.SetMessage(messageData);
        auto requestFuture = echoMessage->Activate(echoMessageRequest, s_onMessageFlush);
        requestFuture = echoMessage->Activate(echoMessageRequest, s_onMessageFlush);
        MessageData differentMessage;
        differentMessage.SetBooleanMessage(true);
        echoMessageRequest.SetMessage(differentMessage);
        requestFuture = echoMessage->Activate(echoMessageRequest, s_onMessageFlush);
        requestFuture.wait();
        auto closeFuture = echoMessage->Close();
        requestFuture = echoMessage->Activate(echoMessageRequest, s_onMessageFlush);
        closeFuture.wait();
        requestFuture.wait();
    }

    /* Connect thrice and verify that the future of the first attempt succeeds.
     * The rest of the attempts must fail with an error.
     * Use the client to perform an operation and verify that the operation still succeeds. */
    {
        Awstest::EchoTestRpcClient client(*testContext->clientBootstrap, allocator);
        auto connectedStatus = client.Connect(lifecycleHandler);
        auto failedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(failedStatus.get().baseStatus == EVENT_STREAM_RPC_CONNECTION_ALREADY_ESTABLISHED);
        failedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_SUCCESS);
        ASSERT_TRUE(failedStatus.get().baseStatus == EVENT_STREAM_RPC_CONNECTION_ALREADY_ESTABLISHED);
        auto echoMessage = client.NewEchoMessage();
        echoMessageRequest.SetMessage(messageData);
        auto requestFuture = echoMessage->Activate(echoMessageRequest, s_onMessageFlush);
        requestFuture.wait();
        auto result = echoMessage->GetResult().get();
        ASSERT_TRUE(result);
        auto response = result.GetOperationResponse();
        ASSERT_NOT_NULL(response);
        ASSERT_TRUE(response->GetMessage().value().GetStringMessage().value() == expectedMessage);
    }

    /* Connect twice sequentially.
     * Use the client to perform an operation and verify that the operation still succeeds. */
    {
        Awstest::EchoTestRpcClient client(*testContext->clientBootstrap, allocator);
        auto connectedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_SUCCESS);
        connectedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_CONNECTION_ALREADY_ESTABLISHED);
        auto echoMessage = client.NewEchoMessage();
        echoMessageRequest.SetMessage(messageData);
        auto requestFuture = echoMessage->Activate(echoMessageRequest, s_onMessageFlush);
        requestFuture.wait();
        auto result = echoMessage->GetResult().get();
        ASSERT_TRUE(result);
        auto response = result.GetOperationResponse();
        ASSERT_NOT_NULL(response);
        ASSERT_TRUE(response->GetMessage().value().GetStringMessage().value() == expectedMessage);
    }

    return AWS_OP_SUCCESS;
}

class ThreadPool
{
  public:
    ThreadPool(int numThreads = std::thread::hardware_concurrency()) noexcept
        : m_numThreads(numThreads), m_stopped(false)
    {
        for (int i = 0; i < numThreads; i++)
        {
            m_threadPool.push_back(std::thread(&ThreadPool::TaskWorker, this));
        }
        m_taskErrorCode = AWS_OP_SUCCESS;
    }

    void AddTask(std::function<int()> task) noexcept
    {
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_queue.push(task);
        }
        m_taskReady.notify_one();
    }

    void Shutdown() noexcept
    {
        /* Wake up all threads so that they can complete. */
        m_taskReady.notify_all();

        /* Wait for all threads to complete. */
        for (std::thread &thread : m_threadPool)
        {
            thread.join();
        }

        m_threadPool.clear();
        m_stopped = true;
    }

    void BlockUntilTasksFinish() noexcept
    {
        while (true)
        {
            if (m_queueMutex.try_lock())
            {
                if (m_queue.empty())
                {
                    m_stopped = true;
                    m_queueMutex.unlock();
                    /* Wait for all threads to complete. */
                    m_taskReady.notify_all();
                    for (std::thread &thread : m_threadPool)
                    {
                        thread.join();
                    }
                    break;
                }
                else
                {
                    m_queueMutex.unlock();
                    std::this_thread::yield();
                }
            }
        }
    }

    int GetErrorCode() noexcept { return m_taskErrorCode; }

    ~ThreadPool() noexcept
    {
        if (!m_stopped)
            Shutdown();
    }

  private:
    int m_numThreads;
    std::mutex m_poolMutex;
    std::vector<std::thread> m_threadPool;
    std::mutex m_queueMutex;
    std::queue<std::function<int()>> m_queue;
    std::condition_variable m_taskReady;
    int m_taskErrorCode;
    bool m_stopped;

    void TaskWorker()
    {
        while (true)
        {
            {
                std::unique_lock<std::mutex> lock(m_queueMutex);

                m_taskReady.wait(lock, [this] { return !m_queue.empty() || m_stopped; });
                if (!m_queue.empty())
                {
                    std::function<int()> currentJob = m_queue.front();
                    m_queue.pop();
                    lock.unlock();
                    if (currentJob)
                    {
                        int errorCode = currentJob();
                        if (errorCode)
                            m_taskErrorCode = errorCode;
                    }
                }
                else if (m_stopped)
                {
                    break;
                }
            }
        }
    }
};

AWS_TEST_CASE_FIXTURE(StressTestClient, s_testSetup, s_TestStressClient, s_testTeardown, &s_testContext);
static int s_TestStressClient(struct aws_allocator *allocator, void *ctx)
{
    auto *testContext = static_cast<EventStreamClientTestContext *>(ctx);
    if (!s_isEchoserverSetup(*testContext))
    {
        printf("Environment Variables are not set for the test, skip the test");
        return AWS_OP_SKIP;
    }

    ThreadPool threadPool;
    ConnectionLifecycleHandler lifecycleHandler;
    Aws::Crt::String expectedMessage("Async I0 FTW");
    EchoMessageRequest echoMessageRequest;
    MessageData messageData;
    messageData.SetStringMessage(expectedMessage);

    {
        Awstest::EchoTestRpcClient client(*testContext->clientBootstrap, allocator);
        auto connectedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_SUCCESS);
        auto invokeOperation = [&](void) -> int
        {
            auto echoMessage = client.NewEchoMessage();
            messageData.SetStringMessage(expectedMessage);
            echoMessageRequest.SetMessage(messageData);
            auto requestStatus = echoMessage->Activate(echoMessageRequest, s_onMessageFlush).get();
            auto resultFuture = echoMessage->GetResult();
            /* The response may never arrive depending on how many ongoing requests are made
             * so in case of timeout, assume success. */
            std::future_status status = resultFuture.wait_for(std::chrono::seconds(5));
            if (status != std::future_status::ready)
            {
                return AWS_OP_SUCCESS;
            }
            auto result = echoMessage->GetResult().get();
            ASSERT_TRUE(result);
            auto response = result.GetOperationResponse();
            ASSERT_NOT_NULL(response);
            ASSERT_TRUE(response->GetMessage().value().GetStringMessage().value() == expectedMessage);

            return AWS_OP_SUCCESS;
        };

        for (int i = 0; i < 1000; i++)
            threadPool.AddTask(invokeOperation);

        threadPool.BlockUntilTasksFinish();

        if (threadPool.GetErrorCode() != AWS_OP_SUCCESS)
            return threadPool.GetErrorCode();
    }

    return AWS_OP_SUCCESS;
}
#endif
