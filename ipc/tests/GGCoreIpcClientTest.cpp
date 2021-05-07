/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>

#include <aws/ipc/GGCoreIpcClient.h>

#include <aws/testing/aws_test_harness.h>

#include <iostream>
#include <sstream>

using namespace Aws::Eventstreamrpc;

static int s_PublishToIoTCore(struct aws_allocator *allocator, void *ctx);

class TestLifecycleHandler : public ConnectionLifecycleHandler
{
  public:
    TestLifecycleHandler()
    {
        semaphoreULock = std::unique_lock<std::mutex>(semaphoreLock);
        isConnected = false;
        lastErrorCode = AWS_OP_ERR;
    }

    void OnConnectCallback() override
    {
        std::lock_guard<std::mutex> lockGuard(semaphoreLock);

        isConnected = true;

        semaphore.notify_one();
    }

    void OnDisconnectCallback(int errorCode) override
    {
        std::lock_guard<std::mutex> lockGuard(semaphoreLock);

        lastErrorCode = errorCode;

        semaphore.notify_one();
    }

    void WaitOnCondition(std::function<bool(void)> condition) { semaphore.wait(semaphoreULock, condition); }

  private:
    friend int s_PublishToIoTCore(struct aws_allocator *allocator, void *ctx);
    std::condition_variable semaphore;
    std::mutex semaphoreLock;
    std::unique_lock<std::mutex> semaphoreULock;
    int isConnected;
    int lastErrorCode;
};

static int s_PublishToIoTCore(struct aws_allocator *allocator, void *ctx)
{
    (void)ctx;
    {
        Aws::Crt::ApiHandle apiHandle(allocator);
        Aws::Crt::Io::TlsContextOptions tlsCtxOptions = Aws::Crt::Io::TlsContextOptions::InitDefaultClient();
        Aws::Crt::Io::TlsContext tlsContext(tlsCtxOptions, Aws::Crt::Io::TlsMode::CLIENT, allocator);
        ASSERT_TRUE(tlsContext);

        Aws::Crt::Io::TlsConnectionOptions tlsConnectionOptions = tlsContext.NewConnectionOptions();
        Aws::Crt::Io::SocketOptions socketOptions;
        socketOptions.SetConnectTimeoutMs(1000);

        Aws::Crt::Io::EventLoopGroup eventLoopGroup(0, allocator);
        ASSERT_TRUE(eventLoopGroup);

        Aws::Crt::Io::DefaultHostResolver defaultHostResolver(eventLoopGroup, 8, 30, allocator);
        ASSERT_TRUE(defaultHostResolver);

        Aws::Crt::Io::ClientBootstrap clientBootstrap(eventLoopGroup, defaultHostResolver, allocator);
        ASSERT_TRUE(clientBootstrap);
        clientBootstrap.EnableBlockingShutdown();
        MessageAmendment connectionAmendment;
        auto messageAmender = [&](void) -> MessageAmendment & { return connectionAmendment; };

        ClientConnectionOptions options;
        options.Bootstrap = &clientBootstrap;
        options.SocketOptions = socketOptions;
        options.HostName = Aws::Crt::String("127.0.0.1");
        options.Port = 8033;

        ClientConnection connection(allocator);

        {
            TestLifecycleHandler lifecycleHandler;
            connectionAmendment.AddHeader(EventStreamHeader(
                Aws::Crt::String("client-name"), Aws::Crt::String("accepted.testy_mc_testerson"), allocator));
            ASSERT_TRUE(connection.Connect(options, &lifecycleHandler, messageAmender));
            lifecycleHandler.WaitOnCondition([&]() { return lifecycleHandler.isConnected; });
            /* Publish to Topic */
            //Ipc::PublishToTopicOperation operation;
            /* Close connection gracefully. */
            //connection.NewStream()
            connection.Close();
            lifecycleHandler.WaitOnCondition([&]() { return lifecycleHandler.lastErrorCode == AWS_OP_SUCCESS; });
        }


    }

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(EventStreamConnect, s_PublishToIoTCore)
