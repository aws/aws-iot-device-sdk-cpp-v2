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
        class DefaultConnectionConfig : public ConnectionConfig
        {
          public:
            DefaultConnectionConfig() noexcept;
        };

        class GreengrassCoreIpcClient
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
            SubscribeToIoTCoreOperation NewSubscribeToIoTCore(SubscribeToIoTCoreStreamHandler &streamHandler) noexcept;
            std::unique_ptr<SubscribeToIoTCoreOperation> NewPtrSubscribeToIoTCore(
                std::shared_ptr<SubscribeToIoTCoreStreamHandler> streamHandler) noexcept;

            ResumeComponentOperation NewResumeComponent() noexcept;
            std::unique_ptr<ResumeComponentOperation> NewPtrResumeComponent() noexcept;

            PublishToIoTCoreOperation NewPublishToIoTCore() noexcept;
            std::unique_ptr<PublishToIoTCoreOperation> NewPtrPublishToIoTCore() noexcept;

            SubscribeToConfigurationUpdateOperation NewSubscribeToConfigurationUpdate(
                SubscribeToConfigurationUpdateStreamHandler &streamHandler) noexcept;
            std::unique_ptr<SubscribeToConfigurationUpdateOperation> NewPtrSubscribeToConfigurationUpdate(
                std::shared_ptr<SubscribeToConfigurationUpdateStreamHandler> streamHandler) noexcept;

            DeleteThingShadowOperation NewDeleteThingShadow() noexcept;
            std::unique_ptr<DeleteThingShadowOperation> NewPtrDeleteThingShadow() noexcept;

            DeferComponentUpdateOperation NewDeferComponentUpdate() noexcept;
            std::unique_ptr<DeferComponentUpdateOperation> NewPtrDeferComponentUpdate() noexcept;

            SubscribeToValidateConfigurationUpdatesOperation NewSubscribeToValidateConfigurationUpdates(
                SubscribeToValidateConfigurationUpdatesStreamHandler &streamHandler) noexcept;
            std::unique_ptr<SubscribeToValidateConfigurationUpdatesOperation>
                NewPtrSubscribeToValidateConfigurationUpdates(
                    std::shared_ptr<SubscribeToValidateConfigurationUpdatesStreamHandler> streamHandler) noexcept;

            GetConfigurationOperation NewGetConfiguration() noexcept;
            std::unique_ptr<GetConfigurationOperation> NewPtrGetConfiguration() noexcept;

            SubscribeToTopicOperation NewSubscribeToTopic(SubscribeToTopicStreamHandler &streamHandler) noexcept;
            std::unique_ptr<SubscribeToTopicOperation> NewPtrSubscribeToTopic(
                std::shared_ptr<SubscribeToTopicStreamHandler> streamHandler) noexcept;

            GetComponentDetailsOperation NewGetComponentDetails() noexcept;
            std::unique_ptr<GetComponentDetailsOperation> NewPtrGetComponentDetails() noexcept;

            PublishToTopicOperation NewPublishToTopic() noexcept;
            std::unique_ptr<PublishToTopicOperation> NewPtrPublishToTopic() noexcept;

            ListComponentsOperation NewListComponents() noexcept;
            std::unique_ptr<ListComponentsOperation> NewPtrListComponents() noexcept;

            CreateDebugPasswordOperation NewCreateDebugPassword() noexcept;
            std::unique_ptr<CreateDebugPasswordOperation> NewPtrCreateDebugPassword() noexcept;

            GetThingShadowOperation NewGetThingShadow() noexcept;
            std::unique_ptr<GetThingShadowOperation> NewPtrGetThingShadow() noexcept;

            SendConfigurationValidityReportOperation NewSendConfigurationValidityReport() noexcept;
            std::unique_ptr<SendConfigurationValidityReportOperation> NewPtrSendConfigurationValidityReport() noexcept;

            UpdateThingShadowOperation NewUpdateThingShadow() noexcept;
            std::unique_ptr<UpdateThingShadowOperation> NewPtrUpdateThingShadow() noexcept;

            UpdateConfigurationOperation NewUpdateConfiguration() noexcept;
            std::unique_ptr<UpdateConfigurationOperation> NewPtrUpdateConfiguration() noexcept;

            ValidateAuthorizationTokenOperation NewValidateAuthorizationToken() noexcept;
            std::unique_ptr<ValidateAuthorizationTokenOperation> NewPtrValidateAuthorizationToken() noexcept;

            RestartComponentOperation NewRestartComponent() noexcept;
            std::unique_ptr<RestartComponentOperation> NewPtrRestartComponent() noexcept;

            GetLocalDeploymentStatusOperation NewGetLocalDeploymentStatus() noexcept;
            std::unique_ptr<GetLocalDeploymentStatusOperation> NewPtrGetLocalDeploymentStatus() noexcept;

            GetSecretValueOperation NewGetSecretValue() noexcept;
            std::unique_ptr<GetSecretValueOperation> NewPtrGetSecretValue() noexcept;

            UpdateStateOperation NewUpdateState() noexcept;
            std::unique_ptr<UpdateStateOperation> NewPtrUpdateState() noexcept;

            ListNamedShadowsForThingOperation NewListNamedShadowsForThing() noexcept;
            std::unique_ptr<ListNamedShadowsForThingOperation> NewPtrListNamedShadowsForThing() noexcept;

            SubscribeToComponentUpdatesOperation NewSubscribeToComponentUpdates(
                SubscribeToComponentUpdatesStreamHandler &streamHandler) noexcept;
            std::unique_ptr<SubscribeToComponentUpdatesOperation> NewPtrSubscribeToComponentUpdates(
                std::shared_ptr<SubscribeToComponentUpdatesStreamHandler> streamHandler) noexcept;

            ListLocalDeploymentsOperation NewListLocalDeployments() noexcept;
            std::unique_ptr<ListLocalDeploymentsOperation> NewPtrListLocalDeployments() noexcept;

            StopComponentOperation NewStopComponent() noexcept;
            std::unique_ptr<StopComponentOperation> NewPtrStopComponent() noexcept;

            PauseComponentOperation NewPauseComponent() noexcept;
            std::unique_ptr<PauseComponentOperation> NewPtrPauseComponent() noexcept;

            CreateLocalDeploymentOperation NewCreateLocalDeployment() noexcept;
            std::unique_ptr<CreateLocalDeploymentOperation> NewPtrCreateLocalDeployment() noexcept;

            ~GreengrassCoreIpcClient() noexcept;

          private:
            GreengrassCoreIpcServiceModel m_greengrassCoreIpcServiceModel;
            ClientConnection m_connection;
            Aws::Crt::Io::ClientBootstrap &m_clientBootstrap;
            Aws::Crt::Allocator *m_allocator;
            MessageAmendment m_connectAmendment;
        };
    } // namespace Greengrass
} // namespace Aws
