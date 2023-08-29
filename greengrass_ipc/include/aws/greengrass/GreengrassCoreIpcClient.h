#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

/* This file is generated. */

#include <aws/greengrass/GreengrassCoreIpcModel.h>

#include <memory>

using namespace Aws::Eventstreamrpc;

namespace Aws
{
    namespace Greengrass
    {
        class AWS_GREENGRASSCOREIPC_API DefaultConnectionConfig : public ConnectionConfig
        {
          public:
            DefaultConnectionConfig() noexcept;
        };

        class AWS_GREENGRASSCOREIPC_API GreengrassCoreIpcClient
        {
          public:
            GreengrassCoreIpcClient(
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
            void WithLaunchMode(std::launch mode) noexcept;

            std::shared_ptr<SubscribeToIoTCoreOperation> NewSubscribeToIoTCore(
                std::shared_ptr<SubscribeToIoTCoreStreamHandler> streamHandler) noexcept;

            std::shared_ptr<ResumeComponentOperation> NewResumeComponent() noexcept;

            std::shared_ptr<PublishToIoTCoreOperation> NewPublishToIoTCore() noexcept;

            std::shared_ptr<SubscribeToConfigurationUpdateOperation> NewSubscribeToConfigurationUpdate(
                std::shared_ptr<SubscribeToConfigurationUpdateStreamHandler> streamHandler) noexcept;

            std::shared_ptr<DeleteThingShadowOperation> NewDeleteThingShadow() noexcept;

            std::shared_ptr<PutComponentMetricOperation> NewPutComponentMetric() noexcept;

            std::shared_ptr<DeferComponentUpdateOperation> NewDeferComponentUpdate() noexcept;

            std::shared_ptr<SubscribeToValidateConfigurationUpdatesOperation>
                NewSubscribeToValidateConfigurationUpdates(
                    std::shared_ptr<SubscribeToValidateConfigurationUpdatesStreamHandler> streamHandler) noexcept;

            std::shared_ptr<GetConfigurationOperation> NewGetConfiguration() noexcept;

            std::shared_ptr<SubscribeToTopicOperation> NewSubscribeToTopic(
                std::shared_ptr<SubscribeToTopicStreamHandler> streamHandler) noexcept;

            std::shared_ptr<GetComponentDetailsOperation> NewGetComponentDetails() noexcept;

            std::shared_ptr<GetClientDeviceAuthTokenOperation> NewGetClientDeviceAuthToken() noexcept;

            std::shared_ptr<PublishToTopicOperation> NewPublishToTopic() noexcept;

            std::shared_ptr<SubscribeToCertificateUpdatesOperation> NewSubscribeToCertificateUpdates(
                std::shared_ptr<SubscribeToCertificateUpdatesStreamHandler> streamHandler) noexcept;

            std::shared_ptr<VerifyClientDeviceIdentityOperation> NewVerifyClientDeviceIdentity() noexcept;

            std::shared_ptr<AuthorizeClientDeviceActionOperation> NewAuthorizeClientDeviceAction() noexcept;

            std::shared_ptr<ListComponentsOperation> NewListComponents() noexcept;

            std::shared_ptr<CreateDebugPasswordOperation> NewCreateDebugPassword() noexcept;

            std::shared_ptr<GetThingShadowOperation> NewGetThingShadow() noexcept;

            std::shared_ptr<SendConfigurationValidityReportOperation> NewSendConfigurationValidityReport() noexcept;

            std::shared_ptr<UpdateThingShadowOperation> NewUpdateThingShadow() noexcept;

            std::shared_ptr<UpdateConfigurationOperation> NewUpdateConfiguration() noexcept;

            std::shared_ptr<ValidateAuthorizationTokenOperation> NewValidateAuthorizationToken() noexcept;

            std::shared_ptr<RestartComponentOperation> NewRestartComponent() noexcept;

            std::shared_ptr<GetLocalDeploymentStatusOperation> NewGetLocalDeploymentStatus() noexcept;

            std::shared_ptr<GetSecretValueOperation> NewGetSecretValue() noexcept;

            std::shared_ptr<UpdateStateOperation> NewUpdateState() noexcept;

            std::shared_ptr<CancelLocalDeploymentOperation> NewCancelLocalDeployment() noexcept;

            std::shared_ptr<ListNamedShadowsForThingOperation> NewListNamedShadowsForThing() noexcept;

            std::shared_ptr<SubscribeToComponentUpdatesOperation> NewSubscribeToComponentUpdates(
                std::shared_ptr<SubscribeToComponentUpdatesStreamHandler> streamHandler) noexcept;

            std::shared_ptr<ListLocalDeploymentsOperation> NewListLocalDeployments() noexcept;

            std::shared_ptr<StopComponentOperation> NewStopComponent() noexcept;

            std::shared_ptr<PauseComponentOperation> NewPauseComponent() noexcept;

            std::shared_ptr<CreateLocalDeploymentOperation> NewCreateLocalDeployment() noexcept;

            ~GreengrassCoreIpcClient() noexcept;

          private:
            GreengrassCoreIpcServiceModel m_greengrassCoreIpcServiceModel;
            ClientConnection m_connection;
            Aws::Crt::Io::ClientBootstrap &m_clientBootstrap;
            Aws::Crt::Allocator *m_allocator;
            MessageAmendment m_connectAmendment;
            std::launch m_asyncLaunchMode;
        };
    } // namespace Greengrass
} // namespace Aws
