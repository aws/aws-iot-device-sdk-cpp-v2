#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

/* This file is generated. */

#include <aws/eventstreamrpc/EventStreamClient.h>

#include <aws/crt/DateTime.h>
#include <aws/greengrass/Exports.h>

#include <memory>

using namespace Aws::Eventstreamrpc;

namespace Aws
{
    namespace Greengrass
    {
        class GreengrassCoreIpcClient;
        class GreengrassCoreIpcServiceModel;
        class AWS_GREENGRASSCOREIPC_API UserProperty : public AbstractShapeBase
        {
          public:
            UserProperty() noexcept {}
            UserProperty(const UserProperty &) = default;

            void SetKey(const Aws::Crt::String &key) noexcept { m_key = key; }

            Aws::Crt::Optional<Aws::Crt::String> GetKey() noexcept { return m_key; }

            void SetValue(const Aws::Crt::String &value) noexcept { m_value = value; }

            Aws::Crt::Optional<Aws::Crt::String> GetValue() noexcept { return m_value; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(UserProperty &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(UserProperty *) noexcept;
            /* This needs to be defined so that `UserProperty` can be used as a key in maps. */
            bool operator<(const UserProperty &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_key;
            Aws::Crt::Optional<Aws::Crt::String> m_value;
        };

        enum DetailedDeploymentStatus
        {
            DETAILED_DEPLOYMENT_STATUS_SUCCESSFUL,
            DETAILED_DEPLOYMENT_STATUS_FAILED_NO_STATE_CHANGE,
            DETAILED_DEPLOYMENT_STATUS_FAILED_ROLLBACK_NOT_REQUESTED,
            DETAILED_DEPLOYMENT_STATUS_FAILED_ROLLBACK_COMPLETE,
            DETAILED_DEPLOYMENT_STATUS_REJECTED
        };

        /**
         * Contextual information about the message.
         * NOTE The context is ignored if used in PublishMessage.
         */
        class AWS_GREENGRASSCOREIPC_API MessageContext : public AbstractShapeBase
        {
          public:
            MessageContext() noexcept {}
            MessageContext(const MessageContext &) = default;
            /**
             * The topic where the message was published.
             */
            void SetTopic(const Aws::Crt::String &topic) noexcept { m_topic = topic; }
            /**
             * The topic where the message was published.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetTopic() noexcept { return m_topic; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(MessageContext &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(MessageContext *) noexcept;
            /* This needs to be defined so that `MessageContext` can be used as a key in maps. */
            bool operator<(const MessageContext &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_topic;
        };

        enum PayloadFormat
        {
            PAYLOAD_FORMAT_BYTES,
            PAYLOAD_FORMAT_UTF8
        };

        enum MetricUnitType
        {
            METRIC_UNIT_TYPE_BYTES,
            METRIC_UNIT_TYPE_BYTES_PER_SECOND,
            METRIC_UNIT_TYPE_COUNT,
            METRIC_UNIT_TYPE_COUNT_PER_SECOND,
            METRIC_UNIT_TYPE_MEGABYTES,
            METRIC_UNIT_TYPE_SECONDS
        };

        class AWS_GREENGRASSCOREIPC_API DeploymentStatusDetails : public AbstractShapeBase
        {
          public:
            DeploymentStatusDetails() noexcept {}
            DeploymentStatusDetails(const DeploymentStatusDetails &) = default;
            /**
             * The detailed deployment status of the local deployment.
             */
            void SetDetailedDeploymentStatus(DetailedDeploymentStatus detailedDeploymentStatus) noexcept;
            /**
             * The detailed deployment status of the local deployment.
             */
            Aws::Crt::Optional<DetailedDeploymentStatus> GetDetailedDeploymentStatus() noexcept;
            /**
             * (Optional) The list of local deployment errors
             */
            void SetDeploymentErrorStack(const Aws::Crt::Vector<Aws::Crt::String> &deploymentErrorStack) noexcept
            {
                m_deploymentErrorStack = deploymentErrorStack;
            }
            /**
             * (Optional) The list of local deployment errors
             */
            Aws::Crt::Optional<Aws::Crt::Vector<Aws::Crt::String>> GetDeploymentErrorStack() noexcept
            {
                return m_deploymentErrorStack;
            }
            /**
             * (Optional) The list of local deployment error types
             */
            void SetDeploymentErrorTypes(const Aws::Crt::Vector<Aws::Crt::String> &deploymentErrorTypes) noexcept
            {
                m_deploymentErrorTypes = deploymentErrorTypes;
            }
            /**
             * (Optional) The list of local deployment error types
             */
            Aws::Crt::Optional<Aws::Crt::Vector<Aws::Crt::String>> GetDeploymentErrorTypes() noexcept
            {
                return m_deploymentErrorTypes;
            }
            /**
             * (Optional) The cause of local deployment failure
             */
            void SetDeploymentFailureCause(const Aws::Crt::String &deploymentFailureCause) noexcept
            {
                m_deploymentFailureCause = deploymentFailureCause;
            }
            /**
             * (Optional) The cause of local deployment failure
             */
            Aws::Crt::Optional<Aws::Crt::String> GetDeploymentFailureCause() noexcept
            {
                return m_deploymentFailureCause;
            }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(DeploymentStatusDetails &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(DeploymentStatusDetails *) noexcept;
            /* This needs to be defined so that `DeploymentStatusDetails` can be used as a key in maps. */
            bool operator<(const DeploymentStatusDetails &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_detailedDeploymentStatus;
            Aws::Crt::Optional<Aws::Crt::Vector<Aws::Crt::String>> m_deploymentErrorStack;
            Aws::Crt::Optional<Aws::Crt::Vector<Aws::Crt::String>> m_deploymentErrorTypes;
            Aws::Crt::Optional<Aws::Crt::String> m_deploymentFailureCause;
        };

        enum DeploymentStatus
        {
            DEPLOYMENT_STATUS_QUEUED,
            DEPLOYMENT_STATUS_IN_PROGRESS,
            DEPLOYMENT_STATUS_SUCCEEDED,
            DEPLOYMENT_STATUS_FAILED,
            DEPLOYMENT_STATUS_CANCELED
        };

        enum LifecycleState
        {
            LIFECYCLE_STATE_RUNNING,
            LIFECYCLE_STATE_ERRORED,
            LIFECYCLE_STATE_NEW,
            LIFECYCLE_STATE_FINISHED,
            LIFECYCLE_STATE_INSTALLED,
            LIFECYCLE_STATE_BROKEN,
            LIFECYCLE_STATE_STARTING,
            LIFECYCLE_STATE_STOPPING
        };

        class AWS_GREENGRASSCOREIPC_API SystemResourceLimits : public AbstractShapeBase
        {
          public:
            SystemResourceLimits() noexcept {}
            SystemResourceLimits(const SystemResourceLimits &) = default;
            /**
             * (Optional) The maximum amount of RAM (in kilobytes) that this component's processes can use on the core
             * device.
             */
            void SetMemory(const int64_t &memory) noexcept { m_memory = memory; }
            /**
             * (Optional) The maximum amount of RAM (in kilobytes) that this component's processes can use on the core
             * device.
             */
            Aws::Crt::Optional<int64_t> GetMemory() noexcept { return m_memory; }
            /**
             * (Optional) The maximum amount of CPU time that this component's processes can use on the core device.
             */
            void SetCpus(const double &cpus) noexcept { m_cpus = cpus; }
            /**
             * (Optional) The maximum amount of CPU time that this component's processes can use on the core device.
             */
            Aws::Crt::Optional<double> GetCpus() noexcept { return m_cpus; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(SystemResourceLimits &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(SystemResourceLimits *) noexcept;
            /* This needs to be defined so that `SystemResourceLimits` can be used as a key in maps. */
            bool operator<(const SystemResourceLimits &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<int64_t> m_memory;
            Aws::Crt::Optional<double> m_cpus;
        };

        class AWS_GREENGRASSCOREIPC_API ValidateConfigurationUpdateEvent : public AbstractShapeBase
        {
          public:
            ValidateConfigurationUpdateEvent() noexcept {}
            ValidateConfigurationUpdateEvent(const ValidateConfigurationUpdateEvent &) = default;
            /**
             * The object that contains the new configuration.
             */
            void SetConfiguration(const Aws::Crt::JsonObject &configuration) noexcept
            {
                m_configuration = configuration;
            }
            /**
             * The object that contains the new configuration.
             */
            Aws::Crt::Optional<Aws::Crt::JsonObject> GetConfiguration() noexcept { return m_configuration; }
            /**
             * The ID of the AWS IoT Greengrass deployment that updates the component.
             */
            void SetDeploymentId(const Aws::Crt::String &deploymentId) noexcept { m_deploymentId = deploymentId; }
            /**
             * The ID of the AWS IoT Greengrass deployment that updates the component.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetDeploymentId() noexcept { return m_deploymentId; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(ValidateConfigurationUpdateEvent &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(ValidateConfigurationUpdateEvent *) noexcept;
            /* This needs to be defined so that `ValidateConfigurationUpdateEvent` can be used as a key in maps. */
            bool operator<(const ValidateConfigurationUpdateEvent &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::JsonObject> m_configuration;
            Aws::Crt::Optional<Aws::Crt::String> m_deploymentId;
        };

        class AWS_GREENGRASSCOREIPC_API BinaryMessage : public AbstractShapeBase
        {
          public:
            BinaryMessage() noexcept {}
            BinaryMessage(const BinaryMessage &) = default;
            /**
             * The binary message as a blob.
             */
            void SetMessage(const Aws::Crt::Vector<uint8_t> &message) noexcept { m_message = message; }
            /**
             * The binary message as a blob.
             */
            Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>> GetMessage() noexcept { return m_message; }
            /**
             * The context of the message, such as the topic where the message was published.
             */
            void SetContext(const MessageContext &context) noexcept { m_context = context; }
            /**
             * The context of the message, such as the topic where the message was published.
             */
            Aws::Crt::Optional<MessageContext> GetContext() noexcept { return m_context; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(BinaryMessage &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(BinaryMessage *) noexcept;
            /* This needs to be defined so that `BinaryMessage` can be used as a key in maps. */
            bool operator<(const BinaryMessage &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>> m_message;
            Aws::Crt::Optional<MessageContext> m_context;
        };

        class AWS_GREENGRASSCOREIPC_API JsonMessage : public AbstractShapeBase
        {
          public:
            JsonMessage() noexcept {}
            JsonMessage(const JsonMessage &) = default;
            /**
             * The JSON message as an object.
             */
            void SetMessage(const Aws::Crt::JsonObject &message) noexcept { m_message = message; }
            /**
             * The JSON message as an object.
             */
            Aws::Crt::Optional<Aws::Crt::JsonObject> GetMessage() noexcept { return m_message; }
            /**
             * The context of the message, such as the topic where the message was published.
             */
            void SetContext(const MessageContext &context) noexcept { m_context = context; }
            /**
             * The context of the message, such as the topic where the message was published.
             */
            Aws::Crt::Optional<MessageContext> GetContext() noexcept { return m_context; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(JsonMessage &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(JsonMessage *) noexcept;
            /* This needs to be defined so that `JsonMessage` can be used as a key in maps. */
            bool operator<(const JsonMessage &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::JsonObject> m_message;
            Aws::Crt::Optional<MessageContext> m_context;
        };

        class AWS_GREENGRASSCOREIPC_API MQTTMessage : public AbstractShapeBase
        {
          public:
            MQTTMessage() noexcept {}
            MQTTMessage(const MQTTMessage &) = default;
            /**
             * The topic to which the message was published.
             */
            void SetTopicName(const Aws::Crt::String &topicName) noexcept { m_topicName = topicName; }
            /**
             * The topic to which the message was published.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetTopicName() noexcept { return m_topicName; }
            /**
             * (Optional) The message payload as a blob.
             */
            void SetPayload(const Aws::Crt::Vector<uint8_t> &payload) noexcept { m_payload = payload; }
            /**
             * (Optional) The message payload as a blob.
             */
            Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>> GetPayload() noexcept { return m_payload; }
            /**
             * (Optional) The value of the retain flag.
             */
            void SetRetain(const bool &retain) noexcept { m_retain = retain; }
            /**
             * (Optional) The value of the retain flag.
             */
            Aws::Crt::Optional<bool> GetRetain() noexcept { return m_retain; }
            /**
             * (Optional) MQTT user properties associated with the message.
             */
            void SetUserProperties(const Aws::Crt::Vector<UserProperty> &userProperties) noexcept
            {
                m_userProperties = userProperties;
            }
            /**
             * (Optional) MQTT user properties associated with the message.
             */
            Aws::Crt::Optional<Aws::Crt::Vector<UserProperty>> GetUserProperties() noexcept { return m_userProperties; }
            /**
             * (Optional) Message expiry interval in seconds.
             */
            void SetMessageExpiryIntervalSeconds(const int64_t &messageExpiryIntervalSeconds) noexcept
            {
                m_messageExpiryIntervalSeconds = messageExpiryIntervalSeconds;
            }
            /**
             * (Optional) Message expiry interval in seconds.
             */
            Aws::Crt::Optional<int64_t> GetMessageExpiryIntervalSeconds() noexcept
            {
                return m_messageExpiryIntervalSeconds;
            }
            /**
             * (Optional) Correlation data blob for request/response.
             */
            void SetCorrelationData(const Aws::Crt::Vector<uint8_t> &correlationData) noexcept
            {
                m_correlationData = correlationData;
            }
            /**
             * (Optional) Correlation data blob for request/response.
             */
            Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>> GetCorrelationData() noexcept { return m_correlationData; }
            /**
             * (Optional) Response topic for request/response.
             */
            void SetResponseTopic(const Aws::Crt::String &responseTopic) noexcept { m_responseTopic = responseTopic; }
            /**
             * (Optional) Response topic for request/response.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetResponseTopic() noexcept { return m_responseTopic; }
            /**
             * (Optional) Message payload format.
             */
            void SetPayloadFormat(PayloadFormat payloadFormat) noexcept;
            /**
             * (Optional) Message payload format.
             */
            Aws::Crt::Optional<PayloadFormat> GetPayloadFormat() noexcept;
            /**
             * (Optional) Message content type.
             */
            void SetContentType(const Aws::Crt::String &contentType) noexcept { m_contentType = contentType; }
            /**
             * (Optional) Message content type.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetContentType() noexcept { return m_contentType; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(MQTTMessage &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(MQTTMessage *) noexcept;
            /* This needs to be defined so that `MQTTMessage` can be used as a key in maps. */
            bool operator<(const MQTTMessage &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_topicName;
            Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>> m_payload;
            Aws::Crt::Optional<bool> m_retain;
            Aws::Crt::Optional<Aws::Crt::Vector<UserProperty>> m_userProperties;
            Aws::Crt::Optional<int64_t> m_messageExpiryIntervalSeconds;
            Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>> m_correlationData;
            Aws::Crt::Optional<Aws::Crt::String> m_responseTopic;
            Aws::Crt::Optional<Aws::Crt::String> m_payloadFormat;
            Aws::Crt::Optional<Aws::Crt::String> m_contentType;
        };

        class AWS_GREENGRASSCOREIPC_API ConfigurationUpdateEvent : public AbstractShapeBase
        {
          public:
            ConfigurationUpdateEvent() noexcept {}
            ConfigurationUpdateEvent(const ConfigurationUpdateEvent &) = default;
            /**
             * The name of the component.
             */
            void SetComponentName(const Aws::Crt::String &componentName) noexcept { m_componentName = componentName; }
            /**
             * The name of the component.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetComponentName() noexcept { return m_componentName; }
            /**
             * The key path to the configuration value that updated.
             */
            void SetKeyPath(const Aws::Crt::Vector<Aws::Crt::String> &keyPath) noexcept { m_keyPath = keyPath; }
            /**
             * The key path to the configuration value that updated.
             */
            Aws::Crt::Optional<Aws::Crt::Vector<Aws::Crt::String>> GetKeyPath() noexcept { return m_keyPath; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(ConfigurationUpdateEvent &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(ConfigurationUpdateEvent *) noexcept;
            /* This needs to be defined so that `ConfigurationUpdateEvent` can be used as a key in maps. */
            bool operator<(const ConfigurationUpdateEvent &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_componentName;
            Aws::Crt::Optional<Aws::Crt::Vector<Aws::Crt::String>> m_keyPath;
        };

        class AWS_GREENGRASSCOREIPC_API PostComponentUpdateEvent : public AbstractShapeBase
        {
          public:
            PostComponentUpdateEvent() noexcept {}
            PostComponentUpdateEvent(const PostComponentUpdateEvent &) = default;
            /**
             * The ID of the AWS IoT Greengrass deployment that updated the component.
             */
            void SetDeploymentId(const Aws::Crt::String &deploymentId) noexcept { m_deploymentId = deploymentId; }
            /**
             * The ID of the AWS IoT Greengrass deployment that updated the component.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetDeploymentId() noexcept { return m_deploymentId; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(PostComponentUpdateEvent &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(PostComponentUpdateEvent *) noexcept;
            /* This needs to be defined so that `PostComponentUpdateEvent` can be used as a key in maps. */
            bool operator<(const PostComponentUpdateEvent &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_deploymentId;
        };

        class AWS_GREENGRASSCOREIPC_API PreComponentUpdateEvent : public AbstractShapeBase
        {
          public:
            PreComponentUpdateEvent() noexcept {}
            PreComponentUpdateEvent(const PreComponentUpdateEvent &) = default;
            /**
             * The ID of the AWS IoT Greengrass deployment that updates the component.
             */
            void SetDeploymentId(const Aws::Crt::String &deploymentId) noexcept { m_deploymentId = deploymentId; }
            /**
             * The ID of the AWS IoT Greengrass deployment that updates the component.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetDeploymentId() noexcept { return m_deploymentId; }
            /**
             * Whether or not Greengrass needs to restart to apply the update.
             */
            void SetIsGgcRestarting(const bool &isGgcRestarting) noexcept { m_isGgcRestarting = isGgcRestarting; }
            /**
             * Whether or not Greengrass needs to restart to apply the update.
             */
            Aws::Crt::Optional<bool> GetIsGgcRestarting() noexcept { return m_isGgcRestarting; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(PreComponentUpdateEvent &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(PreComponentUpdateEvent *) noexcept;
            /* This needs to be defined so that `PreComponentUpdateEvent` can be used as a key in maps. */
            bool operator<(const PreComponentUpdateEvent &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_deploymentId;
            Aws::Crt::Optional<bool> m_isGgcRestarting;
        };

        class AWS_GREENGRASSCOREIPC_API CertificateUpdate : public AbstractShapeBase
        {
          public:
            CertificateUpdate() noexcept {}
            CertificateUpdate(const CertificateUpdate &) = default;
            /**
             * The private key in pem format.
             */
            void SetPrivateKey(const Aws::Crt::String &privateKey) noexcept { m_privateKey = privateKey; }
            /**
             * The private key in pem format.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetPrivateKey() noexcept { return m_privateKey; }
            /**
             * The public key in pem format.
             */
            void SetPublicKey(const Aws::Crt::String &publicKey) noexcept { m_publicKey = publicKey; }
            /**
             * The public key in pem format.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetPublicKey() noexcept { return m_publicKey; }
            /**
             * The certificate in pem format.
             */
            void SetCertificate(const Aws::Crt::String &certificate) noexcept { m_certificate = certificate; }
            /**
             * The certificate in pem format.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetCertificate() noexcept { return m_certificate; }
            /**
             * List of CA certificates in pem format.
             */
            void SetCaCertificates(const Aws::Crt::Vector<Aws::Crt::String> &caCertificates) noexcept
            {
                m_caCertificates = caCertificates;
            }
            /**
             * List of CA certificates in pem format.
             */
            Aws::Crt::Optional<Aws::Crt::Vector<Aws::Crt::String>> GetCaCertificates() noexcept
            {
                return m_caCertificates;
            }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(CertificateUpdate &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(CertificateUpdate *) noexcept;
            /* This needs to be defined so that `CertificateUpdate` can be used as a key in maps. */
            bool operator<(const CertificateUpdate &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_privateKey;
            Aws::Crt::Optional<Aws::Crt::String> m_publicKey;
            Aws::Crt::Optional<Aws::Crt::String> m_certificate;
            Aws::Crt::Optional<Aws::Crt::Vector<Aws::Crt::String>> m_caCertificates;
        };

        enum CertificateType
        {
            CERTIFICATE_TYPE_SERVER
        };

        enum ConfigurationValidityStatus
        {
            CONFIGURATION_VALIDITY_STATUS_ACCEPTED,
            CONFIGURATION_VALIDITY_STATUS_REJECTED
        };

        class AWS_GREENGRASSCOREIPC_API Metric : public AbstractShapeBase
        {
          public:
            Metric() noexcept {}
            Metric(const Metric &) = default;

            void SetName(const Aws::Crt::String &name) noexcept { m_name = name; }

            Aws::Crt::Optional<Aws::Crt::String> GetName() noexcept { return m_name; }

            void SetUnit(MetricUnitType unit) noexcept;

            Aws::Crt::Optional<MetricUnitType> GetUnit() noexcept;

            void SetValue(const double &value) noexcept { m_value = value; }

            Aws::Crt::Optional<double> GetValue() noexcept { return m_value; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(Metric &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(Metric *) noexcept;
            /* This needs to be defined so that `Metric` can be used as a key in maps. */
            bool operator<(const Metric &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_name;
            Aws::Crt::Optional<Aws::Crt::String> m_unit;
            Aws::Crt::Optional<double> m_value;
        };

        class AWS_GREENGRASSCOREIPC_API LocalDeployment : public AbstractShapeBase
        {
          public:
            LocalDeployment() noexcept {}
            LocalDeployment(const LocalDeployment &) = default;
            /**
             * The ID of the local deployment.
             */
            void SetDeploymentId(const Aws::Crt::String &deploymentId) noexcept { m_deploymentId = deploymentId; }
            /**
             * The ID of the local deployment.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetDeploymentId() noexcept { return m_deploymentId; }
            /**
             * The status of the local deployment.
             */
            void SetStatus(DeploymentStatus status) noexcept;
            /**
             * The status of the local deployment.
             */
            Aws::Crt::Optional<DeploymentStatus> GetStatus() noexcept;
            /**
             * (Optional) The timestamp at which the local deployment was created in MM/dd/yyyy hh:mm:ss format
             */
            void SetCreatedOn(const Aws::Crt::String &createdOn) noexcept { m_createdOn = createdOn; }
            /**
             * (Optional) The timestamp at which the local deployment was created in MM/dd/yyyy hh:mm:ss format
             */
            Aws::Crt::Optional<Aws::Crt::String> GetCreatedOn() noexcept { return m_createdOn; }
            /**
             * (Optional) The status details of the local deployment.
             */
            void SetDeploymentStatusDetails(const DeploymentStatusDetails &deploymentStatusDetails) noexcept
            {
                m_deploymentStatusDetails = deploymentStatusDetails;
            }
            /**
             * (Optional) The status details of the local deployment.
             */
            Aws::Crt::Optional<DeploymentStatusDetails> GetDeploymentStatusDetails() noexcept
            {
                return m_deploymentStatusDetails;
            }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(LocalDeployment &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(LocalDeployment *) noexcept;
            /* This needs to be defined so that `LocalDeployment` can be used as a key in maps. */
            bool operator<(const LocalDeployment &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_deploymentId;
            Aws::Crt::Optional<Aws::Crt::String> m_status;
            Aws::Crt::Optional<Aws::Crt::String> m_createdOn;
            Aws::Crt::Optional<DeploymentStatusDetails> m_deploymentStatusDetails;
        };

        class AWS_GREENGRASSCOREIPC_API ComponentDetails : public AbstractShapeBase
        {
          public:
            ComponentDetails() noexcept {}
            ComponentDetails(const ComponentDetails &) = default;
            /**
             * The name of the component.
             */
            void SetComponentName(const Aws::Crt::String &componentName) noexcept { m_componentName = componentName; }
            /**
             * The name of the component.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetComponentName() noexcept { return m_componentName; }
            /**
             * The version of the component.
             */
            void SetVersion(const Aws::Crt::String &version) noexcept { m_version = version; }
            /**
             * The version of the component.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetVersion() noexcept { return m_version; }
            /**
             * The state of the component.
             */
            void SetState(LifecycleState state) noexcept;
            /**
             * The state of the component.
             */
            Aws::Crt::Optional<LifecycleState> GetState() noexcept;
            /**
             * The component's configuration as a JSON object.
             */
            void SetConfiguration(const Aws::Crt::JsonObject &configuration) noexcept
            {
                m_configuration = configuration;
            }
            /**
             * The component's configuration as a JSON object.
             */
            Aws::Crt::Optional<Aws::Crt::JsonObject> GetConfiguration() noexcept { return m_configuration; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(ComponentDetails &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(ComponentDetails *) noexcept;
            /* This needs to be defined so that `ComponentDetails` can be used as a key in maps. */
            bool operator<(const ComponentDetails &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_componentName;
            Aws::Crt::Optional<Aws::Crt::String> m_version;
            Aws::Crt::Optional<Aws::Crt::String> m_state;
            Aws::Crt::Optional<Aws::Crt::JsonObject> m_configuration;
        };

        class AWS_GREENGRASSCOREIPC_API MQTTCredential : public AbstractShapeBase
        {
          public:
            MQTTCredential() noexcept {}
            MQTTCredential(const MQTTCredential &) = default;
            /**
             * The client ID to used to connect.
             */
            void SetClientId(const Aws::Crt::String &clientId) noexcept { m_clientId = clientId; }
            /**
             * The client ID to used to connect.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetClientId() noexcept { return m_clientId; }
            /**
             * The client certificate in pem format.
             */
            void SetCertificatePem(const Aws::Crt::String &certificatePem) noexcept
            {
                m_certificatePem = certificatePem;
            }
            /**
             * The client certificate in pem format.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetCertificatePem() noexcept { return m_certificatePem; }
            /**
             * The username. (unused).
             */
            void SetUsername(const Aws::Crt::String &username) noexcept { m_username = username; }
            /**
             * The username. (unused).
             */
            Aws::Crt::Optional<Aws::Crt::String> GetUsername() noexcept { return m_username; }
            /**
             * The password. (unused).
             */
            void SetPassword(const Aws::Crt::String &password) noexcept { m_password = password; }
            /**
             * The password. (unused).
             */
            Aws::Crt::Optional<Aws::Crt::String> GetPassword() noexcept { return m_password; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(MQTTCredential &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(MQTTCredential *) noexcept;
            /* This needs to be defined so that `MQTTCredential` can be used as a key in maps. */
            bool operator<(const MQTTCredential &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_clientId;
            Aws::Crt::Optional<Aws::Crt::String> m_certificatePem;
            Aws::Crt::Optional<Aws::Crt::String> m_username;
            Aws::Crt::Optional<Aws::Crt::String> m_password;
        };

        class AWS_GREENGRASSCOREIPC_API RunWithInfo : public AbstractShapeBase
        {
          public:
            RunWithInfo() noexcept {}
            RunWithInfo(const RunWithInfo &) = default;
            /**
             * (Optional) The POSIX system user and, optionally, group to use to run this component on Linux core
             * devices.
             */
            void SetPosixUser(const Aws::Crt::String &posixUser) noexcept { m_posixUser = posixUser; }
            /**
             * (Optional) The POSIX system user and, optionally, group to use to run this component on Linux core
             * devices.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetPosixUser() noexcept { return m_posixUser; }
            /**
             * (Optional) The Windows user to use to run this component on Windows core devices.
             */
            void SetWindowsUser(const Aws::Crt::String &windowsUser) noexcept { m_windowsUser = windowsUser; }
            /**
             * (Optional) The Windows user to use to run this component on Windows core devices.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetWindowsUser() noexcept { return m_windowsUser; }
            /**
             * (Optional) The system resource limits to apply to this component's processes.
             */
            void SetSystemResourceLimits(const SystemResourceLimits &systemResourceLimits) noexcept
            {
                m_systemResourceLimits = systemResourceLimits;
            }
            /**
             * (Optional) The system resource limits to apply to this component's processes.
             */
            Aws::Crt::Optional<SystemResourceLimits> GetSystemResourceLimits() noexcept
            {
                return m_systemResourceLimits;
            }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(RunWithInfo &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(RunWithInfo *) noexcept;
            /* This needs to be defined so that `RunWithInfo` can be used as a key in maps. */
            bool operator<(const RunWithInfo &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_posixUser;
            Aws::Crt::Optional<Aws::Crt::String> m_windowsUser;
            Aws::Crt::Optional<SystemResourceLimits> m_systemResourceLimits;
        };

        class AWS_GREENGRASSCOREIPC_API ClientDeviceCredential : public AbstractShapeBase
        {
          public:
            ClientDeviceCredential() noexcept {}
            ClientDeviceCredential &operator=(const ClientDeviceCredential &) noexcept;
            ClientDeviceCredential(const ClientDeviceCredential &objectToCopy) { *this = objectToCopy; }
            /**
             * The client device's X.509 device certificate.
             */
            void SetClientDeviceCertificate(const Aws::Crt::String &clientDeviceCertificate) noexcept
            {
                m_clientDeviceCertificate = clientDeviceCertificate;
                m_chosenMember = TAG_CLIENT_DEVICE_CERTIFICATE;
            }
            /**
             * The client device's X.509 device certificate.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetClientDeviceCertificate() noexcept
            {
                if (m_chosenMember == TAG_CLIENT_DEVICE_CERTIFICATE)
                {
                    return m_clientDeviceCertificate;
                }
                else
                {
                    return Aws::Crt::Optional<Aws::Crt::String>();
                }
            }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(ClientDeviceCredential &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(ClientDeviceCredential *) noexcept;
            /* This needs to be defined so that `ClientDeviceCredential` can be used as a key in maps. */
            bool operator<(const ClientDeviceCredential &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            enum ChosenMember
            {
                TAG_CLIENT_DEVICE_CERTIFICATE
            } m_chosenMember;
            Aws::Crt::Optional<Aws::Crt::String> m_clientDeviceCertificate;
        };

        enum ReportedLifecycleState
        {
            REPORTED_LIFECYCLE_STATE_RUNNING,
            REPORTED_LIFECYCLE_STATE_ERRORED
        };

        class AWS_GREENGRASSCOREIPC_API ValidateConfigurationUpdateEvents : public AbstractShapeBase
        {
          public:
            ValidateConfigurationUpdateEvents() noexcept {}
            ValidateConfigurationUpdateEvents &operator=(const ValidateConfigurationUpdateEvents &) noexcept;
            ValidateConfigurationUpdateEvents(const ValidateConfigurationUpdateEvents &objectToCopy)
            {
                *this = objectToCopy;
            }
            /**
             * The configuration update event.
             */
            void SetValidateConfigurationUpdateEvent(
                const ValidateConfigurationUpdateEvent &validateConfigurationUpdateEvent) noexcept
            {
                m_validateConfigurationUpdateEvent = validateConfigurationUpdateEvent;
                m_chosenMember = TAG_VALIDATE_CONFIGURATION_UPDATE_EVENT;
            }
            /**
             * The configuration update event.
             */
            Aws::Crt::Optional<ValidateConfigurationUpdateEvent> GetValidateConfigurationUpdateEvent() noexcept
            {
                if (m_chosenMember == TAG_VALIDATE_CONFIGURATION_UPDATE_EVENT)
                {
                    return m_validateConfigurationUpdateEvent;
                }
                else
                {
                    return Aws::Crt::Optional<ValidateConfigurationUpdateEvent>();
                }
            }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(ValidateConfigurationUpdateEvents &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(ValidateConfigurationUpdateEvents *) noexcept;
            /* This needs to be defined so that `ValidateConfigurationUpdateEvents` can be used as a key in maps. */
            bool operator<(const ValidateConfigurationUpdateEvents &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            enum ChosenMember
            {
                TAG_VALIDATE_CONFIGURATION_UPDATE_EVENT
            } m_chosenMember;
            Aws::Crt::Optional<ValidateConfigurationUpdateEvent> m_validateConfigurationUpdateEvent;
        };

        class AWS_GREENGRASSCOREIPC_API SubscriptionResponseMessage : public AbstractShapeBase
        {
          public:
            SubscriptionResponseMessage() noexcept {}
            SubscriptionResponseMessage &operator=(const SubscriptionResponseMessage &) noexcept;
            SubscriptionResponseMessage(const SubscriptionResponseMessage &objectToCopy) { *this = objectToCopy; }
            /**
             * (Optional) A JSON message.
             */
            void SetJsonMessage(const JsonMessage &jsonMessage) noexcept
            {
                m_jsonMessage = jsonMessage;
                m_chosenMember = TAG_JSON_MESSAGE;
            }
            /**
             * (Optional) A JSON message.
             */
            Aws::Crt::Optional<JsonMessage> GetJsonMessage() noexcept
            {
                if (m_chosenMember == TAG_JSON_MESSAGE)
                {
                    return m_jsonMessage;
                }
                else
                {
                    return Aws::Crt::Optional<JsonMessage>();
                }
            }
            /**
             * (Optional) A binary message.
             */
            void SetBinaryMessage(const BinaryMessage &binaryMessage) noexcept
            {
                m_binaryMessage = binaryMessage;
                m_chosenMember = TAG_BINARY_MESSAGE;
            }
            /**
             * (Optional) A binary message.
             */
            Aws::Crt::Optional<BinaryMessage> GetBinaryMessage() noexcept
            {
                if (m_chosenMember == TAG_BINARY_MESSAGE)
                {
                    return m_binaryMessage;
                }
                else
                {
                    return Aws::Crt::Optional<BinaryMessage>();
                }
            }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(SubscriptionResponseMessage &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(SubscriptionResponseMessage *) noexcept;
            /* This needs to be defined so that `SubscriptionResponseMessage` can be used as a key in maps. */
            bool operator<(const SubscriptionResponseMessage &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            enum ChosenMember
            {
                TAG_JSON_MESSAGE,
                TAG_BINARY_MESSAGE
            } m_chosenMember;
            Aws::Crt::Optional<JsonMessage> m_jsonMessage;
            Aws::Crt::Optional<BinaryMessage> m_binaryMessage;
        };

        enum ReceiveMode
        {
            RECEIVE_MODE_RECEIVE_ALL_MESSAGES,
            RECEIVE_MODE_RECEIVE_MESSAGES_FROM_OTHERS
        };

        class AWS_GREENGRASSCOREIPC_API IoTCoreMessage : public AbstractShapeBase
        {
          public:
            IoTCoreMessage() noexcept {}
            IoTCoreMessage &operator=(const IoTCoreMessage &) noexcept;
            IoTCoreMessage(const IoTCoreMessage &objectToCopy) { *this = objectToCopy; }
            /**
             * The MQTT message.
             */
            void SetMessage(const MQTTMessage &message) noexcept
            {
                m_message = message;
                m_chosenMember = TAG_MESSAGE;
            }
            /**
             * The MQTT message.
             */
            Aws::Crt::Optional<MQTTMessage> GetMessage() noexcept
            {
                if (m_chosenMember == TAG_MESSAGE)
                {
                    return m_message;
                }
                else
                {
                    return Aws::Crt::Optional<MQTTMessage>();
                }
            }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(IoTCoreMessage &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(IoTCoreMessage *) noexcept;
            /* This needs to be defined so that `IoTCoreMessage` can be used as a key in maps. */
            bool operator<(const IoTCoreMessage &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            enum ChosenMember
            {
                TAG_MESSAGE
            } m_chosenMember;
            Aws::Crt::Optional<MQTTMessage> m_message;
        };

        enum QOS
        {
            QOS_AT_MOST_ONCE,
            QOS_AT_LEAST_ONCE
        };

        class AWS_GREENGRASSCOREIPC_API ConfigurationUpdateEvents : public AbstractShapeBase
        {
          public:
            ConfigurationUpdateEvents() noexcept {}
            ConfigurationUpdateEvents &operator=(const ConfigurationUpdateEvents &) noexcept;
            ConfigurationUpdateEvents(const ConfigurationUpdateEvents &objectToCopy) { *this = objectToCopy; }
            /**
             * The configuration update event.
             */
            void SetConfigurationUpdateEvent(const ConfigurationUpdateEvent &configurationUpdateEvent) noexcept
            {
                m_configurationUpdateEvent = configurationUpdateEvent;
                m_chosenMember = TAG_CONFIGURATION_UPDATE_EVENT;
            }
            /**
             * The configuration update event.
             */
            Aws::Crt::Optional<ConfigurationUpdateEvent> GetConfigurationUpdateEvent() noexcept
            {
                if (m_chosenMember == TAG_CONFIGURATION_UPDATE_EVENT)
                {
                    return m_configurationUpdateEvent;
                }
                else
                {
                    return Aws::Crt::Optional<ConfigurationUpdateEvent>();
                }
            }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(ConfigurationUpdateEvents &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(ConfigurationUpdateEvents *) noexcept;
            /* This needs to be defined so that `ConfigurationUpdateEvents` can be used as a key in maps. */
            bool operator<(const ConfigurationUpdateEvents &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            enum ChosenMember
            {
                TAG_CONFIGURATION_UPDATE_EVENT
            } m_chosenMember;
            Aws::Crt::Optional<ConfigurationUpdateEvent> m_configurationUpdateEvent;
        };

        class AWS_GREENGRASSCOREIPC_API ComponentUpdatePolicyEvents : public AbstractShapeBase
        {
          public:
            ComponentUpdatePolicyEvents() noexcept {}
            ComponentUpdatePolicyEvents &operator=(const ComponentUpdatePolicyEvents &) noexcept;
            ComponentUpdatePolicyEvents(const ComponentUpdatePolicyEvents &objectToCopy) { *this = objectToCopy; }
            /**
             * An event that indicates that the Greengrass wants to update a component.
             */
            void SetPreUpdateEvent(const PreComponentUpdateEvent &preUpdateEvent) noexcept
            {
                m_preUpdateEvent = preUpdateEvent;
                m_chosenMember = TAG_PRE_UPDATE_EVENT;
            }
            /**
             * An event that indicates that the Greengrass wants to update a component.
             */
            Aws::Crt::Optional<PreComponentUpdateEvent> GetPreUpdateEvent() noexcept
            {
                if (m_chosenMember == TAG_PRE_UPDATE_EVENT)
                {
                    return m_preUpdateEvent;
                }
                else
                {
                    return Aws::Crt::Optional<PreComponentUpdateEvent>();
                }
            }
            /**
             * An event that indicates that the nucleus updated a component.
             */
            void SetPostUpdateEvent(const PostComponentUpdateEvent &postUpdateEvent) noexcept
            {
                m_postUpdateEvent = postUpdateEvent;
                m_chosenMember = TAG_POST_UPDATE_EVENT;
            }
            /**
             * An event that indicates that the nucleus updated a component.
             */
            Aws::Crt::Optional<PostComponentUpdateEvent> GetPostUpdateEvent() noexcept
            {
                if (m_chosenMember == TAG_POST_UPDATE_EVENT)
                {
                    return m_postUpdateEvent;
                }
                else
                {
                    return Aws::Crt::Optional<PostComponentUpdateEvent>();
                }
            }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(ComponentUpdatePolicyEvents &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(ComponentUpdatePolicyEvents *) noexcept;
            /* This needs to be defined so that `ComponentUpdatePolicyEvents` can be used as a key in maps. */
            bool operator<(const ComponentUpdatePolicyEvents &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            enum ChosenMember
            {
                TAG_PRE_UPDATE_EVENT,
                TAG_POST_UPDATE_EVENT
            } m_chosenMember;
            Aws::Crt::Optional<PreComponentUpdateEvent> m_preUpdateEvent;
            Aws::Crt::Optional<PostComponentUpdateEvent> m_postUpdateEvent;
        };

        class AWS_GREENGRASSCOREIPC_API CertificateUpdateEvent : public AbstractShapeBase
        {
          public:
            CertificateUpdateEvent() noexcept {}
            CertificateUpdateEvent &operator=(const CertificateUpdateEvent &) noexcept;
            CertificateUpdateEvent(const CertificateUpdateEvent &objectToCopy) { *this = objectToCopy; }
            /**
             * The information about the new certificate.
             */
            void SetCertificateUpdate(const CertificateUpdate &certificateUpdate) noexcept
            {
                m_certificateUpdate = certificateUpdate;
                m_chosenMember = TAG_CERTIFICATE_UPDATE;
            }
            /**
             * The information about the new certificate.
             */
            Aws::Crt::Optional<CertificateUpdate> GetCertificateUpdate() noexcept
            {
                if (m_chosenMember == TAG_CERTIFICATE_UPDATE)
                {
                    return m_certificateUpdate;
                }
                else
                {
                    return Aws::Crt::Optional<CertificateUpdate>();
                }
            }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(CertificateUpdateEvent &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(CertificateUpdateEvent *) noexcept;
            /* This needs to be defined so that `CertificateUpdateEvent` can be used as a key in maps. */
            bool operator<(const CertificateUpdateEvent &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            enum ChosenMember
            {
                TAG_CERTIFICATE_UPDATE
            } m_chosenMember;
            Aws::Crt::Optional<CertificateUpdate> m_certificateUpdate;
        };

        class AWS_GREENGRASSCOREIPC_API CertificateOptions : public AbstractShapeBase
        {
          public:
            CertificateOptions() noexcept {}
            CertificateOptions(const CertificateOptions &) = default;
            /**
             * The types of certificate updates to subscribe to.
             */
            void SetCertificateType(CertificateType certificateType) noexcept;
            /**
             * The types of certificate updates to subscribe to.
             */
            Aws::Crt::Optional<CertificateType> GetCertificateType() noexcept;
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(CertificateOptions &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(CertificateOptions *) noexcept;
            /* This needs to be defined so that `CertificateOptions` can be used as a key in maps. */
            bool operator<(const CertificateOptions &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_certificateType;
        };

        enum RequestStatus
        {
            REQUEST_STATUS_SUCCEEDED,
            REQUEST_STATUS_FAILED
        };

        class AWS_GREENGRASSCOREIPC_API ConfigurationValidityReport : public AbstractShapeBase
        {
          public:
            ConfigurationValidityReport() noexcept {}
            ConfigurationValidityReport(const ConfigurationValidityReport &) = default;
            /**
             * The validity status.
             */
            void SetStatus(ConfigurationValidityStatus status) noexcept;
            /**
             * The validity status.
             */
            Aws::Crt::Optional<ConfigurationValidityStatus> GetStatus() noexcept;
            /**
             * The ID of the AWS IoT Greengrass deployment that requested the configuration update.
             */
            void SetDeploymentId(const Aws::Crt::String &deploymentId) noexcept { m_deploymentId = deploymentId; }
            /**
             * The ID of the AWS IoT Greengrass deployment that requested the configuration update.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetDeploymentId() noexcept { return m_deploymentId; }
            /**
             * (Optional) A message that reports why the configuration isn't valid.
             */
            void SetMessage(const Aws::Crt::String &message) noexcept { m_message = message; }
            /**
             * (Optional) A message that reports why the configuration isn't valid.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetMessage() noexcept { return m_message; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(ConfigurationValidityReport &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(ConfigurationValidityReport *) noexcept;
            /* This needs to be defined so that `ConfigurationValidityReport` can be used as a key in maps. */
            bool operator<(const ConfigurationValidityReport &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_status;
            Aws::Crt::Optional<Aws::Crt::String> m_deploymentId;
            Aws::Crt::Optional<Aws::Crt::String> m_message;
        };

        class AWS_GREENGRASSCOREIPC_API PublishMessage : public AbstractShapeBase
        {
          public:
            PublishMessage() noexcept {}
            PublishMessage &operator=(const PublishMessage &) noexcept;
            PublishMessage(const PublishMessage &objectToCopy) { *this = objectToCopy; }
            /**
             * (Optional) A JSON message.
             */
            void SetJsonMessage(const JsonMessage &jsonMessage) noexcept
            {
                m_jsonMessage = jsonMessage;
                m_chosenMember = TAG_JSON_MESSAGE;
            }
            /**
             * (Optional) A JSON message.
             */
            Aws::Crt::Optional<JsonMessage> GetJsonMessage() noexcept
            {
                if (m_chosenMember == TAG_JSON_MESSAGE)
                {
                    return m_jsonMessage;
                }
                else
                {
                    return Aws::Crt::Optional<JsonMessage>();
                }
            }
            /**
             * (Optional) A binary message.
             */
            void SetBinaryMessage(const BinaryMessage &binaryMessage) noexcept
            {
                m_binaryMessage = binaryMessage;
                m_chosenMember = TAG_BINARY_MESSAGE;
            }
            /**
             * (Optional) A binary message.
             */
            Aws::Crt::Optional<BinaryMessage> GetBinaryMessage() noexcept
            {
                if (m_chosenMember == TAG_BINARY_MESSAGE)
                {
                    return m_binaryMessage;
                }
                else
                {
                    return Aws::Crt::Optional<BinaryMessage>();
                }
            }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(PublishMessage &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(PublishMessage *) noexcept;
            /* This needs to be defined so that `PublishMessage` can be used as a key in maps. */
            bool operator<(const PublishMessage &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            enum ChosenMember
            {
                TAG_JSON_MESSAGE,
                TAG_BINARY_MESSAGE
            } m_chosenMember;
            Aws::Crt::Optional<JsonMessage> m_jsonMessage;
            Aws::Crt::Optional<BinaryMessage> m_binaryMessage;
        };

        class AWS_GREENGRASSCOREIPC_API SecretValue : public AbstractShapeBase
        {
          public:
            SecretValue() noexcept {}
            SecretValue &operator=(const SecretValue &) noexcept;
            SecretValue(const SecretValue &objectToCopy) { *this = objectToCopy; }
            /**
             * The decrypted part of the protected secret information that you provided to Secrets Manager as a string.
             */
            void SetSecretString(const Aws::Crt::String &secretString) noexcept
            {
                m_secretString = secretString;
                m_chosenMember = TAG_SECRET_STRING;
            }
            /**
             * The decrypted part of the protected secret information that you provided to Secrets Manager as a string.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetSecretString() noexcept
            {
                if (m_chosenMember == TAG_SECRET_STRING)
                {
                    return m_secretString;
                }
                else
                {
                    return Aws::Crt::Optional<Aws::Crt::String>();
                }
            }
            /**
             * (Optional) The decrypted part of the protected secret information that you provided to Secrets Manager as
             * binary data in the form of a byte array.
             */
            void SetSecretBinary(const Aws::Crt::Vector<uint8_t> &secretBinary) noexcept
            {
                m_secretBinary = secretBinary;
                m_chosenMember = TAG_SECRET_BINARY;
            }
            /**
             * (Optional) The decrypted part of the protected secret information that you provided to Secrets Manager as
             * binary data in the form of a byte array.
             */
            Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>> GetSecretBinary() noexcept
            {
                if (m_chosenMember == TAG_SECRET_BINARY)
                {
                    return m_secretBinary;
                }
                else
                {
                    return Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>>();
                }
            }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(SecretValue &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(SecretValue *) noexcept;
            /* This needs to be defined so that `SecretValue` can be used as a key in maps. */
            bool operator<(const SecretValue &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            enum ChosenMember
            {
                TAG_SECRET_STRING,
                TAG_SECRET_BINARY
            } m_chosenMember;
            Aws::Crt::Optional<Aws::Crt::String> m_secretString;
            Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>> m_secretBinary;
        };

        class AWS_GREENGRASSCOREIPC_API CredentialDocument : public AbstractShapeBase
        {
          public:
            CredentialDocument() noexcept {}
            CredentialDocument &operator=(const CredentialDocument &) noexcept;
            CredentialDocument(const CredentialDocument &objectToCopy) { *this = objectToCopy; }
            /**
             * The client device's MQTT credentials. Specify the client ID and certificate that the client device uses
             * to connect.
             */
            void SetMqttCredential(const MQTTCredential &mqttCredential) noexcept
            {
                m_mqttCredential = mqttCredential;
                m_chosenMember = TAG_MQTT_CREDENTIAL;
            }
            /**
             * The client device's MQTT credentials. Specify the client ID and certificate that the client device uses
             * to connect.
             */
            Aws::Crt::Optional<MQTTCredential> GetMqttCredential() noexcept
            {
                if (m_chosenMember == TAG_MQTT_CREDENTIAL)
                {
                    return m_mqttCredential;
                }
                else
                {
                    return Aws::Crt::Optional<MQTTCredential>();
                }
            }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(CredentialDocument &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(CredentialDocument *) noexcept;
            /* This needs to be defined so that `CredentialDocument` can be used as a key in maps. */
            bool operator<(const CredentialDocument &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            enum ChosenMember
            {
                TAG_MQTT_CREDENTIAL
            } m_chosenMember;
            Aws::Crt::Optional<MQTTCredential> m_mqttCredential;
        };

        enum FailureHandlingPolicy
        {
            FAILURE_HANDLING_POLICY_ROLLBACK,
            FAILURE_HANDLING_POLICY_DO_NOTHING
        };

        class AWS_GREENGRASSCOREIPC_API InvalidArgumentsError : public OperationError
        {
          public:
            InvalidArgumentsError() noexcept {}
            InvalidArgumentsError(const InvalidArgumentsError &) = default;

            void SetMessage(const Aws::Crt::String &message) noexcept { m_message = message; }

            Aws::Crt::Optional<Aws::Crt::String> GetMessage() noexcept override { return m_message; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(InvalidArgumentsError &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<OperationError> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(InvalidArgumentsError *) noexcept;
            /* This needs to be defined so that `InvalidArgumentsError` can be used as a key in maps. */
            bool operator<(const InvalidArgumentsError &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_message;
        };

        class AWS_GREENGRASSCOREIPC_API ServiceError : public OperationError
        {
          public:
            ServiceError() noexcept {}
            ServiceError(const ServiceError &) = default;

            void SetMessage(const Aws::Crt::String &message) noexcept { m_message = message; }

            Aws::Crt::Optional<Aws::Crt::String> GetMessage() noexcept override { return m_message; }

            void SetContext(const Aws::Crt::JsonObject &context) noexcept { m_context = context; }

            Aws::Crt::Optional<Aws::Crt::JsonObject> GetContext() noexcept { return m_context; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(ServiceError &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<OperationError> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(ServiceError *) noexcept;
            /* This needs to be defined so that `ServiceError` can be used as a key in maps. */
            bool operator<(const ServiceError &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_message;
            Aws::Crt::Optional<Aws::Crt::JsonObject> m_context;
        };

        class AWS_GREENGRASSCOREIPC_API UnauthorizedError : public OperationError
        {
          public:
            UnauthorizedError() noexcept {}
            UnauthorizedError(const UnauthorizedError &) = default;

            void SetMessage(const Aws::Crt::String &message) noexcept { m_message = message; }

            Aws::Crt::Optional<Aws::Crt::String> GetMessage() noexcept override { return m_message; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(UnauthorizedError &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<OperationError> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(UnauthorizedError *) noexcept;
            /* This needs to be defined so that `UnauthorizedError` can be used as a key in maps. */
            bool operator<(const UnauthorizedError &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_message;
        };

        class AWS_GREENGRASSCOREIPC_API VerifyClientDeviceIdentityResponse : public AbstractShapeBase
        {
          public:
            VerifyClientDeviceIdentityResponse() noexcept {}
            VerifyClientDeviceIdentityResponse(const VerifyClientDeviceIdentityResponse &) = default;
            /**
             * Whether the client device's identity is valid.
             */
            void SetIsValidClientDevice(const bool &isValidClientDevice) noexcept
            {
                m_isValidClientDevice = isValidClientDevice;
            }
            /**
             * Whether the client device's identity is valid.
             */
            Aws::Crt::Optional<bool> GetIsValidClientDevice() noexcept { return m_isValidClientDevice; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(VerifyClientDeviceIdentityResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(VerifyClientDeviceIdentityResponse *) noexcept;
            /* This needs to be defined so that `VerifyClientDeviceIdentityResponse` can be used as a key in maps. */
            bool operator<(const VerifyClientDeviceIdentityResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<bool> m_isValidClientDevice;
        };

        class AWS_GREENGRASSCOREIPC_API VerifyClientDeviceIdentityRequest : public AbstractShapeBase
        {
          public:
            VerifyClientDeviceIdentityRequest() noexcept {}
            VerifyClientDeviceIdentityRequest(const VerifyClientDeviceIdentityRequest &) = default;
            /**
             * The client device's credentials.
             */
            void SetCredential(const ClientDeviceCredential &credential) noexcept { m_credential = credential; }
            /**
             * The client device's credentials.
             */
            Aws::Crt::Optional<ClientDeviceCredential> GetCredential() noexcept { return m_credential; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(VerifyClientDeviceIdentityRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(VerifyClientDeviceIdentityRequest *) noexcept;
            /* This needs to be defined so that `VerifyClientDeviceIdentityRequest` can be used as a key in maps. */
            bool operator<(const VerifyClientDeviceIdentityRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<ClientDeviceCredential> m_credential;
        };

        class AWS_GREENGRASSCOREIPC_API InvalidTokenError : public OperationError
        {
          public:
            InvalidTokenError() noexcept {}
            InvalidTokenError(const InvalidTokenError &) = default;

            void SetMessage(const Aws::Crt::String &message) noexcept { m_message = message; }

            Aws::Crt::Optional<Aws::Crt::String> GetMessage() noexcept override { return m_message; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(InvalidTokenError &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<OperationError> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(InvalidTokenError *) noexcept;
            /* This needs to be defined so that `InvalidTokenError` can be used as a key in maps. */
            bool operator<(const InvalidTokenError &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_message;
        };

        class AWS_GREENGRASSCOREIPC_API ValidateAuthorizationTokenResponse : public AbstractShapeBase
        {
          public:
            ValidateAuthorizationTokenResponse() noexcept {}
            ValidateAuthorizationTokenResponse(const ValidateAuthorizationTokenResponse &) = default;

            void SetIsValid(const bool &isValid) noexcept { m_isValid = isValid; }

            Aws::Crt::Optional<bool> GetIsValid() noexcept { return m_isValid; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(ValidateAuthorizationTokenResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(ValidateAuthorizationTokenResponse *) noexcept;
            /* This needs to be defined so that `ValidateAuthorizationTokenResponse` can be used as a key in maps. */
            bool operator<(const ValidateAuthorizationTokenResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<bool> m_isValid;
        };

        class AWS_GREENGRASSCOREIPC_API ValidateAuthorizationTokenRequest : public AbstractShapeBase
        {
          public:
            ValidateAuthorizationTokenRequest() noexcept {}
            ValidateAuthorizationTokenRequest(const ValidateAuthorizationTokenRequest &) = default;

            void SetToken(const Aws::Crt::String &token) noexcept { m_token = token; }

            Aws::Crt::Optional<Aws::Crt::String> GetToken() noexcept { return m_token; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(ValidateAuthorizationTokenRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(ValidateAuthorizationTokenRequest *) noexcept;
            /* This needs to be defined so that `ValidateAuthorizationTokenRequest` can be used as a key in maps. */
            bool operator<(const ValidateAuthorizationTokenRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_token;
        };

        class AWS_GREENGRASSCOREIPC_API ConflictError : public OperationError
        {
          public:
            ConflictError() noexcept {}
            ConflictError(const ConflictError &) = default;

            void SetMessage(const Aws::Crt::String &message) noexcept { m_message = message; }

            Aws::Crt::Optional<Aws::Crt::String> GetMessage() noexcept override { return m_message; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(ConflictError &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<OperationError> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(ConflictError *) noexcept;
            /* This needs to be defined so that `ConflictError` can be used as a key in maps. */
            bool operator<(const ConflictError &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_message;
        };

        class AWS_GREENGRASSCOREIPC_API UpdateThingShadowResponse : public AbstractShapeBase
        {
          public:
            UpdateThingShadowResponse() noexcept {}
            UpdateThingShadowResponse(const UpdateThingShadowResponse &) = default;
            /**
             * The response state document as a JSON encoded blob.
             */
            void SetPayload(const Aws::Crt::Vector<uint8_t> &payload) noexcept { m_payload = payload; }
            /**
             * The response state document as a JSON encoded blob.
             */
            Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>> GetPayload() noexcept { return m_payload; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(UpdateThingShadowResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(UpdateThingShadowResponse *) noexcept;
            /* This needs to be defined so that `UpdateThingShadowResponse` can be used as a key in maps. */
            bool operator<(const UpdateThingShadowResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>> m_payload;
        };

        class AWS_GREENGRASSCOREIPC_API UpdateThingShadowRequest : public AbstractShapeBase
        {
          public:
            UpdateThingShadowRequest() noexcept {}
            UpdateThingShadowRequest(const UpdateThingShadowRequest &) = default;
            /**
             * The name of the thing.
             */
            void SetThingName(const Aws::Crt::String &thingName) noexcept { m_thingName = thingName; }
            /**
             * The name of the thing.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetThingName() noexcept { return m_thingName; }
            /**
             * The name of the shadow. To specify the thing's classic shadow, set this parameter to an empty string
             * ("").
             */
            void SetShadowName(const Aws::Crt::String &shadowName) noexcept { m_shadowName = shadowName; }
            /**
             * The name of the shadow. To specify the thing's classic shadow, set this parameter to an empty string
             * ("").
             */
            Aws::Crt::Optional<Aws::Crt::String> GetShadowName() noexcept { return m_shadowName; }
            /**
             * The request state document as a JSON encoded blob.
             */
            void SetPayload(const Aws::Crt::Vector<uint8_t> &payload) noexcept { m_payload = payload; }
            /**
             * The request state document as a JSON encoded blob.
             */
            Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>> GetPayload() noexcept { return m_payload; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(UpdateThingShadowRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(UpdateThingShadowRequest *) noexcept;
            /* This needs to be defined so that `UpdateThingShadowRequest` can be used as a key in maps. */
            bool operator<(const UpdateThingShadowRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_thingName;
            Aws::Crt::Optional<Aws::Crt::String> m_shadowName;
            Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>> m_payload;
        };

        class AWS_GREENGRASSCOREIPC_API ResourceNotFoundError : public OperationError
        {
          public:
            ResourceNotFoundError() noexcept {}
            ResourceNotFoundError(const ResourceNotFoundError &) = default;

            void SetMessage(const Aws::Crt::String &message) noexcept { m_message = message; }

            Aws::Crt::Optional<Aws::Crt::String> GetMessage() noexcept override { return m_message; }

            void SetResourceType(const Aws::Crt::String &resourceType) noexcept { m_resourceType = resourceType; }

            Aws::Crt::Optional<Aws::Crt::String> GetResourceType() noexcept { return m_resourceType; }

            void SetResourceName(const Aws::Crt::String &resourceName) noexcept { m_resourceName = resourceName; }

            Aws::Crt::Optional<Aws::Crt::String> GetResourceName() noexcept { return m_resourceName; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(ResourceNotFoundError &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<OperationError> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(ResourceNotFoundError *) noexcept;
            /* This needs to be defined so that `ResourceNotFoundError` can be used as a key in maps. */
            bool operator<(const ResourceNotFoundError &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_message;
            Aws::Crt::Optional<Aws::Crt::String> m_resourceType;
            Aws::Crt::Optional<Aws::Crt::String> m_resourceName;
        };

        class AWS_GREENGRASSCOREIPC_API UpdateStateResponse : public AbstractShapeBase
        {
          public:
            UpdateStateResponse() noexcept {}
            UpdateStateResponse(const UpdateStateResponse &) = default;
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(UpdateStateResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(UpdateStateResponse *) noexcept;
            /* This needs to be defined so that `UpdateStateResponse` can be used as a key in maps. */
            bool operator<(const UpdateStateResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
        };

        class AWS_GREENGRASSCOREIPC_API UpdateStateRequest : public AbstractShapeBase
        {
          public:
            UpdateStateRequest() noexcept {}
            UpdateStateRequest(const UpdateStateRequest &) = default;
            /**
             * The state to set this component to.
             */
            void SetState(ReportedLifecycleState state) noexcept;
            /**
             * The state to set this component to.
             */
            Aws::Crt::Optional<ReportedLifecycleState> GetState() noexcept;
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(UpdateStateRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(UpdateStateRequest *) noexcept;
            /* This needs to be defined so that `UpdateStateRequest` can be used as a key in maps. */
            bool operator<(const UpdateStateRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_state;
        };

        class AWS_GREENGRASSCOREIPC_API FailedUpdateConditionCheckError : public OperationError
        {
          public:
            FailedUpdateConditionCheckError() noexcept {}
            FailedUpdateConditionCheckError(const FailedUpdateConditionCheckError &) = default;

            void SetMessage(const Aws::Crt::String &message) noexcept { m_message = message; }

            Aws::Crt::Optional<Aws::Crt::String> GetMessage() noexcept override { return m_message; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(FailedUpdateConditionCheckError &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<OperationError> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(FailedUpdateConditionCheckError *) noexcept;
            /* This needs to be defined so that `FailedUpdateConditionCheckError` can be used as a key in maps. */
            bool operator<(const FailedUpdateConditionCheckError &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_message;
        };

        class AWS_GREENGRASSCOREIPC_API UpdateConfigurationResponse : public AbstractShapeBase
        {
          public:
            UpdateConfigurationResponse() noexcept {}
            UpdateConfigurationResponse(const UpdateConfigurationResponse &) = default;
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(UpdateConfigurationResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(UpdateConfigurationResponse *) noexcept;
            /* This needs to be defined so that `UpdateConfigurationResponse` can be used as a key in maps. */
            bool operator<(const UpdateConfigurationResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
        };

        class AWS_GREENGRASSCOREIPC_API UpdateConfigurationRequest : public AbstractShapeBase
        {
          public:
            UpdateConfigurationRequest() noexcept {}
            UpdateConfigurationRequest(const UpdateConfigurationRequest &) = default;
            /**
             * (Optional) The key path to the container node (the object) to update. Specify a list where each entry is
             * the key for a single level in the configuration object. Defaults to the root of the configuration object.
             */
            void SetKeyPath(const Aws::Crt::Vector<Aws::Crt::String> &keyPath) noexcept { m_keyPath = keyPath; }
            /**
             * (Optional) The key path to the container node (the object) to update. Specify a list where each entry is
             * the key for a single level in the configuration object. Defaults to the root of the configuration object.
             */
            Aws::Crt::Optional<Aws::Crt::Vector<Aws::Crt::String>> GetKeyPath() noexcept { return m_keyPath; }
            /**
             * The current Unix epoch time in milliseconds. This operation uses this timestamp to resolve concurrent
             * updates to the key. If the key in the component configuration has a greater timestamp than the timestamp
             * in the request, then the request fails.
             */
            void SetTimestamp(const Aws::Crt::DateTime &timestamp) noexcept { m_timestamp = timestamp; }
            /**
             * The current Unix epoch time in milliseconds. This operation uses this timestamp to resolve concurrent
             * updates to the key. If the key in the component configuration has a greater timestamp than the timestamp
             * in the request, then the request fails.
             */
            Aws::Crt::Optional<Aws::Crt::DateTime> GetTimestamp() noexcept { return m_timestamp; }
            /**
             * The configuration object to merge at the location that you specify in keyPath.
             */
            void SetValueToMerge(const Aws::Crt::JsonObject &valueToMerge) noexcept { m_valueToMerge = valueToMerge; }
            /**
             * The configuration object to merge at the location that you specify in keyPath.
             */
            Aws::Crt::Optional<Aws::Crt::JsonObject> GetValueToMerge() noexcept { return m_valueToMerge; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(UpdateConfigurationRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(UpdateConfigurationRequest *) noexcept;
            /* This needs to be defined so that `UpdateConfigurationRequest` can be used as a key in maps. */
            bool operator<(const UpdateConfigurationRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::Vector<Aws::Crt::String>> m_keyPath;
            Aws::Crt::Optional<Aws::Crt::DateTime> m_timestamp;
            Aws::Crt::Optional<Aws::Crt::JsonObject> m_valueToMerge;
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToValidateConfigurationUpdatesResponse : public AbstractShapeBase
        {
          public:
            SubscribeToValidateConfigurationUpdatesResponse() noexcept {}
            SubscribeToValidateConfigurationUpdatesResponse(const SubscribeToValidateConfigurationUpdatesResponse &) =
                default;
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(
                SubscribeToValidateConfigurationUpdatesResponse &,
                const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(SubscribeToValidateConfigurationUpdatesResponse *) noexcept;
            /* This needs to be defined so that `SubscribeToValidateConfigurationUpdatesResponse` can be used as a key
             * in maps. */
            bool operator<(const SubscribeToValidateConfigurationUpdatesResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToValidateConfigurationUpdatesRequest : public AbstractShapeBase
        {
          public:
            SubscribeToValidateConfigurationUpdatesRequest() noexcept {}
            SubscribeToValidateConfigurationUpdatesRequest(const SubscribeToValidateConfigurationUpdatesRequest &) =
                default;
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(
                SubscribeToValidateConfigurationUpdatesRequest &,
                const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(SubscribeToValidateConfigurationUpdatesRequest *) noexcept;
            /* This needs to be defined so that `SubscribeToValidateConfigurationUpdatesRequest` can be used as a key in
             * maps. */
            bool operator<(const SubscribeToValidateConfigurationUpdatesRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToTopicResponse : public AbstractShapeBase
        {
          public:
            SubscribeToTopicResponse() noexcept {}
            SubscribeToTopicResponse(const SubscribeToTopicResponse &) = default;
            /**
             * @deprecated No longer used
             */
            void SetTopicName(const Aws::Crt::String &topicName) noexcept { m_topicName = topicName; }
            /**
             * @deprecated No longer used
             */
            Aws::Crt::Optional<Aws::Crt::String> GetTopicName() noexcept { return m_topicName; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(SubscribeToTopicResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(SubscribeToTopicResponse *) noexcept;
            /* This needs to be defined so that `SubscribeToTopicResponse` can be used as a key in maps. */
            bool operator<(const SubscribeToTopicResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_topicName;
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToTopicRequest : public AbstractShapeBase
        {
          public:
            SubscribeToTopicRequest() noexcept {}
            SubscribeToTopicRequest(const SubscribeToTopicRequest &) = default;
            /**
             * The topic to subscribe to. Supports MQTT-style wildcards.
             */
            void SetTopic(const Aws::Crt::String &topic) noexcept { m_topic = topic; }
            /**
             * The topic to subscribe to. Supports MQTT-style wildcards.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetTopic() noexcept { return m_topic; }
            /**
             * (Optional) The behavior that specifies whether the component receives messages from itself.
             */
            void SetReceiveMode(ReceiveMode receiveMode) noexcept;
            /**
             * (Optional) The behavior that specifies whether the component receives messages from itself.
             */
            Aws::Crt::Optional<ReceiveMode> GetReceiveMode() noexcept;
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(SubscribeToTopicRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(SubscribeToTopicRequest *) noexcept;
            /* This needs to be defined so that `SubscribeToTopicRequest` can be used as a key in maps. */
            bool operator<(const SubscribeToTopicRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_topic;
            Aws::Crt::Optional<Aws::Crt::String> m_receiveMode;
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToIoTCoreResponse : public AbstractShapeBase
        {
          public:
            SubscribeToIoTCoreResponse() noexcept {}
            SubscribeToIoTCoreResponse(const SubscribeToIoTCoreResponse &) = default;
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(SubscribeToIoTCoreResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(SubscribeToIoTCoreResponse *) noexcept;
            /* This needs to be defined so that `SubscribeToIoTCoreResponse` can be used as a key in maps. */
            bool operator<(const SubscribeToIoTCoreResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToIoTCoreRequest : public AbstractShapeBase
        {
          public:
            SubscribeToIoTCoreRequest() noexcept {}
            SubscribeToIoTCoreRequest(const SubscribeToIoTCoreRequest &) = default;
            /**
             * The topic to which to subscribe. Supports MQTT wildcards.
             */
            void SetTopicName(const Aws::Crt::String &topicName) noexcept { m_topicName = topicName; }
            /**
             * The topic to which to subscribe. Supports MQTT wildcards.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetTopicName() noexcept { return m_topicName; }
            /**
             * The MQTT QoS to use.
             */
            void SetQos(QOS qos) noexcept;
            /**
             * The MQTT QoS to use.
             */
            Aws::Crt::Optional<QOS> GetQos() noexcept;
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(SubscribeToIoTCoreRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(SubscribeToIoTCoreRequest *) noexcept;
            /* This needs to be defined so that `SubscribeToIoTCoreRequest` can be used as a key in maps. */
            bool operator<(const SubscribeToIoTCoreRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_topicName;
            Aws::Crt::Optional<Aws::Crt::String> m_qos;
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToConfigurationUpdateResponse : public AbstractShapeBase
        {
          public:
            SubscribeToConfigurationUpdateResponse() noexcept {}
            SubscribeToConfigurationUpdateResponse(const SubscribeToConfigurationUpdateResponse &) = default;
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(
                SubscribeToConfigurationUpdateResponse &,
                const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(SubscribeToConfigurationUpdateResponse *) noexcept;
            /* This needs to be defined so that `SubscribeToConfigurationUpdateResponse` can be used as a key in maps.
             */
            bool operator<(const SubscribeToConfigurationUpdateResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToConfigurationUpdateRequest : public AbstractShapeBase
        {
          public:
            SubscribeToConfigurationUpdateRequest() noexcept {}
            SubscribeToConfigurationUpdateRequest(const SubscribeToConfigurationUpdateRequest &) = default;
            /**
             * (Optional) The name of the component. Defaults to the name of the component that makes the request.
             */
            void SetComponentName(const Aws::Crt::String &componentName) noexcept { m_componentName = componentName; }
            /**
             * (Optional) The name of the component. Defaults to the name of the component that makes the request.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetComponentName() noexcept { return m_componentName; }
            /**
             * The key path to the configuration value for which to subscribe. Specify a list where each entry is the
             * key for a single level in the configuration object.
             */
            void SetKeyPath(const Aws::Crt::Vector<Aws::Crt::String> &keyPath) noexcept { m_keyPath = keyPath; }
            /**
             * The key path to the configuration value for which to subscribe. Specify a list where each entry is the
             * key for a single level in the configuration object.
             */
            Aws::Crt::Optional<Aws::Crt::Vector<Aws::Crt::String>> GetKeyPath() noexcept { return m_keyPath; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(
                SubscribeToConfigurationUpdateRequest &,
                const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(SubscribeToConfigurationUpdateRequest *) noexcept;
            /* This needs to be defined so that `SubscribeToConfigurationUpdateRequest` can be used as a key in maps. */
            bool operator<(const SubscribeToConfigurationUpdateRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_componentName;
            Aws::Crt::Optional<Aws::Crt::Vector<Aws::Crt::String>> m_keyPath;
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToComponentUpdatesResponse : public AbstractShapeBase
        {
          public:
            SubscribeToComponentUpdatesResponse() noexcept {}
            SubscribeToComponentUpdatesResponse(const SubscribeToComponentUpdatesResponse &) = default;
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(SubscribeToComponentUpdatesResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(SubscribeToComponentUpdatesResponse *) noexcept;
            /* This needs to be defined so that `SubscribeToComponentUpdatesResponse` can be used as a key in maps. */
            bool operator<(const SubscribeToComponentUpdatesResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToComponentUpdatesRequest : public AbstractShapeBase
        {
          public:
            SubscribeToComponentUpdatesRequest() noexcept {}
            SubscribeToComponentUpdatesRequest(const SubscribeToComponentUpdatesRequest &) = default;
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(SubscribeToComponentUpdatesRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(SubscribeToComponentUpdatesRequest *) noexcept;
            /* This needs to be defined so that `SubscribeToComponentUpdatesRequest` can be used as a key in maps. */
            bool operator<(const SubscribeToComponentUpdatesRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToCertificateUpdatesResponse : public AbstractShapeBase
        {
          public:
            SubscribeToCertificateUpdatesResponse() noexcept {}
            SubscribeToCertificateUpdatesResponse(const SubscribeToCertificateUpdatesResponse &) = default;
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(
                SubscribeToCertificateUpdatesResponse &,
                const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(SubscribeToCertificateUpdatesResponse *) noexcept;
            /* This needs to be defined so that `SubscribeToCertificateUpdatesResponse` can be used as a key in maps. */
            bool operator<(const SubscribeToCertificateUpdatesResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToCertificateUpdatesRequest : public AbstractShapeBase
        {
          public:
            SubscribeToCertificateUpdatesRequest() noexcept {}
            SubscribeToCertificateUpdatesRequest(const SubscribeToCertificateUpdatesRequest &) = default;

            void SetCertificateOptions(const CertificateOptions &certificateOptions) noexcept
            {
                m_certificateOptions = certificateOptions;
            }

            Aws::Crt::Optional<CertificateOptions> GetCertificateOptions() noexcept { return m_certificateOptions; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(SubscribeToCertificateUpdatesRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(SubscribeToCertificateUpdatesRequest *) noexcept;
            /* This needs to be defined so that `SubscribeToCertificateUpdatesRequest` can be used as a key in maps. */
            bool operator<(const SubscribeToCertificateUpdatesRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<CertificateOptions> m_certificateOptions;
        };

        class AWS_GREENGRASSCOREIPC_API ComponentNotFoundError : public OperationError
        {
          public:
            ComponentNotFoundError() noexcept {}
            ComponentNotFoundError(const ComponentNotFoundError &) = default;

            void SetMessage(const Aws::Crt::String &message) noexcept { m_message = message; }

            Aws::Crt::Optional<Aws::Crt::String> GetMessage() noexcept override { return m_message; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(ComponentNotFoundError &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<OperationError> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(ComponentNotFoundError *) noexcept;
            /* This needs to be defined so that `ComponentNotFoundError` can be used as a key in maps. */
            bool operator<(const ComponentNotFoundError &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_message;
        };

        class AWS_GREENGRASSCOREIPC_API StopComponentResponse : public AbstractShapeBase
        {
          public:
            StopComponentResponse() noexcept {}
            StopComponentResponse(const StopComponentResponse &) = default;
            /**
             * The status of the stop request.
             */
            void SetStopStatus(RequestStatus stopStatus) noexcept;
            /**
             * The status of the stop request.
             */
            Aws::Crt::Optional<RequestStatus> GetStopStatus() noexcept;
            /**
             * A message about why the component failed to stop, if the request failed.
             */
            void SetMessage(const Aws::Crt::String &message) noexcept { m_message = message; }
            /**
             * A message about why the component failed to stop, if the request failed.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetMessage() noexcept { return m_message; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(StopComponentResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(StopComponentResponse *) noexcept;
            /* This needs to be defined so that `StopComponentResponse` can be used as a key in maps. */
            bool operator<(const StopComponentResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_stopStatus;
            Aws::Crt::Optional<Aws::Crt::String> m_message;
        };

        class AWS_GREENGRASSCOREIPC_API StopComponentRequest : public AbstractShapeBase
        {
          public:
            StopComponentRequest() noexcept {}
            StopComponentRequest(const StopComponentRequest &) = default;
            /**
             * The name of the component.
             */
            void SetComponentName(const Aws::Crt::String &componentName) noexcept { m_componentName = componentName; }
            /**
             * The name of the component.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetComponentName() noexcept { return m_componentName; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(StopComponentRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(StopComponentRequest *) noexcept;
            /* This needs to be defined so that `StopComponentRequest` can be used as a key in maps. */
            bool operator<(const StopComponentRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_componentName;
        };

        class AWS_GREENGRASSCOREIPC_API SendConfigurationValidityReportResponse : public AbstractShapeBase
        {
          public:
            SendConfigurationValidityReportResponse() noexcept {}
            SendConfigurationValidityReportResponse(const SendConfigurationValidityReportResponse &) = default;
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(
                SendConfigurationValidityReportResponse &,
                const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(SendConfigurationValidityReportResponse *) noexcept;
            /* This needs to be defined so that `SendConfigurationValidityReportResponse` can be used as a key in maps.
             */
            bool operator<(const SendConfigurationValidityReportResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
        };

        class AWS_GREENGRASSCOREIPC_API SendConfigurationValidityReportRequest : public AbstractShapeBase
        {
          public:
            SendConfigurationValidityReportRequest() noexcept {}
            SendConfigurationValidityReportRequest(const SendConfigurationValidityReportRequest &) = default;
            /**
             * The report that tells Greengrass whether or not the configuration update is valid.
             */
            void SetConfigurationValidityReport(const ConfigurationValidityReport &configurationValidityReport) noexcept
            {
                m_configurationValidityReport = configurationValidityReport;
            }
            /**
             * The report that tells Greengrass whether or not the configuration update is valid.
             */
            Aws::Crt::Optional<ConfigurationValidityReport> GetConfigurationValidityReport() noexcept
            {
                return m_configurationValidityReport;
            }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(
                SendConfigurationValidityReportRequest &,
                const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(SendConfigurationValidityReportRequest *) noexcept;
            /* This needs to be defined so that `SendConfigurationValidityReportRequest` can be used as a key in maps.
             */
            bool operator<(const SendConfigurationValidityReportRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<ConfigurationValidityReport> m_configurationValidityReport;
        };

        class AWS_GREENGRASSCOREIPC_API ResumeComponentResponse : public AbstractShapeBase
        {
          public:
            ResumeComponentResponse() noexcept {}
            ResumeComponentResponse(const ResumeComponentResponse &) = default;
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(ResumeComponentResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(ResumeComponentResponse *) noexcept;
            /* This needs to be defined so that `ResumeComponentResponse` can be used as a key in maps. */
            bool operator<(const ResumeComponentResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
        };

        class AWS_GREENGRASSCOREIPC_API ResumeComponentRequest : public AbstractShapeBase
        {
          public:
            ResumeComponentRequest() noexcept {}
            ResumeComponentRequest(const ResumeComponentRequest &) = default;
            /**
             * The name of the component to resume.
             */
            void SetComponentName(const Aws::Crt::String &componentName) noexcept { m_componentName = componentName; }
            /**
             * The name of the component to resume.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetComponentName() noexcept { return m_componentName; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(ResumeComponentRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(ResumeComponentRequest *) noexcept;
            /* This needs to be defined so that `ResumeComponentRequest` can be used as a key in maps. */
            bool operator<(const ResumeComponentRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_componentName;
        };

        class AWS_GREENGRASSCOREIPC_API RestartComponentResponse : public AbstractShapeBase
        {
          public:
            RestartComponentResponse() noexcept {}
            RestartComponentResponse(const RestartComponentResponse &) = default;
            /**
             * The status of the restart request.
             */
            void SetRestartStatus(RequestStatus restartStatus) noexcept;
            /**
             * The status of the restart request.
             */
            Aws::Crt::Optional<RequestStatus> GetRestartStatus() noexcept;
            /**
             * A message about why the component failed to restart, if the request failed.
             */
            void SetMessage(const Aws::Crt::String &message) noexcept { m_message = message; }
            /**
             * A message about why the component failed to restart, if the request failed.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetMessage() noexcept { return m_message; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(RestartComponentResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(RestartComponentResponse *) noexcept;
            /* This needs to be defined so that `RestartComponentResponse` can be used as a key in maps. */
            bool operator<(const RestartComponentResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_restartStatus;
            Aws::Crt::Optional<Aws::Crt::String> m_message;
        };

        class AWS_GREENGRASSCOREIPC_API RestartComponentRequest : public AbstractShapeBase
        {
          public:
            RestartComponentRequest() noexcept {}
            RestartComponentRequest(const RestartComponentRequest &) = default;
            /**
             * The name of the component.
             */
            void SetComponentName(const Aws::Crt::String &componentName) noexcept { m_componentName = componentName; }
            /**
             * The name of the component.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetComponentName() noexcept { return m_componentName; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(RestartComponentRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(RestartComponentRequest *) noexcept;
            /* This needs to be defined so that `RestartComponentRequest` can be used as a key in maps. */
            bool operator<(const RestartComponentRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_componentName;
        };

        class AWS_GREENGRASSCOREIPC_API PutComponentMetricResponse : public AbstractShapeBase
        {
          public:
            PutComponentMetricResponse() noexcept {}
            PutComponentMetricResponse(const PutComponentMetricResponse &) = default;
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(PutComponentMetricResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(PutComponentMetricResponse *) noexcept;
            /* This needs to be defined so that `PutComponentMetricResponse` can be used as a key in maps. */
            bool operator<(const PutComponentMetricResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
        };

        class AWS_GREENGRASSCOREIPC_API PutComponentMetricRequest : public AbstractShapeBase
        {
          public:
            PutComponentMetricRequest() noexcept {}
            PutComponentMetricRequest(const PutComponentMetricRequest &) = default;

            void SetMetrics(const Aws::Crt::Vector<Metric> &metrics) noexcept { m_metrics = metrics; }

            Aws::Crt::Optional<Aws::Crt::Vector<Metric>> GetMetrics() noexcept { return m_metrics; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(PutComponentMetricRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(PutComponentMetricRequest *) noexcept;
            /* This needs to be defined so that `PutComponentMetricRequest` can be used as a key in maps. */
            bool operator<(const PutComponentMetricRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::Vector<Metric>> m_metrics;
        };

        class AWS_GREENGRASSCOREIPC_API PublishToTopicResponse : public AbstractShapeBase
        {
          public:
            PublishToTopicResponse() noexcept {}
            PublishToTopicResponse(const PublishToTopicResponse &) = default;
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(PublishToTopicResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(PublishToTopicResponse *) noexcept;
            /* This needs to be defined so that `PublishToTopicResponse` can be used as a key in maps. */
            bool operator<(const PublishToTopicResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
        };

        class AWS_GREENGRASSCOREIPC_API PublishToTopicRequest : public AbstractShapeBase
        {
          public:
            PublishToTopicRequest() noexcept {}
            PublishToTopicRequest(const PublishToTopicRequest &) = default;
            /**
             * The topic to publish the message.
             */
            void SetTopic(const Aws::Crt::String &topic) noexcept { m_topic = topic; }
            /**
             * The topic to publish the message.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetTopic() noexcept { return m_topic; }
            /**
             * The message to publish.
             */
            void SetPublishMessage(const PublishMessage &publishMessage) noexcept { m_publishMessage = publishMessage; }
            /**
             * The message to publish.
             */
            Aws::Crt::Optional<PublishMessage> GetPublishMessage() noexcept { return m_publishMessage; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(PublishToTopicRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(PublishToTopicRequest *) noexcept;
            /* This needs to be defined so that `PublishToTopicRequest` can be used as a key in maps. */
            bool operator<(const PublishToTopicRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_topic;
            Aws::Crt::Optional<PublishMessage> m_publishMessage;
        };

        class AWS_GREENGRASSCOREIPC_API PublishToIoTCoreResponse : public AbstractShapeBase
        {
          public:
            PublishToIoTCoreResponse() noexcept {}
            PublishToIoTCoreResponse(const PublishToIoTCoreResponse &) = default;
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(PublishToIoTCoreResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(PublishToIoTCoreResponse *) noexcept;
            /* This needs to be defined so that `PublishToIoTCoreResponse` can be used as a key in maps. */
            bool operator<(const PublishToIoTCoreResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
        };

        class AWS_GREENGRASSCOREIPC_API PublishToIoTCoreRequest : public AbstractShapeBase
        {
          public:
            PublishToIoTCoreRequest() noexcept {}
            PublishToIoTCoreRequest(const PublishToIoTCoreRequest &) = default;
            /**
             * The topic to which to publish the message.
             */
            void SetTopicName(const Aws::Crt::String &topicName) noexcept { m_topicName = topicName; }
            /**
             * The topic to which to publish the message.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetTopicName() noexcept { return m_topicName; }
            /**
             * The MQTT QoS to use.
             */
            void SetQos(QOS qos) noexcept;
            /**
             * The MQTT QoS to use.
             */
            Aws::Crt::Optional<QOS> GetQos() noexcept;
            /**
             * (Optional) The message payload as a blob.
             */
            void SetPayload(const Aws::Crt::Vector<uint8_t> &payload) noexcept { m_payload = payload; }
            /**
             * (Optional) The message payload as a blob.
             */
            Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>> GetPayload() noexcept { return m_payload; }
            /**
             * (Optional) Whether to set MQTT retain option to true when publishing.
             */
            void SetRetain(const bool &retain) noexcept { m_retain = retain; }
            /**
             * (Optional) Whether to set MQTT retain option to true when publishing.
             */
            Aws::Crt::Optional<bool> GetRetain() noexcept { return m_retain; }
            /**
             * (Optional) MQTT user properties associated with the message.
             */
            void SetUserProperties(const Aws::Crt::Vector<UserProperty> &userProperties) noexcept
            {
                m_userProperties = userProperties;
            }
            /**
             * (Optional) MQTT user properties associated with the message.
             */
            Aws::Crt::Optional<Aws::Crt::Vector<UserProperty>> GetUserProperties() noexcept { return m_userProperties; }
            /**
             * (Optional) Message expiry interval in seconds.
             */
            void SetMessageExpiryIntervalSeconds(const int64_t &messageExpiryIntervalSeconds) noexcept
            {
                m_messageExpiryIntervalSeconds = messageExpiryIntervalSeconds;
            }
            /**
             * (Optional) Message expiry interval in seconds.
             */
            Aws::Crt::Optional<int64_t> GetMessageExpiryIntervalSeconds() noexcept
            {
                return m_messageExpiryIntervalSeconds;
            }
            /**
             * (Optional) Correlation data blob for request/response.
             */
            void SetCorrelationData(const Aws::Crt::Vector<uint8_t> &correlationData) noexcept
            {
                m_correlationData = correlationData;
            }
            /**
             * (Optional) Correlation data blob for request/response.
             */
            Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>> GetCorrelationData() noexcept { return m_correlationData; }
            /**
             * (Optional) Response topic for request/response.
             */
            void SetResponseTopic(const Aws::Crt::String &responseTopic) noexcept { m_responseTopic = responseTopic; }
            /**
             * (Optional) Response topic for request/response.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetResponseTopic() noexcept { return m_responseTopic; }
            /**
             * (Optional) Message payload format.
             */
            void SetPayloadFormat(PayloadFormat payloadFormat) noexcept;
            /**
             * (Optional) Message payload format.
             */
            Aws::Crt::Optional<PayloadFormat> GetPayloadFormat() noexcept;
            /**
             * (Optional) Message content type.
             */
            void SetContentType(const Aws::Crt::String &contentType) noexcept { m_contentType = contentType; }
            /**
             * (Optional) Message content type.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetContentType() noexcept { return m_contentType; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(PublishToIoTCoreRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(PublishToIoTCoreRequest *) noexcept;
            /* This needs to be defined so that `PublishToIoTCoreRequest` can be used as a key in maps. */
            bool operator<(const PublishToIoTCoreRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_topicName;
            Aws::Crt::Optional<Aws::Crt::String> m_qos;
            Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>> m_payload;
            Aws::Crt::Optional<bool> m_retain;
            Aws::Crt::Optional<Aws::Crt::Vector<UserProperty>> m_userProperties;
            Aws::Crt::Optional<int64_t> m_messageExpiryIntervalSeconds;
            Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>> m_correlationData;
            Aws::Crt::Optional<Aws::Crt::String> m_responseTopic;
            Aws::Crt::Optional<Aws::Crt::String> m_payloadFormat;
            Aws::Crt::Optional<Aws::Crt::String> m_contentType;
        };

        class AWS_GREENGRASSCOREIPC_API PauseComponentResponse : public AbstractShapeBase
        {
          public:
            PauseComponentResponse() noexcept {}
            PauseComponentResponse(const PauseComponentResponse &) = default;
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(PauseComponentResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(PauseComponentResponse *) noexcept;
            /* This needs to be defined so that `PauseComponentResponse` can be used as a key in maps. */
            bool operator<(const PauseComponentResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
        };

        class AWS_GREENGRASSCOREIPC_API PauseComponentRequest : public AbstractShapeBase
        {
          public:
            PauseComponentRequest() noexcept {}
            PauseComponentRequest(const PauseComponentRequest &) = default;
            /**
             * The name of the component to pause, which must be a generic component.
             */
            void SetComponentName(const Aws::Crt::String &componentName) noexcept { m_componentName = componentName; }
            /**
             * The name of the component to pause, which must be a generic component.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetComponentName() noexcept { return m_componentName; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(PauseComponentRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(PauseComponentRequest *) noexcept;
            /* This needs to be defined so that `PauseComponentRequest` can be used as a key in maps. */
            bool operator<(const PauseComponentRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_componentName;
        };

        class AWS_GREENGRASSCOREIPC_API ListNamedShadowsForThingResponse : public AbstractShapeBase
        {
          public:
            ListNamedShadowsForThingResponse() noexcept {}
            ListNamedShadowsForThingResponse(const ListNamedShadowsForThingResponse &) = default;
            /**
             * The list of shadow names.
             */
            void SetResults(const Aws::Crt::Vector<Aws::Crt::String> &results) noexcept { m_results = results; }
            /**
             * The list of shadow names.
             */
            Aws::Crt::Optional<Aws::Crt::Vector<Aws::Crt::String>> GetResults() noexcept { return m_results; }
            /**
             * (Optional) The date and time that the response was generated.
             */
            void SetTimestamp(const Aws::Crt::DateTime &timestamp) noexcept { m_timestamp = timestamp; }
            /**
             * (Optional) The date and time that the response was generated.
             */
            Aws::Crt::Optional<Aws::Crt::DateTime> GetTimestamp() noexcept { return m_timestamp; }
            /**
             * (Optional) The token value to use in paged requests to retrieve the next page in the sequence. This token
             * isn't present when there are no more shadow names to return.
             */
            void SetNextToken(const Aws::Crt::String &nextToken) noexcept { m_nextToken = nextToken; }
            /**
             * (Optional) The token value to use in paged requests to retrieve the next page in the sequence. This token
             * isn't present when there are no more shadow names to return.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetNextToken() noexcept { return m_nextToken; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(ListNamedShadowsForThingResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(ListNamedShadowsForThingResponse *) noexcept;
            /* This needs to be defined so that `ListNamedShadowsForThingResponse` can be used as a key in maps. */
            bool operator<(const ListNamedShadowsForThingResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::Vector<Aws::Crt::String>> m_results;
            Aws::Crt::Optional<Aws::Crt::DateTime> m_timestamp;
            Aws::Crt::Optional<Aws::Crt::String> m_nextToken;
        };

        class AWS_GREENGRASSCOREIPC_API ListNamedShadowsForThingRequest : public AbstractShapeBase
        {
          public:
            ListNamedShadowsForThingRequest() noexcept {}
            ListNamedShadowsForThingRequest(const ListNamedShadowsForThingRequest &) = default;
            /**
             * The name of the thing.
             */
            void SetThingName(const Aws::Crt::String &thingName) noexcept { m_thingName = thingName; }
            /**
             * The name of the thing.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetThingName() noexcept { return m_thingName; }
            /**
             * (Optional) The token to retrieve the next set of results. This value is returned on paged results and is
             * used in the call that returns the next page.
             */
            void SetNextToken(const Aws::Crt::String &nextToken) noexcept { m_nextToken = nextToken; }
            /**
             * (Optional) The token to retrieve the next set of results. This value is returned on paged results and is
             * used in the call that returns the next page.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetNextToken() noexcept { return m_nextToken; }
            /**
             * (Optional) The number of shadow names to return in each call. Value must be between 1 and 100. Default
             * is 25.
             */
            void SetPageSize(const int &pageSize) noexcept { m_pageSize = pageSize; }
            /**
             * (Optional) The number of shadow names to return in each call. Value must be between 1 and 100. Default
             * is 25.
             */
            Aws::Crt::Optional<int> GetPageSize() noexcept { return m_pageSize; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(ListNamedShadowsForThingRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(ListNamedShadowsForThingRequest *) noexcept;
            /* This needs to be defined so that `ListNamedShadowsForThingRequest` can be used as a key in maps. */
            bool operator<(const ListNamedShadowsForThingRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_thingName;
            Aws::Crt::Optional<Aws::Crt::String> m_nextToken;
            Aws::Crt::Optional<int> m_pageSize;
        };

        class AWS_GREENGRASSCOREIPC_API ListLocalDeploymentsResponse : public AbstractShapeBase
        {
          public:
            ListLocalDeploymentsResponse() noexcept {}
            ListLocalDeploymentsResponse(const ListLocalDeploymentsResponse &) = default;
            /**
             * The list of local deployments.
             */
            void SetLocalDeployments(const Aws::Crt::Vector<LocalDeployment> &localDeployments) noexcept
            {
                m_localDeployments = localDeployments;
            }
            /**
             * The list of local deployments.
             */
            Aws::Crt::Optional<Aws::Crt::Vector<LocalDeployment>> GetLocalDeployments() noexcept
            {
                return m_localDeployments;
            }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(ListLocalDeploymentsResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(ListLocalDeploymentsResponse *) noexcept;
            /* This needs to be defined so that `ListLocalDeploymentsResponse` can be used as a key in maps. */
            bool operator<(const ListLocalDeploymentsResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::Vector<LocalDeployment>> m_localDeployments;
        };

        class AWS_GREENGRASSCOREIPC_API ListLocalDeploymentsRequest : public AbstractShapeBase
        {
          public:
            ListLocalDeploymentsRequest() noexcept {}
            ListLocalDeploymentsRequest(const ListLocalDeploymentsRequest &) = default;
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(ListLocalDeploymentsRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(ListLocalDeploymentsRequest *) noexcept;
            /* This needs to be defined so that `ListLocalDeploymentsRequest` can be used as a key in maps. */
            bool operator<(const ListLocalDeploymentsRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
        };

        class AWS_GREENGRASSCOREIPC_API ListComponentsResponse : public AbstractShapeBase
        {
          public:
            ListComponentsResponse() noexcept {}
            ListComponentsResponse(const ListComponentsResponse &) = default;
            /**
             * The list of components.
             */
            void SetComponents(const Aws::Crt::Vector<ComponentDetails> &components) noexcept
            {
                m_components = components;
            }
            /**
             * The list of components.
             */
            Aws::Crt::Optional<Aws::Crt::Vector<ComponentDetails>> GetComponents() noexcept { return m_components; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(ListComponentsResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(ListComponentsResponse *) noexcept;
            /* This needs to be defined so that `ListComponentsResponse` can be used as a key in maps. */
            bool operator<(const ListComponentsResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::Vector<ComponentDetails>> m_components;
        };

        class AWS_GREENGRASSCOREIPC_API ListComponentsRequest : public AbstractShapeBase
        {
          public:
            ListComponentsRequest() noexcept {}
            ListComponentsRequest(const ListComponentsRequest &) = default;
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(ListComponentsRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(ListComponentsRequest *) noexcept;
            /* This needs to be defined so that `ListComponentsRequest` can be used as a key in maps. */
            bool operator<(const ListComponentsRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
        };

        class AWS_GREENGRASSCOREIPC_API GetThingShadowResponse : public AbstractShapeBase
        {
          public:
            GetThingShadowResponse() noexcept {}
            GetThingShadowResponse(const GetThingShadowResponse &) = default;
            /**
             * The response state document as a JSON encoded blob.
             */
            void SetPayload(const Aws::Crt::Vector<uint8_t> &payload) noexcept { m_payload = payload; }
            /**
             * The response state document as a JSON encoded blob.
             */
            Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>> GetPayload() noexcept { return m_payload; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(GetThingShadowResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(GetThingShadowResponse *) noexcept;
            /* This needs to be defined so that `GetThingShadowResponse` can be used as a key in maps. */
            bool operator<(const GetThingShadowResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>> m_payload;
        };

        class AWS_GREENGRASSCOREIPC_API GetThingShadowRequest : public AbstractShapeBase
        {
          public:
            GetThingShadowRequest() noexcept {}
            GetThingShadowRequest(const GetThingShadowRequest &) = default;
            /**
             * The name of the thing.
             */
            void SetThingName(const Aws::Crt::String &thingName) noexcept { m_thingName = thingName; }
            /**
             * The name of the thing.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetThingName() noexcept { return m_thingName; }
            /**
             * The name of the shadow. To specify the thing's classic shadow, set this parameter to an empty string
             * ("").
             */
            void SetShadowName(const Aws::Crt::String &shadowName) noexcept { m_shadowName = shadowName; }
            /**
             * The name of the shadow. To specify the thing's classic shadow, set this parameter to an empty string
             * ("").
             */
            Aws::Crt::Optional<Aws::Crt::String> GetShadowName() noexcept { return m_shadowName; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(GetThingShadowRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(GetThingShadowRequest *) noexcept;
            /* This needs to be defined so that `GetThingShadowRequest` can be used as a key in maps. */
            bool operator<(const GetThingShadowRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_thingName;
            Aws::Crt::Optional<Aws::Crt::String> m_shadowName;
        };

        class AWS_GREENGRASSCOREIPC_API GetSecretValueResponse : public AbstractShapeBase
        {
          public:
            GetSecretValueResponse() noexcept {}
            GetSecretValueResponse(const GetSecretValueResponse &) = default;
            /**
             * The ID of the secret.
             */
            void SetSecretId(const Aws::Crt::String &secretId) noexcept { m_secretId = secretId; }
            /**
             * The ID of the secret.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetSecretId() noexcept { return m_secretId; }
            /**
             * The ID of this version of the secret.
             */
            void SetVersionId(const Aws::Crt::String &versionId) noexcept { m_versionId = versionId; }
            /**
             * The ID of this version of the secret.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetVersionId() noexcept { return m_versionId; }
            /**
             * The list of staging labels attached to this version of the secret.
             */
            void SetVersionStage(const Aws::Crt::Vector<Aws::Crt::String> &versionStage) noexcept
            {
                m_versionStage = versionStage;
            }
            /**
             * The list of staging labels attached to this version of the secret.
             */
            Aws::Crt::Optional<Aws::Crt::Vector<Aws::Crt::String>> GetVersionStage() noexcept { return m_versionStage; }
            /**
             * The value of this version of the secret.
             */
            void SetSecretValue(const SecretValue &secretValue) noexcept { m_secretValue = secretValue; }
            /**
             * The value of this version of the secret.
             */
            Aws::Crt::Optional<SecretValue> GetSecretValue() noexcept { return m_secretValue; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(GetSecretValueResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(GetSecretValueResponse *) noexcept;
            /* This needs to be defined so that `GetSecretValueResponse` can be used as a key in maps. */
            bool operator<(const GetSecretValueResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_secretId;
            Aws::Crt::Optional<Aws::Crt::String> m_versionId;
            Aws::Crt::Optional<Aws::Crt::Vector<Aws::Crt::String>> m_versionStage;
            Aws::Crt::Optional<SecretValue> m_secretValue;
        };

        class AWS_GREENGRASSCOREIPC_API GetSecretValueRequest : public AbstractShapeBase
        {
          public:
            GetSecretValueRequest() noexcept {}
            GetSecretValueRequest(const GetSecretValueRequest &) = default;
            /**
             * The name of the secret to get. You can specify either the Amazon Resource Name (ARN) or the friendly name
             * of the secret.
             */
            void SetSecretId(const Aws::Crt::String &secretId) noexcept { m_secretId = secretId; }
            /**
             * The name of the secret to get. You can specify either the Amazon Resource Name (ARN) or the friendly name
             * of the secret.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetSecretId() noexcept { return m_secretId; }
            /**
             * (Optional) The ID of the version to get. If you don't specify versionId or versionStage, this operation
             * defaults to the version with the AWSCURRENT label.
             */
            void SetVersionId(const Aws::Crt::String &versionId) noexcept { m_versionId = versionId; }
            /**
             * (Optional) The ID of the version to get. If you don't specify versionId or versionStage, this operation
             * defaults to the version with the AWSCURRENT label.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetVersionId() noexcept { return m_versionId; }
            /**
             * (Optional) The staging label of the version to get. If you don't specify versionId or versionStage, this
             * operation defaults to the version with the AWSCURRENT label.
             */
            void SetVersionStage(const Aws::Crt::String &versionStage) noexcept { m_versionStage = versionStage; }
            /**
             * (Optional) The staging label of the version to get. If you don't specify versionId or versionStage, this
             * operation defaults to the version with the AWSCURRENT label.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetVersionStage() noexcept { return m_versionStage; }
            /**
             * (Optional) Whether to fetch the latest secret from cloud when the request is handled. Defaults to false.
             */
            void SetRefresh(const bool &refresh) noexcept { m_refresh = refresh; }
            /**
             * (Optional) Whether to fetch the latest secret from cloud when the request is handled. Defaults to false.
             */
            Aws::Crt::Optional<bool> GetRefresh() noexcept { return m_refresh; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(GetSecretValueRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(GetSecretValueRequest *) noexcept;
            /* This needs to be defined so that `GetSecretValueRequest` can be used as a key in maps. */
            bool operator<(const GetSecretValueRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_secretId;
            Aws::Crt::Optional<Aws::Crt::String> m_versionId;
            Aws::Crt::Optional<Aws::Crt::String> m_versionStage;
            Aws::Crt::Optional<bool> m_refresh;
        };

        class AWS_GREENGRASSCOREIPC_API GetLocalDeploymentStatusResponse : public AbstractShapeBase
        {
          public:
            GetLocalDeploymentStatusResponse() noexcept {}
            GetLocalDeploymentStatusResponse(const GetLocalDeploymentStatusResponse &) = default;
            /**
             * The local deployment.
             */
            void SetDeployment(const LocalDeployment &deployment) noexcept { m_deployment = deployment; }
            /**
             * The local deployment.
             */
            Aws::Crt::Optional<LocalDeployment> GetDeployment() noexcept { return m_deployment; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(GetLocalDeploymentStatusResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(GetLocalDeploymentStatusResponse *) noexcept;
            /* This needs to be defined so that `GetLocalDeploymentStatusResponse` can be used as a key in maps. */
            bool operator<(const GetLocalDeploymentStatusResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<LocalDeployment> m_deployment;
        };

        class AWS_GREENGRASSCOREIPC_API GetLocalDeploymentStatusRequest : public AbstractShapeBase
        {
          public:
            GetLocalDeploymentStatusRequest() noexcept {}
            GetLocalDeploymentStatusRequest(const GetLocalDeploymentStatusRequest &) = default;
            /**
             * The ID of the local deployment to get.
             */
            void SetDeploymentId(const Aws::Crt::String &deploymentId) noexcept { m_deploymentId = deploymentId; }
            /**
             * The ID of the local deployment to get.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetDeploymentId() noexcept { return m_deploymentId; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(GetLocalDeploymentStatusRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(GetLocalDeploymentStatusRequest *) noexcept;
            /* This needs to be defined so that `GetLocalDeploymentStatusRequest` can be used as a key in maps. */
            bool operator<(const GetLocalDeploymentStatusRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_deploymentId;
        };

        class AWS_GREENGRASSCOREIPC_API GetConfigurationResponse : public AbstractShapeBase
        {
          public:
            GetConfigurationResponse() noexcept {}
            GetConfigurationResponse(const GetConfigurationResponse &) = default;
            /**
             * The name of the component.
             */
            void SetComponentName(const Aws::Crt::String &componentName) noexcept { m_componentName = componentName; }
            /**
             * The name of the component.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetComponentName() noexcept { return m_componentName; }
            /**
             * The requested configuration as an object.
             */
            void SetValue(const Aws::Crt::JsonObject &value) noexcept { m_value = value; }
            /**
             * The requested configuration as an object.
             */
            Aws::Crt::Optional<Aws::Crt::JsonObject> GetValue() noexcept { return m_value; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(GetConfigurationResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(GetConfigurationResponse *) noexcept;
            /* This needs to be defined so that `GetConfigurationResponse` can be used as a key in maps. */
            bool operator<(const GetConfigurationResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_componentName;
            Aws::Crt::Optional<Aws::Crt::JsonObject> m_value;
        };

        class AWS_GREENGRASSCOREIPC_API GetConfigurationRequest : public AbstractShapeBase
        {
          public:
            GetConfigurationRequest() noexcept {}
            GetConfigurationRequest(const GetConfigurationRequest &) = default;
            /**
             * (Optional) The name of the component. Defaults to the name of the component that makes the request.
             */
            void SetComponentName(const Aws::Crt::String &componentName) noexcept { m_componentName = componentName; }
            /**
             * (Optional) The name of the component. Defaults to the name of the component that makes the request.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetComponentName() noexcept { return m_componentName; }
            /**
             * The key path to the configuration value. Specify a list where each entry is the key for a single level in
             * the configuration object.
             */
            void SetKeyPath(const Aws::Crt::Vector<Aws::Crt::String> &keyPath) noexcept { m_keyPath = keyPath; }
            /**
             * The key path to the configuration value. Specify a list where each entry is the key for a single level in
             * the configuration object.
             */
            Aws::Crt::Optional<Aws::Crt::Vector<Aws::Crt::String>> GetKeyPath() noexcept { return m_keyPath; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(GetConfigurationRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(GetConfigurationRequest *) noexcept;
            /* This needs to be defined so that `GetConfigurationRequest` can be used as a key in maps. */
            bool operator<(const GetConfigurationRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_componentName;
            Aws::Crt::Optional<Aws::Crt::Vector<Aws::Crt::String>> m_keyPath;
        };

        class AWS_GREENGRASSCOREIPC_API GetComponentDetailsResponse : public AbstractShapeBase
        {
          public:
            GetComponentDetailsResponse() noexcept {}
            GetComponentDetailsResponse(const GetComponentDetailsResponse &) = default;
            /**
             * The component's details.
             */
            void SetComponentDetails(const ComponentDetails &componentDetails) noexcept
            {
                m_componentDetails = componentDetails;
            }
            /**
             * The component's details.
             */
            Aws::Crt::Optional<ComponentDetails> GetComponentDetails() noexcept { return m_componentDetails; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(GetComponentDetailsResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(GetComponentDetailsResponse *) noexcept;
            /* This needs to be defined so that `GetComponentDetailsResponse` can be used as a key in maps. */
            bool operator<(const GetComponentDetailsResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<ComponentDetails> m_componentDetails;
        };

        class AWS_GREENGRASSCOREIPC_API GetComponentDetailsRequest : public AbstractShapeBase
        {
          public:
            GetComponentDetailsRequest() noexcept {}
            GetComponentDetailsRequest(const GetComponentDetailsRequest &) = default;
            /**
             * The name of the component to get.
             */
            void SetComponentName(const Aws::Crt::String &componentName) noexcept { m_componentName = componentName; }
            /**
             * The name of the component to get.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetComponentName() noexcept { return m_componentName; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(GetComponentDetailsRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(GetComponentDetailsRequest *) noexcept;
            /* This needs to be defined so that `GetComponentDetailsRequest` can be used as a key in maps. */
            bool operator<(const GetComponentDetailsRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_componentName;
        };

        class AWS_GREENGRASSCOREIPC_API InvalidCredentialError : public OperationError
        {
          public:
            InvalidCredentialError() noexcept {}
            InvalidCredentialError(const InvalidCredentialError &) = default;

            void SetMessage(const Aws::Crt::String &message) noexcept { m_message = message; }

            Aws::Crt::Optional<Aws::Crt::String> GetMessage() noexcept override { return m_message; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(InvalidCredentialError &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<OperationError> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(InvalidCredentialError *) noexcept;
            /* This needs to be defined so that `InvalidCredentialError` can be used as a key in maps. */
            bool operator<(const InvalidCredentialError &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_message;
        };

        class AWS_GREENGRASSCOREIPC_API GetClientDeviceAuthTokenResponse : public AbstractShapeBase
        {
          public:
            GetClientDeviceAuthTokenResponse() noexcept {}
            GetClientDeviceAuthTokenResponse(const GetClientDeviceAuthTokenResponse &) = default;
            /**
             * The session token for the client device. You can use this session token in subsequent requests to
             * authorize this client device's actions.
             */
            void SetClientDeviceAuthToken(const Aws::Crt::String &clientDeviceAuthToken) noexcept
            {
                m_clientDeviceAuthToken = clientDeviceAuthToken;
            }
            /**
             * The session token for the client device. You can use this session token in subsequent requests to
             * authorize this client device's actions.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetClientDeviceAuthToken() noexcept { return m_clientDeviceAuthToken; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(GetClientDeviceAuthTokenResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(GetClientDeviceAuthTokenResponse *) noexcept;
            /* This needs to be defined so that `GetClientDeviceAuthTokenResponse` can be used as a key in maps. */
            bool operator<(const GetClientDeviceAuthTokenResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_clientDeviceAuthToken;
        };

        class AWS_GREENGRASSCOREIPC_API GetClientDeviceAuthTokenRequest : public AbstractShapeBase
        {
          public:
            GetClientDeviceAuthTokenRequest() noexcept {}
            GetClientDeviceAuthTokenRequest(const GetClientDeviceAuthTokenRequest &) = default;
            /**
             * The client device's credentials.
             */
            void SetCredential(const CredentialDocument &credential) noexcept { m_credential = credential; }
            /**
             * The client device's credentials.
             */
            Aws::Crt::Optional<CredentialDocument> GetCredential() noexcept { return m_credential; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(GetClientDeviceAuthTokenRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(GetClientDeviceAuthTokenRequest *) noexcept;
            /* This needs to be defined so that `GetClientDeviceAuthTokenRequest` can be used as a key in maps. */
            bool operator<(const GetClientDeviceAuthTokenRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<CredentialDocument> m_credential;
        };

        class AWS_GREENGRASSCOREIPC_API DeleteThingShadowResponse : public AbstractShapeBase
        {
          public:
            DeleteThingShadowResponse() noexcept {}
            DeleteThingShadowResponse(const DeleteThingShadowResponse &) = default;
            /**
             * An empty response state document.
             */
            void SetPayload(const Aws::Crt::Vector<uint8_t> &payload) noexcept { m_payload = payload; }
            /**
             * An empty response state document.
             */
            Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>> GetPayload() noexcept { return m_payload; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(DeleteThingShadowResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(DeleteThingShadowResponse *) noexcept;
            /* This needs to be defined so that `DeleteThingShadowResponse` can be used as a key in maps. */
            bool operator<(const DeleteThingShadowResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>> m_payload;
        };

        class AWS_GREENGRASSCOREIPC_API DeleteThingShadowRequest : public AbstractShapeBase
        {
          public:
            DeleteThingShadowRequest() noexcept {}
            DeleteThingShadowRequest(const DeleteThingShadowRequest &) = default;
            /**
             * The name of the thing.
             */
            void SetThingName(const Aws::Crt::String &thingName) noexcept { m_thingName = thingName; }
            /**
             * The name of the thing.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetThingName() noexcept { return m_thingName; }
            /**
             * The name of the shadow. To specify the thing's classic shadow, set this parameter to an empty string
             * ("").
             */
            void SetShadowName(const Aws::Crt::String &shadowName) noexcept { m_shadowName = shadowName; }
            /**
             * The name of the shadow. To specify the thing's classic shadow, set this parameter to an empty string
             * ("").
             */
            Aws::Crt::Optional<Aws::Crt::String> GetShadowName() noexcept { return m_shadowName; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(DeleteThingShadowRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(DeleteThingShadowRequest *) noexcept;
            /* This needs to be defined so that `DeleteThingShadowRequest` can be used as a key in maps. */
            bool operator<(const DeleteThingShadowRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_thingName;
            Aws::Crt::Optional<Aws::Crt::String> m_shadowName;
        };

        class AWS_GREENGRASSCOREIPC_API DeferComponentUpdateResponse : public AbstractShapeBase
        {
          public:
            DeferComponentUpdateResponse() noexcept {}
            DeferComponentUpdateResponse(const DeferComponentUpdateResponse &) = default;
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(DeferComponentUpdateResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(DeferComponentUpdateResponse *) noexcept;
            /* This needs to be defined so that `DeferComponentUpdateResponse` can be used as a key in maps. */
            bool operator<(const DeferComponentUpdateResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
        };

        class AWS_GREENGRASSCOREIPC_API DeferComponentUpdateRequest : public AbstractShapeBase
        {
          public:
            DeferComponentUpdateRequest() noexcept {}
            DeferComponentUpdateRequest(const DeferComponentUpdateRequest &) = default;
            /**
             * The ID of the AWS IoT Greengrass deployment to defer.
             */
            void SetDeploymentId(const Aws::Crt::String &deploymentId) noexcept { m_deploymentId = deploymentId; }
            /**
             * The ID of the AWS IoT Greengrass deployment to defer.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetDeploymentId() noexcept { return m_deploymentId; }
            /**
             * (Optional) The name of the component for which to defer updates. Defaults to the name of the component
             * that makes the request.
             */
            void SetMessage(const Aws::Crt::String &message) noexcept { m_message = message; }
            /**
             * (Optional) The name of the component for which to defer updates. Defaults to the name of the component
             * that makes the request.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetMessage() noexcept { return m_message; }
            /**
             * The amount of time in milliseconds for which to defer the update. Greengrass waits for this amount of
             * time and then sends another PreComponentUpdateEvent
             */
            void SetRecheckAfterMs(const int64_t &recheckAfterMs) noexcept { m_recheckAfterMs = recheckAfterMs; }
            /**
             * The amount of time in milliseconds for which to defer the update. Greengrass waits for this amount of
             * time and then sends another PreComponentUpdateEvent
             */
            Aws::Crt::Optional<int64_t> GetRecheckAfterMs() noexcept { return m_recheckAfterMs; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(DeferComponentUpdateRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(DeferComponentUpdateRequest *) noexcept;
            /* This needs to be defined so that `DeferComponentUpdateRequest` can be used as a key in maps. */
            bool operator<(const DeferComponentUpdateRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_deploymentId;
            Aws::Crt::Optional<Aws::Crt::String> m_message;
            Aws::Crt::Optional<int64_t> m_recheckAfterMs;
        };

        class AWS_GREENGRASSCOREIPC_API InvalidArtifactsDirectoryPathError : public OperationError
        {
          public:
            InvalidArtifactsDirectoryPathError() noexcept {}
            InvalidArtifactsDirectoryPathError(const InvalidArtifactsDirectoryPathError &) = default;

            void SetMessage(const Aws::Crt::String &message) noexcept { m_message = message; }

            Aws::Crt::Optional<Aws::Crt::String> GetMessage() noexcept override { return m_message; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(InvalidArtifactsDirectoryPathError &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<OperationError> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(InvalidArtifactsDirectoryPathError *) noexcept;
            /* This needs to be defined so that `InvalidArtifactsDirectoryPathError` can be used as a key in maps. */
            bool operator<(const InvalidArtifactsDirectoryPathError &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_message;
        };

        class AWS_GREENGRASSCOREIPC_API InvalidRecipeDirectoryPathError : public OperationError
        {
          public:
            InvalidRecipeDirectoryPathError() noexcept {}
            InvalidRecipeDirectoryPathError(const InvalidRecipeDirectoryPathError &) = default;

            void SetMessage(const Aws::Crt::String &message) noexcept { m_message = message; }

            Aws::Crt::Optional<Aws::Crt::String> GetMessage() noexcept override { return m_message; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(InvalidRecipeDirectoryPathError &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<OperationError> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(InvalidRecipeDirectoryPathError *) noexcept;
            /* This needs to be defined so that `InvalidRecipeDirectoryPathError` can be used as a key in maps. */
            bool operator<(const InvalidRecipeDirectoryPathError &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_message;
        };

        class AWS_GREENGRASSCOREIPC_API CreateLocalDeploymentResponse : public AbstractShapeBase
        {
          public:
            CreateLocalDeploymentResponse() noexcept {}
            CreateLocalDeploymentResponse(const CreateLocalDeploymentResponse &) = default;
            /**
             * The ID of the local deployment that the request created.
             */
            void SetDeploymentId(const Aws::Crt::String &deploymentId) noexcept { m_deploymentId = deploymentId; }
            /**
             * The ID of the local deployment that the request created.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetDeploymentId() noexcept { return m_deploymentId; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(CreateLocalDeploymentResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(CreateLocalDeploymentResponse *) noexcept;
            /* This needs to be defined so that `CreateLocalDeploymentResponse` can be used as a key in maps. */
            bool operator<(const CreateLocalDeploymentResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_deploymentId;
        };

        class AWS_GREENGRASSCOREIPC_API CreateLocalDeploymentRequest : public AbstractShapeBase
        {
          public:
            CreateLocalDeploymentRequest() noexcept {}
            CreateLocalDeploymentRequest(const CreateLocalDeploymentRequest &) = default;
            /**
             * The thing group name the deployment is targeting. If the group name is not specified, "LOCAL_DEPLOYMENT"
             * will be used.
             */
            void SetGroupName(const Aws::Crt::String &groupName) noexcept { m_groupName = groupName; }
            /**
             * The thing group name the deployment is targeting. If the group name is not specified, "LOCAL_DEPLOYMENT"
             * will be used.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetGroupName() noexcept { return m_groupName; }
            /**
             * Map of component name to version. Components will be added to the group's existing root components.
             */
            void SetRootComponentVersionsToAdd(
                const Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String> &rootComponentVersionsToAdd) noexcept
            {
                m_rootComponentVersionsToAdd = rootComponentVersionsToAdd;
            }
            /**
             * Map of component name to version. Components will be added to the group's existing root components.
             */
            Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String>>
                GetRootComponentVersionsToAdd() noexcept
            {
                return m_rootComponentVersionsToAdd;
            }
            /**
             * List of components that need to be removed from the group, for example if new artifacts were loaded in
             * this request but recipe version did not change.
             */
            void SetRootComponentsToRemove(const Aws::Crt::Vector<Aws::Crt::String> &rootComponentsToRemove) noexcept
            {
                m_rootComponentsToRemove = rootComponentsToRemove;
            }
            /**
             * List of components that need to be removed from the group, for example if new artifacts were loaded in
             * this request but recipe version did not change.
             */
            Aws::Crt::Optional<Aws::Crt::Vector<Aws::Crt::String>> GetRootComponentsToRemove() noexcept
            {
                return m_rootComponentsToRemove;
            }
            /**
             * Map of component names to configuration.
             */
            void SetComponentToConfiguration(
                const Aws::Crt::Map<Aws::Crt::String, Aws::Crt::JsonObject> &componentToConfiguration) noexcept
            {
                m_componentToConfiguration = componentToConfiguration;
            }
            /**
             * Map of component names to configuration.
             */
            Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, Aws::Crt::JsonObject>>
                GetComponentToConfiguration() noexcept
            {
                return m_componentToConfiguration;
            }
            /**
             * Map of component names to component run as info.
             */
            void SetComponentToRunWithInfo(
                const Aws::Crt::Map<Aws::Crt::String, RunWithInfo> &componentToRunWithInfo) noexcept
            {
                m_componentToRunWithInfo = componentToRunWithInfo;
            }
            /**
             * Map of component names to component run as info.
             */
            Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, RunWithInfo>> GetComponentToRunWithInfo() noexcept
            {
                return m_componentToRunWithInfo;
            }
            /**
             * All recipes files in this directory will be copied over to the Greengrass package store.
             */
            void SetRecipeDirectoryPath(const Aws::Crt::String &recipeDirectoryPath) noexcept
            {
                m_recipeDirectoryPath = recipeDirectoryPath;
            }
            /**
             * All recipes files in this directory will be copied over to the Greengrass package store.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetRecipeDirectoryPath() noexcept { return m_recipeDirectoryPath; }
            /**
             * All artifact files in this directory will be copied over to the Greengrass package store.
             */
            void SetArtifactsDirectoryPath(const Aws::Crt::String &artifactsDirectoryPath) noexcept
            {
                m_artifactsDirectoryPath = artifactsDirectoryPath;
            }
            /**
             * All artifact files in this directory will be copied over to the Greengrass package store.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetArtifactsDirectoryPath() noexcept
            {
                return m_artifactsDirectoryPath;
            }
            /**
             * Deployment failure handling policy.
             */
            void SetFailureHandlingPolicy(FailureHandlingPolicy failureHandlingPolicy) noexcept;
            /**
             * Deployment failure handling policy.
             */
            Aws::Crt::Optional<FailureHandlingPolicy> GetFailureHandlingPolicy() noexcept;
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(CreateLocalDeploymentRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(CreateLocalDeploymentRequest *) noexcept;
            /* This needs to be defined so that `CreateLocalDeploymentRequest` can be used as a key in maps. */
            bool operator<(const CreateLocalDeploymentRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_groupName;
            Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String>> m_rootComponentVersionsToAdd;
            Aws::Crt::Optional<Aws::Crt::Vector<Aws::Crt::String>> m_rootComponentsToRemove;
            Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, Aws::Crt::JsonObject>> m_componentToConfiguration;
            Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, RunWithInfo>> m_componentToRunWithInfo;
            Aws::Crt::Optional<Aws::Crt::String> m_recipeDirectoryPath;
            Aws::Crt::Optional<Aws::Crt::String> m_artifactsDirectoryPath;
            Aws::Crt::Optional<Aws::Crt::String> m_failureHandlingPolicy;
        };

        class AWS_GREENGRASSCOREIPC_API CreateDebugPasswordResponse : public AbstractShapeBase
        {
          public:
            CreateDebugPasswordResponse() noexcept {}
            CreateDebugPasswordResponse(const CreateDebugPasswordResponse &) = default;

            void SetPassword(const Aws::Crt::String &password) noexcept { m_password = password; }

            Aws::Crt::Optional<Aws::Crt::String> GetPassword() noexcept { return m_password; }

            void SetUsername(const Aws::Crt::String &username) noexcept { m_username = username; }

            Aws::Crt::Optional<Aws::Crt::String> GetUsername() noexcept { return m_username; }

            void SetPasswordExpiration(const Aws::Crt::DateTime &passwordExpiration) noexcept
            {
                m_passwordExpiration = passwordExpiration;
            }

            Aws::Crt::Optional<Aws::Crt::DateTime> GetPasswordExpiration() noexcept { return m_passwordExpiration; }

            void SetCertificateSHA256Hash(const Aws::Crt::String &certificateSHA256Hash) noexcept
            {
                m_certificateSHA256Hash = certificateSHA256Hash;
            }

            Aws::Crt::Optional<Aws::Crt::String> GetCertificateSHA256Hash() noexcept { return m_certificateSHA256Hash; }

            void SetCertificateSHA1Hash(const Aws::Crt::String &certificateSHA1Hash) noexcept
            {
                m_certificateSHA1Hash = certificateSHA1Hash;
            }

            Aws::Crt::Optional<Aws::Crt::String> GetCertificateSHA1Hash() noexcept { return m_certificateSHA1Hash; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(CreateDebugPasswordResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(CreateDebugPasswordResponse *) noexcept;
            /* This needs to be defined so that `CreateDebugPasswordResponse` can be used as a key in maps. */
            bool operator<(const CreateDebugPasswordResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_password;
            Aws::Crt::Optional<Aws::Crt::String> m_username;
            Aws::Crt::Optional<Aws::Crt::DateTime> m_passwordExpiration;
            Aws::Crt::Optional<Aws::Crt::String> m_certificateSHA256Hash;
            Aws::Crt::Optional<Aws::Crt::String> m_certificateSHA1Hash;
        };

        class AWS_GREENGRASSCOREIPC_API CreateDebugPasswordRequest : public AbstractShapeBase
        {
          public:
            CreateDebugPasswordRequest() noexcept {}
            CreateDebugPasswordRequest(const CreateDebugPasswordRequest &) = default;
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(CreateDebugPasswordRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(CreateDebugPasswordRequest *) noexcept;
            /* This needs to be defined so that `CreateDebugPasswordRequest` can be used as a key in maps. */
            bool operator<(const CreateDebugPasswordRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
        };

        class AWS_GREENGRASSCOREIPC_API CancelLocalDeploymentResponse : public AbstractShapeBase
        {
          public:
            CancelLocalDeploymentResponse() noexcept {}
            CancelLocalDeploymentResponse(const CancelLocalDeploymentResponse &) = default;

            void SetMessage(const Aws::Crt::String &message) noexcept { m_message = message; }

            Aws::Crt::Optional<Aws::Crt::String> GetMessage() noexcept { return m_message; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(CancelLocalDeploymentResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(CancelLocalDeploymentResponse *) noexcept;
            /* This needs to be defined so that `CancelLocalDeploymentResponse` can be used as a key in maps. */
            bool operator<(const CancelLocalDeploymentResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_message;
        };

        class AWS_GREENGRASSCOREIPC_API CancelLocalDeploymentRequest : public AbstractShapeBase
        {
          public:
            CancelLocalDeploymentRequest() noexcept {}
            CancelLocalDeploymentRequest(const CancelLocalDeploymentRequest &) = default;
            /**
             * (Optional) The ID of the local deployment to cancel.
             */
            void SetDeploymentId(const Aws::Crt::String &deploymentId) noexcept { m_deploymentId = deploymentId; }
            /**
             * (Optional) The ID of the local deployment to cancel.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetDeploymentId() noexcept { return m_deploymentId; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(CancelLocalDeploymentRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(CancelLocalDeploymentRequest *) noexcept;
            /* This needs to be defined so that `CancelLocalDeploymentRequest` can be used as a key in maps. */
            bool operator<(const CancelLocalDeploymentRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_deploymentId;
        };

        class AWS_GREENGRASSCOREIPC_API InvalidClientDeviceAuthTokenError : public OperationError
        {
          public:
            InvalidClientDeviceAuthTokenError() noexcept {}
            InvalidClientDeviceAuthTokenError(const InvalidClientDeviceAuthTokenError &) = default;

            void SetMessage(const Aws::Crt::String &message) noexcept { m_message = message; }

            Aws::Crt::Optional<Aws::Crt::String> GetMessage() noexcept override { return m_message; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(InvalidClientDeviceAuthTokenError &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<OperationError> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(InvalidClientDeviceAuthTokenError *) noexcept;
            /* This needs to be defined so that `InvalidClientDeviceAuthTokenError` can be used as a key in maps. */
            bool operator<(const InvalidClientDeviceAuthTokenError &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_message;
        };

        class AWS_GREENGRASSCOREIPC_API AuthorizeClientDeviceActionResponse : public AbstractShapeBase
        {
          public:
            AuthorizeClientDeviceActionResponse() noexcept {}
            AuthorizeClientDeviceActionResponse(const AuthorizeClientDeviceActionResponse &) = default;
            /**
             * Whether the client device is authorized to perform the operation on the resource.
             */
            void SetIsAuthorized(const bool &isAuthorized) noexcept { m_isAuthorized = isAuthorized; }
            /**
             * Whether the client device is authorized to perform the operation on the resource.
             */
            Aws::Crt::Optional<bool> GetIsAuthorized() noexcept { return m_isAuthorized; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(AuthorizeClientDeviceActionResponse &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(AuthorizeClientDeviceActionResponse *) noexcept;
            /* This needs to be defined so that `AuthorizeClientDeviceActionResponse` can be used as a key in maps. */
            bool operator<(const AuthorizeClientDeviceActionResponse &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<bool> m_isAuthorized;
        };

        class AWS_GREENGRASSCOREIPC_API AuthorizeClientDeviceActionRequest : public AbstractShapeBase
        {
          public:
            AuthorizeClientDeviceActionRequest() noexcept {}
            AuthorizeClientDeviceActionRequest(const AuthorizeClientDeviceActionRequest &) = default;
            /**
             * The session token for the client device from GetClientDeviceAuthToken.
             */
            void SetClientDeviceAuthToken(const Aws::Crt::String &clientDeviceAuthToken) noexcept
            {
                m_clientDeviceAuthToken = clientDeviceAuthToken;
            }
            /**
             * The session token for the client device from GetClientDeviceAuthToken.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetClientDeviceAuthToken() noexcept { return m_clientDeviceAuthToken; }
            /**
             * The operation to authorize.
             */
            void SetOperation(const Aws::Crt::String &operation) noexcept { m_operation = operation; }
            /**
             * The operation to authorize.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetOperation() noexcept { return m_operation; }
            /**
             * The resource the client device performs the operation on.
             */
            void SetResource(const Aws::Crt::String &resource) noexcept { m_resource = resource; }
            /**
             * The resource the client device performs the operation on.
             */
            Aws::Crt::Optional<Aws::Crt::String> GetResource() noexcept { return m_resource; }
            void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
            static void s_loadFromJsonView(AuthorizeClientDeviceActionRequest &, const Aws::Crt::JsonView &) noexcept;
            static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
                Aws::Crt::StringView,
                Aws::Crt::Allocator *) noexcept;
            static void s_customDeleter(AuthorizeClientDeviceActionRequest *) noexcept;
            /* This needs to be defined so that `AuthorizeClientDeviceActionRequest` can be used as a key in maps. */
            bool operator<(const AuthorizeClientDeviceActionRequest &) const noexcept;
            static const char *MODEL_NAME;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;

          private:
            Aws::Crt::Optional<Aws::Crt::String> m_clientDeviceAuthToken;
            Aws::Crt::Optional<Aws::Crt::String> m_operation;
            Aws::Crt::Optional<Aws::Crt::String> m_resource;
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToIoTCoreStreamHandler : public StreamResponseHandler
        {
          public:
            virtual void OnStreamEvent(IoTCoreMessage *response) { (void)response; }

            /**
             * A callback that is invoked when an error occurs while parsing a message from the stream.
             * @param rpcError The RPC error containing the status and possibly a CRT error.
             */
            virtual bool OnStreamError(RpcError rpcError)
            {
                (void)rpcError;
                return true;
            }

            /**
             * A callback that is invoked upon receiving an error of type `ServiceError`.
             * @param operationError The error message being received.
             */
            virtual bool OnStreamError(ServiceError *operationError)
            {
                (void)operationError;
                return true;
            }

            /**
             * A callback that is invoked upon receiving an error of type `UnauthorizedError`.
             * @param operationError The error message being received.
             */
            virtual bool OnStreamError(UnauthorizedError *operationError)
            {
                (void)operationError;
                return true;
            }

            /**
             * A callback that is invoked upon receiving ANY error response from the server.
             * @param operationError The error message being received.
             */
            virtual bool OnStreamError(OperationError *operationError)
            {
                (void)operationError;
                return true;
            }

          private:
            /**
             * Invoked when a message is received on this continuation.
             */
            void OnStreamEvent(Aws::Crt::ScopedResource<AbstractShapeBase> response) override;
            /**
             * Invoked when a message is received on this continuation but results in an error.
             *
             * This callback can return true so that the stream is closed afterwards.
             */
            bool OnStreamError(Aws::Crt::ScopedResource<OperationError> error, RpcError rpcError) override;
        };
        class AWS_GREENGRASSCOREIPC_API SubscribeToIoTCoreOperationContext : public OperationModelContext
        {
          public:
            SubscribeToIoTCoreOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToIoTCoreResult
        {
          public:
            SubscribeToIoTCoreResult() noexcept {}
            SubscribeToIoTCoreResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
            SubscribeToIoTCoreResponse *GetOperationResponse() const noexcept
            {
                return static_cast<SubscribeToIoTCoreResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToIoTCoreOperation : public ClientOperation
        {
          public:
            SubscribeToIoTCoreOperation(
                ClientConnection &connection,
                std::shared_ptr<SubscribeToIoTCoreStreamHandler> streamHandler,
                const SubscribeToIoTCoreOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `SubscribeToIoTCoreOperation`
             * @param request The request used for the `SubscribeToIoTCoreOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const SubscribeToIoTCoreRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<SubscribeToIoTCoreResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API ResumeComponentOperationContext : public OperationModelContext
        {
          public:
            ResumeComponentOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API ResumeComponentResult
        {
          public:
            ResumeComponentResult() noexcept {}
            ResumeComponentResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
            ResumeComponentResponse *GetOperationResponse() const noexcept
            {
                return static_cast<ResumeComponentResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API ResumeComponentOperation : public ClientOperation
        {
          public:
            ResumeComponentOperation(
                ClientConnection &connection,
                const ResumeComponentOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `ResumeComponentOperation`
             * @param request The request used for the `ResumeComponentOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const ResumeComponentRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<ResumeComponentResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API PublishToIoTCoreOperationContext : public OperationModelContext
        {
          public:
            PublishToIoTCoreOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API PublishToIoTCoreResult
        {
          public:
            PublishToIoTCoreResult() noexcept {}
            PublishToIoTCoreResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
            PublishToIoTCoreResponse *GetOperationResponse() const noexcept
            {
                return static_cast<PublishToIoTCoreResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API PublishToIoTCoreOperation : public ClientOperation
        {
          public:
            PublishToIoTCoreOperation(
                ClientConnection &connection,
                const PublishToIoTCoreOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `PublishToIoTCoreOperation`
             * @param request The request used for the `PublishToIoTCoreOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const PublishToIoTCoreRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<PublishToIoTCoreResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToConfigurationUpdateStreamHandler : public StreamResponseHandler
        {
          public:
            virtual void OnStreamEvent(ConfigurationUpdateEvents *response) { (void)response; }

            /**
             * A callback that is invoked when an error occurs while parsing a message from the stream.
             * @param rpcError The RPC error containing the status and possibly a CRT error.
             */
            virtual bool OnStreamError(RpcError rpcError)
            {
                (void)rpcError;
                return true;
            }

            /**
             * A callback that is invoked upon receiving an error of type `ServiceError`.
             * @param operationError The error message being received.
             */
            virtual bool OnStreamError(ServiceError *operationError)
            {
                (void)operationError;
                return true;
            }

            /**
             * A callback that is invoked upon receiving an error of type `ResourceNotFoundError`.
             * @param operationError The error message being received.
             */
            virtual bool OnStreamError(ResourceNotFoundError *operationError)
            {
                (void)operationError;
                return true;
            }

            /**
             * A callback that is invoked upon receiving ANY error response from the server.
             * @param operationError The error message being received.
             */
            virtual bool OnStreamError(OperationError *operationError)
            {
                (void)operationError;
                return true;
            }

          private:
            /**
             * Invoked when a message is received on this continuation.
             */
            void OnStreamEvent(Aws::Crt::ScopedResource<AbstractShapeBase> response) override;
            /**
             * Invoked when a message is received on this continuation but results in an error.
             *
             * This callback can return true so that the stream is closed afterwards.
             */
            bool OnStreamError(Aws::Crt::ScopedResource<OperationError> error, RpcError rpcError) override;
        };
        class AWS_GREENGRASSCOREIPC_API SubscribeToConfigurationUpdateOperationContext : public OperationModelContext
        {
          public:
            SubscribeToConfigurationUpdateOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToConfigurationUpdateResult
        {
          public:
            SubscribeToConfigurationUpdateResult() noexcept {}
            SubscribeToConfigurationUpdateResult(TaggedResult &&taggedResult) noexcept
                : m_taggedResult(std::move(taggedResult))
            {
            }
            SubscribeToConfigurationUpdateResponse *GetOperationResponse() const noexcept
            {
                return static_cast<SubscribeToConfigurationUpdateResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToConfigurationUpdateOperation : public ClientOperation
        {
          public:
            SubscribeToConfigurationUpdateOperation(
                ClientConnection &connection,
                std::shared_ptr<SubscribeToConfigurationUpdateStreamHandler> streamHandler,
                const SubscribeToConfigurationUpdateOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `SubscribeToConfigurationUpdateOperation`
             * @param request The request used for the `SubscribeToConfigurationUpdateOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const SubscribeToConfigurationUpdateRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<SubscribeToConfigurationUpdateResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API DeleteThingShadowOperationContext : public OperationModelContext
        {
          public:
            DeleteThingShadowOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API DeleteThingShadowResult
        {
          public:
            DeleteThingShadowResult() noexcept {}
            DeleteThingShadowResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
            DeleteThingShadowResponse *GetOperationResponse() const noexcept
            {
                return static_cast<DeleteThingShadowResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API DeleteThingShadowOperation : public ClientOperation
        {
          public:
            DeleteThingShadowOperation(
                ClientConnection &connection,
                const DeleteThingShadowOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `DeleteThingShadowOperation`
             * @param request The request used for the `DeleteThingShadowOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const DeleteThingShadowRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<DeleteThingShadowResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API PutComponentMetricOperationContext : public OperationModelContext
        {
          public:
            PutComponentMetricOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API PutComponentMetricResult
        {
          public:
            PutComponentMetricResult() noexcept {}
            PutComponentMetricResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
            PutComponentMetricResponse *GetOperationResponse() const noexcept
            {
                return static_cast<PutComponentMetricResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API PutComponentMetricOperation : public ClientOperation
        {
          public:
            PutComponentMetricOperation(
                ClientConnection &connection,
                const PutComponentMetricOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `PutComponentMetricOperation`
             * @param request The request used for the `PutComponentMetricOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const PutComponentMetricRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<PutComponentMetricResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API DeferComponentUpdateOperationContext : public OperationModelContext
        {
          public:
            DeferComponentUpdateOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API DeferComponentUpdateResult
        {
          public:
            DeferComponentUpdateResult() noexcept {}
            DeferComponentUpdateResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult))
            {
            }
            DeferComponentUpdateResponse *GetOperationResponse() const noexcept
            {
                return static_cast<DeferComponentUpdateResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API DeferComponentUpdateOperation : public ClientOperation
        {
          public:
            DeferComponentUpdateOperation(
                ClientConnection &connection,
                const DeferComponentUpdateOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `DeferComponentUpdateOperation`
             * @param request The request used for the `DeferComponentUpdateOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const DeferComponentUpdateRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<DeferComponentUpdateResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToValidateConfigurationUpdatesStreamHandler
            : public StreamResponseHandler
        {
          public:
            virtual void OnStreamEvent(ValidateConfigurationUpdateEvents *response) { (void)response; }

            /**
             * A callback that is invoked when an error occurs while parsing a message from the stream.
             * @param rpcError The RPC error containing the status and possibly a CRT error.
             */
            virtual bool OnStreamError(RpcError rpcError)
            {
                (void)rpcError;
                return true;
            }

            /**
             * A callback that is invoked upon receiving an error of type `ServiceError`.
             * @param operationError The error message being received.
             */
            virtual bool OnStreamError(ServiceError *operationError)
            {
                (void)operationError;
                return true;
            }

            /**
             * A callback that is invoked upon receiving ANY error response from the server.
             * @param operationError The error message being received.
             */
            virtual bool OnStreamError(OperationError *operationError)
            {
                (void)operationError;
                return true;
            }

          private:
            /**
             * Invoked when a message is received on this continuation.
             */
            void OnStreamEvent(Aws::Crt::ScopedResource<AbstractShapeBase> response) override;
            /**
             * Invoked when a message is received on this continuation but results in an error.
             *
             * This callback can return true so that the stream is closed afterwards.
             */
            bool OnStreamError(Aws::Crt::ScopedResource<OperationError> error, RpcError rpcError) override;
        };
        class AWS_GREENGRASSCOREIPC_API SubscribeToValidateConfigurationUpdatesOperationContext
            : public OperationModelContext
        {
          public:
            SubscribeToValidateConfigurationUpdatesOperationContext(
                const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToValidateConfigurationUpdatesResult
        {
          public:
            SubscribeToValidateConfigurationUpdatesResult() noexcept {}
            SubscribeToValidateConfigurationUpdatesResult(TaggedResult &&taggedResult) noexcept
                : m_taggedResult(std::move(taggedResult))
            {
            }
            SubscribeToValidateConfigurationUpdatesResponse *GetOperationResponse() const noexcept
            {
                return static_cast<SubscribeToValidateConfigurationUpdatesResponse *>(
                    m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToValidateConfigurationUpdatesOperation : public ClientOperation
        {
          public:
            SubscribeToValidateConfigurationUpdatesOperation(
                ClientConnection &connection,
                std::shared_ptr<SubscribeToValidateConfigurationUpdatesStreamHandler> streamHandler,
                const SubscribeToValidateConfigurationUpdatesOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `SubscribeToValidateConfigurationUpdatesOperation`
             * @param request The request used for the `SubscribeToValidateConfigurationUpdatesOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const SubscribeToValidateConfigurationUpdatesRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<SubscribeToValidateConfigurationUpdatesResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API GetConfigurationOperationContext : public OperationModelContext
        {
          public:
            GetConfigurationOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API GetConfigurationResult
        {
          public:
            GetConfigurationResult() noexcept {}
            GetConfigurationResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
            GetConfigurationResponse *GetOperationResponse() const noexcept
            {
                return static_cast<GetConfigurationResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API GetConfigurationOperation : public ClientOperation
        {
          public:
            GetConfigurationOperation(
                ClientConnection &connection,
                const GetConfigurationOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `GetConfigurationOperation`
             * @param request The request used for the `GetConfigurationOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const GetConfigurationRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<GetConfigurationResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToTopicStreamHandler : public StreamResponseHandler
        {
          public:
            virtual void OnStreamEvent(SubscriptionResponseMessage *response) { (void)response; }

            /**
             * A callback that is invoked when an error occurs while parsing a message from the stream.
             * @param rpcError The RPC error containing the status and possibly a CRT error.
             */
            virtual bool OnStreamError(RpcError rpcError)
            {
                (void)rpcError;
                return true;
            }

            /**
             * A callback that is invoked upon receiving an error of type `InvalidArgumentsError`.
             * @param operationError The error message being received.
             */
            virtual bool OnStreamError(InvalidArgumentsError *operationError)
            {
                (void)operationError;
                return true;
            }

            /**
             * A callback that is invoked upon receiving an error of type `ServiceError`.
             * @param operationError The error message being received.
             */
            virtual bool OnStreamError(ServiceError *operationError)
            {
                (void)operationError;
                return true;
            }

            /**
             * A callback that is invoked upon receiving an error of type `UnauthorizedError`.
             * @param operationError The error message being received.
             */
            virtual bool OnStreamError(UnauthorizedError *operationError)
            {
                (void)operationError;
                return true;
            }

            /**
             * A callback that is invoked upon receiving ANY error response from the server.
             * @param operationError The error message being received.
             */
            virtual bool OnStreamError(OperationError *operationError)
            {
                (void)operationError;
                return true;
            }

          private:
            /**
             * Invoked when a message is received on this continuation.
             */
            void OnStreamEvent(Aws::Crt::ScopedResource<AbstractShapeBase> response) override;
            /**
             * Invoked when a message is received on this continuation but results in an error.
             *
             * This callback can return true so that the stream is closed afterwards.
             */
            bool OnStreamError(Aws::Crt::ScopedResource<OperationError> error, RpcError rpcError) override;
        };
        class AWS_GREENGRASSCOREIPC_API SubscribeToTopicOperationContext : public OperationModelContext
        {
          public:
            SubscribeToTopicOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToTopicResult
        {
          public:
            SubscribeToTopicResult() noexcept {}
            SubscribeToTopicResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
            SubscribeToTopicResponse *GetOperationResponse() const noexcept
            {
                return static_cast<SubscribeToTopicResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToTopicOperation : public ClientOperation
        {
          public:
            SubscribeToTopicOperation(
                ClientConnection &connection,
                std::shared_ptr<SubscribeToTopicStreamHandler> streamHandler,
                const SubscribeToTopicOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `SubscribeToTopicOperation`
             * @param request The request used for the `SubscribeToTopicOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const SubscribeToTopicRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<SubscribeToTopicResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API GetComponentDetailsOperationContext : public OperationModelContext
        {
          public:
            GetComponentDetailsOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API GetComponentDetailsResult
        {
          public:
            GetComponentDetailsResult() noexcept {}
            GetComponentDetailsResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
            GetComponentDetailsResponse *GetOperationResponse() const noexcept
            {
                return static_cast<GetComponentDetailsResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API GetComponentDetailsOperation : public ClientOperation
        {
          public:
            GetComponentDetailsOperation(
                ClientConnection &connection,
                const GetComponentDetailsOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `GetComponentDetailsOperation`
             * @param request The request used for the `GetComponentDetailsOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const GetComponentDetailsRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<GetComponentDetailsResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API GetClientDeviceAuthTokenOperationContext : public OperationModelContext
        {
          public:
            GetClientDeviceAuthTokenOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API GetClientDeviceAuthTokenResult
        {
          public:
            GetClientDeviceAuthTokenResult() noexcept {}
            GetClientDeviceAuthTokenResult(TaggedResult &&taggedResult) noexcept
                : m_taggedResult(std::move(taggedResult))
            {
            }
            GetClientDeviceAuthTokenResponse *GetOperationResponse() const noexcept
            {
                return static_cast<GetClientDeviceAuthTokenResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API GetClientDeviceAuthTokenOperation : public ClientOperation
        {
          public:
            GetClientDeviceAuthTokenOperation(
                ClientConnection &connection,
                const GetClientDeviceAuthTokenOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `GetClientDeviceAuthTokenOperation`
             * @param request The request used for the `GetClientDeviceAuthTokenOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const GetClientDeviceAuthTokenRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<GetClientDeviceAuthTokenResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API PublishToTopicOperationContext : public OperationModelContext
        {
          public:
            PublishToTopicOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API PublishToTopicResult
        {
          public:
            PublishToTopicResult() noexcept {}
            PublishToTopicResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
            PublishToTopicResponse *GetOperationResponse() const noexcept
            {
                return static_cast<PublishToTopicResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API PublishToTopicOperation : public ClientOperation
        {
          public:
            PublishToTopicOperation(
                ClientConnection &connection,
                const PublishToTopicOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `PublishToTopicOperation`
             * @param request The request used for the `PublishToTopicOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const PublishToTopicRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<PublishToTopicResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToCertificateUpdatesStreamHandler : public StreamResponseHandler
        {
          public:
            virtual void OnStreamEvent(CertificateUpdateEvent *response) { (void)response; }

            /**
             * A callback that is invoked when an error occurs while parsing a message from the stream.
             * @param rpcError The RPC error containing the status and possibly a CRT error.
             */
            virtual bool OnStreamError(RpcError rpcError)
            {
                (void)rpcError;
                return true;
            }

            /**
             * A callback that is invoked upon receiving an error of type `ServiceError`.
             * @param operationError The error message being received.
             */
            virtual bool OnStreamError(ServiceError *operationError)
            {
                (void)operationError;
                return true;
            }

            /**
             * A callback that is invoked upon receiving an error of type `UnauthorizedError`.
             * @param operationError The error message being received.
             */
            virtual bool OnStreamError(UnauthorizedError *operationError)
            {
                (void)operationError;
                return true;
            }

            /**
             * A callback that is invoked upon receiving an error of type `InvalidArgumentsError`.
             * @param operationError The error message being received.
             */
            virtual bool OnStreamError(InvalidArgumentsError *operationError)
            {
                (void)operationError;
                return true;
            }

            /**
             * A callback that is invoked upon receiving ANY error response from the server.
             * @param operationError The error message being received.
             */
            virtual bool OnStreamError(OperationError *operationError)
            {
                (void)operationError;
                return true;
            }

          private:
            /**
             * Invoked when a message is received on this continuation.
             */
            void OnStreamEvent(Aws::Crt::ScopedResource<AbstractShapeBase> response) override;
            /**
             * Invoked when a message is received on this continuation but results in an error.
             *
             * This callback can return true so that the stream is closed afterwards.
             */
            bool OnStreamError(Aws::Crt::ScopedResource<OperationError> error, RpcError rpcError) override;
        };
        class AWS_GREENGRASSCOREIPC_API SubscribeToCertificateUpdatesOperationContext : public OperationModelContext
        {
          public:
            SubscribeToCertificateUpdatesOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToCertificateUpdatesResult
        {
          public:
            SubscribeToCertificateUpdatesResult() noexcept {}
            SubscribeToCertificateUpdatesResult(TaggedResult &&taggedResult) noexcept
                : m_taggedResult(std::move(taggedResult))
            {
            }
            SubscribeToCertificateUpdatesResponse *GetOperationResponse() const noexcept
            {
                return static_cast<SubscribeToCertificateUpdatesResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToCertificateUpdatesOperation : public ClientOperation
        {
          public:
            SubscribeToCertificateUpdatesOperation(
                ClientConnection &connection,
                std::shared_ptr<SubscribeToCertificateUpdatesStreamHandler> streamHandler,
                const SubscribeToCertificateUpdatesOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `SubscribeToCertificateUpdatesOperation`
             * @param request The request used for the `SubscribeToCertificateUpdatesOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const SubscribeToCertificateUpdatesRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<SubscribeToCertificateUpdatesResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API VerifyClientDeviceIdentityOperationContext : public OperationModelContext
        {
          public:
            VerifyClientDeviceIdentityOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API VerifyClientDeviceIdentityResult
        {
          public:
            VerifyClientDeviceIdentityResult() noexcept {}
            VerifyClientDeviceIdentityResult(TaggedResult &&taggedResult) noexcept
                : m_taggedResult(std::move(taggedResult))
            {
            }
            VerifyClientDeviceIdentityResponse *GetOperationResponse() const noexcept
            {
                return static_cast<VerifyClientDeviceIdentityResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API VerifyClientDeviceIdentityOperation : public ClientOperation
        {
          public:
            VerifyClientDeviceIdentityOperation(
                ClientConnection &connection,
                const VerifyClientDeviceIdentityOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `VerifyClientDeviceIdentityOperation`
             * @param request The request used for the `VerifyClientDeviceIdentityOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const VerifyClientDeviceIdentityRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<VerifyClientDeviceIdentityResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API AuthorizeClientDeviceActionOperationContext : public OperationModelContext
        {
          public:
            AuthorizeClientDeviceActionOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API AuthorizeClientDeviceActionResult
        {
          public:
            AuthorizeClientDeviceActionResult() noexcept {}
            AuthorizeClientDeviceActionResult(TaggedResult &&taggedResult) noexcept
                : m_taggedResult(std::move(taggedResult))
            {
            }
            AuthorizeClientDeviceActionResponse *GetOperationResponse() const noexcept
            {
                return static_cast<AuthorizeClientDeviceActionResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API AuthorizeClientDeviceActionOperation : public ClientOperation
        {
          public:
            AuthorizeClientDeviceActionOperation(
                ClientConnection &connection,
                const AuthorizeClientDeviceActionOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `AuthorizeClientDeviceActionOperation`
             * @param request The request used for the `AuthorizeClientDeviceActionOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const AuthorizeClientDeviceActionRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<AuthorizeClientDeviceActionResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API ListComponentsOperationContext : public OperationModelContext
        {
          public:
            ListComponentsOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API ListComponentsResult
        {
          public:
            ListComponentsResult() noexcept {}
            ListComponentsResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
            ListComponentsResponse *GetOperationResponse() const noexcept
            {
                return static_cast<ListComponentsResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API ListComponentsOperation : public ClientOperation
        {
          public:
            ListComponentsOperation(
                ClientConnection &connection,
                const ListComponentsOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `ListComponentsOperation`
             * @param request The request used for the `ListComponentsOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const ListComponentsRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<ListComponentsResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API CreateDebugPasswordOperationContext : public OperationModelContext
        {
          public:
            CreateDebugPasswordOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API CreateDebugPasswordResult
        {
          public:
            CreateDebugPasswordResult() noexcept {}
            CreateDebugPasswordResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
            CreateDebugPasswordResponse *GetOperationResponse() const noexcept
            {
                return static_cast<CreateDebugPasswordResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API CreateDebugPasswordOperation : public ClientOperation
        {
          public:
            CreateDebugPasswordOperation(
                ClientConnection &connection,
                const CreateDebugPasswordOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `CreateDebugPasswordOperation`
             * @param request The request used for the `CreateDebugPasswordOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const CreateDebugPasswordRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<CreateDebugPasswordResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API GetThingShadowOperationContext : public OperationModelContext
        {
          public:
            GetThingShadowOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API GetThingShadowResult
        {
          public:
            GetThingShadowResult() noexcept {}
            GetThingShadowResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
            GetThingShadowResponse *GetOperationResponse() const noexcept
            {
                return static_cast<GetThingShadowResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API GetThingShadowOperation : public ClientOperation
        {
          public:
            GetThingShadowOperation(
                ClientConnection &connection,
                const GetThingShadowOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `GetThingShadowOperation`
             * @param request The request used for the `GetThingShadowOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const GetThingShadowRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<GetThingShadowResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API SendConfigurationValidityReportOperationContext : public OperationModelContext
        {
          public:
            SendConfigurationValidityReportOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API SendConfigurationValidityReportResult
        {
          public:
            SendConfigurationValidityReportResult() noexcept {}
            SendConfigurationValidityReportResult(TaggedResult &&taggedResult) noexcept
                : m_taggedResult(std::move(taggedResult))
            {
            }
            SendConfigurationValidityReportResponse *GetOperationResponse() const noexcept
            {
                return static_cast<SendConfigurationValidityReportResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API SendConfigurationValidityReportOperation : public ClientOperation
        {
          public:
            SendConfigurationValidityReportOperation(
                ClientConnection &connection,
                const SendConfigurationValidityReportOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `SendConfigurationValidityReportOperation`
             * @param request The request used for the `SendConfigurationValidityReportOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const SendConfigurationValidityReportRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<SendConfigurationValidityReportResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API UpdateThingShadowOperationContext : public OperationModelContext
        {
          public:
            UpdateThingShadowOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API UpdateThingShadowResult
        {
          public:
            UpdateThingShadowResult() noexcept {}
            UpdateThingShadowResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
            UpdateThingShadowResponse *GetOperationResponse() const noexcept
            {
                return static_cast<UpdateThingShadowResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API UpdateThingShadowOperation : public ClientOperation
        {
          public:
            UpdateThingShadowOperation(
                ClientConnection &connection,
                const UpdateThingShadowOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `UpdateThingShadowOperation`
             * @param request The request used for the `UpdateThingShadowOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const UpdateThingShadowRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<UpdateThingShadowResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API UpdateConfigurationOperationContext : public OperationModelContext
        {
          public:
            UpdateConfigurationOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API UpdateConfigurationResult
        {
          public:
            UpdateConfigurationResult() noexcept {}
            UpdateConfigurationResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
            UpdateConfigurationResponse *GetOperationResponse() const noexcept
            {
                return static_cast<UpdateConfigurationResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API UpdateConfigurationOperation : public ClientOperation
        {
          public:
            UpdateConfigurationOperation(
                ClientConnection &connection,
                const UpdateConfigurationOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `UpdateConfigurationOperation`
             * @param request The request used for the `UpdateConfigurationOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const UpdateConfigurationRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<UpdateConfigurationResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API ValidateAuthorizationTokenOperationContext : public OperationModelContext
        {
          public:
            ValidateAuthorizationTokenOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API ValidateAuthorizationTokenResult
        {
          public:
            ValidateAuthorizationTokenResult() noexcept {}
            ValidateAuthorizationTokenResult(TaggedResult &&taggedResult) noexcept
                : m_taggedResult(std::move(taggedResult))
            {
            }
            ValidateAuthorizationTokenResponse *GetOperationResponse() const noexcept
            {
                return static_cast<ValidateAuthorizationTokenResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API ValidateAuthorizationTokenOperation : public ClientOperation
        {
          public:
            ValidateAuthorizationTokenOperation(
                ClientConnection &connection,
                const ValidateAuthorizationTokenOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `ValidateAuthorizationTokenOperation`
             * @param request The request used for the `ValidateAuthorizationTokenOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const ValidateAuthorizationTokenRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<ValidateAuthorizationTokenResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API RestartComponentOperationContext : public OperationModelContext
        {
          public:
            RestartComponentOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API RestartComponentResult
        {
          public:
            RestartComponentResult() noexcept {}
            RestartComponentResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
            RestartComponentResponse *GetOperationResponse() const noexcept
            {
                return static_cast<RestartComponentResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API RestartComponentOperation : public ClientOperation
        {
          public:
            RestartComponentOperation(
                ClientConnection &connection,
                const RestartComponentOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `RestartComponentOperation`
             * @param request The request used for the `RestartComponentOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const RestartComponentRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<RestartComponentResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API GetLocalDeploymentStatusOperationContext : public OperationModelContext
        {
          public:
            GetLocalDeploymentStatusOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API GetLocalDeploymentStatusResult
        {
          public:
            GetLocalDeploymentStatusResult() noexcept {}
            GetLocalDeploymentStatusResult(TaggedResult &&taggedResult) noexcept
                : m_taggedResult(std::move(taggedResult))
            {
            }
            GetLocalDeploymentStatusResponse *GetOperationResponse() const noexcept
            {
                return static_cast<GetLocalDeploymentStatusResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API GetLocalDeploymentStatusOperation : public ClientOperation
        {
          public:
            GetLocalDeploymentStatusOperation(
                ClientConnection &connection,
                const GetLocalDeploymentStatusOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `GetLocalDeploymentStatusOperation`
             * @param request The request used for the `GetLocalDeploymentStatusOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const GetLocalDeploymentStatusRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<GetLocalDeploymentStatusResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API GetSecretValueOperationContext : public OperationModelContext
        {
          public:
            GetSecretValueOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API GetSecretValueResult
        {
          public:
            GetSecretValueResult() noexcept {}
            GetSecretValueResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
            GetSecretValueResponse *GetOperationResponse() const noexcept
            {
                return static_cast<GetSecretValueResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API GetSecretValueOperation : public ClientOperation
        {
          public:
            GetSecretValueOperation(
                ClientConnection &connection,
                const GetSecretValueOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `GetSecretValueOperation`
             * @param request The request used for the `GetSecretValueOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const GetSecretValueRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<GetSecretValueResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API UpdateStateOperationContext : public OperationModelContext
        {
          public:
            UpdateStateOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API UpdateStateResult
        {
          public:
            UpdateStateResult() noexcept {}
            UpdateStateResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
            UpdateStateResponse *GetOperationResponse() const noexcept
            {
                return static_cast<UpdateStateResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API UpdateStateOperation : public ClientOperation
        {
          public:
            UpdateStateOperation(
                ClientConnection &connection,
                const UpdateStateOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `UpdateStateOperation`
             * @param request The request used for the `UpdateStateOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const UpdateStateRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<UpdateStateResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API CancelLocalDeploymentOperationContext : public OperationModelContext
        {
          public:
            CancelLocalDeploymentOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API CancelLocalDeploymentResult
        {
          public:
            CancelLocalDeploymentResult() noexcept {}
            CancelLocalDeploymentResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult))
            {
            }
            CancelLocalDeploymentResponse *GetOperationResponse() const noexcept
            {
                return static_cast<CancelLocalDeploymentResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API CancelLocalDeploymentOperation : public ClientOperation
        {
          public:
            CancelLocalDeploymentOperation(
                ClientConnection &connection,
                const CancelLocalDeploymentOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `CancelLocalDeploymentOperation`
             * @param request The request used for the `CancelLocalDeploymentOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const CancelLocalDeploymentRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<CancelLocalDeploymentResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API ListNamedShadowsForThingOperationContext : public OperationModelContext
        {
          public:
            ListNamedShadowsForThingOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API ListNamedShadowsForThingResult
        {
          public:
            ListNamedShadowsForThingResult() noexcept {}
            ListNamedShadowsForThingResult(TaggedResult &&taggedResult) noexcept
                : m_taggedResult(std::move(taggedResult))
            {
            }
            ListNamedShadowsForThingResponse *GetOperationResponse() const noexcept
            {
                return static_cast<ListNamedShadowsForThingResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API ListNamedShadowsForThingOperation : public ClientOperation
        {
          public:
            ListNamedShadowsForThingOperation(
                ClientConnection &connection,
                const ListNamedShadowsForThingOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `ListNamedShadowsForThingOperation`
             * @param request The request used for the `ListNamedShadowsForThingOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const ListNamedShadowsForThingRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<ListNamedShadowsForThingResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToComponentUpdatesStreamHandler : public StreamResponseHandler
        {
          public:
            virtual void OnStreamEvent(ComponentUpdatePolicyEvents *response) { (void)response; }

            /**
             * A callback that is invoked when an error occurs while parsing a message from the stream.
             * @param rpcError The RPC error containing the status and possibly a CRT error.
             */
            virtual bool OnStreamError(RpcError rpcError)
            {
                (void)rpcError;
                return true;
            }

            /**
             * A callback that is invoked upon receiving an error of type `ServiceError`.
             * @param operationError The error message being received.
             */
            virtual bool OnStreamError(ServiceError *operationError)
            {
                (void)operationError;
                return true;
            }

            /**
             * A callback that is invoked upon receiving an error of type `ResourceNotFoundError`.
             * @param operationError The error message being received.
             */
            virtual bool OnStreamError(ResourceNotFoundError *operationError)
            {
                (void)operationError;
                return true;
            }

            /**
             * A callback that is invoked upon receiving ANY error response from the server.
             * @param operationError The error message being received.
             */
            virtual bool OnStreamError(OperationError *operationError)
            {
                (void)operationError;
                return true;
            }

          private:
            /**
             * Invoked when a message is received on this continuation.
             */
            void OnStreamEvent(Aws::Crt::ScopedResource<AbstractShapeBase> response) override;
            /**
             * Invoked when a message is received on this continuation but results in an error.
             *
             * This callback can return true so that the stream is closed afterwards.
             */
            bool OnStreamError(Aws::Crt::ScopedResource<OperationError> error, RpcError rpcError) override;
        };
        class AWS_GREENGRASSCOREIPC_API SubscribeToComponentUpdatesOperationContext : public OperationModelContext
        {
          public:
            SubscribeToComponentUpdatesOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToComponentUpdatesResult
        {
          public:
            SubscribeToComponentUpdatesResult() noexcept {}
            SubscribeToComponentUpdatesResult(TaggedResult &&taggedResult) noexcept
                : m_taggedResult(std::move(taggedResult))
            {
            }
            SubscribeToComponentUpdatesResponse *GetOperationResponse() const noexcept
            {
                return static_cast<SubscribeToComponentUpdatesResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API SubscribeToComponentUpdatesOperation : public ClientOperation
        {
          public:
            SubscribeToComponentUpdatesOperation(
                ClientConnection &connection,
                std::shared_ptr<SubscribeToComponentUpdatesStreamHandler> streamHandler,
                const SubscribeToComponentUpdatesOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `SubscribeToComponentUpdatesOperation`
             * @param request The request used for the `SubscribeToComponentUpdatesOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const SubscribeToComponentUpdatesRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<SubscribeToComponentUpdatesResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API ListLocalDeploymentsOperationContext : public OperationModelContext
        {
          public:
            ListLocalDeploymentsOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API ListLocalDeploymentsResult
        {
          public:
            ListLocalDeploymentsResult() noexcept {}
            ListLocalDeploymentsResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult))
            {
            }
            ListLocalDeploymentsResponse *GetOperationResponse() const noexcept
            {
                return static_cast<ListLocalDeploymentsResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API ListLocalDeploymentsOperation : public ClientOperation
        {
          public:
            ListLocalDeploymentsOperation(
                ClientConnection &connection,
                const ListLocalDeploymentsOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `ListLocalDeploymentsOperation`
             * @param request The request used for the `ListLocalDeploymentsOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const ListLocalDeploymentsRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<ListLocalDeploymentsResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API StopComponentOperationContext : public OperationModelContext
        {
          public:
            StopComponentOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API StopComponentResult
        {
          public:
            StopComponentResult() noexcept {}
            StopComponentResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
            StopComponentResponse *GetOperationResponse() const noexcept
            {
                return static_cast<StopComponentResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API StopComponentOperation : public ClientOperation
        {
          public:
            StopComponentOperation(
                ClientConnection &connection,
                const StopComponentOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `StopComponentOperation`
             * @param request The request used for the `StopComponentOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const StopComponentRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<StopComponentResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API PauseComponentOperationContext : public OperationModelContext
        {
          public:
            PauseComponentOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API PauseComponentResult
        {
          public:
            PauseComponentResult() noexcept {}
            PauseComponentResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
            PauseComponentResponse *GetOperationResponse() const noexcept
            {
                return static_cast<PauseComponentResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API PauseComponentOperation : public ClientOperation
        {
          public:
            PauseComponentOperation(
                ClientConnection &connection,
                const PauseComponentOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `PauseComponentOperation`
             * @param request The request used for the `PauseComponentOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const PauseComponentRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<PauseComponentResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API CreateLocalDeploymentOperationContext : public OperationModelContext
        {
          public:
            CreateLocalDeploymentOperationContext(const GreengrassCoreIpcServiceModel &serviceModel) noexcept;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            Aws::Crt::String GetRequestModelName() const noexcept override;
            Aws::Crt::String GetInitialResponseModelName() const noexcept override;
            Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
            Aws::Crt::String GetOperationName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API CreateLocalDeploymentResult
        {
          public:
            CreateLocalDeploymentResult() noexcept {}
            CreateLocalDeploymentResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult))
            {
            }
            CreateLocalDeploymentResponse *GetOperationResponse() const noexcept
            {
                return static_cast<CreateLocalDeploymentResponse *>(m_taggedResult.GetOperationResponse());
            }
            /**
             * @return true if the response is associated with an expected response;
             * false if the response is associated with an error.
             */
            operator bool() const noexcept { return m_taggedResult == true; }
            OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
            RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
            ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

          private:
            TaggedResult m_taggedResult;
        };

        class AWS_GREENGRASSCOREIPC_API CreateLocalDeploymentOperation : public ClientOperation
        {
          public:
            CreateLocalDeploymentOperation(
                ClientConnection &connection,
                const CreateLocalDeploymentOperationContext &operationContext,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Used to activate a stream for the `CreateLocalDeploymentOperation`
             * @param request The request used for the `CreateLocalDeploymentOperation`
             * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
             * @return An `RpcError` that can be used to check whether the stream was activated.
             */
            std::future<RpcError> Activate(
                const CreateLocalDeploymentRequest &request,
                OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
            /**
             * Retrieve the result from activating the stream.
             */
            std::future<CreateLocalDeploymentResult> GetResult() noexcept;

          protected:
            Aws::Crt::String GetModelName() const noexcept override;
        };

        class AWS_GREENGRASSCOREIPC_API GreengrassCoreIpcServiceModel : public ServiceModel
        {
          public:
            GreengrassCoreIpcServiceModel() noexcept;
            Aws::Crt::ScopedResource<OperationError> AllocateOperationErrorFromPayload(
                const Aws::Crt::String &errorModelName,
                Aws::Crt::StringView stringView,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
            void AssignModelNameToErrorResponse(Aws::Crt::String, ErrorResponseFactory) noexcept;

          private:
            friend class GreengrassCoreIpcClient;
            SubscribeToIoTCoreOperationContext m_subscribeToIoTCoreOperationContext;
            ResumeComponentOperationContext m_resumeComponentOperationContext;
            PublishToIoTCoreOperationContext m_publishToIoTCoreOperationContext;
            SubscribeToConfigurationUpdateOperationContext m_subscribeToConfigurationUpdateOperationContext;
            DeleteThingShadowOperationContext m_deleteThingShadowOperationContext;
            PutComponentMetricOperationContext m_putComponentMetricOperationContext;
            DeferComponentUpdateOperationContext m_deferComponentUpdateOperationContext;
            SubscribeToValidateConfigurationUpdatesOperationContext
                m_subscribeToValidateConfigurationUpdatesOperationContext;
            GetConfigurationOperationContext m_getConfigurationOperationContext;
            SubscribeToTopicOperationContext m_subscribeToTopicOperationContext;
            GetComponentDetailsOperationContext m_getComponentDetailsOperationContext;
            GetClientDeviceAuthTokenOperationContext m_getClientDeviceAuthTokenOperationContext;
            PublishToTopicOperationContext m_publishToTopicOperationContext;
            SubscribeToCertificateUpdatesOperationContext m_subscribeToCertificateUpdatesOperationContext;
            VerifyClientDeviceIdentityOperationContext m_verifyClientDeviceIdentityOperationContext;
            AuthorizeClientDeviceActionOperationContext m_authorizeClientDeviceActionOperationContext;
            ListComponentsOperationContext m_listComponentsOperationContext;
            CreateDebugPasswordOperationContext m_createDebugPasswordOperationContext;
            GetThingShadowOperationContext m_getThingShadowOperationContext;
            SendConfigurationValidityReportOperationContext m_sendConfigurationValidityReportOperationContext;
            UpdateThingShadowOperationContext m_updateThingShadowOperationContext;
            UpdateConfigurationOperationContext m_updateConfigurationOperationContext;
            ValidateAuthorizationTokenOperationContext m_validateAuthorizationTokenOperationContext;
            RestartComponentOperationContext m_restartComponentOperationContext;
            GetLocalDeploymentStatusOperationContext m_getLocalDeploymentStatusOperationContext;
            GetSecretValueOperationContext m_getSecretValueOperationContext;
            UpdateStateOperationContext m_updateStateOperationContext;
            CancelLocalDeploymentOperationContext m_cancelLocalDeploymentOperationContext;
            ListNamedShadowsForThingOperationContext m_listNamedShadowsForThingOperationContext;
            SubscribeToComponentUpdatesOperationContext m_subscribeToComponentUpdatesOperationContext;
            ListLocalDeploymentsOperationContext m_listLocalDeploymentsOperationContext;
            StopComponentOperationContext m_stopComponentOperationContext;
            PauseComponentOperationContext m_pauseComponentOperationContext;
            CreateLocalDeploymentOperationContext m_createLocalDeploymentOperationContext;
            Aws::Crt::Map<Aws::Crt::String, ErrorResponseFactory> m_modelNameToErrorResponse;
        };
    } // namespace Greengrass
} // namespace Aws
