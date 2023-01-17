#include <aws/crt/Types.h>
#include <aws/crt/io/Bootstrap.h>
#include <awstest/EchoTestRpcClient.h>

namespace Awstest
{
    EchoTestRpcClient::EchoTestRpcClient(
        Aws::Crt::Io::ClientBootstrap &clientBootstrap,
        Aws::Crt::Allocator *allocator) noexcept
        : m_connection(allocator), m_clientBootstrap(clientBootstrap), m_allocator(allocator)
    {
        m_echoTestRpcServiceModel.AssignModelNameToErrorResponse(
            Aws::Crt::String("awstest#ServiceError"), ServiceError::s_allocateFromPayload);
    }

    std::future<RpcError> EchoTestRpcClient::Connect(
        ConnectionLifecycleHandler &lifecycleHandler,
        const ConnectionConfig &connectionConfig) noexcept
    {
        return m_connection.Connect(connectionConfig, &lifecycleHandler, m_clientBootstrap);
    }

    void EchoTestRpcClient::Close() noexcept
    {
        m_connection.Close();
    }

    EchoTestRpcClient::~EchoTestRpcClient() noexcept
    {
        Close();
    }

    std::shared_ptr<GetAllProductsOperation> EchoTestRpcClient::NewGetAllProducts() noexcept
    {
        return Aws::Crt::MakeShared<GetAllProductsOperation>(
            m_allocator, m_connection, m_echoTestRpcServiceModel.m_getAllProductsOperationContext, m_allocator);
    }

    std::shared_ptr<CauseServiceErrorOperation> EchoTestRpcClient::NewCauseServiceError() noexcept
    {
        return Aws::Crt::MakeShared<CauseServiceErrorOperation>(
            m_allocator, m_connection, m_echoTestRpcServiceModel.m_causeServiceErrorOperationContext, m_allocator);
    }

    std::shared_ptr<CauseStreamServiceToErrorOperation> EchoTestRpcClient::NewCauseStreamServiceToError(
        std::shared_ptr<CauseStreamServiceToErrorStreamHandler> streamHandler) noexcept
    {
        return Aws::Crt::MakeShared<CauseStreamServiceToErrorOperation>(
            m_allocator,
            m_connection,
            std::move(streamHandler),
            m_echoTestRpcServiceModel.m_causeStreamServiceToErrorOperationContext,
            m_allocator);
    }

    std::shared_ptr<EchoStreamMessagesOperation> EchoTestRpcClient::NewEchoStreamMessages(
        std::shared_ptr<EchoStreamMessagesStreamHandler> streamHandler) noexcept
    {
        return Aws::Crt::MakeShared<EchoStreamMessagesOperation>(
            m_allocator,
            m_connection,
            std::move(streamHandler),
            m_echoTestRpcServiceModel.m_echoStreamMessagesOperationContext,
            m_allocator);
    }

    std::shared_ptr<EchoMessageOperation> EchoTestRpcClient::NewEchoMessage() noexcept
    {
        return Aws::Crt::MakeShared<EchoMessageOperation>(
            m_allocator, m_connection, m_echoTestRpcServiceModel.m_echoMessageOperationContext, m_allocator);
    }

    std::shared_ptr<GetAllCustomersOperation> EchoTestRpcClient::NewGetAllCustomers() noexcept
    {
        return Aws::Crt::MakeShared<GetAllCustomersOperation>(
            m_allocator, m_connection, m_echoTestRpcServiceModel.m_getAllCustomersOperationContext, m_allocator);
    }

} // namespace Awstest
