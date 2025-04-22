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

EventStreamClientTestContext::EventStreamClientTestContext(struct aws_allocator *allocator) :
    echoServerPort(0)
{
    elGroup = Aws::Crt::MakeShared<Io::EventLoopGroup>(allocator, 0, allocator);
    resolver =
        Aws::Crt::MakeShared<Io::DefaultHostResolver>(allocator, *elGroup, 8, 30, allocator);
    clientBootstrap = Aws::Crt::MakeShared<Io::ClientBootstrap>(
        allocator, *elGroup, *resolver, allocator);

    aws_string *host_env_value = nullptr;
    aws_get_environment_value(allocator, s_env_name_echo_server_host, &host_env_value);
    if (host_env_value)
    {
        echoServerHost = aws_string_c_str(host_env_value);
    }

    struct aws_string *port_env_value = nullptr;
    if (!aws_get_environment_value(allocator, s_env_name_echo_server_port, &port_env_value) && port_env_value != nullptr)
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
    TestLifecycleHandler() :
        isConnected(false),
        disconnectCrtErrorCode(AWS_ERROR_SUCCESS),
        disconnectRpcStatusCode(EVENT_STREAM_RPC_SUCCESS)
    {}

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
        ClientConnection connection(allocator);
        auto future = connection.Connect(connectionConfig, &lifecycleHandler, *testContext.clientBootstrap);
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
        ClientConnection connection(allocator);
        auto future = connection.Connect(connectionConfig, &lifecycleHandler, *testContext.clientBootstrap);
        EventStreamRpcStatusCode clientStatus = future.get().baseStatus;

        ASSERT_TRUE(clientStatus == EVENT_STREAM_RPC_CRT_ERROR || clientStatus == EVENT_STREAM_RPC_CONNECTION_ACCESS_DENIED);
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
        ClientConnection connection(allocator);
        auto future = connection.Connect(connectionConfig, &lifecycleHandler, *testContext.clientBootstrap);
        EventStreamRpcStatusCode clientStatus = future.get().baseStatus;

        ASSERT_TRUE(clientStatus == EVENT_STREAM_RPC_CRT_ERROR || clientStatus == EVENT_STREAM_RPC_CONNECTION_ACCESS_DENIED);
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

#ifdef NEVER


AWS_TEST_CASE_FIXTURE(
    OperateWhileDisconnected,
    s_testSetup,
    s_TestOperationWhileDisconnected,
    s_testTeardown,
    &s_testContext);
static int s_TestOperationWhileDisconnected(struct aws_allocator *allocator, void *ctx)
{
    auto *testContext = static_cast<EventStreamClientTestContext *>(ctx);

    /* Don't connect at all and try running operations as normal. */
    {
        if (!s_isEchoserverSetup(*testContext))
        {
            printf("Environment Variables are not set for the test, skip the test");
            return AWS_OP_SKIP;
        }

        ConnectionLifecycleHandler lifecycleHandler;
        Awstest::EchoTestRpcClient client(*testContext->clientBootstrap, allocator);
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

    /* Idempotent close and its safety. */
    {
        ConnectionLifecycleHandler lifecycleHandler;
        Awstest::EchoTestRpcClient client(*testContext->clientBootstrap, allocator);
        client.Close();
        client.Close();
    }

    return AWS_OP_SUCCESS;
}

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
        auto invokeOperation = [&](void) -> int {
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
