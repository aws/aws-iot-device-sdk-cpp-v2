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
             * Subscribe to a topic in AWS IoT message broker.
             */
            std::shared_ptr<SubscribeToIoTCoreOperation> NewSubscribeToIoTCore(
                std::shared_ptr<SubscribeToIoTCoreStreamHandler> streamHandler) noexcept;

            /**
             * Resume a paused component
             */
            std::shared_ptr<ResumeComponentOperation> NewResumeComponent() noexcept;

            /**
             * Publish an MQTT message to AWS IoT message broker
             */
            std::shared_ptr<PublishToIoTCoreOperation> NewPublishToIoTCore() noexcept;

            /**
             * Subscribes to be notified when GGC updates the configuration for a given componentName and keyName.
             */
            std::shared_ptr<SubscribeToConfigurationUpdateOperation> NewSubscribeToConfigurationUpdate(
                std::shared_ptr<SubscribeToConfigurationUpdateStreamHandler> streamHandler) noexcept;

            /**
             * Deletes a device shadow document stored in the local shadow service
             */
            std::shared_ptr<DeleteThingShadowOperation> NewDeleteThingShadow() noexcept;

            /**
             * Send component metrics
             * NOTE Only usable by AWS components
             */
            std::shared_ptr<PutComponentMetricOperation> NewPutComponentMetric() noexcept;

            /**
             * Defer the update of components by a given amount of time and check again after that.
             */
            std::shared_ptr<DeferComponentUpdateOperation> NewDeferComponentUpdate() noexcept;

            /**
             * Subscribes to be notified when GGC is about to update configuration for this component
             * GGC will wait for a timeout period before it proceeds with the update.
             * If the new configuration is not valid this component can use the SendConfigurationValidityReport
             * operation to indicate that
             */
            std::shared_ptr<SubscribeToValidateConfigurationUpdatesOperation>
                NewSubscribeToValidateConfigurationUpdates(
                    std::shared_ptr<SubscribeToValidateConfigurationUpdatesStreamHandler> streamHandler) noexcept;

            /**
             * Get value of a given key from the configuration
             */
            std::shared_ptr<GetConfigurationOperation> NewGetConfiguration() noexcept;

            /**
             * Creates a subscription for a custom topic
             */
            std::shared_ptr<SubscribeToTopicOperation> NewSubscribeToTopic(
                std::shared_ptr<SubscribeToTopicStreamHandler> streamHandler) noexcept;

            /**
             * Gets the status and version of the component with the given component name
             */
            std::shared_ptr<GetComponentDetailsOperation> NewGetComponentDetails() noexcept;

            /**
             * Get session token for a client device
             */
            std::shared_ptr<GetClientDeviceAuthTokenOperation> NewGetClientDeviceAuthToken() noexcept;

            /**
             * Publish to a custom topic.
             */
            std::shared_ptr<PublishToTopicOperation> NewPublishToTopic() noexcept;

            /**
             * Create a subscription for new certificates
             */
            std::shared_ptr<SubscribeToCertificateUpdatesOperation> NewSubscribeToCertificateUpdates(
                std::shared_ptr<SubscribeToCertificateUpdatesStreamHandler> streamHandler) noexcept;

            /**
             * Verify client device credentials
             */
            std::shared_ptr<VerifyClientDeviceIdentityOperation> NewVerifyClientDeviceIdentity() noexcept;

            /**
             * Send a request to authorize action on some resource
             */
            std::shared_ptr<AuthorizeClientDeviceActionOperation> NewAuthorizeClientDeviceAction() noexcept;

            /**
             * Request for a list of components
             */
            std::shared_ptr<ListComponentsOperation> NewListComponents() noexcept;

            /**
             * Generate a password for the LocalDebugConsole component
             */
            std::shared_ptr<CreateDebugPasswordOperation> NewCreateDebugPassword() noexcept;

            /**
             * Retrieves a device shadow document stored by the local shadow service
             */
            std::shared_ptr<GetThingShadowOperation> NewGetThingShadow() noexcept;

            /**
             * This operation should be used in response to event received as part of
             * SubscribeToValidateConfigurationUpdates subscription. It is not necessary to send the report if the
             * configuration is valid (GGC will wait for timeout period and proceed). Sending the report with invalid
             * config status will prevent GGC from applying the updates
             */
            std::shared_ptr<SendConfigurationValidityReportOperation> NewSendConfigurationValidityReport() noexcept;

            /**
             * Updates a device shadow document stored in the local shadow service
             * The update is an upsert operation, with optimistic locking support
             */
            std::shared_ptr<UpdateThingShadowOperation> NewUpdateThingShadow() noexcept;

            /**
             * Update this component's configuration by replacing the value of given keyName with the newValue.
             * If an oldValue is specified then update will only take effect id the current value matches the given
             * oldValue
             */
            std::shared_ptr<UpdateConfigurationOperation> NewUpdateConfiguration() noexcept;

            /**
             * Validate authorization token
             * NOTE This API can be used only by stream manager, customer component calling this API will receive
             * UnauthorizedError
             */
            std::shared_ptr<ValidateAuthorizationTokenOperation> NewValidateAuthorizationToken() noexcept;

            /**
             * Restarts a component with the given name
             */
            std::shared_ptr<RestartComponentOperation> NewRestartComponent() noexcept;

            /**
             * Get status of a local deployment with the given deploymentId
             */
            std::shared_ptr<GetLocalDeploymentStatusOperation> NewGetLocalDeploymentStatus() noexcept;

            /**
             * Retrieves a secret stored in AWS secrets manager
             */
            std::shared_ptr<GetSecretValueOperation> NewGetSecretValue() noexcept;

            /**
             * Update status of this component
             */
            std::shared_ptr<UpdateStateOperation> NewUpdateState() noexcept;

            /**
             * Cancel a local deployment on the device.
             */
            std::shared_ptr<CancelLocalDeploymentOperation> NewCancelLocalDeployment() noexcept;

            /**
             * Lists the named shadows for the specified thing
             */
            std::shared_ptr<ListNamedShadowsForThingOperation> NewListNamedShadowsForThing() noexcept;

            /**
             * Subscribe to receive notification if GGC is about to update any components
             */
            std::shared_ptr<SubscribeToComponentUpdatesOperation> NewSubscribeToComponentUpdates(
                std::shared_ptr<SubscribeToComponentUpdatesStreamHandler> streamHandler) noexcept;

            /**
             * Lists the last 5 local deployments along with their statuses
             */
            std::shared_ptr<ListLocalDeploymentsOperation> NewListLocalDeployments() noexcept;

            /**
             * Stops a component with the given name
             */
            std::shared_ptr<StopComponentOperation> NewStopComponent() noexcept;

            /**
             * Pause a running component
             */
            std::shared_ptr<PauseComponentOperation> NewPauseComponent() noexcept;

            /**
             * Creates a local deployment on the device.  Also allows to remove existing components.
             */
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
