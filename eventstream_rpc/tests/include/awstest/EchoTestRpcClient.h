#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

/* This file is generated. */

#include <awstest/EchoTestRpcModel.h>

#include <memory>

using namespace Aws::Eventstreamrpc;

namespace Awstest
{
    class AWS_ECHOTESTRPC_API DefaultConnectionConfig : public ConnectionConfig
    {
      public:
        DefaultConnectionConfig() noexcept;
    };

    class AWS_ECHOTESTRPC_API EchoTestRpcClient
    {
      public:
        EchoTestRpcClient(
            Aws::Crt::Io::ClientBootstrap &clientBootstrap,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
        /**
         * Connect the client to the server
         * @param lifecycleHandler An interface that is called upon when lifecycle events relating to the connection
         * occur.
         * @param connectionConfig The configuration parameters used for establishing the connection.
         * @return An `RpcError` that can be used to check whether the connection was established.
         */
        std::future<RpcError> Connect(
            ConnectionLifecycleHandler &lifecycleHandler,
            const ConnectionConfig &connectionConfig = DefaultConnectionConfig()) noexcept;
        bool IsConnected() const noexcept { return m_connection.IsOpen(); }
        void Close() noexcept;
        void WithLaunchMode(std::launch mode) noexcept;

        /**
         * Fetches all products, indexed by SKU
         */
        std::shared_ptr<GetAllProductsOperation> NewGetAllProducts() noexcept;

        /**
         * Throws a ServiceError instead of returning a response.
         */
        std::shared_ptr<CauseServiceErrorOperation> NewCauseServiceError() noexcept;

        /**
         * Responds to initial request normally then throws a ServiceError on stream response
         */
        std::shared_ptr<CauseStreamServiceToErrorOperation> NewCauseStreamServiceToError(
            std::shared_ptr<CauseStreamServiceToErrorStreamHandler> streamHandler) noexcept;

        /**
         * Initial request and response are empty, but echos streaming messages sent by client
         */
        std::shared_ptr<EchoStreamMessagesOperation> NewEchoStreamMessages(
            std::shared_ptr<EchoStreamMessagesStreamHandler> streamHandler) noexcept;

        /**
         * Returns the same data sent in the request to the response
         */
        std::shared_ptr<EchoMessageOperation> NewEchoMessage() noexcept;

        /**
         * Fetches all customers
         */
        std::shared_ptr<GetAllCustomersOperation> NewGetAllCustomers() noexcept;

        ~EchoTestRpcClient() noexcept;

      private:
        EchoTestRpcServiceModel m_echoTestRpcServiceModel;
        ClientConnection m_connection;
        Aws::Crt::Io::ClientBootstrap &m_clientBootstrap;
        Aws::Crt::Allocator *m_allocator;
        MessageAmendment m_connectAmendment;
        std::launch m_asyncLaunchMode;
    };
} // namespace Awstest
