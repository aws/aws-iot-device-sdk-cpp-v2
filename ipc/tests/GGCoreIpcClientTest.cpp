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

static int s_PublishToIoTCore(struct aws_allocator *allocator, void *ctx)
{
    (void)ctx;
    {
        Aws::Crt::ApiHandle apiHandle(allocator);
        Aws::Crt::Io::TlsContextOptions tlsCtxOptions = Aws::Crt::Io::TlsContextOptions::InitDefaultClient();
        Aws::Crt::Io::TlsContext tlsContext(tlsCtxOptions, Aws::Crt::Io::TlsMode::CLIENT, allocator);
        ASSERT_TRUE(tlsContext);

        Aws::Crt::Io::TlsConnectionOptions tlsConnectionOptions = tlsContext.NewConnectionOptions();

        Aws::Crt::Io::EventLoopGroup eventLoopGroup(0, allocator);
        ASSERT_TRUE(eventLoopGroup);

        Aws::Crt::Io::DefaultHostResolver defaultHostResolver(eventLoopGroup, 8, 30, allocator);
        ASSERT_TRUE(defaultHostResolver);

        Aws::Crt::Io::ClientBootstrap clientBootstrap(eventLoopGroup, defaultHostResolver, allocator);
        ASSERT_TRUE(clientBootstrap);
        clientBootstrap.EnableBlockingShutdown();
        MessageAmendment connectionAmendment;
        auto messageAmender = [&](void) -> MessageAmendment & { return connectionAmendment; };

        ConnectionLifecycleHandler lifecycleHandler;
        Ipc::GreengrassIpcClient client(lifecycleHandler, clientBootstrap);
        auto connectedStatus = client.Connect(lifecycleHandler);
        connectedStatus.get();

        /* Subscribe to Topic */
        {
            Ipc::SubscribeToTopicStreamHandler streamHandler;
            Ipc::SubscribeToTopicOperation operation = client.NewSubscribeToTopic(&streamHandler);
            Ipc::SubscribeToTopicRequest request(Aws::Crt::String("topic"), allocator);
            auto activate = operation.Activate(request, nullptr);
            activate.wait();
            auto response = operation.GetResponse();
            response.wait();
        }

        /* Publish to Topic */
        {
            Ipc::PublishToTopicOperation operation = client.NewPublishToTopic();
            Ipc::PublishMessage publishMessage(allocator);
            Ipc::PublishToTopicRequest request(
                Aws::Crt::String("example"), Ipc::PublishMessage(publishMessage), allocator);
            auto activate = operation.Activate(request, nullptr);
            activate.wait();
            auto response = operation.GetResponse();
            response.get();
        }

        client.Close();
    }

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(EventStreamConnect, s_PublishToIoTCore)
