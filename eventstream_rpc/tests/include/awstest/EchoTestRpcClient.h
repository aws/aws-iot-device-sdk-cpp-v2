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
    class DefaultConnectionConfig : public ConnectionConfig
    {
      public:
        DefaultConnectionConfig() noexcept;
    };

    class EchoTestRpcClient
    {
      public:
        EchoTestRpcClient(
            Aws::Crt::Io::ClientBootstrap &clientBootstrap,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
        /**
         * Connect the client to the server
         * @param lifecycleHandler An interface that is called upon when lifecycle
         * events relating to the connection occur.
         * @param connectionConfig The configuration parameters used for establishing
         * the connection.
         * @return An `RpcError` that can be used to check whether the connection was
         * established.
         */
        std::future<RpcError> Connect(
            ConnectionLifecycleHandler &lifecycleHandler,
            const ConnectionConfig &connectionConfig = DefaultConnectionConfig()) noexcept;
        bool IsConnected() const noexcept { return m_connection.IsOpen(); }
        void Close() noexcept;
        GetAllProductsOperation NewGetAllProducts() noexcept;
        std::unique_ptr<GetAllProductsOperation> NewPtrGetAllProducts() noexcept;

        CauseServiceErrorOperation NewCauseServiceError() noexcept;
        std::unique_ptr<CauseServiceErrorOperation> NewPtrCauseServiceError() noexcept;

        CauseStreamServiceToErrorOperation NewCauseStreamServiceToError(
            CauseStreamServiceToErrorStreamHandler &streamHandler) noexcept;
        std::unique_ptr<CauseStreamServiceToErrorOperation> NewPtrCauseStreamServiceToError(
            std::shared_ptr<CauseStreamServiceToErrorStreamHandler> streamHandler) noexcept;

        EchoStreamMessagesOperation NewEchoStreamMessages(EchoStreamMessagesStreamHandler &streamHandler) noexcept;
        std::unique_ptr<EchoStreamMessagesOperation> NewPtrEchoStreamMessages(
            std::shared_ptr<EchoStreamMessagesStreamHandler> streamHandler) noexcept;

        EchoMessageOperation NewEchoMessage() noexcept;
        std::unique_ptr<EchoMessageOperation> NewPtrEchoMessage() noexcept;

        GetAllCustomersOperation NewGetAllCustomers() noexcept;
        std::unique_ptr<GetAllCustomersOperation> NewPtrGetAllCustomers() noexcept;

        ~EchoTestRpcClient() noexcept;

      private:
        EchoTestRpcServiceModel m_echoTestRpcServiceModel;
        ClientConnection m_connection;
        Aws::Crt::Io::ClientBootstrap &m_clientBootstrap;
        Aws::Crt::Allocator *m_allocator;
        MessageAmendment m_connectAmendment;
    };
} // namespace Awstest
