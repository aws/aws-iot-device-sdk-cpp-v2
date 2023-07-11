/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

/* This file is generated. */

#include <aws/crt/Types.h>
#include <aws/crt/io/Bootstrap.h>
#include <aws/greengrass/GreengrassCoreIpcClient.h>

namespace Aws
{
    namespace Greengrass
    {
        GreengrassCoreIpcClient::GreengrassCoreIpcClient(
            Aws::Crt::Io::ClientBootstrap &clientBootstrap,
            Aws::Crt::Allocator *allocator) noexcept
            : m_connection(allocator), m_clientBootstrap(clientBootstrap), m_allocator(allocator),
              m_asyncLaunchMode(std::launch::deferred)
        {
            m_greengrassCoreIpcServiceModel.AssignModelNameToErrorResponse(
                Aws::Crt::String("aws.greengrass#InvalidArgumentsError"), InvalidArgumentsError::s_allocateFromPayload);
            m_greengrassCoreIpcServiceModel.AssignModelNameToErrorResponse(
                Aws::Crt::String("aws.greengrass#ServiceError"), ServiceError::s_allocateFromPayload);
            m_greengrassCoreIpcServiceModel.AssignModelNameToErrorResponse(
                Aws::Crt::String("aws.greengrass#UnauthorizedError"), UnauthorizedError::s_allocateFromPayload);
            m_greengrassCoreIpcServiceModel.AssignModelNameToErrorResponse(
                Aws::Crt::String("aws.greengrass#InvalidTokenError"), InvalidTokenError::s_allocateFromPayload);
            m_greengrassCoreIpcServiceModel.AssignModelNameToErrorResponse(
                Aws::Crt::String("aws.greengrass#ConflictError"), ConflictError::s_allocateFromPayload);
            m_greengrassCoreIpcServiceModel.AssignModelNameToErrorResponse(
                Aws::Crt::String("aws.greengrass#ResourceNotFoundError"), ResourceNotFoundError::s_allocateFromPayload);
            m_greengrassCoreIpcServiceModel.AssignModelNameToErrorResponse(
                Aws::Crt::String("aws.greengrass#FailedUpdateConditionCheckError"),
                FailedUpdateConditionCheckError::s_allocateFromPayload);
            m_greengrassCoreIpcServiceModel.AssignModelNameToErrorResponse(
                Aws::Crt::String("aws.greengrass#ComponentNotFoundError"),
                ComponentNotFoundError::s_allocateFromPayload);
            m_greengrassCoreIpcServiceModel.AssignModelNameToErrorResponse(
                Aws::Crt::String("aws.greengrass#InvalidCredentialError"),
                InvalidCredentialError::s_allocateFromPayload);
            m_greengrassCoreIpcServiceModel.AssignModelNameToErrorResponse(
                Aws::Crt::String("aws.greengrass#InvalidArtifactsDirectoryPathError"),
                InvalidArtifactsDirectoryPathError::s_allocateFromPayload);
            m_greengrassCoreIpcServiceModel.AssignModelNameToErrorResponse(
                Aws::Crt::String("aws.greengrass#InvalidRecipeDirectoryPathError"),
                InvalidRecipeDirectoryPathError::s_allocateFromPayload);
            m_greengrassCoreIpcServiceModel.AssignModelNameToErrorResponse(
                Aws::Crt::String("aws.greengrass#InvalidClientDeviceAuthTokenError"),
                InvalidClientDeviceAuthTokenError::s_allocateFromPayload);
        }

        std::future<RpcError> GreengrassCoreIpcClient::Connect(
            ConnectionLifecycleHandler &lifecycleHandler,
            const ConnectionConfig &connectionConfig) noexcept
        {
            return m_connection.Connect(connectionConfig, &lifecycleHandler, m_clientBootstrap);
        }

        void GreengrassCoreIpcClient::Close() noexcept { m_connection.Close(); }

        void GreengrassCoreIpcClient::WithLaunchMode(std::launch mode) noexcept { m_asyncLaunchMode = mode; }

        GreengrassCoreIpcClient::~GreengrassCoreIpcClient() noexcept { Close(); }

        std::shared_ptr<SubscribeToIoTCoreOperation> GreengrassCoreIpcClient::NewSubscribeToIoTCore(
            std::shared_ptr<SubscribeToIoTCoreStreamHandler> streamHandler) noexcept
        {
            return Aws::Crt::MakeShared<SubscribeToIoTCoreOperation>(
                m_allocator,
                m_connection,
                std::move(streamHandler),
                m_greengrassCoreIpcServiceModel.m_subscribeToIoTCoreOperationContext,
                m_allocator);
        }

        std::shared_ptr<ResumeComponentOperation> GreengrassCoreIpcClient::NewResumeComponent() noexcept
        {
            auto operation = Aws::Crt::MakeShared<ResumeComponentOperation>(
                m_allocator,
                m_connection,
                m_greengrassCoreIpcServiceModel.m_resumeComponentOperationContext,
                m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

        std::shared_ptr<PublishToIoTCoreOperation> GreengrassCoreIpcClient::NewPublishToIoTCore() noexcept
        {
            auto operation = Aws::Crt::MakeShared<PublishToIoTCoreOperation>(
                m_allocator,
                m_connection,
                m_greengrassCoreIpcServiceModel.m_publishToIoTCoreOperationContext,
                m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

        std::shared_ptr<SubscribeToConfigurationUpdateOperation> GreengrassCoreIpcClient::
            NewSubscribeToConfigurationUpdate(
                std::shared_ptr<SubscribeToConfigurationUpdateStreamHandler> streamHandler) noexcept
        {
            return Aws::Crt::MakeShared<SubscribeToConfigurationUpdateOperation>(
                m_allocator,
                m_connection,
                std::move(streamHandler),
                m_greengrassCoreIpcServiceModel.m_subscribeToConfigurationUpdateOperationContext,
                m_allocator);
        }

        std::shared_ptr<DeleteThingShadowOperation> GreengrassCoreIpcClient::NewDeleteThingShadow() noexcept
        {
            auto operation = Aws::Crt::MakeShared<DeleteThingShadowOperation>(
                m_allocator,
                m_connection,
                m_greengrassCoreIpcServiceModel.m_deleteThingShadowOperationContext,
                m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

        std::shared_ptr<PutComponentMetricOperation> GreengrassCoreIpcClient::NewPutComponentMetric() noexcept
        {
            auto operation = Aws::Crt::MakeShared<PutComponentMetricOperation>(
                m_allocator,
                m_connection,
                m_greengrassCoreIpcServiceModel.m_putComponentMetricOperationContext,
                m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

        std::shared_ptr<DeferComponentUpdateOperation> GreengrassCoreIpcClient::NewDeferComponentUpdate() noexcept
        {
            auto operation = Aws::Crt::MakeShared<DeferComponentUpdateOperation>(
                m_allocator,
                m_connection,
                m_greengrassCoreIpcServiceModel.m_deferComponentUpdateOperationContext,
                m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

        std::shared_ptr<SubscribeToValidateConfigurationUpdatesOperation> GreengrassCoreIpcClient::
            NewSubscribeToValidateConfigurationUpdates(
                std::shared_ptr<SubscribeToValidateConfigurationUpdatesStreamHandler> streamHandler) noexcept
        {
            return Aws::Crt::MakeShared<SubscribeToValidateConfigurationUpdatesOperation>(
                m_allocator,
                m_connection,
                std::move(streamHandler),
                m_greengrassCoreIpcServiceModel.m_subscribeToValidateConfigurationUpdatesOperationContext,
                m_allocator);
        }

        std::shared_ptr<GetConfigurationOperation> GreengrassCoreIpcClient::NewGetConfiguration() noexcept
        {
            auto operation = Aws::Crt::MakeShared<GetConfigurationOperation>(
                m_allocator,
                m_connection,
                m_greengrassCoreIpcServiceModel.m_getConfigurationOperationContext,
                m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

        std::shared_ptr<SubscribeToTopicOperation> GreengrassCoreIpcClient::NewSubscribeToTopic(
            std::shared_ptr<SubscribeToTopicStreamHandler> streamHandler) noexcept
        {
            return Aws::Crt::MakeShared<SubscribeToTopicOperation>(
                m_allocator,
                m_connection,
                std::move(streamHandler),
                m_greengrassCoreIpcServiceModel.m_subscribeToTopicOperationContext,
                m_allocator);
        }

        std::shared_ptr<GetComponentDetailsOperation> GreengrassCoreIpcClient::NewGetComponentDetails() noexcept
        {
            auto operation = Aws::Crt::MakeShared<GetComponentDetailsOperation>(
                m_allocator,
                m_connection,
                m_greengrassCoreIpcServiceModel.m_getComponentDetailsOperationContext,
                m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

        std::shared_ptr<GetClientDeviceAuthTokenOperation> GreengrassCoreIpcClient::
            NewGetClientDeviceAuthToken() noexcept
        {
            auto operation = Aws::Crt::MakeShared<GetClientDeviceAuthTokenOperation>(
                m_allocator,
                m_connection,
                m_greengrassCoreIpcServiceModel.m_getClientDeviceAuthTokenOperationContext,
                m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

        std::shared_ptr<PublishToTopicOperation> GreengrassCoreIpcClient::NewPublishToTopic() noexcept
        {
            auto operation = Aws::Crt::MakeShared<PublishToTopicOperation>(
                m_allocator,
                m_connection,
                m_greengrassCoreIpcServiceModel.m_publishToTopicOperationContext,
                m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

        std::shared_ptr<SubscribeToCertificateUpdatesOperation> GreengrassCoreIpcClient::
            NewSubscribeToCertificateUpdates(
                std::shared_ptr<SubscribeToCertificateUpdatesStreamHandler> streamHandler) noexcept
        {
            return Aws::Crt::MakeShared<SubscribeToCertificateUpdatesOperation>(
                m_allocator,
                m_connection,
                std::move(streamHandler),
                m_greengrassCoreIpcServiceModel.m_subscribeToCertificateUpdatesOperationContext,
                m_allocator);
        }

        std::shared_ptr<VerifyClientDeviceIdentityOperation> GreengrassCoreIpcClient::
            NewVerifyClientDeviceIdentity() noexcept
        {
            auto operation = Aws::Crt::MakeShared<VerifyClientDeviceIdentityOperation>(
                m_allocator,
                m_connection,
                m_greengrassCoreIpcServiceModel.m_verifyClientDeviceIdentityOperationContext,
                m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

        std::shared_ptr<AuthorizeClientDeviceActionOperation> GreengrassCoreIpcClient::
            NewAuthorizeClientDeviceAction() noexcept
        {
            auto operation = Aws::Crt::MakeShared<AuthorizeClientDeviceActionOperation>(
                m_allocator,
                m_connection,
                m_greengrassCoreIpcServiceModel.m_authorizeClientDeviceActionOperationContext,
                m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

        std::shared_ptr<ListComponentsOperation> GreengrassCoreIpcClient::NewListComponents() noexcept
        {
            auto operation = Aws::Crt::MakeShared<ListComponentsOperation>(
                m_allocator,
                m_connection,
                m_greengrassCoreIpcServiceModel.m_listComponentsOperationContext,
                m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

        std::shared_ptr<CreateDebugPasswordOperation> GreengrassCoreIpcClient::NewCreateDebugPassword() noexcept
        {
            auto operation = Aws::Crt::MakeShared<CreateDebugPasswordOperation>(
                m_allocator,
                m_connection,
                m_greengrassCoreIpcServiceModel.m_createDebugPasswordOperationContext,
                m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

        std::shared_ptr<GetThingShadowOperation> GreengrassCoreIpcClient::NewGetThingShadow() noexcept
        {
            auto operation = Aws::Crt::MakeShared<GetThingShadowOperation>(
                m_allocator,
                m_connection,
                m_greengrassCoreIpcServiceModel.m_getThingShadowOperationContext,
                m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

        std::shared_ptr<SendConfigurationValidityReportOperation> GreengrassCoreIpcClient::
            NewSendConfigurationValidityReport() noexcept
        {
            auto operation = Aws::Crt::MakeShared<SendConfigurationValidityReportOperation>(
                m_allocator,
                m_connection,
                m_greengrassCoreIpcServiceModel.m_sendConfigurationValidityReportOperationContext,
                m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

        std::shared_ptr<UpdateThingShadowOperation> GreengrassCoreIpcClient::NewUpdateThingShadow() noexcept
        {
            auto operation = Aws::Crt::MakeShared<UpdateThingShadowOperation>(
                m_allocator,
                m_connection,
                m_greengrassCoreIpcServiceModel.m_updateThingShadowOperationContext,
                m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

        std::shared_ptr<UpdateConfigurationOperation> GreengrassCoreIpcClient::NewUpdateConfiguration() noexcept
        {
            auto operation = Aws::Crt::MakeShared<UpdateConfigurationOperation>(
                m_allocator,
                m_connection,
                m_greengrassCoreIpcServiceModel.m_updateConfigurationOperationContext,
                m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

        std::shared_ptr<ValidateAuthorizationTokenOperation> GreengrassCoreIpcClient::
            NewValidateAuthorizationToken() noexcept
        {
            auto operation = Aws::Crt::MakeShared<ValidateAuthorizationTokenOperation>(
                m_allocator,
                m_connection,
                m_greengrassCoreIpcServiceModel.m_validateAuthorizationTokenOperationContext,
                m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

        std::shared_ptr<RestartComponentOperation> GreengrassCoreIpcClient::NewRestartComponent() noexcept
        {
            auto operation = Aws::Crt::MakeShared<RestartComponentOperation>(
                m_allocator,
                m_connection,
                m_greengrassCoreIpcServiceModel.m_restartComponentOperationContext,
                m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

        std::shared_ptr<GetLocalDeploymentStatusOperation> GreengrassCoreIpcClient::
            NewGetLocalDeploymentStatus() noexcept
        {
            auto operation = Aws::Crt::MakeShared<GetLocalDeploymentStatusOperation>(
                m_allocator,
                m_connection,
                m_greengrassCoreIpcServiceModel.m_getLocalDeploymentStatusOperationContext,
                m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

        std::shared_ptr<GetSecretValueOperation> GreengrassCoreIpcClient::NewGetSecretValue() noexcept
        {
            auto operation = Aws::Crt::MakeShared<GetSecretValueOperation>(
                m_allocator,
                m_connection,
                m_greengrassCoreIpcServiceModel.m_getSecretValueOperationContext,
                m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

        std::shared_ptr<UpdateStateOperation> GreengrassCoreIpcClient::NewUpdateState() noexcept
        {
            auto operation = Aws::Crt::MakeShared<UpdateStateOperation>(
                m_allocator, m_connection, m_greengrassCoreIpcServiceModel.m_updateStateOperationContext, m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

        std::shared_ptr<CancelLocalDeploymentOperation> GreengrassCoreIpcClient::NewCancelLocalDeployment() noexcept
        {
            auto operation = Aws::Crt::MakeShared<CancelLocalDeploymentOperation>(
                m_allocator,
                m_connection,
                m_greengrassCoreIpcServiceModel.m_cancelLocalDeploymentOperationContext,
                m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

        std::shared_ptr<ListNamedShadowsForThingOperation> GreengrassCoreIpcClient::
            NewListNamedShadowsForThing() noexcept
        {
            auto operation = Aws::Crt::MakeShared<ListNamedShadowsForThingOperation>(
                m_allocator,
                m_connection,
                m_greengrassCoreIpcServiceModel.m_listNamedShadowsForThingOperationContext,
                m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

        std::shared_ptr<SubscribeToComponentUpdatesOperation> GreengrassCoreIpcClient::NewSubscribeToComponentUpdates(
            std::shared_ptr<SubscribeToComponentUpdatesStreamHandler> streamHandler) noexcept
        {
            return Aws::Crt::MakeShared<SubscribeToComponentUpdatesOperation>(
                m_allocator,
                m_connection,
                std::move(streamHandler),
                m_greengrassCoreIpcServiceModel.m_subscribeToComponentUpdatesOperationContext,
                m_allocator);
        }

        std::shared_ptr<ListLocalDeploymentsOperation> GreengrassCoreIpcClient::NewListLocalDeployments() noexcept
        {
            auto operation = Aws::Crt::MakeShared<ListLocalDeploymentsOperation>(
                m_allocator,
                m_connection,
                m_greengrassCoreIpcServiceModel.m_listLocalDeploymentsOperationContext,
                m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

        std::shared_ptr<StopComponentOperation> GreengrassCoreIpcClient::NewStopComponent() noexcept
        {
            auto operation = Aws::Crt::MakeShared<StopComponentOperation>(
                m_allocator,
                m_connection,
                m_greengrassCoreIpcServiceModel.m_stopComponentOperationContext,
                m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

        std::shared_ptr<PauseComponentOperation> GreengrassCoreIpcClient::NewPauseComponent() noexcept
        {
            auto operation = Aws::Crt::MakeShared<PauseComponentOperation>(
                m_allocator,
                m_connection,
                m_greengrassCoreIpcServiceModel.m_pauseComponentOperationContext,
                m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

        std::shared_ptr<CreateLocalDeploymentOperation> GreengrassCoreIpcClient::NewCreateLocalDeployment() noexcept
        {
            auto operation = Aws::Crt::MakeShared<CreateLocalDeploymentOperation>(
                m_allocator,
                m_connection,
                m_greengrassCoreIpcServiceModel.m_createLocalDeploymentOperationContext,
                m_allocator);
            operation->WithLaunchMode(m_asyncLaunchMode);
            return operation;
        }

    } // namespace Greengrass
} // namespace Aws
