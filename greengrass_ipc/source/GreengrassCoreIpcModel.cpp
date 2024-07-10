/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

/* This file is generated. */

#include <aws/crt/Api.h>
#include <aws/greengrass/GreengrassCoreIpcModel.h>

namespace Aws
{
    namespace Greengrass
    {
        void UserProperty::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_key.has_value())
            {
                payloadObject.WithString("key", m_key.value());
            }
            if (m_value.has_value())
            {
                payloadObject.WithString("value", m_value.value());
            }
        }

        void UserProperty::s_loadFromJsonView(UserProperty &userProperty, const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("key"))
            {
                userProperty.m_key = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("key"));
            }
            if (jsonView.ValueExists("value"))
            {
                userProperty.m_value = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("value"));
            }
        }

        const char *UserProperty::MODEL_NAME = "aws.greengrass#UserProperty";

        Aws::Crt::String UserProperty::GetModelName() const noexcept { return UserProperty::MODEL_NAME; }

        Aws::Crt::ScopedResource<AbstractShapeBase> UserProperty::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<UserProperty> shape(
                Aws::Crt::New<UserProperty>(allocator), UserProperty::s_customDeleter);
            shape->m_allocator = allocator;
            UserProperty::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void UserProperty::s_customDeleter(UserProperty *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void MessageContext::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_topic.has_value())
            {
                payloadObject.WithString("topic", m_topic.value());
            }
        }

        void MessageContext::s_loadFromJsonView(
            MessageContext &messageContext,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("topic"))
            {
                messageContext.m_topic = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("topic"));
            }
        }

        const char *MessageContext::MODEL_NAME = "aws.greengrass#MessageContext";

        Aws::Crt::String MessageContext::GetModelName() const noexcept { return MessageContext::MODEL_NAME; }

        Aws::Crt::ScopedResource<AbstractShapeBase> MessageContext::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<MessageContext> shape(
                Aws::Crt::New<MessageContext>(allocator), MessageContext::s_customDeleter);
            shape->m_allocator = allocator;
            MessageContext::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void MessageContext::s_customDeleter(MessageContext *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void DeploymentStatusDetails::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_detailedDeploymentStatus.has_value())
            {
                payloadObject.WithString("detailedDeploymentStatus", m_detailedDeploymentStatus.value());
            }
            if (m_deploymentErrorStack.has_value())
            {
                Aws::Crt::JsonObject deploymentErrorStack;
                Aws::Crt::Vector<Aws::Crt::JsonObject> deploymentErrorStackJsonArray;
                for (const auto &deploymentErrorStackItem : m_deploymentErrorStack.value())
                {
                    Aws::Crt::JsonObject deploymentErrorStackJsonArrayItem;
                    deploymentErrorStackJsonArrayItem.AsString(deploymentErrorStackItem);
                    deploymentErrorStackJsonArray.emplace_back(std::move(deploymentErrorStackJsonArrayItem));
                }
                deploymentErrorStack.AsArray(std::move(deploymentErrorStackJsonArray));
                payloadObject.WithObject("deploymentErrorStack", std::move(deploymentErrorStack));
            }
            if (m_deploymentErrorTypes.has_value())
            {
                Aws::Crt::JsonObject deploymentErrorTypes;
                Aws::Crt::Vector<Aws::Crt::JsonObject> deploymentErrorTypesJsonArray;
                for (const auto &deploymentErrorTypesItem : m_deploymentErrorTypes.value())
                {
                    Aws::Crt::JsonObject deploymentErrorTypesJsonArrayItem;
                    deploymentErrorTypesJsonArrayItem.AsString(deploymentErrorTypesItem);
                    deploymentErrorTypesJsonArray.emplace_back(std::move(deploymentErrorTypesJsonArrayItem));
                }
                deploymentErrorTypes.AsArray(std::move(deploymentErrorTypesJsonArray));
                payloadObject.WithObject("deploymentErrorTypes", std::move(deploymentErrorTypes));
            }
            if (m_deploymentFailureCause.has_value())
            {
                payloadObject.WithString("deploymentFailureCause", m_deploymentFailureCause.value());
            }
        }

        void DeploymentStatusDetails::s_loadFromJsonView(
            DeploymentStatusDetails &deploymentStatusDetails,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("detailedDeploymentStatus"))
            {
                deploymentStatusDetails.m_detailedDeploymentStatus =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("detailedDeploymentStatus"));
            }
            if (jsonView.ValueExists("deploymentErrorStack"))
            {
                deploymentStatusDetails.m_deploymentErrorStack = Aws::Crt::Vector<Aws::Crt::String>();
                for (const Aws::Crt::JsonView &deploymentErrorStackJsonView : jsonView.GetArray("deploymentErrorStack"))
                {
                    Aws::Crt::Optional<Aws::Crt::String> deploymentErrorStackItem;
                    deploymentErrorStackItem =
                        Aws::Crt::Optional<Aws::Crt::String>(deploymentErrorStackJsonView.AsString());
                    deploymentStatusDetails.m_deploymentErrorStack.value().push_back(deploymentErrorStackItem.value());
                }
            }
            if (jsonView.ValueExists("deploymentErrorTypes"))
            {
                deploymentStatusDetails.m_deploymentErrorTypes = Aws::Crt::Vector<Aws::Crt::String>();
                for (const Aws::Crt::JsonView &deploymentErrorTypesJsonView : jsonView.GetArray("deploymentErrorTypes"))
                {
                    Aws::Crt::Optional<Aws::Crt::String> deploymentErrorTypesItem;
                    deploymentErrorTypesItem =
                        Aws::Crt::Optional<Aws::Crt::String>(deploymentErrorTypesJsonView.AsString());
                    deploymentStatusDetails.m_deploymentErrorTypes.value().push_back(deploymentErrorTypesItem.value());
                }
            }
            if (jsonView.ValueExists("deploymentFailureCause"))
            {
                deploymentStatusDetails.m_deploymentFailureCause =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("deploymentFailureCause"));
            }
        }

        void DeploymentStatusDetails::SetDetailedDeploymentStatus(
            DetailedDeploymentStatus detailedDeploymentStatus) noexcept
        {
            switch (detailedDeploymentStatus)
            {
                case DETAILED_DEPLOYMENT_STATUS_SUCCESSFUL:
                    m_detailedDeploymentStatus = Aws::Crt::String("SUCCESSFUL");
                    break;
                case DETAILED_DEPLOYMENT_STATUS_FAILED_NO_STATE_CHANGE:
                    m_detailedDeploymentStatus = Aws::Crt::String("FAILED_NO_STATE_CHANGE");
                    break;
                case DETAILED_DEPLOYMENT_STATUS_FAILED_ROLLBACK_NOT_REQUESTED:
                    m_detailedDeploymentStatus = Aws::Crt::String("FAILED_ROLLBACK_NOT_REQUESTED");
                    break;
                case DETAILED_DEPLOYMENT_STATUS_FAILED_ROLLBACK_COMPLETE:
                    m_detailedDeploymentStatus = Aws::Crt::String("FAILED_ROLLBACK_COMPLETE");
                    break;
                case DETAILED_DEPLOYMENT_STATUS_REJECTED:
                    m_detailedDeploymentStatus = Aws::Crt::String("REJECTED");
                    break;
                default:
                    break;
            }
        }

        Aws::Crt::Optional<DetailedDeploymentStatus> DeploymentStatusDetails::GetDetailedDeploymentStatus() noexcept
        {
            if (!m_detailedDeploymentStatus.has_value())
                return Aws::Crt::Optional<DetailedDeploymentStatus>();
            if (m_detailedDeploymentStatus.value() == Aws::Crt::String("SUCCESSFUL"))
            {
                return Aws::Crt::Optional<DetailedDeploymentStatus>(DETAILED_DEPLOYMENT_STATUS_SUCCESSFUL);
            }
            if (m_detailedDeploymentStatus.value() == Aws::Crt::String("FAILED_NO_STATE_CHANGE"))
            {
                return Aws::Crt::Optional<DetailedDeploymentStatus>(DETAILED_DEPLOYMENT_STATUS_FAILED_NO_STATE_CHANGE);
            }
            if (m_detailedDeploymentStatus.value() == Aws::Crt::String("FAILED_ROLLBACK_NOT_REQUESTED"))
            {
                return Aws::Crt::Optional<DetailedDeploymentStatus>(
                    DETAILED_DEPLOYMENT_STATUS_FAILED_ROLLBACK_NOT_REQUESTED);
            }
            if (m_detailedDeploymentStatus.value() == Aws::Crt::String("FAILED_ROLLBACK_COMPLETE"))
            {
                return Aws::Crt::Optional<DetailedDeploymentStatus>(
                    DETAILED_DEPLOYMENT_STATUS_FAILED_ROLLBACK_COMPLETE);
            }
            if (m_detailedDeploymentStatus.value() == Aws::Crt::String("REJECTED"))
            {
                return Aws::Crt::Optional<DetailedDeploymentStatus>(DETAILED_DEPLOYMENT_STATUS_REJECTED);
            }

            return Aws::Crt::Optional<DetailedDeploymentStatus>();
        }

        const char *DeploymentStatusDetails::MODEL_NAME = "aws.greengrass#DeploymentStatusDetails";

        Aws::Crt::String DeploymentStatusDetails::GetModelName() const noexcept
        {
            return DeploymentStatusDetails::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> DeploymentStatusDetails::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<DeploymentStatusDetails> shape(
                Aws::Crt::New<DeploymentStatusDetails>(allocator), DeploymentStatusDetails::s_customDeleter);
            shape->m_allocator = allocator;
            DeploymentStatusDetails::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void DeploymentStatusDetails::s_customDeleter(DeploymentStatusDetails *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void SystemResourceLimits::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_memory.has_value())
            {
                payloadObject.WithInt64("memory", m_memory.value());
            }
            if (m_cpus.has_value())
            {
                payloadObject.WithDouble("cpus", m_cpus.value());
            }
        }

        void SystemResourceLimits::s_loadFromJsonView(
            SystemResourceLimits &systemResourceLimits,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("memory"))
            {
                systemResourceLimits.m_memory = Aws::Crt::Optional<int64_t>(jsonView.GetInt64("memory"));
            }
            if (jsonView.ValueExists("cpus"))
            {
                systemResourceLimits.m_cpus = Aws::Crt::Optional<double>(jsonView.GetDouble("cpus"));
            }
        }

        const char *SystemResourceLimits::MODEL_NAME = "aws.greengrass#SystemResourceLimits";

        Aws::Crt::String SystemResourceLimits::GetModelName() const noexcept
        {
            return SystemResourceLimits::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SystemResourceLimits::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<SystemResourceLimits> shape(
                Aws::Crt::New<SystemResourceLimits>(allocator), SystemResourceLimits::s_customDeleter);
            shape->m_allocator = allocator;
            SystemResourceLimits::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void SystemResourceLimits::s_customDeleter(SystemResourceLimits *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void ValidateConfigurationUpdateEvent::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_configuration.has_value())
            {
                payloadObject.WithObject("configuration", m_configuration.value());
            }
            if (m_deploymentId.has_value())
            {
                payloadObject.WithString("deploymentId", m_deploymentId.value());
            }
        }

        void ValidateConfigurationUpdateEvent::s_loadFromJsonView(
            ValidateConfigurationUpdateEvent &validateConfigurationUpdateEvent,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("configuration"))
            {
                validateConfigurationUpdateEvent.m_configuration =
                    Aws::Crt::Optional<Aws::Crt::JsonObject>(jsonView.GetJsonObject("configuration").Materialize());
            }
            if (jsonView.ValueExists("deploymentId"))
            {
                validateConfigurationUpdateEvent.m_deploymentId =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("deploymentId"));
            }
        }

        const char *ValidateConfigurationUpdateEvent::MODEL_NAME = "aws.greengrass#ValidateConfigurationUpdateEvent";

        Aws::Crt::String ValidateConfigurationUpdateEvent::GetModelName() const noexcept
        {
            return ValidateConfigurationUpdateEvent::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> ValidateConfigurationUpdateEvent::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<ValidateConfigurationUpdateEvent> shape(
                Aws::Crt::New<ValidateConfigurationUpdateEvent>(allocator),
                ValidateConfigurationUpdateEvent::s_customDeleter);
            shape->m_allocator = allocator;
            ValidateConfigurationUpdateEvent::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void ValidateConfigurationUpdateEvent::s_customDeleter(ValidateConfigurationUpdateEvent *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void BinaryMessage::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_message.has_value())
            {
                if (m_message.value().size() > 0)
                {
                    payloadObject.WithString("message", Aws::Crt::Base64Encode(m_message.value()));
                }
            }
            if (m_context.has_value())
            {
                Aws::Crt::JsonObject messageContextValue;
                m_context.value().SerializeToJsonObject(messageContextValue);
                payloadObject.WithObject("context", std::move(messageContextValue));
            }
        }

        void BinaryMessage::s_loadFromJsonView(
            BinaryMessage &binaryMessage,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("message"))
            {
                if (jsonView.GetString("message").size() > 0)
                {
                    binaryMessage.m_message = Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>>(
                        Aws::Crt::Base64Decode(jsonView.GetString("message")));
                }
            }
            if (jsonView.ValueExists("context"))
            {
                binaryMessage.m_context = MessageContext();
                MessageContext::s_loadFromJsonView(binaryMessage.m_context.value(), jsonView.GetJsonObject("context"));
            }
        }

        const char *BinaryMessage::MODEL_NAME = "aws.greengrass#BinaryMessage";

        Aws::Crt::String BinaryMessage::GetModelName() const noexcept { return BinaryMessage::MODEL_NAME; }

        Aws::Crt::ScopedResource<AbstractShapeBase> BinaryMessage::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<BinaryMessage> shape(
                Aws::Crt::New<BinaryMessage>(allocator), BinaryMessage::s_customDeleter);
            shape->m_allocator = allocator;
            BinaryMessage::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void BinaryMessage::s_customDeleter(BinaryMessage *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void JsonMessage::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_message.has_value())
            {
                payloadObject.WithObject("message", m_message.value());
            }
            if (m_context.has_value())
            {
                Aws::Crt::JsonObject messageContextValue;
                m_context.value().SerializeToJsonObject(messageContextValue);
                payloadObject.WithObject("context", std::move(messageContextValue));
            }
        }

        void JsonMessage::s_loadFromJsonView(JsonMessage &jsonMessage, const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("message"))
            {
                jsonMessage.m_message =
                    Aws::Crt::Optional<Aws::Crt::JsonObject>(jsonView.GetJsonObject("message").Materialize());
            }
            if (jsonView.ValueExists("context"))
            {
                jsonMessage.m_context = MessageContext();
                MessageContext::s_loadFromJsonView(jsonMessage.m_context.value(), jsonView.GetJsonObject("context"));
            }
        }

        const char *JsonMessage::MODEL_NAME = "aws.greengrass#JsonMessage";

        Aws::Crt::String JsonMessage::GetModelName() const noexcept { return JsonMessage::MODEL_NAME; }

        Aws::Crt::ScopedResource<AbstractShapeBase> JsonMessage::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<JsonMessage> shape(
                Aws::Crt::New<JsonMessage>(allocator), JsonMessage::s_customDeleter);
            shape->m_allocator = allocator;
            JsonMessage::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void JsonMessage::s_customDeleter(JsonMessage *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void MQTTMessage::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_topicName.has_value())
            {
                payloadObject.WithString("topicName", m_topicName.value());
            }
            if (m_payload.has_value())
            {
                if (m_payload.value().size() > 0)
                {
                    payloadObject.WithString("payload", Aws::Crt::Base64Encode(m_payload.value()));
                }
            }
            if (m_retain.has_value())
            {
                payloadObject.WithBool("retain", m_retain.value());
            }
            if (m_userProperties.has_value())
            {
                Aws::Crt::JsonObject listOfUserProperties;
                Aws::Crt::Vector<Aws::Crt::JsonObject> listOfUserPropertiesJsonArray;
                for (const auto &listOfUserPropertiesItem : m_userProperties.value())
                {
                    Aws::Crt::JsonObject listOfUserPropertiesJsonArrayItem;
                    listOfUserPropertiesItem.SerializeToJsonObject(listOfUserPropertiesJsonArrayItem);
                    listOfUserPropertiesJsonArray.emplace_back(std::move(listOfUserPropertiesJsonArrayItem));
                }
                listOfUserProperties.AsArray(std::move(listOfUserPropertiesJsonArray));
                payloadObject.WithObject("userProperties", std::move(listOfUserProperties));
            }
            if (m_messageExpiryIntervalSeconds.has_value())
            {
                payloadObject.WithInt64("messageExpiryIntervalSeconds", m_messageExpiryIntervalSeconds.value());
            }
            if (m_correlationData.has_value())
            {
                if (m_correlationData.value().size() > 0)
                {
                    payloadObject.WithString("correlationData", Aws::Crt::Base64Encode(m_correlationData.value()));
                }
            }
            if (m_responseTopic.has_value())
            {
                payloadObject.WithString("responseTopic", m_responseTopic.value());
            }
            if (m_payloadFormat.has_value())
            {
                payloadObject.WithString("payloadFormat", m_payloadFormat.value());
            }
            if (m_contentType.has_value())
            {
                payloadObject.WithString("contentType", m_contentType.value());
            }
        }

        void MQTTMessage::s_loadFromJsonView(MQTTMessage &mQTTMessage, const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("topicName"))
            {
                mQTTMessage.m_topicName = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("topicName"));
            }
            if (jsonView.ValueExists("payload"))
            {
                if (jsonView.GetString("payload").size() > 0)
                {
                    mQTTMessage.m_payload = Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>>(
                        Aws::Crt::Base64Decode(jsonView.GetString("payload")));
                }
            }
            if (jsonView.ValueExists("retain"))
            {
                mQTTMessage.m_retain = Aws::Crt::Optional<bool>(jsonView.GetBool("retain"));
            }
            if (jsonView.ValueExists("userProperties"))
            {
                mQTTMessage.m_userProperties = Aws::Crt::Vector<UserProperty>();
                for (const Aws::Crt::JsonView &listOfUserPropertiesJsonView : jsonView.GetArray("userProperties"))
                {
                    Aws::Crt::Optional<UserProperty> listOfUserPropertiesItem;
                    listOfUserPropertiesItem = UserProperty();
                    UserProperty::s_loadFromJsonView(listOfUserPropertiesItem.value(), listOfUserPropertiesJsonView);
                    mQTTMessage.m_userProperties.value().push_back(listOfUserPropertiesItem.value());
                }
            }
            if (jsonView.ValueExists("messageExpiryIntervalSeconds"))
            {
                mQTTMessage.m_messageExpiryIntervalSeconds =
                    Aws::Crt::Optional<int64_t>(jsonView.GetInt64("messageExpiryIntervalSeconds"));
            }
            if (jsonView.ValueExists("correlationData"))
            {
                if (jsonView.GetString("correlationData").size() > 0)
                {
                    mQTTMessage.m_correlationData = Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>>(
                        Aws::Crt::Base64Decode(jsonView.GetString("correlationData")));
                }
            }
            if (jsonView.ValueExists("responseTopic"))
            {
                mQTTMessage.m_responseTopic = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("responseTopic"));
            }
            if (jsonView.ValueExists("payloadFormat"))
            {
                mQTTMessage.m_payloadFormat = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("payloadFormat"));
            }
            if (jsonView.ValueExists("contentType"))
            {
                mQTTMessage.m_contentType = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("contentType"));
            }
        }

        void MQTTMessage::SetPayloadFormat(PayloadFormat payloadFormat) noexcept
        {
            switch (payloadFormat)
            {
                case PAYLOAD_FORMAT_BYTES:
                    m_payloadFormat = Aws::Crt::String("0");
                    break;
                case PAYLOAD_FORMAT_UTF8:
                    m_payloadFormat = Aws::Crt::String("1");
                    break;
                default:
                    break;
            }
        }

        Aws::Crt::Optional<PayloadFormat> MQTTMessage::GetPayloadFormat() noexcept
        {
            if (!m_payloadFormat.has_value())
                return Aws::Crt::Optional<PayloadFormat>();
            if (m_payloadFormat.value() == Aws::Crt::String("0"))
            {
                return Aws::Crt::Optional<PayloadFormat>(PAYLOAD_FORMAT_BYTES);
            }
            if (m_payloadFormat.value() == Aws::Crt::String("1"))
            {
                return Aws::Crt::Optional<PayloadFormat>(PAYLOAD_FORMAT_UTF8);
            }

            return Aws::Crt::Optional<PayloadFormat>();
        }

        const char *MQTTMessage::MODEL_NAME = "aws.greengrass#MQTTMessage";

        Aws::Crt::String MQTTMessage::GetModelName() const noexcept { return MQTTMessage::MODEL_NAME; }

        Aws::Crt::ScopedResource<AbstractShapeBase> MQTTMessage::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<MQTTMessage> shape(
                Aws::Crt::New<MQTTMessage>(allocator), MQTTMessage::s_customDeleter);
            shape->m_allocator = allocator;
            MQTTMessage::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void MQTTMessage::s_customDeleter(MQTTMessage *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void ConfigurationUpdateEvent::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_componentName.has_value())
            {
                payloadObject.WithString("componentName", m_componentName.value());
            }
            if (m_keyPath.has_value())
            {
                Aws::Crt::JsonObject keyPath;
                Aws::Crt::Vector<Aws::Crt::JsonObject> keyPathJsonArray;
                for (const auto &keyPathItem : m_keyPath.value())
                {
                    Aws::Crt::JsonObject keyPathJsonArrayItem;
                    keyPathJsonArrayItem.AsString(keyPathItem);
                    keyPathJsonArray.emplace_back(std::move(keyPathJsonArrayItem));
                }
                keyPath.AsArray(std::move(keyPathJsonArray));
                payloadObject.WithObject("keyPath", std::move(keyPath));
            }
        }

        void ConfigurationUpdateEvent::s_loadFromJsonView(
            ConfigurationUpdateEvent &configurationUpdateEvent,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("componentName"))
            {
                configurationUpdateEvent.m_componentName =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("componentName"));
            }
            if (jsonView.ValueExists("keyPath"))
            {
                configurationUpdateEvent.m_keyPath = Aws::Crt::Vector<Aws::Crt::String>();
                for (const Aws::Crt::JsonView &keyPathJsonView : jsonView.GetArray("keyPath"))
                {
                    Aws::Crt::Optional<Aws::Crt::String> keyPathItem;
                    keyPathItem = Aws::Crt::Optional<Aws::Crt::String>(keyPathJsonView.AsString());
                    configurationUpdateEvent.m_keyPath.value().push_back(keyPathItem.value());
                }
            }
        }

        const char *ConfigurationUpdateEvent::MODEL_NAME = "aws.greengrass#ConfigurationUpdateEvent";

        Aws::Crt::String ConfigurationUpdateEvent::GetModelName() const noexcept
        {
            return ConfigurationUpdateEvent::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> ConfigurationUpdateEvent::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<ConfigurationUpdateEvent> shape(
                Aws::Crt::New<ConfigurationUpdateEvent>(allocator), ConfigurationUpdateEvent::s_customDeleter);
            shape->m_allocator = allocator;
            ConfigurationUpdateEvent::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void ConfigurationUpdateEvent::s_customDeleter(ConfigurationUpdateEvent *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void PostComponentUpdateEvent::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_deploymentId.has_value())
            {
                payloadObject.WithString("deploymentId", m_deploymentId.value());
            }
        }

        void PostComponentUpdateEvent::s_loadFromJsonView(
            PostComponentUpdateEvent &postComponentUpdateEvent,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("deploymentId"))
            {
                postComponentUpdateEvent.m_deploymentId =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("deploymentId"));
            }
        }

        const char *PostComponentUpdateEvent::MODEL_NAME = "aws.greengrass#PostComponentUpdateEvent";

        Aws::Crt::String PostComponentUpdateEvent::GetModelName() const noexcept
        {
            return PostComponentUpdateEvent::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> PostComponentUpdateEvent::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<PostComponentUpdateEvent> shape(
                Aws::Crt::New<PostComponentUpdateEvent>(allocator), PostComponentUpdateEvent::s_customDeleter);
            shape->m_allocator = allocator;
            PostComponentUpdateEvent::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void PostComponentUpdateEvent::s_customDeleter(PostComponentUpdateEvent *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void PreComponentUpdateEvent::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_deploymentId.has_value())
            {
                payloadObject.WithString("deploymentId", m_deploymentId.value());
            }
            if (m_isGgcRestarting.has_value())
            {
                payloadObject.WithBool("isGgcRestarting", m_isGgcRestarting.value());
            }
        }

        void PreComponentUpdateEvent::s_loadFromJsonView(
            PreComponentUpdateEvent &preComponentUpdateEvent,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("deploymentId"))
            {
                preComponentUpdateEvent.m_deploymentId =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("deploymentId"));
            }
            if (jsonView.ValueExists("isGgcRestarting"))
            {
                preComponentUpdateEvent.m_isGgcRestarting =
                    Aws::Crt::Optional<bool>(jsonView.GetBool("isGgcRestarting"));
            }
        }

        const char *PreComponentUpdateEvent::MODEL_NAME = "aws.greengrass#PreComponentUpdateEvent";

        Aws::Crt::String PreComponentUpdateEvent::GetModelName() const noexcept
        {
            return PreComponentUpdateEvent::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> PreComponentUpdateEvent::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<PreComponentUpdateEvent> shape(
                Aws::Crt::New<PreComponentUpdateEvent>(allocator), PreComponentUpdateEvent::s_customDeleter);
            shape->m_allocator = allocator;
            PreComponentUpdateEvent::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void PreComponentUpdateEvent::s_customDeleter(PreComponentUpdateEvent *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void CertificateUpdate::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_privateKey.has_value())
            {
                payloadObject.WithString("privateKey", m_privateKey.value());
            }
            if (m_publicKey.has_value())
            {
                payloadObject.WithString("publicKey", m_publicKey.value());
            }
            if (m_certificate.has_value())
            {
                payloadObject.WithString("certificate", m_certificate.value());
            }
            if (m_caCertificates.has_value())
            {
                Aws::Crt::JsonObject cACertificates;
                Aws::Crt::Vector<Aws::Crt::JsonObject> cACertificatesJsonArray;
                for (const auto &cACertificatesItem : m_caCertificates.value())
                {
                    Aws::Crt::JsonObject cACertificatesJsonArrayItem;
                    cACertificatesJsonArrayItem.AsString(cACertificatesItem);
                    cACertificatesJsonArray.emplace_back(std::move(cACertificatesJsonArrayItem));
                }
                cACertificates.AsArray(std::move(cACertificatesJsonArray));
                payloadObject.WithObject("caCertificates", std::move(cACertificates));
            }
        }

        void CertificateUpdate::s_loadFromJsonView(
            CertificateUpdate &certificateUpdate,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("privateKey"))
            {
                certificateUpdate.m_privateKey = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("privateKey"));
            }
            if (jsonView.ValueExists("publicKey"))
            {
                certificateUpdate.m_publicKey = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("publicKey"));
            }
            if (jsonView.ValueExists("certificate"))
            {
                certificateUpdate.m_certificate =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("certificate"));
            }
            if (jsonView.ValueExists("caCertificates"))
            {
                certificateUpdate.m_caCertificates = Aws::Crt::Vector<Aws::Crt::String>();
                for (const Aws::Crt::JsonView &cACertificatesJsonView : jsonView.GetArray("caCertificates"))
                {
                    Aws::Crt::Optional<Aws::Crt::String> cACertificatesItem;
                    cACertificatesItem = Aws::Crt::Optional<Aws::Crt::String>(cACertificatesJsonView.AsString());
                    certificateUpdate.m_caCertificates.value().push_back(cACertificatesItem.value());
                }
            }
        }

        const char *CertificateUpdate::MODEL_NAME = "aws.greengrass#CertificateUpdate";

        Aws::Crt::String CertificateUpdate::GetModelName() const noexcept { return CertificateUpdate::MODEL_NAME; }

        Aws::Crt::ScopedResource<AbstractShapeBase> CertificateUpdate::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<CertificateUpdate> shape(
                Aws::Crt::New<CertificateUpdate>(allocator), CertificateUpdate::s_customDeleter);
            shape->m_allocator = allocator;
            CertificateUpdate::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void CertificateUpdate::s_customDeleter(CertificateUpdate *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void Metric::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_name.has_value())
            {
                payloadObject.WithString("name", m_name.value());
            }
            if (m_unit.has_value())
            {
                payloadObject.WithString("unit", m_unit.value());
            }
            if (m_value.has_value())
            {
                payloadObject.WithDouble("value", m_value.value());
            }
        }

        void Metric::s_loadFromJsonView(Metric &metric, const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("name"))
            {
                metric.m_name = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("name"));
            }
            if (jsonView.ValueExists("unit"))
            {
                metric.m_unit = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("unit"));
            }
            if (jsonView.ValueExists("value"))
            {
                metric.m_value = Aws::Crt::Optional<double>(jsonView.GetDouble("value"));
            }
        }

        void Metric::SetUnit(MetricUnitType unit) noexcept
        {
            switch (unit)
            {
                case METRIC_UNIT_TYPE_BYTES:
                    m_unit = Aws::Crt::String("BYTES");
                    break;
                case METRIC_UNIT_TYPE_BYTES_PER_SECOND:
                    m_unit = Aws::Crt::String("BYTES_PER_SECOND");
                    break;
                case METRIC_UNIT_TYPE_COUNT:
                    m_unit = Aws::Crt::String("COUNT");
                    break;
                case METRIC_UNIT_TYPE_COUNT_PER_SECOND:
                    m_unit = Aws::Crt::String("COUNT_PER_SECOND");
                    break;
                case METRIC_UNIT_TYPE_MEGABYTES:
                    m_unit = Aws::Crt::String("MEGABYTES");
                    break;
                case METRIC_UNIT_TYPE_SECONDS:
                    m_unit = Aws::Crt::String("SECONDS");
                    break;
                default:
                    break;
            }
        }

        Aws::Crt::Optional<MetricUnitType> Metric::GetUnit() noexcept
        {
            if (!m_unit.has_value())
                return Aws::Crt::Optional<MetricUnitType>();
            if (m_unit.value() == Aws::Crt::String("BYTES"))
            {
                return Aws::Crt::Optional<MetricUnitType>(METRIC_UNIT_TYPE_BYTES);
            }
            if (m_unit.value() == Aws::Crt::String("BYTES_PER_SECOND"))
            {
                return Aws::Crt::Optional<MetricUnitType>(METRIC_UNIT_TYPE_BYTES_PER_SECOND);
            }
            if (m_unit.value() == Aws::Crt::String("COUNT"))
            {
                return Aws::Crt::Optional<MetricUnitType>(METRIC_UNIT_TYPE_COUNT);
            }
            if (m_unit.value() == Aws::Crt::String("COUNT_PER_SECOND"))
            {
                return Aws::Crt::Optional<MetricUnitType>(METRIC_UNIT_TYPE_COUNT_PER_SECOND);
            }
            if (m_unit.value() == Aws::Crt::String("MEGABYTES"))
            {
                return Aws::Crt::Optional<MetricUnitType>(METRIC_UNIT_TYPE_MEGABYTES);
            }
            if (m_unit.value() == Aws::Crt::String("SECONDS"))
            {
                return Aws::Crt::Optional<MetricUnitType>(METRIC_UNIT_TYPE_SECONDS);
            }

            return Aws::Crt::Optional<MetricUnitType>();
        }

        const char *Metric::MODEL_NAME = "aws.greengrass#Metric";

        Aws::Crt::String Metric::GetModelName() const noexcept { return Metric::MODEL_NAME; }

        Aws::Crt::ScopedResource<AbstractShapeBase> Metric::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<Metric> shape(Aws::Crt::New<Metric>(allocator), Metric::s_customDeleter);
            shape->m_allocator = allocator;
            Metric::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void Metric::s_customDeleter(Metric *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void LocalDeployment::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_deploymentId.has_value())
            {
                payloadObject.WithString("deploymentId", m_deploymentId.value());
            }
            if (m_status.has_value())
            {
                payloadObject.WithString("status", m_status.value());
            }
            if (m_createdOn.has_value())
            {
                payloadObject.WithString("createdOn", m_createdOn.value());
            }
            if (m_deploymentStatusDetails.has_value())
            {
                Aws::Crt::JsonObject deploymentStatusDetailsValue;
                m_deploymentStatusDetails.value().SerializeToJsonObject(deploymentStatusDetailsValue);
                payloadObject.WithObject("deploymentStatusDetails", std::move(deploymentStatusDetailsValue));
            }
        }

        void LocalDeployment::s_loadFromJsonView(
            LocalDeployment &localDeployment,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("deploymentId"))
            {
                localDeployment.m_deploymentId =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("deploymentId"));
            }
            if (jsonView.ValueExists("status"))
            {
                localDeployment.m_status = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("status"));
            }
            if (jsonView.ValueExists("createdOn"))
            {
                localDeployment.m_createdOn = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("createdOn"));
            }
            if (jsonView.ValueExists("deploymentStatusDetails"))
            {
                localDeployment.m_deploymentStatusDetails = DeploymentStatusDetails();
                DeploymentStatusDetails::s_loadFromJsonView(
                    localDeployment.m_deploymentStatusDetails.value(),
                    jsonView.GetJsonObject("deploymentStatusDetails"));
            }
        }

        void LocalDeployment::SetStatus(DeploymentStatus status) noexcept
        {
            switch (status)
            {
                case DEPLOYMENT_STATUS_QUEUED:
                    m_status = Aws::Crt::String("QUEUED");
                    break;
                case DEPLOYMENT_STATUS_IN_PROGRESS:
                    m_status = Aws::Crt::String("IN_PROGRESS");
                    break;
                case DEPLOYMENT_STATUS_SUCCEEDED:
                    m_status = Aws::Crt::String("SUCCEEDED");
                    break;
                case DEPLOYMENT_STATUS_FAILED:
                    m_status = Aws::Crt::String("FAILED");
                    break;
                case DEPLOYMENT_STATUS_CANCELED:
                    m_status = Aws::Crt::String("CANCELED");
                    break;
                default:
                    break;
            }
        }

        Aws::Crt::Optional<DeploymentStatus> LocalDeployment::GetStatus() noexcept
        {
            if (!m_status.has_value())
                return Aws::Crt::Optional<DeploymentStatus>();
            if (m_status.value() == Aws::Crt::String("QUEUED"))
            {
                return Aws::Crt::Optional<DeploymentStatus>(DEPLOYMENT_STATUS_QUEUED);
            }
            if (m_status.value() == Aws::Crt::String("IN_PROGRESS"))
            {
                return Aws::Crt::Optional<DeploymentStatus>(DEPLOYMENT_STATUS_IN_PROGRESS);
            }
            if (m_status.value() == Aws::Crt::String("SUCCEEDED"))
            {
                return Aws::Crt::Optional<DeploymentStatus>(DEPLOYMENT_STATUS_SUCCEEDED);
            }
            if (m_status.value() == Aws::Crt::String("FAILED"))
            {
                return Aws::Crt::Optional<DeploymentStatus>(DEPLOYMENT_STATUS_FAILED);
            }
            if (m_status.value() == Aws::Crt::String("CANCELED"))
            {
                return Aws::Crt::Optional<DeploymentStatus>(DEPLOYMENT_STATUS_CANCELED);
            }

            return Aws::Crt::Optional<DeploymentStatus>();
        }

        const char *LocalDeployment::MODEL_NAME = "aws.greengrass#LocalDeployment";

        Aws::Crt::String LocalDeployment::GetModelName() const noexcept { return LocalDeployment::MODEL_NAME; }

        Aws::Crt::ScopedResource<AbstractShapeBase> LocalDeployment::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<LocalDeployment> shape(
                Aws::Crt::New<LocalDeployment>(allocator), LocalDeployment::s_customDeleter);
            shape->m_allocator = allocator;
            LocalDeployment::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void LocalDeployment::s_customDeleter(LocalDeployment *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void ComponentDetails::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_componentName.has_value())
            {
                payloadObject.WithString("componentName", m_componentName.value());
            }
            if (m_version.has_value())
            {
                payloadObject.WithString("version", m_version.value());
            }
            if (m_state.has_value())
            {
                payloadObject.WithString("state", m_state.value());
            }
            if (m_configuration.has_value())
            {
                payloadObject.WithObject("configuration", m_configuration.value());
            }
        }

        void ComponentDetails::s_loadFromJsonView(
            ComponentDetails &componentDetails,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("componentName"))
            {
                componentDetails.m_componentName =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("componentName"));
            }
            if (jsonView.ValueExists("version"))
            {
                componentDetails.m_version = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("version"));
            }
            if (jsonView.ValueExists("state"))
            {
                componentDetails.m_state = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("state"));
            }
            if (jsonView.ValueExists("configuration"))
            {
                componentDetails.m_configuration =
                    Aws::Crt::Optional<Aws::Crt::JsonObject>(jsonView.GetJsonObject("configuration").Materialize());
            }
        }

        void ComponentDetails::SetState(LifecycleState state) noexcept
        {
            switch (state)
            {
                case LIFECYCLE_STATE_RUNNING:
                    m_state = Aws::Crt::String("RUNNING");
                    break;
                case LIFECYCLE_STATE_ERRORED:
                    m_state = Aws::Crt::String("ERRORED");
                    break;
                case LIFECYCLE_STATE_NEW:
                    m_state = Aws::Crt::String("NEW");
                    break;
                case LIFECYCLE_STATE_FINISHED:
                    m_state = Aws::Crt::String("FINISHED");
                    break;
                case LIFECYCLE_STATE_INSTALLED:
                    m_state = Aws::Crt::String("INSTALLED");
                    break;
                case LIFECYCLE_STATE_BROKEN:
                    m_state = Aws::Crt::String("BROKEN");
                    break;
                case LIFECYCLE_STATE_STARTING:
                    m_state = Aws::Crt::String("STARTING");
                    break;
                case LIFECYCLE_STATE_STOPPING:
                    m_state = Aws::Crt::String("STOPPING");
                    break;
                default:
                    break;
            }
        }

        Aws::Crt::Optional<LifecycleState> ComponentDetails::GetState() noexcept
        {
            if (!m_state.has_value())
                return Aws::Crt::Optional<LifecycleState>();
            if (m_state.value() == Aws::Crt::String("RUNNING"))
            {
                return Aws::Crt::Optional<LifecycleState>(LIFECYCLE_STATE_RUNNING);
            }
            if (m_state.value() == Aws::Crt::String("ERRORED"))
            {
                return Aws::Crt::Optional<LifecycleState>(LIFECYCLE_STATE_ERRORED);
            }
            if (m_state.value() == Aws::Crt::String("NEW"))
            {
                return Aws::Crt::Optional<LifecycleState>(LIFECYCLE_STATE_NEW);
            }
            if (m_state.value() == Aws::Crt::String("FINISHED"))
            {
                return Aws::Crt::Optional<LifecycleState>(LIFECYCLE_STATE_FINISHED);
            }
            if (m_state.value() == Aws::Crt::String("INSTALLED"))
            {
                return Aws::Crt::Optional<LifecycleState>(LIFECYCLE_STATE_INSTALLED);
            }
            if (m_state.value() == Aws::Crt::String("BROKEN"))
            {
                return Aws::Crt::Optional<LifecycleState>(LIFECYCLE_STATE_BROKEN);
            }
            if (m_state.value() == Aws::Crt::String("STARTING"))
            {
                return Aws::Crt::Optional<LifecycleState>(LIFECYCLE_STATE_STARTING);
            }
            if (m_state.value() == Aws::Crt::String("STOPPING"))
            {
                return Aws::Crt::Optional<LifecycleState>(LIFECYCLE_STATE_STOPPING);
            }

            return Aws::Crt::Optional<LifecycleState>();
        }

        const char *ComponentDetails::MODEL_NAME = "aws.greengrass#ComponentDetails";

        Aws::Crt::String ComponentDetails::GetModelName() const noexcept { return ComponentDetails::MODEL_NAME; }

        Aws::Crt::ScopedResource<AbstractShapeBase> ComponentDetails::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<ComponentDetails> shape(
                Aws::Crt::New<ComponentDetails>(allocator), ComponentDetails::s_customDeleter);
            shape->m_allocator = allocator;
            ComponentDetails::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void ComponentDetails::s_customDeleter(ComponentDetails *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void MQTTCredential::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_clientId.has_value())
            {
                payloadObject.WithString("clientId", m_clientId.value());
            }
            if (m_certificatePem.has_value())
            {
                payloadObject.WithString("certificatePem", m_certificatePem.value());
            }
            if (m_username.has_value())
            {
                payloadObject.WithString("username", m_username.value());
            }
            if (m_password.has_value())
            {
                payloadObject.WithString("password", m_password.value());
            }
        }

        void MQTTCredential::s_loadFromJsonView(
            MQTTCredential &mQTTCredential,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("clientId"))
            {
                mQTTCredential.m_clientId = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("clientId"));
            }
            if (jsonView.ValueExists("certificatePem"))
            {
                mQTTCredential.m_certificatePem =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("certificatePem"));
            }
            if (jsonView.ValueExists("username"))
            {
                mQTTCredential.m_username = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("username"));
            }
            if (jsonView.ValueExists("password"))
            {
                mQTTCredential.m_password = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("password"));
            }
        }

        const char *MQTTCredential::MODEL_NAME = "aws.greengrass#MQTTCredential";

        Aws::Crt::String MQTTCredential::GetModelName() const noexcept { return MQTTCredential::MODEL_NAME; }

        Aws::Crt::ScopedResource<AbstractShapeBase> MQTTCredential::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<MQTTCredential> shape(
                Aws::Crt::New<MQTTCredential>(allocator), MQTTCredential::s_customDeleter);
            shape->m_allocator = allocator;
            MQTTCredential::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void MQTTCredential::s_customDeleter(MQTTCredential *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void RunWithInfo::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_posixUser.has_value())
            {
                payloadObject.WithString("posixUser", m_posixUser.value());
            }
            if (m_windowsUser.has_value())
            {
                payloadObject.WithString("windowsUser", m_windowsUser.value());
            }
            if (m_systemResourceLimits.has_value())
            {
                Aws::Crt::JsonObject systemResourceLimitsValue;
                m_systemResourceLimits.value().SerializeToJsonObject(systemResourceLimitsValue);
                payloadObject.WithObject("systemResourceLimits", std::move(systemResourceLimitsValue));
            }
        }

        void RunWithInfo::s_loadFromJsonView(RunWithInfo &runWithInfo, const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("posixUser"))
            {
                runWithInfo.m_posixUser = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("posixUser"));
            }
            if (jsonView.ValueExists("windowsUser"))
            {
                runWithInfo.m_windowsUser = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("windowsUser"));
            }
            if (jsonView.ValueExists("systemResourceLimits"))
            {
                runWithInfo.m_systemResourceLimits = SystemResourceLimits();
                SystemResourceLimits::s_loadFromJsonView(
                    runWithInfo.m_systemResourceLimits.value(), jsonView.GetJsonObject("systemResourceLimits"));
            }
        }

        const char *RunWithInfo::MODEL_NAME = "aws.greengrass#RunWithInfo";

        Aws::Crt::String RunWithInfo::GetModelName() const noexcept { return RunWithInfo::MODEL_NAME; }

        Aws::Crt::ScopedResource<AbstractShapeBase> RunWithInfo::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<RunWithInfo> shape(
                Aws::Crt::New<RunWithInfo>(allocator), RunWithInfo::s_customDeleter);
            shape->m_allocator = allocator;
            RunWithInfo::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void RunWithInfo::s_customDeleter(RunWithInfo *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void ClientDeviceCredential::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_chosenMember == TAG_CLIENT_DEVICE_CERTIFICATE && m_clientDeviceCertificate.has_value())
            {
                payloadObject.WithString("clientDeviceCertificate", m_clientDeviceCertificate.value());
            }
        }

        void ClientDeviceCredential::s_loadFromJsonView(
            ClientDeviceCredential &clientDeviceCredential,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("clientDeviceCertificate"))
            {
                clientDeviceCredential.m_clientDeviceCertificate =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("clientDeviceCertificate"));
                clientDeviceCredential.m_chosenMember = TAG_CLIENT_DEVICE_CERTIFICATE;
            }
        }

        ClientDeviceCredential &ClientDeviceCredential::operator=(const ClientDeviceCredential &objectToCopy) noexcept
        {
            if (objectToCopy.m_chosenMember == TAG_CLIENT_DEVICE_CERTIFICATE)
            {
                m_clientDeviceCertificate = objectToCopy.m_clientDeviceCertificate;
                m_chosenMember = objectToCopy.m_chosenMember;
            }
            return *this;
        }

        const char *ClientDeviceCredential::MODEL_NAME = "aws.greengrass#ClientDeviceCredential";

        Aws::Crt::String ClientDeviceCredential::GetModelName() const noexcept
        {
            return ClientDeviceCredential::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> ClientDeviceCredential::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<ClientDeviceCredential> shape(
                Aws::Crt::New<ClientDeviceCredential>(allocator), ClientDeviceCredential::s_customDeleter);
            shape->m_allocator = allocator;
            ClientDeviceCredential::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void ClientDeviceCredential::s_customDeleter(ClientDeviceCredential *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void ValidateConfigurationUpdateEvents::SerializeToJsonObject(
            Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_chosenMember == TAG_VALIDATE_CONFIGURATION_UPDATE_EVENT &&
                m_validateConfigurationUpdateEvent.has_value())
            {
                Aws::Crt::JsonObject validateConfigurationUpdateEventValue;
                m_validateConfigurationUpdateEvent.value().SerializeToJsonObject(validateConfigurationUpdateEventValue);
                payloadObject.WithObject(
                    "validateConfigurationUpdateEvent", std::move(validateConfigurationUpdateEventValue));
            }
        }

        void ValidateConfigurationUpdateEvents::s_loadFromJsonView(
            ValidateConfigurationUpdateEvents &validateConfigurationUpdateEvents,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("validateConfigurationUpdateEvent"))
            {
                validateConfigurationUpdateEvents.m_validateConfigurationUpdateEvent =
                    ValidateConfigurationUpdateEvent();
                ValidateConfigurationUpdateEvent::s_loadFromJsonView(
                    validateConfigurationUpdateEvents.m_validateConfigurationUpdateEvent.value(),
                    jsonView.GetJsonObject("validateConfigurationUpdateEvent"));
                validateConfigurationUpdateEvents.m_chosenMember = TAG_VALIDATE_CONFIGURATION_UPDATE_EVENT;
            }
        }

        ValidateConfigurationUpdateEvents &ValidateConfigurationUpdateEvents::operator=(
            const ValidateConfigurationUpdateEvents &objectToCopy) noexcept
        {
            if (objectToCopy.m_chosenMember == TAG_VALIDATE_CONFIGURATION_UPDATE_EVENT)
            {
                m_validateConfigurationUpdateEvent = objectToCopy.m_validateConfigurationUpdateEvent;
                m_chosenMember = objectToCopy.m_chosenMember;
            }
            return *this;
        }

        const char *ValidateConfigurationUpdateEvents::MODEL_NAME = "aws.greengrass#ValidateConfigurationUpdateEvents";

        Aws::Crt::String ValidateConfigurationUpdateEvents::GetModelName() const noexcept
        {
            return ValidateConfigurationUpdateEvents::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> ValidateConfigurationUpdateEvents::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<ValidateConfigurationUpdateEvents> shape(
                Aws::Crt::New<ValidateConfigurationUpdateEvents>(allocator),
                ValidateConfigurationUpdateEvents::s_customDeleter);
            shape->m_allocator = allocator;
            ValidateConfigurationUpdateEvents::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void ValidateConfigurationUpdateEvents::s_customDeleter(ValidateConfigurationUpdateEvents *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void SubscriptionResponseMessage::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_chosenMember == TAG_JSON_MESSAGE && m_jsonMessage.has_value())
            {
                Aws::Crt::JsonObject jsonMessageValue;
                m_jsonMessage.value().SerializeToJsonObject(jsonMessageValue);
                payloadObject.WithObject("jsonMessage", std::move(jsonMessageValue));
            }
            else if (m_chosenMember == TAG_BINARY_MESSAGE && m_binaryMessage.has_value())
            {
                Aws::Crt::JsonObject binaryMessageValue;
                m_binaryMessage.value().SerializeToJsonObject(binaryMessageValue);
                payloadObject.WithObject("binaryMessage", std::move(binaryMessageValue));
            }
        }

        void SubscriptionResponseMessage::s_loadFromJsonView(
            SubscriptionResponseMessage &subscriptionResponseMessage,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("jsonMessage"))
            {
                subscriptionResponseMessage.m_jsonMessage = JsonMessage();
                JsonMessage::s_loadFromJsonView(
                    subscriptionResponseMessage.m_jsonMessage.value(), jsonView.GetJsonObject("jsonMessage"));
                subscriptionResponseMessage.m_chosenMember = TAG_JSON_MESSAGE;
            }
            else if (jsonView.ValueExists("binaryMessage"))
            {
                subscriptionResponseMessage.m_binaryMessage = BinaryMessage();
                BinaryMessage::s_loadFromJsonView(
                    subscriptionResponseMessage.m_binaryMessage.value(), jsonView.GetJsonObject("binaryMessage"));
                subscriptionResponseMessage.m_chosenMember = TAG_BINARY_MESSAGE;
            }
        }

        SubscriptionResponseMessage &SubscriptionResponseMessage::operator=(
            const SubscriptionResponseMessage &objectToCopy) noexcept
        {
            if (objectToCopy.m_chosenMember == TAG_JSON_MESSAGE)
            {
                m_jsonMessage = objectToCopy.m_jsonMessage;
                m_chosenMember = objectToCopy.m_chosenMember;
            }
            else if (objectToCopy.m_chosenMember == TAG_BINARY_MESSAGE)
            {
                m_binaryMessage = objectToCopy.m_binaryMessage;
                m_chosenMember = objectToCopy.m_chosenMember;
            }
            return *this;
        }

        const char *SubscriptionResponseMessage::MODEL_NAME = "aws.greengrass#SubscriptionResponseMessage";

        Aws::Crt::String SubscriptionResponseMessage::GetModelName() const noexcept
        {
            return SubscriptionResponseMessage::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SubscriptionResponseMessage::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<SubscriptionResponseMessage> shape(
                Aws::Crt::New<SubscriptionResponseMessage>(allocator), SubscriptionResponseMessage::s_customDeleter);
            shape->m_allocator = allocator;
            SubscriptionResponseMessage::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void SubscriptionResponseMessage::s_customDeleter(SubscriptionResponseMessage *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void IoTCoreMessage::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_chosenMember == TAG_MESSAGE && m_message.has_value())
            {
                Aws::Crt::JsonObject mQTTMessageValue;
                m_message.value().SerializeToJsonObject(mQTTMessageValue);
                payloadObject.WithObject("message", std::move(mQTTMessageValue));
            }
        }

        void IoTCoreMessage::s_loadFromJsonView(
            IoTCoreMessage &ioTCoreMessage,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("message"))
            {
                ioTCoreMessage.m_message = MQTTMessage();
                MQTTMessage::s_loadFromJsonView(ioTCoreMessage.m_message.value(), jsonView.GetJsonObject("message"));
                ioTCoreMessage.m_chosenMember = TAG_MESSAGE;
            }
        }

        IoTCoreMessage &IoTCoreMessage::operator=(const IoTCoreMessage &objectToCopy) noexcept
        {
            if (objectToCopy.m_chosenMember == TAG_MESSAGE)
            {
                m_message = objectToCopy.m_message;
                m_chosenMember = objectToCopy.m_chosenMember;
            }
            return *this;
        }

        const char *IoTCoreMessage::MODEL_NAME = "aws.greengrass#IoTCoreMessage";

        Aws::Crt::String IoTCoreMessage::GetModelName() const noexcept { return IoTCoreMessage::MODEL_NAME; }

        Aws::Crt::ScopedResource<AbstractShapeBase> IoTCoreMessage::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<IoTCoreMessage> shape(
                Aws::Crt::New<IoTCoreMessage>(allocator), IoTCoreMessage::s_customDeleter);
            shape->m_allocator = allocator;
            IoTCoreMessage::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void IoTCoreMessage::s_customDeleter(IoTCoreMessage *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void ConfigurationUpdateEvents::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_chosenMember == TAG_CONFIGURATION_UPDATE_EVENT && m_configurationUpdateEvent.has_value())
            {
                Aws::Crt::JsonObject configurationUpdateEventValue;
                m_configurationUpdateEvent.value().SerializeToJsonObject(configurationUpdateEventValue);
                payloadObject.WithObject("configurationUpdateEvent", std::move(configurationUpdateEventValue));
            }
        }

        void ConfigurationUpdateEvents::s_loadFromJsonView(
            ConfigurationUpdateEvents &configurationUpdateEvents,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("configurationUpdateEvent"))
            {
                configurationUpdateEvents.m_configurationUpdateEvent = ConfigurationUpdateEvent();
                ConfigurationUpdateEvent::s_loadFromJsonView(
                    configurationUpdateEvents.m_configurationUpdateEvent.value(),
                    jsonView.GetJsonObject("configurationUpdateEvent"));
                configurationUpdateEvents.m_chosenMember = TAG_CONFIGURATION_UPDATE_EVENT;
            }
        }

        ConfigurationUpdateEvents &ConfigurationUpdateEvents::operator=(
            const ConfigurationUpdateEvents &objectToCopy) noexcept
        {
            if (objectToCopy.m_chosenMember == TAG_CONFIGURATION_UPDATE_EVENT)
            {
                m_configurationUpdateEvent = objectToCopy.m_configurationUpdateEvent;
                m_chosenMember = objectToCopy.m_chosenMember;
            }
            return *this;
        }

        const char *ConfigurationUpdateEvents::MODEL_NAME = "aws.greengrass#ConfigurationUpdateEvents";

        Aws::Crt::String ConfigurationUpdateEvents::GetModelName() const noexcept
        {
            return ConfigurationUpdateEvents::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> ConfigurationUpdateEvents::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<ConfigurationUpdateEvents> shape(
                Aws::Crt::New<ConfigurationUpdateEvents>(allocator), ConfigurationUpdateEvents::s_customDeleter);
            shape->m_allocator = allocator;
            ConfigurationUpdateEvents::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void ConfigurationUpdateEvents::s_customDeleter(ConfigurationUpdateEvents *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void ComponentUpdatePolicyEvents::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_chosenMember == TAG_PRE_UPDATE_EVENT && m_preUpdateEvent.has_value())
            {
                Aws::Crt::JsonObject preComponentUpdateEventValue;
                m_preUpdateEvent.value().SerializeToJsonObject(preComponentUpdateEventValue);
                payloadObject.WithObject("preUpdateEvent", std::move(preComponentUpdateEventValue));
            }
            else if (m_chosenMember == TAG_POST_UPDATE_EVENT && m_postUpdateEvent.has_value())
            {
                Aws::Crt::JsonObject postComponentUpdateEventValue;
                m_postUpdateEvent.value().SerializeToJsonObject(postComponentUpdateEventValue);
                payloadObject.WithObject("postUpdateEvent", std::move(postComponentUpdateEventValue));
            }
        }

        void ComponentUpdatePolicyEvents::s_loadFromJsonView(
            ComponentUpdatePolicyEvents &componentUpdatePolicyEvents,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("preUpdateEvent"))
            {
                componentUpdatePolicyEvents.m_preUpdateEvent = PreComponentUpdateEvent();
                PreComponentUpdateEvent::s_loadFromJsonView(
                    componentUpdatePolicyEvents.m_preUpdateEvent.value(), jsonView.GetJsonObject("preUpdateEvent"));
                componentUpdatePolicyEvents.m_chosenMember = TAG_PRE_UPDATE_EVENT;
            }
            else if (jsonView.ValueExists("postUpdateEvent"))
            {
                componentUpdatePolicyEvents.m_postUpdateEvent = PostComponentUpdateEvent();
                PostComponentUpdateEvent::s_loadFromJsonView(
                    componentUpdatePolicyEvents.m_postUpdateEvent.value(), jsonView.GetJsonObject("postUpdateEvent"));
                componentUpdatePolicyEvents.m_chosenMember = TAG_POST_UPDATE_EVENT;
            }
        }

        ComponentUpdatePolicyEvents &ComponentUpdatePolicyEvents::operator=(
            const ComponentUpdatePolicyEvents &objectToCopy) noexcept
        {
            if (objectToCopy.m_chosenMember == TAG_PRE_UPDATE_EVENT)
            {
                m_preUpdateEvent = objectToCopy.m_preUpdateEvent;
                m_chosenMember = objectToCopy.m_chosenMember;
            }
            else if (objectToCopy.m_chosenMember == TAG_POST_UPDATE_EVENT)
            {
                m_postUpdateEvent = objectToCopy.m_postUpdateEvent;
                m_chosenMember = objectToCopy.m_chosenMember;
            }
            return *this;
        }

        const char *ComponentUpdatePolicyEvents::MODEL_NAME = "aws.greengrass#ComponentUpdatePolicyEvents";

        Aws::Crt::String ComponentUpdatePolicyEvents::GetModelName() const noexcept
        {
            return ComponentUpdatePolicyEvents::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> ComponentUpdatePolicyEvents::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<ComponentUpdatePolicyEvents> shape(
                Aws::Crt::New<ComponentUpdatePolicyEvents>(allocator), ComponentUpdatePolicyEvents::s_customDeleter);
            shape->m_allocator = allocator;
            ComponentUpdatePolicyEvents::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void ComponentUpdatePolicyEvents::s_customDeleter(ComponentUpdatePolicyEvents *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void CertificateUpdateEvent::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_chosenMember == TAG_CERTIFICATE_UPDATE && m_certificateUpdate.has_value())
            {
                Aws::Crt::JsonObject certificateUpdateValue;
                m_certificateUpdate.value().SerializeToJsonObject(certificateUpdateValue);
                payloadObject.WithObject("certificateUpdate", std::move(certificateUpdateValue));
            }
        }

        void CertificateUpdateEvent::s_loadFromJsonView(
            CertificateUpdateEvent &certificateUpdateEvent,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("certificateUpdate"))
            {
                certificateUpdateEvent.m_certificateUpdate = CertificateUpdate();
                CertificateUpdate::s_loadFromJsonView(
                    certificateUpdateEvent.m_certificateUpdate.value(), jsonView.GetJsonObject("certificateUpdate"));
                certificateUpdateEvent.m_chosenMember = TAG_CERTIFICATE_UPDATE;
            }
        }

        CertificateUpdateEvent &CertificateUpdateEvent::operator=(const CertificateUpdateEvent &objectToCopy) noexcept
        {
            if (objectToCopy.m_chosenMember == TAG_CERTIFICATE_UPDATE)
            {
                m_certificateUpdate = objectToCopy.m_certificateUpdate;
                m_chosenMember = objectToCopy.m_chosenMember;
            }
            return *this;
        }

        const char *CertificateUpdateEvent::MODEL_NAME = "aws.greengrass#CertificateUpdateEvent";

        Aws::Crt::String CertificateUpdateEvent::GetModelName() const noexcept
        {
            return CertificateUpdateEvent::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> CertificateUpdateEvent::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<CertificateUpdateEvent> shape(
                Aws::Crt::New<CertificateUpdateEvent>(allocator), CertificateUpdateEvent::s_customDeleter);
            shape->m_allocator = allocator;
            CertificateUpdateEvent::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void CertificateUpdateEvent::s_customDeleter(CertificateUpdateEvent *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void CertificateOptions::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_certificateType.has_value())
            {
                payloadObject.WithString("certificateType", m_certificateType.value());
            }
        }

        void CertificateOptions::s_loadFromJsonView(
            CertificateOptions &certificateOptions,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("certificateType"))
            {
                certificateOptions.m_certificateType =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("certificateType"));
            }
        }

        void CertificateOptions::SetCertificateType(CertificateType certificateType) noexcept
        {
            switch (certificateType)
            {
                case CERTIFICATE_TYPE_SERVER:
                    m_certificateType = Aws::Crt::String("SERVER");
                    break;
                default:
                    break;
            }
        }

        Aws::Crt::Optional<CertificateType> CertificateOptions::GetCertificateType() noexcept
        {
            if (!m_certificateType.has_value())
                return Aws::Crt::Optional<CertificateType>();
            if (m_certificateType.value() == Aws::Crt::String("SERVER"))
            {
                return Aws::Crt::Optional<CertificateType>(CERTIFICATE_TYPE_SERVER);
            }

            return Aws::Crt::Optional<CertificateType>();
        }

        const char *CertificateOptions::MODEL_NAME = "aws.greengrass#CertificateOptions";

        Aws::Crt::String CertificateOptions::GetModelName() const noexcept { return CertificateOptions::MODEL_NAME; }

        Aws::Crt::ScopedResource<AbstractShapeBase> CertificateOptions::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<CertificateOptions> shape(
                Aws::Crt::New<CertificateOptions>(allocator), CertificateOptions::s_customDeleter);
            shape->m_allocator = allocator;
            CertificateOptions::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void CertificateOptions::s_customDeleter(CertificateOptions *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void ConfigurationValidityReport::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_status.has_value())
            {
                payloadObject.WithString("status", m_status.value());
            }
            if (m_deploymentId.has_value())
            {
                payloadObject.WithString("deploymentId", m_deploymentId.value());
            }
            if (m_message.has_value())
            {
                payloadObject.WithString("message", m_message.value());
            }
        }

        void ConfigurationValidityReport::s_loadFromJsonView(
            ConfigurationValidityReport &configurationValidityReport,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("status"))
            {
                configurationValidityReport.m_status =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("status"));
            }
            if (jsonView.ValueExists("deploymentId"))
            {
                configurationValidityReport.m_deploymentId =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("deploymentId"));
            }
            if (jsonView.ValueExists("message"))
            {
                configurationValidityReport.m_message =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("message"));
            }
        }

        void ConfigurationValidityReport::SetStatus(ConfigurationValidityStatus status) noexcept
        {
            switch (status)
            {
                case CONFIGURATION_VALIDITY_STATUS_ACCEPTED:
                    m_status = Aws::Crt::String("ACCEPTED");
                    break;
                case CONFIGURATION_VALIDITY_STATUS_REJECTED:
                    m_status = Aws::Crt::String("REJECTED");
                    break;
                default:
                    break;
            }
        }

        Aws::Crt::Optional<ConfigurationValidityStatus> ConfigurationValidityReport::GetStatus() noexcept
        {
            if (!m_status.has_value())
                return Aws::Crt::Optional<ConfigurationValidityStatus>();
            if (m_status.value() == Aws::Crt::String("ACCEPTED"))
            {
                return Aws::Crt::Optional<ConfigurationValidityStatus>(CONFIGURATION_VALIDITY_STATUS_ACCEPTED);
            }
            if (m_status.value() == Aws::Crt::String("REJECTED"))
            {
                return Aws::Crt::Optional<ConfigurationValidityStatus>(CONFIGURATION_VALIDITY_STATUS_REJECTED);
            }

            return Aws::Crt::Optional<ConfigurationValidityStatus>();
        }

        const char *ConfigurationValidityReport::MODEL_NAME = "aws.greengrass#ConfigurationValidityReport";

        Aws::Crt::String ConfigurationValidityReport::GetModelName() const noexcept
        {
            return ConfigurationValidityReport::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> ConfigurationValidityReport::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<ConfigurationValidityReport> shape(
                Aws::Crt::New<ConfigurationValidityReport>(allocator), ConfigurationValidityReport::s_customDeleter);
            shape->m_allocator = allocator;
            ConfigurationValidityReport::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void ConfigurationValidityReport::s_customDeleter(ConfigurationValidityReport *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void PublishMessage::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_chosenMember == TAG_JSON_MESSAGE && m_jsonMessage.has_value())
            {
                Aws::Crt::JsonObject jsonMessageValue;
                m_jsonMessage.value().SerializeToJsonObject(jsonMessageValue);
                payloadObject.WithObject("jsonMessage", std::move(jsonMessageValue));
            }
            else if (m_chosenMember == TAG_BINARY_MESSAGE && m_binaryMessage.has_value())
            {
                Aws::Crt::JsonObject binaryMessageValue;
                m_binaryMessage.value().SerializeToJsonObject(binaryMessageValue);
                payloadObject.WithObject("binaryMessage", std::move(binaryMessageValue));
            }
        }

        void PublishMessage::s_loadFromJsonView(
            PublishMessage &publishMessage,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("jsonMessage"))
            {
                publishMessage.m_jsonMessage = JsonMessage();
                JsonMessage::s_loadFromJsonView(
                    publishMessage.m_jsonMessage.value(), jsonView.GetJsonObject("jsonMessage"));
                publishMessage.m_chosenMember = TAG_JSON_MESSAGE;
            }
            else if (jsonView.ValueExists("binaryMessage"))
            {
                publishMessage.m_binaryMessage = BinaryMessage();
                BinaryMessage::s_loadFromJsonView(
                    publishMessage.m_binaryMessage.value(), jsonView.GetJsonObject("binaryMessage"));
                publishMessage.m_chosenMember = TAG_BINARY_MESSAGE;
            }
        }

        PublishMessage &PublishMessage::operator=(const PublishMessage &objectToCopy) noexcept
        {
            if (objectToCopy.m_chosenMember == TAG_JSON_MESSAGE)
            {
                m_jsonMessage = objectToCopy.m_jsonMessage;
                m_chosenMember = objectToCopy.m_chosenMember;
            }
            else if (objectToCopy.m_chosenMember == TAG_BINARY_MESSAGE)
            {
                m_binaryMessage = objectToCopy.m_binaryMessage;
                m_chosenMember = objectToCopy.m_chosenMember;
            }
            return *this;
        }

        const char *PublishMessage::MODEL_NAME = "aws.greengrass#PublishMessage";

        Aws::Crt::String PublishMessage::GetModelName() const noexcept { return PublishMessage::MODEL_NAME; }

        Aws::Crt::ScopedResource<AbstractShapeBase> PublishMessage::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<PublishMessage> shape(
                Aws::Crt::New<PublishMessage>(allocator), PublishMessage::s_customDeleter);
            shape->m_allocator = allocator;
            PublishMessage::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void PublishMessage::s_customDeleter(PublishMessage *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void SecretValue::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_chosenMember == TAG_SECRET_STRING && m_secretString.has_value())
            {
                payloadObject.WithString("secretString", m_secretString.value());
            }
            else if (m_chosenMember == TAG_SECRET_BINARY && m_secretBinary.has_value())
            {
                if (m_secretBinary.value().size() > 0)
                {
                    payloadObject.WithString("secretBinary", Aws::Crt::Base64Encode(m_secretBinary.value()));
                }
            }
        }

        void SecretValue::s_loadFromJsonView(SecretValue &secretValue, const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("secretString"))
            {
                secretValue.m_secretString = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("secretString"));
                secretValue.m_chosenMember = TAG_SECRET_STRING;
            }
            else if (jsonView.ValueExists("secretBinary"))
            {
                if (jsonView.GetString("secretBinary").size() > 0)
                {
                    secretValue.m_secretBinary = Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>>(
                        Aws::Crt::Base64Decode(jsonView.GetString("secretBinary")));
                }
                secretValue.m_chosenMember = TAG_SECRET_BINARY;
            }
        }

        SecretValue &SecretValue::operator=(const SecretValue &objectToCopy) noexcept
        {
            if (objectToCopy.m_chosenMember == TAG_SECRET_STRING)
            {
                m_secretString = objectToCopy.m_secretString;
                m_chosenMember = objectToCopy.m_chosenMember;
            }
            else if (objectToCopy.m_chosenMember == TAG_SECRET_BINARY)
            {
                m_secretBinary = objectToCopy.m_secretBinary;
                m_chosenMember = objectToCopy.m_chosenMember;
            }
            return *this;
        }

        const char *SecretValue::MODEL_NAME = "aws.greengrass#SecretValue";

        Aws::Crt::String SecretValue::GetModelName() const noexcept { return SecretValue::MODEL_NAME; }

        Aws::Crt::ScopedResource<AbstractShapeBase> SecretValue::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<SecretValue> shape(
                Aws::Crt::New<SecretValue>(allocator), SecretValue::s_customDeleter);
            shape->m_allocator = allocator;
            SecretValue::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void SecretValue::s_customDeleter(SecretValue *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void CredentialDocument::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_chosenMember == TAG_MQTT_CREDENTIAL && m_mqttCredential.has_value())
            {
                Aws::Crt::JsonObject mQTTCredentialValue;
                m_mqttCredential.value().SerializeToJsonObject(mQTTCredentialValue);
                payloadObject.WithObject("mqttCredential", std::move(mQTTCredentialValue));
            }
        }

        void CredentialDocument::s_loadFromJsonView(
            CredentialDocument &credentialDocument,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("mqttCredential"))
            {
                credentialDocument.m_mqttCredential = MQTTCredential();
                MQTTCredential::s_loadFromJsonView(
                    credentialDocument.m_mqttCredential.value(), jsonView.GetJsonObject("mqttCredential"));
                credentialDocument.m_chosenMember = TAG_MQTT_CREDENTIAL;
            }
        }

        CredentialDocument &CredentialDocument::operator=(const CredentialDocument &objectToCopy) noexcept
        {
            if (objectToCopy.m_chosenMember == TAG_MQTT_CREDENTIAL)
            {
                m_mqttCredential = objectToCopy.m_mqttCredential;
                m_chosenMember = objectToCopy.m_chosenMember;
            }
            return *this;
        }

        const char *CredentialDocument::MODEL_NAME = "aws.greengrass#CredentialDocument";

        Aws::Crt::String CredentialDocument::GetModelName() const noexcept { return CredentialDocument::MODEL_NAME; }

        Aws::Crt::ScopedResource<AbstractShapeBase> CredentialDocument::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<CredentialDocument> shape(
                Aws::Crt::New<CredentialDocument>(allocator), CredentialDocument::s_customDeleter);
            shape->m_allocator = allocator;
            CredentialDocument::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void CredentialDocument::s_customDeleter(CredentialDocument *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void InvalidArgumentsError::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_message.has_value())
            {
                payloadObject.WithString("message", m_message.value());
            }
        }

        void InvalidArgumentsError::s_loadFromJsonView(
            InvalidArgumentsError &invalidArgumentsError,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("message"))
            {
                invalidArgumentsError.m_message = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("message"));
            }
        }

        const char *InvalidArgumentsError::MODEL_NAME = "aws.greengrass#InvalidArgumentsError";

        Aws::Crt::String InvalidArgumentsError::GetModelName() const noexcept
        {
            return InvalidArgumentsError::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<OperationError> InvalidArgumentsError::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<InvalidArgumentsError> shape(
                Aws::Crt::New<InvalidArgumentsError>(allocator), InvalidArgumentsError::s_customDeleter);
            shape->m_allocator = allocator;
            InvalidArgumentsError::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<OperationError *>(shape.release());
            return Aws::Crt::ScopedResource<OperationError>(operationResponse, OperationError::s_customDeleter);
        }

        void InvalidArgumentsError::s_customDeleter(InvalidArgumentsError *shape) noexcept
        {
            OperationError::s_customDeleter(static_cast<OperationError *>(shape));
        }

        void ServiceError::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_message.has_value())
            {
                payloadObject.WithString("message", m_message.value());
            }
            if (m_context.has_value())
            {
                payloadObject.WithObject("context", m_context.value());
            }
        }

        void ServiceError::s_loadFromJsonView(ServiceError &serviceError, const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("message"))
            {
                serviceError.m_message = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("message"));
            }
            if (jsonView.ValueExists("context"))
            {
                serviceError.m_context =
                    Aws::Crt::Optional<Aws::Crt::JsonObject>(jsonView.GetJsonObject("context").Materialize());
            }
        }

        const char *ServiceError::MODEL_NAME = "aws.greengrass#ServiceError";

        Aws::Crt::String ServiceError::GetModelName() const noexcept { return ServiceError::MODEL_NAME; }

        Aws::Crt::ScopedResource<OperationError> ServiceError::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<ServiceError> shape(
                Aws::Crt::New<ServiceError>(allocator), ServiceError::s_customDeleter);
            shape->m_allocator = allocator;
            ServiceError::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<OperationError *>(shape.release());
            return Aws::Crt::ScopedResource<OperationError>(operationResponse, OperationError::s_customDeleter);
        }

        void ServiceError::s_customDeleter(ServiceError *shape) noexcept
        {
            OperationError::s_customDeleter(static_cast<OperationError *>(shape));
        }

        void UnauthorizedError::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_message.has_value())
            {
                payloadObject.WithString("message", m_message.value());
            }
        }

        void UnauthorizedError::s_loadFromJsonView(
            UnauthorizedError &unauthorizedError,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("message"))
            {
                unauthorizedError.m_message = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("message"));
            }
        }

        const char *UnauthorizedError::MODEL_NAME = "aws.greengrass#UnauthorizedError";

        Aws::Crt::String UnauthorizedError::GetModelName() const noexcept { return UnauthorizedError::MODEL_NAME; }

        Aws::Crt::ScopedResource<OperationError> UnauthorizedError::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<UnauthorizedError> shape(
                Aws::Crt::New<UnauthorizedError>(allocator), UnauthorizedError::s_customDeleter);
            shape->m_allocator = allocator;
            UnauthorizedError::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<OperationError *>(shape.release());
            return Aws::Crt::ScopedResource<OperationError>(operationResponse, OperationError::s_customDeleter);
        }

        void UnauthorizedError::s_customDeleter(UnauthorizedError *shape) noexcept
        {
            OperationError::s_customDeleter(static_cast<OperationError *>(shape));
        }

        void VerifyClientDeviceIdentityResponse::SerializeToJsonObject(
            Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_isValidClientDevice.has_value())
            {
                payloadObject.WithBool("isValidClientDevice", m_isValidClientDevice.value());
            }
        }

        void VerifyClientDeviceIdentityResponse::s_loadFromJsonView(
            VerifyClientDeviceIdentityResponse &verifyClientDeviceIdentityResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("isValidClientDevice"))
            {
                verifyClientDeviceIdentityResponse.m_isValidClientDevice =
                    Aws::Crt::Optional<bool>(jsonView.GetBool("isValidClientDevice"));
            }
        }

        const char *VerifyClientDeviceIdentityResponse::MODEL_NAME =
            "aws.greengrass#VerifyClientDeviceIdentityResponse";

        Aws::Crt::String VerifyClientDeviceIdentityResponse::GetModelName() const noexcept
        {
            return VerifyClientDeviceIdentityResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> VerifyClientDeviceIdentityResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<VerifyClientDeviceIdentityResponse> shape(
                Aws::Crt::New<VerifyClientDeviceIdentityResponse>(allocator),
                VerifyClientDeviceIdentityResponse::s_customDeleter);
            shape->m_allocator = allocator;
            VerifyClientDeviceIdentityResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void VerifyClientDeviceIdentityResponse::s_customDeleter(VerifyClientDeviceIdentityResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void VerifyClientDeviceIdentityRequest::SerializeToJsonObject(
            Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_credential.has_value())
            {
                Aws::Crt::JsonObject clientDeviceCredentialValue;
                m_credential.value().SerializeToJsonObject(clientDeviceCredentialValue);
                payloadObject.WithObject("credential", std::move(clientDeviceCredentialValue));
            }
        }

        void VerifyClientDeviceIdentityRequest::s_loadFromJsonView(
            VerifyClientDeviceIdentityRequest &verifyClientDeviceIdentityRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("credential"))
            {
                verifyClientDeviceIdentityRequest.m_credential = ClientDeviceCredential();
                ClientDeviceCredential::s_loadFromJsonView(
                    verifyClientDeviceIdentityRequest.m_credential.value(), jsonView.GetJsonObject("credential"));
            }
        }

        const char *VerifyClientDeviceIdentityRequest::MODEL_NAME = "aws.greengrass#VerifyClientDeviceIdentityRequest";

        Aws::Crt::String VerifyClientDeviceIdentityRequest::GetModelName() const noexcept
        {
            return VerifyClientDeviceIdentityRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> VerifyClientDeviceIdentityRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<VerifyClientDeviceIdentityRequest> shape(
                Aws::Crt::New<VerifyClientDeviceIdentityRequest>(allocator),
                VerifyClientDeviceIdentityRequest::s_customDeleter);
            shape->m_allocator = allocator;
            VerifyClientDeviceIdentityRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void VerifyClientDeviceIdentityRequest::s_customDeleter(VerifyClientDeviceIdentityRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void InvalidTokenError::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_message.has_value())
            {
                payloadObject.WithString("message", m_message.value());
            }
        }

        void InvalidTokenError::s_loadFromJsonView(
            InvalidTokenError &invalidTokenError,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("message"))
            {
                invalidTokenError.m_message = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("message"));
            }
        }

        const char *InvalidTokenError::MODEL_NAME = "aws.greengrass#InvalidTokenError";

        Aws::Crt::String InvalidTokenError::GetModelName() const noexcept { return InvalidTokenError::MODEL_NAME; }

        Aws::Crt::ScopedResource<OperationError> InvalidTokenError::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<InvalidTokenError> shape(
                Aws::Crt::New<InvalidTokenError>(allocator), InvalidTokenError::s_customDeleter);
            shape->m_allocator = allocator;
            InvalidTokenError::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<OperationError *>(shape.release());
            return Aws::Crt::ScopedResource<OperationError>(operationResponse, OperationError::s_customDeleter);
        }

        void InvalidTokenError::s_customDeleter(InvalidTokenError *shape) noexcept
        {
            OperationError::s_customDeleter(static_cast<OperationError *>(shape));
        }

        void ValidateAuthorizationTokenResponse::SerializeToJsonObject(
            Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_isValid.has_value())
            {
                payloadObject.WithBool("isValid", m_isValid.value());
            }
        }

        void ValidateAuthorizationTokenResponse::s_loadFromJsonView(
            ValidateAuthorizationTokenResponse &validateAuthorizationTokenResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("isValid"))
            {
                validateAuthorizationTokenResponse.m_isValid = Aws::Crt::Optional<bool>(jsonView.GetBool("isValid"));
            }
        }

        const char *ValidateAuthorizationTokenResponse::MODEL_NAME =
            "aws.greengrass#ValidateAuthorizationTokenResponse";

        Aws::Crt::String ValidateAuthorizationTokenResponse::GetModelName() const noexcept
        {
            return ValidateAuthorizationTokenResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> ValidateAuthorizationTokenResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<ValidateAuthorizationTokenResponse> shape(
                Aws::Crt::New<ValidateAuthorizationTokenResponse>(allocator),
                ValidateAuthorizationTokenResponse::s_customDeleter);
            shape->m_allocator = allocator;
            ValidateAuthorizationTokenResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void ValidateAuthorizationTokenResponse::s_customDeleter(ValidateAuthorizationTokenResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void ValidateAuthorizationTokenRequest::SerializeToJsonObject(
            Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_token.has_value())
            {
                payloadObject.WithString("token", m_token.value());
            }
        }

        void ValidateAuthorizationTokenRequest::s_loadFromJsonView(
            ValidateAuthorizationTokenRequest &validateAuthorizationTokenRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("token"))
            {
                validateAuthorizationTokenRequest.m_token =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("token"));
            }
        }

        const char *ValidateAuthorizationTokenRequest::MODEL_NAME = "aws.greengrass#ValidateAuthorizationTokenRequest";

        Aws::Crt::String ValidateAuthorizationTokenRequest::GetModelName() const noexcept
        {
            return ValidateAuthorizationTokenRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> ValidateAuthorizationTokenRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<ValidateAuthorizationTokenRequest> shape(
                Aws::Crt::New<ValidateAuthorizationTokenRequest>(allocator),
                ValidateAuthorizationTokenRequest::s_customDeleter);
            shape->m_allocator = allocator;
            ValidateAuthorizationTokenRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void ValidateAuthorizationTokenRequest::s_customDeleter(ValidateAuthorizationTokenRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void ConflictError::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_message.has_value())
            {
                payloadObject.WithString("message", m_message.value());
            }
        }

        void ConflictError::s_loadFromJsonView(
            ConflictError &conflictError,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("message"))
            {
                conflictError.m_message = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("message"));
            }
        }

        const char *ConflictError::MODEL_NAME = "aws.greengrass#ConflictError";

        Aws::Crt::String ConflictError::GetModelName() const noexcept { return ConflictError::MODEL_NAME; }

        Aws::Crt::ScopedResource<OperationError> ConflictError::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<ConflictError> shape(
                Aws::Crt::New<ConflictError>(allocator), ConflictError::s_customDeleter);
            shape->m_allocator = allocator;
            ConflictError::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<OperationError *>(shape.release());
            return Aws::Crt::ScopedResource<OperationError>(operationResponse, OperationError::s_customDeleter);
        }

        void ConflictError::s_customDeleter(ConflictError *shape) noexcept
        {
            OperationError::s_customDeleter(static_cast<OperationError *>(shape));
        }

        void UpdateThingShadowResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_payload.has_value())
            {
                if (m_payload.value().size() > 0)
                {
                    payloadObject.WithString("payload", Aws::Crt::Base64Encode(m_payload.value()));
                }
            }
        }

        void UpdateThingShadowResponse::s_loadFromJsonView(
            UpdateThingShadowResponse &updateThingShadowResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("payload"))
            {
                if (jsonView.GetString("payload").size() > 0)
                {
                    updateThingShadowResponse.m_payload = Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>>(
                        Aws::Crt::Base64Decode(jsonView.GetString("payload")));
                }
            }
        }

        const char *UpdateThingShadowResponse::MODEL_NAME = "aws.greengrass#UpdateThingShadowResponse";

        Aws::Crt::String UpdateThingShadowResponse::GetModelName() const noexcept
        {
            return UpdateThingShadowResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> UpdateThingShadowResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<UpdateThingShadowResponse> shape(
                Aws::Crt::New<UpdateThingShadowResponse>(allocator), UpdateThingShadowResponse::s_customDeleter);
            shape->m_allocator = allocator;
            UpdateThingShadowResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void UpdateThingShadowResponse::s_customDeleter(UpdateThingShadowResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void UpdateThingShadowRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_thingName.has_value())
            {
                payloadObject.WithString("thingName", m_thingName.value());
            }
            if (m_shadowName.has_value())
            {
                payloadObject.WithString("shadowName", m_shadowName.value());
            }
            if (m_payload.has_value())
            {
                if (m_payload.value().size() > 0)
                {
                    payloadObject.WithString("payload", Aws::Crt::Base64Encode(m_payload.value()));
                }
            }
        }

        void UpdateThingShadowRequest::s_loadFromJsonView(
            UpdateThingShadowRequest &updateThingShadowRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("thingName"))
            {
                updateThingShadowRequest.m_thingName =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("thingName"));
            }
            if (jsonView.ValueExists("shadowName"))
            {
                updateThingShadowRequest.m_shadowName =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("shadowName"));
            }
            if (jsonView.ValueExists("payload"))
            {
                if (jsonView.GetString("payload").size() > 0)
                {
                    updateThingShadowRequest.m_payload = Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>>(
                        Aws::Crt::Base64Decode(jsonView.GetString("payload")));
                }
            }
        }

        const char *UpdateThingShadowRequest::MODEL_NAME = "aws.greengrass#UpdateThingShadowRequest";

        Aws::Crt::String UpdateThingShadowRequest::GetModelName() const noexcept
        {
            return UpdateThingShadowRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> UpdateThingShadowRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<UpdateThingShadowRequest> shape(
                Aws::Crt::New<UpdateThingShadowRequest>(allocator), UpdateThingShadowRequest::s_customDeleter);
            shape->m_allocator = allocator;
            UpdateThingShadowRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void UpdateThingShadowRequest::s_customDeleter(UpdateThingShadowRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void ResourceNotFoundError::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_message.has_value())
            {
                payloadObject.WithString("message", m_message.value());
            }
            if (m_resourceType.has_value())
            {
                payloadObject.WithString("resourceType", m_resourceType.value());
            }
            if (m_resourceName.has_value())
            {
                payloadObject.WithString("resourceName", m_resourceName.value());
            }
        }

        void ResourceNotFoundError::s_loadFromJsonView(
            ResourceNotFoundError &resourceNotFoundError,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("message"))
            {
                resourceNotFoundError.m_message = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("message"));
            }
            if (jsonView.ValueExists("resourceType"))
            {
                resourceNotFoundError.m_resourceType =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("resourceType"));
            }
            if (jsonView.ValueExists("resourceName"))
            {
                resourceNotFoundError.m_resourceName =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("resourceName"));
            }
        }

        const char *ResourceNotFoundError::MODEL_NAME = "aws.greengrass#ResourceNotFoundError";

        Aws::Crt::String ResourceNotFoundError::GetModelName() const noexcept
        {
            return ResourceNotFoundError::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<OperationError> ResourceNotFoundError::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<ResourceNotFoundError> shape(
                Aws::Crt::New<ResourceNotFoundError>(allocator), ResourceNotFoundError::s_customDeleter);
            shape->m_allocator = allocator;
            ResourceNotFoundError::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<OperationError *>(shape.release());
            return Aws::Crt::ScopedResource<OperationError>(operationResponse, OperationError::s_customDeleter);
        }

        void ResourceNotFoundError::s_customDeleter(ResourceNotFoundError *shape) noexcept
        {
            OperationError::s_customDeleter(static_cast<OperationError *>(shape));
        }

        void UpdateStateResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            (void)payloadObject;
        }

        void UpdateStateResponse::s_loadFromJsonView(
            UpdateStateResponse &updateStateResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            (void)updateStateResponse;
            (void)jsonView;
        }

        const char *UpdateStateResponse::MODEL_NAME = "aws.greengrass#UpdateStateResponse";

        Aws::Crt::String UpdateStateResponse::GetModelName() const noexcept { return UpdateStateResponse::MODEL_NAME; }

        Aws::Crt::ScopedResource<AbstractShapeBase> UpdateStateResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<UpdateStateResponse> shape(
                Aws::Crt::New<UpdateStateResponse>(allocator), UpdateStateResponse::s_customDeleter);
            shape->m_allocator = allocator;
            UpdateStateResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void UpdateStateResponse::s_customDeleter(UpdateStateResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void UpdateStateRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_state.has_value())
            {
                payloadObject.WithString("state", m_state.value());
            }
        }

        void UpdateStateRequest::s_loadFromJsonView(
            UpdateStateRequest &updateStateRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("state"))
            {
                updateStateRequest.m_state = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("state"));
            }
        }

        void UpdateStateRequest::SetState(ReportedLifecycleState state) noexcept
        {
            switch (state)
            {
                case REPORTED_LIFECYCLE_STATE_RUNNING:
                    m_state = Aws::Crt::String("RUNNING");
                    break;
                case REPORTED_LIFECYCLE_STATE_ERRORED:
                    m_state = Aws::Crt::String("ERRORED");
                    break;
                default:
                    break;
            }
        }

        Aws::Crt::Optional<ReportedLifecycleState> UpdateStateRequest::GetState() noexcept
        {
            if (!m_state.has_value())
                return Aws::Crt::Optional<ReportedLifecycleState>();
            if (m_state.value() == Aws::Crt::String("RUNNING"))
            {
                return Aws::Crt::Optional<ReportedLifecycleState>(REPORTED_LIFECYCLE_STATE_RUNNING);
            }
            if (m_state.value() == Aws::Crt::String("ERRORED"))
            {
                return Aws::Crt::Optional<ReportedLifecycleState>(REPORTED_LIFECYCLE_STATE_ERRORED);
            }

            return Aws::Crt::Optional<ReportedLifecycleState>();
        }

        const char *UpdateStateRequest::MODEL_NAME = "aws.greengrass#UpdateStateRequest";

        Aws::Crt::String UpdateStateRequest::GetModelName() const noexcept { return UpdateStateRequest::MODEL_NAME; }

        Aws::Crt::ScopedResource<AbstractShapeBase> UpdateStateRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<UpdateStateRequest> shape(
                Aws::Crt::New<UpdateStateRequest>(allocator), UpdateStateRequest::s_customDeleter);
            shape->m_allocator = allocator;
            UpdateStateRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void UpdateStateRequest::s_customDeleter(UpdateStateRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void FailedUpdateConditionCheckError::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_message.has_value())
            {
                payloadObject.WithString("message", m_message.value());
            }
        }

        void FailedUpdateConditionCheckError::s_loadFromJsonView(
            FailedUpdateConditionCheckError &failedUpdateConditionCheckError,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("message"))
            {
                failedUpdateConditionCheckError.m_message =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("message"));
            }
        }

        const char *FailedUpdateConditionCheckError::MODEL_NAME = "aws.greengrass#FailedUpdateConditionCheckError";

        Aws::Crt::String FailedUpdateConditionCheckError::GetModelName() const noexcept
        {
            return FailedUpdateConditionCheckError::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<OperationError> FailedUpdateConditionCheckError::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<FailedUpdateConditionCheckError> shape(
                Aws::Crt::New<FailedUpdateConditionCheckError>(allocator),
                FailedUpdateConditionCheckError::s_customDeleter);
            shape->m_allocator = allocator;
            FailedUpdateConditionCheckError::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<OperationError *>(shape.release());
            return Aws::Crt::ScopedResource<OperationError>(operationResponse, OperationError::s_customDeleter);
        }

        void FailedUpdateConditionCheckError::s_customDeleter(FailedUpdateConditionCheckError *shape) noexcept
        {
            OperationError::s_customDeleter(static_cast<OperationError *>(shape));
        }

        void UpdateConfigurationResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            (void)payloadObject;
        }

        void UpdateConfigurationResponse::s_loadFromJsonView(
            UpdateConfigurationResponse &updateConfigurationResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            (void)updateConfigurationResponse;
            (void)jsonView;
        }

        const char *UpdateConfigurationResponse::MODEL_NAME = "aws.greengrass#UpdateConfigurationResponse";

        Aws::Crt::String UpdateConfigurationResponse::GetModelName() const noexcept
        {
            return UpdateConfigurationResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> UpdateConfigurationResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<UpdateConfigurationResponse> shape(
                Aws::Crt::New<UpdateConfigurationResponse>(allocator), UpdateConfigurationResponse::s_customDeleter);
            shape->m_allocator = allocator;
            UpdateConfigurationResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void UpdateConfigurationResponse::s_customDeleter(UpdateConfigurationResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void UpdateConfigurationRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_keyPath.has_value())
            {
                Aws::Crt::JsonObject keyPath;
                Aws::Crt::Vector<Aws::Crt::JsonObject> keyPathJsonArray;
                for (const auto &keyPathItem : m_keyPath.value())
                {
                    Aws::Crt::JsonObject keyPathJsonArrayItem;
                    keyPathJsonArrayItem.AsString(keyPathItem);
                    keyPathJsonArray.emplace_back(std::move(keyPathJsonArrayItem));
                }
                keyPath.AsArray(std::move(keyPathJsonArray));
                payloadObject.WithObject("keyPath", std::move(keyPath));
            }
            if (m_timestamp.has_value())
            {
                payloadObject.WithDouble("timestamp", m_timestamp.value().SecondsWithMSPrecision());
            }
            if (m_valueToMerge.has_value())
            {
                payloadObject.WithObject("valueToMerge", m_valueToMerge.value());
            }
        }

        void UpdateConfigurationRequest::s_loadFromJsonView(
            UpdateConfigurationRequest &updateConfigurationRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("keyPath"))
            {
                updateConfigurationRequest.m_keyPath = Aws::Crt::Vector<Aws::Crt::String>();
                for (const Aws::Crt::JsonView &keyPathJsonView : jsonView.GetArray("keyPath"))
                {
                    Aws::Crt::Optional<Aws::Crt::String> keyPathItem;
                    keyPathItem = Aws::Crt::Optional<Aws::Crt::String>(keyPathJsonView.AsString());
                    updateConfigurationRequest.m_keyPath.value().push_back(keyPathItem.value());
                }
            }
            if (jsonView.ValueExists("timestamp"))
            {
                updateConfigurationRequest.m_timestamp =
                    Aws::Crt::Optional<Aws::Crt::DateTime>(Aws::Crt::DateTime(jsonView.GetDouble("timestamp")));
            }
            if (jsonView.ValueExists("valueToMerge"))
            {
                updateConfigurationRequest.m_valueToMerge =
                    Aws::Crt::Optional<Aws::Crt::JsonObject>(jsonView.GetJsonObject("valueToMerge").Materialize());
            }
        }

        const char *UpdateConfigurationRequest::MODEL_NAME = "aws.greengrass#UpdateConfigurationRequest";

        Aws::Crt::String UpdateConfigurationRequest::GetModelName() const noexcept
        {
            return UpdateConfigurationRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> UpdateConfigurationRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<UpdateConfigurationRequest> shape(
                Aws::Crt::New<UpdateConfigurationRequest>(allocator), UpdateConfigurationRequest::s_customDeleter);
            shape->m_allocator = allocator;
            UpdateConfigurationRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void UpdateConfigurationRequest::s_customDeleter(UpdateConfigurationRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void SubscribeToValidateConfigurationUpdatesResponse::SerializeToJsonObject(
            Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            (void)payloadObject;
        }

        void SubscribeToValidateConfigurationUpdatesResponse::s_loadFromJsonView(
            SubscribeToValidateConfigurationUpdatesResponse &subscribeToValidateConfigurationUpdatesResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            (void)subscribeToValidateConfigurationUpdatesResponse;
            (void)jsonView;
        }

        const char *SubscribeToValidateConfigurationUpdatesResponse::MODEL_NAME =
            "aws.greengrass#SubscribeToValidateConfigurationUpdatesResponse";

        Aws::Crt::String SubscribeToValidateConfigurationUpdatesResponse::GetModelName() const noexcept
        {
            return SubscribeToValidateConfigurationUpdatesResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SubscribeToValidateConfigurationUpdatesResponse::
            s_allocateFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<SubscribeToValidateConfigurationUpdatesResponse> shape(
                Aws::Crt::New<SubscribeToValidateConfigurationUpdatesResponse>(allocator),
                SubscribeToValidateConfigurationUpdatesResponse::s_customDeleter);
            shape->m_allocator = allocator;
            SubscribeToValidateConfigurationUpdatesResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void SubscribeToValidateConfigurationUpdatesResponse::s_customDeleter(
            SubscribeToValidateConfigurationUpdatesResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void SubscribeToValidateConfigurationUpdatesRequest::SerializeToJsonObject(
            Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            (void)payloadObject;
        }

        void SubscribeToValidateConfigurationUpdatesRequest::s_loadFromJsonView(
            SubscribeToValidateConfigurationUpdatesRequest &subscribeToValidateConfigurationUpdatesRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            (void)subscribeToValidateConfigurationUpdatesRequest;
            (void)jsonView;
        }

        const char *SubscribeToValidateConfigurationUpdatesRequest::MODEL_NAME =
            "aws.greengrass#SubscribeToValidateConfigurationUpdatesRequest";

        Aws::Crt::String SubscribeToValidateConfigurationUpdatesRequest::GetModelName() const noexcept
        {
            return SubscribeToValidateConfigurationUpdatesRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SubscribeToValidateConfigurationUpdatesRequest::
            s_allocateFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<SubscribeToValidateConfigurationUpdatesRequest> shape(
                Aws::Crt::New<SubscribeToValidateConfigurationUpdatesRequest>(allocator),
                SubscribeToValidateConfigurationUpdatesRequest::s_customDeleter);
            shape->m_allocator = allocator;
            SubscribeToValidateConfigurationUpdatesRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void SubscribeToValidateConfigurationUpdatesRequest::s_customDeleter(
            SubscribeToValidateConfigurationUpdatesRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void SubscribeToTopicResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_topicName.has_value())
            {
                payloadObject.WithString("topicName", m_topicName.value());
            }
        }

        void SubscribeToTopicResponse::s_loadFromJsonView(
            SubscribeToTopicResponse &subscribeToTopicResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("topicName"))
            {
                subscribeToTopicResponse.m_topicName =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("topicName"));
            }
        }

        const char *SubscribeToTopicResponse::MODEL_NAME = "aws.greengrass#SubscribeToTopicResponse";

        Aws::Crt::String SubscribeToTopicResponse::GetModelName() const noexcept
        {
            return SubscribeToTopicResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SubscribeToTopicResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<SubscribeToTopicResponse> shape(
                Aws::Crt::New<SubscribeToTopicResponse>(allocator), SubscribeToTopicResponse::s_customDeleter);
            shape->m_allocator = allocator;
            SubscribeToTopicResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void SubscribeToTopicResponse::s_customDeleter(SubscribeToTopicResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void SubscribeToTopicRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_topic.has_value())
            {
                payloadObject.WithString("topic", m_topic.value());
            }
            if (m_receiveMode.has_value())
            {
                payloadObject.WithString("receiveMode", m_receiveMode.value());
            }
        }

        void SubscribeToTopicRequest::s_loadFromJsonView(
            SubscribeToTopicRequest &subscribeToTopicRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("topic"))
            {
                subscribeToTopicRequest.m_topic = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("topic"));
            }
            if (jsonView.ValueExists("receiveMode"))
            {
                subscribeToTopicRequest.m_receiveMode =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("receiveMode"));
            }
        }

        void SubscribeToTopicRequest::SetReceiveMode(ReceiveMode receiveMode) noexcept
        {
            switch (receiveMode)
            {
                case RECEIVE_MODE_RECEIVE_ALL_MESSAGES:
                    m_receiveMode = Aws::Crt::String("RECEIVE_ALL_MESSAGES");
                    break;
                case RECEIVE_MODE_RECEIVE_MESSAGES_FROM_OTHERS:
                    m_receiveMode = Aws::Crt::String("RECEIVE_MESSAGES_FROM_OTHERS");
                    break;
                default:
                    break;
            }
        }

        Aws::Crt::Optional<ReceiveMode> SubscribeToTopicRequest::GetReceiveMode() noexcept
        {
            if (!m_receiveMode.has_value())
                return Aws::Crt::Optional<ReceiveMode>();
            if (m_receiveMode.value() == Aws::Crt::String("RECEIVE_ALL_MESSAGES"))
            {
                return Aws::Crt::Optional<ReceiveMode>(RECEIVE_MODE_RECEIVE_ALL_MESSAGES);
            }
            if (m_receiveMode.value() == Aws::Crt::String("RECEIVE_MESSAGES_FROM_OTHERS"))
            {
                return Aws::Crt::Optional<ReceiveMode>(RECEIVE_MODE_RECEIVE_MESSAGES_FROM_OTHERS);
            }

            return Aws::Crt::Optional<ReceiveMode>();
        }

        const char *SubscribeToTopicRequest::MODEL_NAME = "aws.greengrass#SubscribeToTopicRequest";

        Aws::Crt::String SubscribeToTopicRequest::GetModelName() const noexcept
        {
            return SubscribeToTopicRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SubscribeToTopicRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<SubscribeToTopicRequest> shape(
                Aws::Crt::New<SubscribeToTopicRequest>(allocator), SubscribeToTopicRequest::s_customDeleter);
            shape->m_allocator = allocator;
            SubscribeToTopicRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void SubscribeToTopicRequest::s_customDeleter(SubscribeToTopicRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void SubscribeToIoTCoreResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            (void)payloadObject;
        }

        void SubscribeToIoTCoreResponse::s_loadFromJsonView(
            SubscribeToIoTCoreResponse &subscribeToIoTCoreResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            (void)subscribeToIoTCoreResponse;
            (void)jsonView;
        }

        const char *SubscribeToIoTCoreResponse::MODEL_NAME = "aws.greengrass#SubscribeToIoTCoreResponse";

        Aws::Crt::String SubscribeToIoTCoreResponse::GetModelName() const noexcept
        {
            return SubscribeToIoTCoreResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SubscribeToIoTCoreResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<SubscribeToIoTCoreResponse> shape(
                Aws::Crt::New<SubscribeToIoTCoreResponse>(allocator), SubscribeToIoTCoreResponse::s_customDeleter);
            shape->m_allocator = allocator;
            SubscribeToIoTCoreResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void SubscribeToIoTCoreResponse::s_customDeleter(SubscribeToIoTCoreResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void SubscribeToIoTCoreRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_topicName.has_value())
            {
                payloadObject.WithString("topicName", m_topicName.value());
            }
            if (m_qos.has_value())
            {
                payloadObject.WithString("qos", m_qos.value());
            }
        }

        void SubscribeToIoTCoreRequest::s_loadFromJsonView(
            SubscribeToIoTCoreRequest &subscribeToIoTCoreRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("topicName"))
            {
                subscribeToIoTCoreRequest.m_topicName =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("topicName"));
            }
            if (jsonView.ValueExists("qos"))
            {
                subscribeToIoTCoreRequest.m_qos = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("qos"));
            }
        }

        void SubscribeToIoTCoreRequest::SetQos(QOS qos) noexcept
        {
            switch (qos)
            {
                case QOS_AT_MOST_ONCE:
                    m_qos = Aws::Crt::String("0");
                    break;
                case QOS_AT_LEAST_ONCE:
                    m_qos = Aws::Crt::String("1");
                    break;
                default:
                    break;
            }
        }

        Aws::Crt::Optional<QOS> SubscribeToIoTCoreRequest::GetQos() noexcept
        {
            if (!m_qos.has_value())
                return Aws::Crt::Optional<QOS>();
            if (m_qos.value() == Aws::Crt::String("0"))
            {
                return Aws::Crt::Optional<QOS>(QOS_AT_MOST_ONCE);
            }
            if (m_qos.value() == Aws::Crt::String("1"))
            {
                return Aws::Crt::Optional<QOS>(QOS_AT_LEAST_ONCE);
            }

            return Aws::Crt::Optional<QOS>();
        }

        const char *SubscribeToIoTCoreRequest::MODEL_NAME = "aws.greengrass#SubscribeToIoTCoreRequest";

        Aws::Crt::String SubscribeToIoTCoreRequest::GetModelName() const noexcept
        {
            return SubscribeToIoTCoreRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SubscribeToIoTCoreRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<SubscribeToIoTCoreRequest> shape(
                Aws::Crt::New<SubscribeToIoTCoreRequest>(allocator), SubscribeToIoTCoreRequest::s_customDeleter);
            shape->m_allocator = allocator;
            SubscribeToIoTCoreRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void SubscribeToIoTCoreRequest::s_customDeleter(SubscribeToIoTCoreRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void SubscribeToConfigurationUpdateResponse::SerializeToJsonObject(
            Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            (void)payloadObject;
        }

        void SubscribeToConfigurationUpdateResponse::s_loadFromJsonView(
            SubscribeToConfigurationUpdateResponse &subscribeToConfigurationUpdateResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            (void)subscribeToConfigurationUpdateResponse;
            (void)jsonView;
        }

        const char *SubscribeToConfigurationUpdateResponse::MODEL_NAME =
            "aws.greengrass#SubscribeToConfigurationUpdateResponse";

        Aws::Crt::String SubscribeToConfigurationUpdateResponse::GetModelName() const noexcept
        {
            return SubscribeToConfigurationUpdateResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SubscribeToConfigurationUpdateResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<SubscribeToConfigurationUpdateResponse> shape(
                Aws::Crt::New<SubscribeToConfigurationUpdateResponse>(allocator),
                SubscribeToConfigurationUpdateResponse::s_customDeleter);
            shape->m_allocator = allocator;
            SubscribeToConfigurationUpdateResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void SubscribeToConfigurationUpdateResponse::s_customDeleter(
            SubscribeToConfigurationUpdateResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void SubscribeToConfigurationUpdateRequest::SerializeToJsonObject(
            Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_componentName.has_value())
            {
                payloadObject.WithString("componentName", m_componentName.value());
            }
            if (m_keyPath.has_value())
            {
                Aws::Crt::JsonObject keyPath;
                Aws::Crt::Vector<Aws::Crt::JsonObject> keyPathJsonArray;
                for (const auto &keyPathItem : m_keyPath.value())
                {
                    Aws::Crt::JsonObject keyPathJsonArrayItem;
                    keyPathJsonArrayItem.AsString(keyPathItem);
                    keyPathJsonArray.emplace_back(std::move(keyPathJsonArrayItem));
                }
                keyPath.AsArray(std::move(keyPathJsonArray));
                payloadObject.WithObject("keyPath", std::move(keyPath));
            }
        }

        void SubscribeToConfigurationUpdateRequest::s_loadFromJsonView(
            SubscribeToConfigurationUpdateRequest &subscribeToConfigurationUpdateRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("componentName"))
            {
                subscribeToConfigurationUpdateRequest.m_componentName =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("componentName"));
            }
            if (jsonView.ValueExists("keyPath"))
            {
                subscribeToConfigurationUpdateRequest.m_keyPath = Aws::Crt::Vector<Aws::Crt::String>();
                for (const Aws::Crt::JsonView &keyPathJsonView : jsonView.GetArray("keyPath"))
                {
                    Aws::Crt::Optional<Aws::Crt::String> keyPathItem;
                    keyPathItem = Aws::Crt::Optional<Aws::Crt::String>(keyPathJsonView.AsString());
                    subscribeToConfigurationUpdateRequest.m_keyPath.value().push_back(keyPathItem.value());
                }
            }
        }

        const char *SubscribeToConfigurationUpdateRequest::MODEL_NAME =
            "aws.greengrass#SubscribeToConfigurationUpdateRequest";

        Aws::Crt::String SubscribeToConfigurationUpdateRequest::GetModelName() const noexcept
        {
            return SubscribeToConfigurationUpdateRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SubscribeToConfigurationUpdateRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<SubscribeToConfigurationUpdateRequest> shape(
                Aws::Crt::New<SubscribeToConfigurationUpdateRequest>(allocator),
                SubscribeToConfigurationUpdateRequest::s_customDeleter);
            shape->m_allocator = allocator;
            SubscribeToConfigurationUpdateRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void SubscribeToConfigurationUpdateRequest::s_customDeleter(
            SubscribeToConfigurationUpdateRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void SubscribeToComponentUpdatesResponse::SerializeToJsonObject(
            Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            (void)payloadObject;
        }

        void SubscribeToComponentUpdatesResponse::s_loadFromJsonView(
            SubscribeToComponentUpdatesResponse &subscribeToComponentUpdatesResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            (void)subscribeToComponentUpdatesResponse;
            (void)jsonView;
        }

        const char *SubscribeToComponentUpdatesResponse::MODEL_NAME =
            "aws.greengrass#SubscribeToComponentUpdatesResponse";

        Aws::Crt::String SubscribeToComponentUpdatesResponse::GetModelName() const noexcept
        {
            return SubscribeToComponentUpdatesResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SubscribeToComponentUpdatesResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<SubscribeToComponentUpdatesResponse> shape(
                Aws::Crt::New<SubscribeToComponentUpdatesResponse>(allocator),
                SubscribeToComponentUpdatesResponse::s_customDeleter);
            shape->m_allocator = allocator;
            SubscribeToComponentUpdatesResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void SubscribeToComponentUpdatesResponse::s_customDeleter(SubscribeToComponentUpdatesResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void SubscribeToComponentUpdatesRequest::SerializeToJsonObject(
            Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            (void)payloadObject;
        }

        void SubscribeToComponentUpdatesRequest::s_loadFromJsonView(
            SubscribeToComponentUpdatesRequest &subscribeToComponentUpdatesRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            (void)subscribeToComponentUpdatesRequest;
            (void)jsonView;
        }

        const char *SubscribeToComponentUpdatesRequest::MODEL_NAME =
            "aws.greengrass#SubscribeToComponentUpdatesRequest";

        Aws::Crt::String SubscribeToComponentUpdatesRequest::GetModelName() const noexcept
        {
            return SubscribeToComponentUpdatesRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SubscribeToComponentUpdatesRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<SubscribeToComponentUpdatesRequest> shape(
                Aws::Crt::New<SubscribeToComponentUpdatesRequest>(allocator),
                SubscribeToComponentUpdatesRequest::s_customDeleter);
            shape->m_allocator = allocator;
            SubscribeToComponentUpdatesRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void SubscribeToComponentUpdatesRequest::s_customDeleter(SubscribeToComponentUpdatesRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void SubscribeToCertificateUpdatesResponse::SerializeToJsonObject(
            Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            (void)payloadObject;
        }

        void SubscribeToCertificateUpdatesResponse::s_loadFromJsonView(
            SubscribeToCertificateUpdatesResponse &subscribeToCertificateUpdatesResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            (void)subscribeToCertificateUpdatesResponse;
            (void)jsonView;
        }

        const char *SubscribeToCertificateUpdatesResponse::MODEL_NAME =
            "aws.greengrass#SubscribeToCertificateUpdatesResponse";

        Aws::Crt::String SubscribeToCertificateUpdatesResponse::GetModelName() const noexcept
        {
            return SubscribeToCertificateUpdatesResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SubscribeToCertificateUpdatesResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<SubscribeToCertificateUpdatesResponse> shape(
                Aws::Crt::New<SubscribeToCertificateUpdatesResponse>(allocator),
                SubscribeToCertificateUpdatesResponse::s_customDeleter);
            shape->m_allocator = allocator;
            SubscribeToCertificateUpdatesResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void SubscribeToCertificateUpdatesResponse::s_customDeleter(
            SubscribeToCertificateUpdatesResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void SubscribeToCertificateUpdatesRequest::SerializeToJsonObject(
            Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_certificateOptions.has_value())
            {
                Aws::Crt::JsonObject certificateOptionsValue;
                m_certificateOptions.value().SerializeToJsonObject(certificateOptionsValue);
                payloadObject.WithObject("certificateOptions", std::move(certificateOptionsValue));
            }
        }

        void SubscribeToCertificateUpdatesRequest::s_loadFromJsonView(
            SubscribeToCertificateUpdatesRequest &subscribeToCertificateUpdatesRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("certificateOptions"))
            {
                subscribeToCertificateUpdatesRequest.m_certificateOptions = CertificateOptions();
                CertificateOptions::s_loadFromJsonView(
                    subscribeToCertificateUpdatesRequest.m_certificateOptions.value(),
                    jsonView.GetJsonObject("certificateOptions"));
            }
        }

        const char *SubscribeToCertificateUpdatesRequest::MODEL_NAME =
            "aws.greengrass#SubscribeToCertificateUpdatesRequest";

        Aws::Crt::String SubscribeToCertificateUpdatesRequest::GetModelName() const noexcept
        {
            return SubscribeToCertificateUpdatesRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SubscribeToCertificateUpdatesRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<SubscribeToCertificateUpdatesRequest> shape(
                Aws::Crt::New<SubscribeToCertificateUpdatesRequest>(allocator),
                SubscribeToCertificateUpdatesRequest::s_customDeleter);
            shape->m_allocator = allocator;
            SubscribeToCertificateUpdatesRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void SubscribeToCertificateUpdatesRequest::s_customDeleter(SubscribeToCertificateUpdatesRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void ComponentNotFoundError::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_message.has_value())
            {
                payloadObject.WithString("message", m_message.value());
            }
        }

        void ComponentNotFoundError::s_loadFromJsonView(
            ComponentNotFoundError &componentNotFoundError,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("message"))
            {
                componentNotFoundError.m_message = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("message"));
            }
        }

        const char *ComponentNotFoundError::MODEL_NAME = "aws.greengrass#ComponentNotFoundError";

        Aws::Crt::String ComponentNotFoundError::GetModelName() const noexcept
        {
            return ComponentNotFoundError::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<OperationError> ComponentNotFoundError::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<ComponentNotFoundError> shape(
                Aws::Crt::New<ComponentNotFoundError>(allocator), ComponentNotFoundError::s_customDeleter);
            shape->m_allocator = allocator;
            ComponentNotFoundError::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<OperationError *>(shape.release());
            return Aws::Crt::ScopedResource<OperationError>(operationResponse, OperationError::s_customDeleter);
        }

        void ComponentNotFoundError::s_customDeleter(ComponentNotFoundError *shape) noexcept
        {
            OperationError::s_customDeleter(static_cast<OperationError *>(shape));
        }

        void StopComponentResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_stopStatus.has_value())
            {
                payloadObject.WithString("stopStatus", m_stopStatus.value());
            }
            if (m_message.has_value())
            {
                payloadObject.WithString("message", m_message.value());
            }
        }

        void StopComponentResponse::s_loadFromJsonView(
            StopComponentResponse &stopComponentResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("stopStatus"))
            {
                stopComponentResponse.m_stopStatus =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("stopStatus"));
            }
            if (jsonView.ValueExists("message"))
            {
                stopComponentResponse.m_message = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("message"));
            }
        }

        void StopComponentResponse::SetStopStatus(RequestStatus stopStatus) noexcept
        {
            switch (stopStatus)
            {
                case REQUEST_STATUS_SUCCEEDED:
                    m_stopStatus = Aws::Crt::String("SUCCEEDED");
                    break;
                case REQUEST_STATUS_FAILED:
                    m_stopStatus = Aws::Crt::String("FAILED");
                    break;
                default:
                    break;
            }
        }

        Aws::Crt::Optional<RequestStatus> StopComponentResponse::GetStopStatus() noexcept
        {
            if (!m_stopStatus.has_value())
                return Aws::Crt::Optional<RequestStatus>();
            if (m_stopStatus.value() == Aws::Crt::String("SUCCEEDED"))
            {
                return Aws::Crt::Optional<RequestStatus>(REQUEST_STATUS_SUCCEEDED);
            }
            if (m_stopStatus.value() == Aws::Crt::String("FAILED"))
            {
                return Aws::Crt::Optional<RequestStatus>(REQUEST_STATUS_FAILED);
            }

            return Aws::Crt::Optional<RequestStatus>();
        }

        const char *StopComponentResponse::MODEL_NAME = "aws.greengrass#StopComponentResponse";

        Aws::Crt::String StopComponentResponse::GetModelName() const noexcept
        {
            return StopComponentResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> StopComponentResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<StopComponentResponse> shape(
                Aws::Crt::New<StopComponentResponse>(allocator), StopComponentResponse::s_customDeleter);
            shape->m_allocator = allocator;
            StopComponentResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void StopComponentResponse::s_customDeleter(StopComponentResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void StopComponentRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_componentName.has_value())
            {
                payloadObject.WithString("componentName", m_componentName.value());
            }
        }

        void StopComponentRequest::s_loadFromJsonView(
            StopComponentRequest &stopComponentRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("componentName"))
            {
                stopComponentRequest.m_componentName =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("componentName"));
            }
        }

        const char *StopComponentRequest::MODEL_NAME = "aws.greengrass#StopComponentRequest";

        Aws::Crt::String StopComponentRequest::GetModelName() const noexcept
        {
            return StopComponentRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> StopComponentRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<StopComponentRequest> shape(
                Aws::Crt::New<StopComponentRequest>(allocator), StopComponentRequest::s_customDeleter);
            shape->m_allocator = allocator;
            StopComponentRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void StopComponentRequest::s_customDeleter(StopComponentRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void SendConfigurationValidityReportResponse::SerializeToJsonObject(
            Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            (void)payloadObject;
        }

        void SendConfigurationValidityReportResponse::s_loadFromJsonView(
            SendConfigurationValidityReportResponse &sendConfigurationValidityReportResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            (void)sendConfigurationValidityReportResponse;
            (void)jsonView;
        }

        const char *SendConfigurationValidityReportResponse::MODEL_NAME =
            "aws.greengrass#SendConfigurationValidityReportResponse";

        Aws::Crt::String SendConfigurationValidityReportResponse::GetModelName() const noexcept
        {
            return SendConfigurationValidityReportResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SendConfigurationValidityReportResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<SendConfigurationValidityReportResponse> shape(
                Aws::Crt::New<SendConfigurationValidityReportResponse>(allocator),
                SendConfigurationValidityReportResponse::s_customDeleter);
            shape->m_allocator = allocator;
            SendConfigurationValidityReportResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void SendConfigurationValidityReportResponse::s_customDeleter(
            SendConfigurationValidityReportResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void SendConfigurationValidityReportRequest::SerializeToJsonObject(
            Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_configurationValidityReport.has_value())
            {
                Aws::Crt::JsonObject configurationValidityReportValue;
                m_configurationValidityReport.value().SerializeToJsonObject(configurationValidityReportValue);
                payloadObject.WithObject("configurationValidityReport", std::move(configurationValidityReportValue));
            }
        }

        void SendConfigurationValidityReportRequest::s_loadFromJsonView(
            SendConfigurationValidityReportRequest &sendConfigurationValidityReportRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("configurationValidityReport"))
            {
                sendConfigurationValidityReportRequest.m_configurationValidityReport = ConfigurationValidityReport();
                ConfigurationValidityReport::s_loadFromJsonView(
                    sendConfigurationValidityReportRequest.m_configurationValidityReport.value(),
                    jsonView.GetJsonObject("configurationValidityReport"));
            }
        }

        const char *SendConfigurationValidityReportRequest::MODEL_NAME =
            "aws.greengrass#SendConfigurationValidityReportRequest";

        Aws::Crt::String SendConfigurationValidityReportRequest::GetModelName() const noexcept
        {
            return SendConfigurationValidityReportRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SendConfigurationValidityReportRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<SendConfigurationValidityReportRequest> shape(
                Aws::Crt::New<SendConfigurationValidityReportRequest>(allocator),
                SendConfigurationValidityReportRequest::s_customDeleter);
            shape->m_allocator = allocator;
            SendConfigurationValidityReportRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void SendConfigurationValidityReportRequest::s_customDeleter(
            SendConfigurationValidityReportRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void ResumeComponentResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            (void)payloadObject;
        }

        void ResumeComponentResponse::s_loadFromJsonView(
            ResumeComponentResponse &resumeComponentResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            (void)resumeComponentResponse;
            (void)jsonView;
        }

        const char *ResumeComponentResponse::MODEL_NAME = "aws.greengrass#ResumeComponentResponse";

        Aws::Crt::String ResumeComponentResponse::GetModelName() const noexcept
        {
            return ResumeComponentResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> ResumeComponentResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<ResumeComponentResponse> shape(
                Aws::Crt::New<ResumeComponentResponse>(allocator), ResumeComponentResponse::s_customDeleter);
            shape->m_allocator = allocator;
            ResumeComponentResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void ResumeComponentResponse::s_customDeleter(ResumeComponentResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void ResumeComponentRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_componentName.has_value())
            {
                payloadObject.WithString("componentName", m_componentName.value());
            }
        }

        void ResumeComponentRequest::s_loadFromJsonView(
            ResumeComponentRequest &resumeComponentRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("componentName"))
            {
                resumeComponentRequest.m_componentName =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("componentName"));
            }
        }

        const char *ResumeComponentRequest::MODEL_NAME = "aws.greengrass#ResumeComponentRequest";

        Aws::Crt::String ResumeComponentRequest::GetModelName() const noexcept
        {
            return ResumeComponentRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> ResumeComponentRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<ResumeComponentRequest> shape(
                Aws::Crt::New<ResumeComponentRequest>(allocator), ResumeComponentRequest::s_customDeleter);
            shape->m_allocator = allocator;
            ResumeComponentRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void ResumeComponentRequest::s_customDeleter(ResumeComponentRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void RestartComponentResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_restartStatus.has_value())
            {
                payloadObject.WithString("restartStatus", m_restartStatus.value());
            }
            if (m_message.has_value())
            {
                payloadObject.WithString("message", m_message.value());
            }
        }

        void RestartComponentResponse::s_loadFromJsonView(
            RestartComponentResponse &restartComponentResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("restartStatus"))
            {
                restartComponentResponse.m_restartStatus =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("restartStatus"));
            }
            if (jsonView.ValueExists("message"))
            {
                restartComponentResponse.m_message =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("message"));
            }
        }

        void RestartComponentResponse::SetRestartStatus(RequestStatus restartStatus) noexcept
        {
            switch (restartStatus)
            {
                case REQUEST_STATUS_SUCCEEDED:
                    m_restartStatus = Aws::Crt::String("SUCCEEDED");
                    break;
                case REQUEST_STATUS_FAILED:
                    m_restartStatus = Aws::Crt::String("FAILED");
                    break;
                default:
                    break;
            }
        }

        Aws::Crt::Optional<RequestStatus> RestartComponentResponse::GetRestartStatus() noexcept
        {
            if (!m_restartStatus.has_value())
                return Aws::Crt::Optional<RequestStatus>();
            if (m_restartStatus.value() == Aws::Crt::String("SUCCEEDED"))
            {
                return Aws::Crt::Optional<RequestStatus>(REQUEST_STATUS_SUCCEEDED);
            }
            if (m_restartStatus.value() == Aws::Crt::String("FAILED"))
            {
                return Aws::Crt::Optional<RequestStatus>(REQUEST_STATUS_FAILED);
            }

            return Aws::Crt::Optional<RequestStatus>();
        }

        const char *RestartComponentResponse::MODEL_NAME = "aws.greengrass#RestartComponentResponse";

        Aws::Crt::String RestartComponentResponse::GetModelName() const noexcept
        {
            return RestartComponentResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> RestartComponentResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<RestartComponentResponse> shape(
                Aws::Crt::New<RestartComponentResponse>(allocator), RestartComponentResponse::s_customDeleter);
            shape->m_allocator = allocator;
            RestartComponentResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void RestartComponentResponse::s_customDeleter(RestartComponentResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void RestartComponentRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_componentName.has_value())
            {
                payloadObject.WithString("componentName", m_componentName.value());
            }
        }

        void RestartComponentRequest::s_loadFromJsonView(
            RestartComponentRequest &restartComponentRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("componentName"))
            {
                restartComponentRequest.m_componentName =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("componentName"));
            }
        }

        const char *RestartComponentRequest::MODEL_NAME = "aws.greengrass#RestartComponentRequest";

        Aws::Crt::String RestartComponentRequest::GetModelName() const noexcept
        {
            return RestartComponentRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> RestartComponentRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<RestartComponentRequest> shape(
                Aws::Crt::New<RestartComponentRequest>(allocator), RestartComponentRequest::s_customDeleter);
            shape->m_allocator = allocator;
            RestartComponentRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void RestartComponentRequest::s_customDeleter(RestartComponentRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void PutComponentMetricResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            (void)payloadObject;
        }

        void PutComponentMetricResponse::s_loadFromJsonView(
            PutComponentMetricResponse &putComponentMetricResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            (void)putComponentMetricResponse;
            (void)jsonView;
        }

        const char *PutComponentMetricResponse::MODEL_NAME = "aws.greengrass#PutComponentMetricResponse";

        Aws::Crt::String PutComponentMetricResponse::GetModelName() const noexcept
        {
            return PutComponentMetricResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> PutComponentMetricResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<PutComponentMetricResponse> shape(
                Aws::Crt::New<PutComponentMetricResponse>(allocator), PutComponentMetricResponse::s_customDeleter);
            shape->m_allocator = allocator;
            PutComponentMetricResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void PutComponentMetricResponse::s_customDeleter(PutComponentMetricResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void PutComponentMetricRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_metrics.has_value())
            {
                Aws::Crt::JsonObject metricsList;
                Aws::Crt::Vector<Aws::Crt::JsonObject> metricsListJsonArray;
                for (const auto &metricsListItem : m_metrics.value())
                {
                    Aws::Crt::JsonObject metricsListJsonArrayItem;
                    metricsListItem.SerializeToJsonObject(metricsListJsonArrayItem);
                    metricsListJsonArray.emplace_back(std::move(metricsListJsonArrayItem));
                }
                metricsList.AsArray(std::move(metricsListJsonArray));
                payloadObject.WithObject("metrics", std::move(metricsList));
            }
        }

        void PutComponentMetricRequest::s_loadFromJsonView(
            PutComponentMetricRequest &putComponentMetricRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("metrics"))
            {
                putComponentMetricRequest.m_metrics = Aws::Crt::Vector<Metric>();
                for (const Aws::Crt::JsonView &metricsListJsonView : jsonView.GetArray("metrics"))
                {
                    Aws::Crt::Optional<Metric> metricsListItem;
                    metricsListItem = Metric();
                    Metric::s_loadFromJsonView(metricsListItem.value(), metricsListJsonView);
                    putComponentMetricRequest.m_metrics.value().push_back(metricsListItem.value());
                }
            }
        }

        const char *PutComponentMetricRequest::MODEL_NAME = "aws.greengrass#PutComponentMetricRequest";

        Aws::Crt::String PutComponentMetricRequest::GetModelName() const noexcept
        {
            return PutComponentMetricRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> PutComponentMetricRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<PutComponentMetricRequest> shape(
                Aws::Crt::New<PutComponentMetricRequest>(allocator), PutComponentMetricRequest::s_customDeleter);
            shape->m_allocator = allocator;
            PutComponentMetricRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void PutComponentMetricRequest::s_customDeleter(PutComponentMetricRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void PublishToTopicResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            (void)payloadObject;
        }

        void PublishToTopicResponse::s_loadFromJsonView(
            PublishToTopicResponse &publishToTopicResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            (void)publishToTopicResponse;
            (void)jsonView;
        }

        const char *PublishToTopicResponse::MODEL_NAME = "aws.greengrass#PublishToTopicResponse";

        Aws::Crt::String PublishToTopicResponse::GetModelName() const noexcept
        {
            return PublishToTopicResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> PublishToTopicResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<PublishToTopicResponse> shape(
                Aws::Crt::New<PublishToTopicResponse>(allocator), PublishToTopicResponse::s_customDeleter);
            shape->m_allocator = allocator;
            PublishToTopicResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void PublishToTopicResponse::s_customDeleter(PublishToTopicResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void PublishToTopicRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_topic.has_value())
            {
                payloadObject.WithString("topic", m_topic.value());
            }
            if (m_publishMessage.has_value())
            {
                Aws::Crt::JsonObject publishMessageValue;
                m_publishMessage.value().SerializeToJsonObject(publishMessageValue);
                payloadObject.WithObject("publishMessage", std::move(publishMessageValue));
            }
        }

        void PublishToTopicRequest::s_loadFromJsonView(
            PublishToTopicRequest &publishToTopicRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("topic"))
            {
                publishToTopicRequest.m_topic = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("topic"));
            }
            if (jsonView.ValueExists("publishMessage"))
            {
                publishToTopicRequest.m_publishMessage = PublishMessage();
                PublishMessage::s_loadFromJsonView(
                    publishToTopicRequest.m_publishMessage.value(), jsonView.GetJsonObject("publishMessage"));
            }
        }

        const char *PublishToTopicRequest::MODEL_NAME = "aws.greengrass#PublishToTopicRequest";

        Aws::Crt::String PublishToTopicRequest::GetModelName() const noexcept
        {
            return PublishToTopicRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> PublishToTopicRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<PublishToTopicRequest> shape(
                Aws::Crt::New<PublishToTopicRequest>(allocator), PublishToTopicRequest::s_customDeleter);
            shape->m_allocator = allocator;
            PublishToTopicRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void PublishToTopicRequest::s_customDeleter(PublishToTopicRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void PublishToIoTCoreResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            (void)payloadObject;
        }

        void PublishToIoTCoreResponse::s_loadFromJsonView(
            PublishToIoTCoreResponse &publishToIoTCoreResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            (void)publishToIoTCoreResponse;
            (void)jsonView;
        }

        const char *PublishToIoTCoreResponse::MODEL_NAME = "aws.greengrass#PublishToIoTCoreResponse";

        Aws::Crt::String PublishToIoTCoreResponse::GetModelName() const noexcept
        {
            return PublishToIoTCoreResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> PublishToIoTCoreResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<PublishToIoTCoreResponse> shape(
                Aws::Crt::New<PublishToIoTCoreResponse>(allocator), PublishToIoTCoreResponse::s_customDeleter);
            shape->m_allocator = allocator;
            PublishToIoTCoreResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void PublishToIoTCoreResponse::s_customDeleter(PublishToIoTCoreResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void PublishToIoTCoreRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_topicName.has_value())
            {
                payloadObject.WithString("topicName", m_topicName.value());
            }
            if (m_qos.has_value())
            {
                payloadObject.WithString("qos", m_qos.value());
            }
            if (m_payload.has_value())
            {
                if (m_payload.value().size() > 0)
                {
                    payloadObject.WithString("payload", Aws::Crt::Base64Encode(m_payload.value()));
                }
            }
            if (m_retain.has_value())
            {
                payloadObject.WithBool("retain", m_retain.value());
            }
            if (m_userProperties.has_value())
            {
                Aws::Crt::JsonObject listOfUserProperties;
                Aws::Crt::Vector<Aws::Crt::JsonObject> listOfUserPropertiesJsonArray;
                for (const auto &listOfUserPropertiesItem : m_userProperties.value())
                {
                    Aws::Crt::JsonObject listOfUserPropertiesJsonArrayItem;
                    listOfUserPropertiesItem.SerializeToJsonObject(listOfUserPropertiesJsonArrayItem);
                    listOfUserPropertiesJsonArray.emplace_back(std::move(listOfUserPropertiesJsonArrayItem));
                }
                listOfUserProperties.AsArray(std::move(listOfUserPropertiesJsonArray));
                payloadObject.WithObject("userProperties", std::move(listOfUserProperties));
            }
            if (m_messageExpiryIntervalSeconds.has_value())
            {
                payloadObject.WithInt64("messageExpiryIntervalSeconds", m_messageExpiryIntervalSeconds.value());
            }
            if (m_correlationData.has_value())
            {
                if (m_correlationData.value().size() > 0)
                {
                    payloadObject.WithString("correlationData", Aws::Crt::Base64Encode(m_correlationData.value()));
                }
            }
            if (m_responseTopic.has_value())
            {
                payloadObject.WithString("responseTopic", m_responseTopic.value());
            }
            if (m_payloadFormat.has_value())
            {
                payloadObject.WithString("payloadFormat", m_payloadFormat.value());
            }
            if (m_contentType.has_value())
            {
                payloadObject.WithString("contentType", m_contentType.value());
            }
        }

        void PublishToIoTCoreRequest::s_loadFromJsonView(
            PublishToIoTCoreRequest &publishToIoTCoreRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("topicName"))
            {
                publishToIoTCoreRequest.m_topicName =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("topicName"));
            }
            if (jsonView.ValueExists("qos"))
            {
                publishToIoTCoreRequest.m_qos = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("qos"));
            }
            if (jsonView.ValueExists("payload"))
            {
                if (jsonView.GetString("payload").size() > 0)
                {
                    publishToIoTCoreRequest.m_payload = Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>>(
                        Aws::Crt::Base64Decode(jsonView.GetString("payload")));
                }
            }
            if (jsonView.ValueExists("retain"))
            {
                publishToIoTCoreRequest.m_retain = Aws::Crt::Optional<bool>(jsonView.GetBool("retain"));
            }
            if (jsonView.ValueExists("userProperties"))
            {
                publishToIoTCoreRequest.m_userProperties = Aws::Crt::Vector<UserProperty>();
                for (const Aws::Crt::JsonView &listOfUserPropertiesJsonView : jsonView.GetArray("userProperties"))
                {
                    Aws::Crt::Optional<UserProperty> listOfUserPropertiesItem;
                    listOfUserPropertiesItem = UserProperty();
                    UserProperty::s_loadFromJsonView(listOfUserPropertiesItem.value(), listOfUserPropertiesJsonView);
                    publishToIoTCoreRequest.m_userProperties.value().push_back(listOfUserPropertiesItem.value());
                }
            }
            if (jsonView.ValueExists("messageExpiryIntervalSeconds"))
            {
                publishToIoTCoreRequest.m_messageExpiryIntervalSeconds =
                    Aws::Crt::Optional<int64_t>(jsonView.GetInt64("messageExpiryIntervalSeconds"));
            }
            if (jsonView.ValueExists("correlationData"))
            {
                if (jsonView.GetString("correlationData").size() > 0)
                {
                    publishToIoTCoreRequest.m_correlationData = Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>>(
                        Aws::Crt::Base64Decode(jsonView.GetString("correlationData")));
                }
            }
            if (jsonView.ValueExists("responseTopic"))
            {
                publishToIoTCoreRequest.m_responseTopic =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("responseTopic"));
            }
            if (jsonView.ValueExists("payloadFormat"))
            {
                publishToIoTCoreRequest.m_payloadFormat =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("payloadFormat"));
            }
            if (jsonView.ValueExists("contentType"))
            {
                publishToIoTCoreRequest.m_contentType =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("contentType"));
            }
        }

        void PublishToIoTCoreRequest::SetQos(QOS qos) noexcept
        {
            switch (qos)
            {
                case QOS_AT_MOST_ONCE:
                    m_qos = Aws::Crt::String("0");
                    break;
                case QOS_AT_LEAST_ONCE:
                    m_qos = Aws::Crt::String("1");
                    break;
                default:
                    break;
            }
        }

        Aws::Crt::Optional<QOS> PublishToIoTCoreRequest::GetQos() noexcept
        {
            if (!m_qos.has_value())
                return Aws::Crt::Optional<QOS>();
            if (m_qos.value() == Aws::Crt::String("0"))
            {
                return Aws::Crt::Optional<QOS>(QOS_AT_MOST_ONCE);
            }
            if (m_qos.value() == Aws::Crt::String("1"))
            {
                return Aws::Crt::Optional<QOS>(QOS_AT_LEAST_ONCE);
            }

            return Aws::Crt::Optional<QOS>();
        }
        void PublishToIoTCoreRequest::SetPayloadFormat(PayloadFormat payloadFormat) noexcept
        {
            switch (payloadFormat)
            {
                case PAYLOAD_FORMAT_BYTES:
                    m_payloadFormat = Aws::Crt::String("0");
                    break;
                case PAYLOAD_FORMAT_UTF8:
                    m_payloadFormat = Aws::Crt::String("1");
                    break;
                default:
                    break;
            }
        }

        Aws::Crt::Optional<PayloadFormat> PublishToIoTCoreRequest::GetPayloadFormat() noexcept
        {
            if (!m_payloadFormat.has_value())
                return Aws::Crt::Optional<PayloadFormat>();
            if (m_payloadFormat.value() == Aws::Crt::String("0"))
            {
                return Aws::Crt::Optional<PayloadFormat>(PAYLOAD_FORMAT_BYTES);
            }
            if (m_payloadFormat.value() == Aws::Crt::String("1"))
            {
                return Aws::Crt::Optional<PayloadFormat>(PAYLOAD_FORMAT_UTF8);
            }

            return Aws::Crt::Optional<PayloadFormat>();
        }

        const char *PublishToIoTCoreRequest::MODEL_NAME = "aws.greengrass#PublishToIoTCoreRequest";

        Aws::Crt::String PublishToIoTCoreRequest::GetModelName() const noexcept
        {
            return PublishToIoTCoreRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> PublishToIoTCoreRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<PublishToIoTCoreRequest> shape(
                Aws::Crt::New<PublishToIoTCoreRequest>(allocator), PublishToIoTCoreRequest::s_customDeleter);
            shape->m_allocator = allocator;
            PublishToIoTCoreRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void PublishToIoTCoreRequest::s_customDeleter(PublishToIoTCoreRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void PauseComponentResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            (void)payloadObject;
        }

        void PauseComponentResponse::s_loadFromJsonView(
            PauseComponentResponse &pauseComponentResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            (void)pauseComponentResponse;
            (void)jsonView;
        }

        const char *PauseComponentResponse::MODEL_NAME = "aws.greengrass#PauseComponentResponse";

        Aws::Crt::String PauseComponentResponse::GetModelName() const noexcept
        {
            return PauseComponentResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> PauseComponentResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<PauseComponentResponse> shape(
                Aws::Crt::New<PauseComponentResponse>(allocator), PauseComponentResponse::s_customDeleter);
            shape->m_allocator = allocator;
            PauseComponentResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void PauseComponentResponse::s_customDeleter(PauseComponentResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void PauseComponentRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_componentName.has_value())
            {
                payloadObject.WithString("componentName", m_componentName.value());
            }
        }

        void PauseComponentRequest::s_loadFromJsonView(
            PauseComponentRequest &pauseComponentRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("componentName"))
            {
                pauseComponentRequest.m_componentName =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("componentName"));
            }
        }

        const char *PauseComponentRequest::MODEL_NAME = "aws.greengrass#PauseComponentRequest";

        Aws::Crt::String PauseComponentRequest::GetModelName() const noexcept
        {
            return PauseComponentRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> PauseComponentRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<PauseComponentRequest> shape(
                Aws::Crt::New<PauseComponentRequest>(allocator), PauseComponentRequest::s_customDeleter);
            shape->m_allocator = allocator;
            PauseComponentRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void PauseComponentRequest::s_customDeleter(PauseComponentRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void ListNamedShadowsForThingResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_results.has_value())
            {
                Aws::Crt::JsonObject namedShadowList;
                Aws::Crt::Vector<Aws::Crt::JsonObject> namedShadowListJsonArray;
                for (const auto &namedShadowListItem : m_results.value())
                {
                    Aws::Crt::JsonObject namedShadowListJsonArrayItem;
                    namedShadowListJsonArrayItem.AsString(namedShadowListItem);
                    namedShadowListJsonArray.emplace_back(std::move(namedShadowListJsonArrayItem));
                }
                namedShadowList.AsArray(std::move(namedShadowListJsonArray));
                payloadObject.WithObject("results", std::move(namedShadowList));
            }
            if (m_timestamp.has_value())
            {
                payloadObject.WithDouble("timestamp", m_timestamp.value().SecondsWithMSPrecision());
            }
            if (m_nextToken.has_value())
            {
                payloadObject.WithString("nextToken", m_nextToken.value());
            }
        }

        void ListNamedShadowsForThingResponse::s_loadFromJsonView(
            ListNamedShadowsForThingResponse &listNamedShadowsForThingResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("results"))
            {
                listNamedShadowsForThingResponse.m_results = Aws::Crt::Vector<Aws::Crt::String>();
                for (const Aws::Crt::JsonView &namedShadowListJsonView : jsonView.GetArray("results"))
                {
                    Aws::Crt::Optional<Aws::Crt::String> namedShadowListItem;
                    namedShadowListItem = Aws::Crt::Optional<Aws::Crt::String>(namedShadowListJsonView.AsString());
                    listNamedShadowsForThingResponse.m_results.value().push_back(namedShadowListItem.value());
                }
            }
            if (jsonView.ValueExists("timestamp"))
            {
                listNamedShadowsForThingResponse.m_timestamp =
                    Aws::Crt::Optional<Aws::Crt::DateTime>(Aws::Crt::DateTime(jsonView.GetDouble("timestamp")));
            }
            if (jsonView.ValueExists("nextToken"))
            {
                listNamedShadowsForThingResponse.m_nextToken =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("nextToken"));
            }
        }

        const char *ListNamedShadowsForThingResponse::MODEL_NAME = "aws.greengrass#ListNamedShadowsForThingResponse";

        Aws::Crt::String ListNamedShadowsForThingResponse::GetModelName() const noexcept
        {
            return ListNamedShadowsForThingResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> ListNamedShadowsForThingResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<ListNamedShadowsForThingResponse> shape(
                Aws::Crt::New<ListNamedShadowsForThingResponse>(allocator),
                ListNamedShadowsForThingResponse::s_customDeleter);
            shape->m_allocator = allocator;
            ListNamedShadowsForThingResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void ListNamedShadowsForThingResponse::s_customDeleter(ListNamedShadowsForThingResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void ListNamedShadowsForThingRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_thingName.has_value())
            {
                payloadObject.WithString("thingName", m_thingName.value());
            }
            if (m_nextToken.has_value())
            {
                payloadObject.WithString("nextToken", m_nextToken.value());
            }
            if (m_pageSize.has_value())
            {
                payloadObject.WithInteger("pageSize", m_pageSize.value());
            }
        }

        void ListNamedShadowsForThingRequest::s_loadFromJsonView(
            ListNamedShadowsForThingRequest &listNamedShadowsForThingRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("thingName"))
            {
                listNamedShadowsForThingRequest.m_thingName =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("thingName"));
            }
            if (jsonView.ValueExists("nextToken"))
            {
                listNamedShadowsForThingRequest.m_nextToken =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("nextToken"));
            }
            if (jsonView.ValueExists("pageSize"))
            {
                listNamedShadowsForThingRequest.m_pageSize = Aws::Crt::Optional<int>(jsonView.GetInteger("pageSize"));
            }
        }

        const char *ListNamedShadowsForThingRequest::MODEL_NAME = "aws.greengrass#ListNamedShadowsForThingRequest";

        Aws::Crt::String ListNamedShadowsForThingRequest::GetModelName() const noexcept
        {
            return ListNamedShadowsForThingRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> ListNamedShadowsForThingRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<ListNamedShadowsForThingRequest> shape(
                Aws::Crt::New<ListNamedShadowsForThingRequest>(allocator),
                ListNamedShadowsForThingRequest::s_customDeleter);
            shape->m_allocator = allocator;
            ListNamedShadowsForThingRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void ListNamedShadowsForThingRequest::s_customDeleter(ListNamedShadowsForThingRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void ListLocalDeploymentsResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_localDeployments.has_value())
            {
                Aws::Crt::JsonObject listOfLocalDeployments;
                Aws::Crt::Vector<Aws::Crt::JsonObject> listOfLocalDeploymentsJsonArray;
                for (const auto &listOfLocalDeploymentsItem : m_localDeployments.value())
                {
                    Aws::Crt::JsonObject listOfLocalDeploymentsJsonArrayItem;
                    listOfLocalDeploymentsItem.SerializeToJsonObject(listOfLocalDeploymentsJsonArrayItem);
                    listOfLocalDeploymentsJsonArray.emplace_back(std::move(listOfLocalDeploymentsJsonArrayItem));
                }
                listOfLocalDeployments.AsArray(std::move(listOfLocalDeploymentsJsonArray));
                payloadObject.WithObject("localDeployments", std::move(listOfLocalDeployments));
            }
        }

        void ListLocalDeploymentsResponse::s_loadFromJsonView(
            ListLocalDeploymentsResponse &listLocalDeploymentsResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("localDeployments"))
            {
                listLocalDeploymentsResponse.m_localDeployments = Aws::Crt::Vector<LocalDeployment>();
                for (const Aws::Crt::JsonView &listOfLocalDeploymentsJsonView : jsonView.GetArray("localDeployments"))
                {
                    Aws::Crt::Optional<LocalDeployment> listOfLocalDeploymentsItem;
                    listOfLocalDeploymentsItem = LocalDeployment();
                    LocalDeployment::s_loadFromJsonView(
                        listOfLocalDeploymentsItem.value(), listOfLocalDeploymentsJsonView);
                    listLocalDeploymentsResponse.m_localDeployments.value().push_back(
                        listOfLocalDeploymentsItem.value());
                }
            }
        }

        const char *ListLocalDeploymentsResponse::MODEL_NAME = "aws.greengrass#ListLocalDeploymentsResponse";

        Aws::Crt::String ListLocalDeploymentsResponse::GetModelName() const noexcept
        {
            return ListLocalDeploymentsResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> ListLocalDeploymentsResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<ListLocalDeploymentsResponse> shape(
                Aws::Crt::New<ListLocalDeploymentsResponse>(allocator), ListLocalDeploymentsResponse::s_customDeleter);
            shape->m_allocator = allocator;
            ListLocalDeploymentsResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void ListLocalDeploymentsResponse::s_customDeleter(ListLocalDeploymentsResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void ListLocalDeploymentsRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            (void)payloadObject;
        }

        void ListLocalDeploymentsRequest::s_loadFromJsonView(
            ListLocalDeploymentsRequest &listLocalDeploymentsRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            (void)listLocalDeploymentsRequest;
            (void)jsonView;
        }

        const char *ListLocalDeploymentsRequest::MODEL_NAME = "aws.greengrass#ListLocalDeploymentsRequest";

        Aws::Crt::String ListLocalDeploymentsRequest::GetModelName() const noexcept
        {
            return ListLocalDeploymentsRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> ListLocalDeploymentsRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<ListLocalDeploymentsRequest> shape(
                Aws::Crt::New<ListLocalDeploymentsRequest>(allocator), ListLocalDeploymentsRequest::s_customDeleter);
            shape->m_allocator = allocator;
            ListLocalDeploymentsRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void ListLocalDeploymentsRequest::s_customDeleter(ListLocalDeploymentsRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void ListComponentsResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_components.has_value())
            {
                Aws::Crt::JsonObject listOfComponents;
                Aws::Crt::Vector<Aws::Crt::JsonObject> listOfComponentsJsonArray;
                for (const auto &listOfComponentsItem : m_components.value())
                {
                    Aws::Crt::JsonObject listOfComponentsJsonArrayItem;
                    listOfComponentsItem.SerializeToJsonObject(listOfComponentsJsonArrayItem);
                    listOfComponentsJsonArray.emplace_back(std::move(listOfComponentsJsonArrayItem));
                }
                listOfComponents.AsArray(std::move(listOfComponentsJsonArray));
                payloadObject.WithObject("components", std::move(listOfComponents));
            }
        }

        void ListComponentsResponse::s_loadFromJsonView(
            ListComponentsResponse &listComponentsResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("components"))
            {
                listComponentsResponse.m_components = Aws::Crt::Vector<ComponentDetails>();
                for (const Aws::Crt::JsonView &listOfComponentsJsonView : jsonView.GetArray("components"))
                {
                    Aws::Crt::Optional<ComponentDetails> listOfComponentsItem;
                    listOfComponentsItem = ComponentDetails();
                    ComponentDetails::s_loadFromJsonView(listOfComponentsItem.value(), listOfComponentsJsonView);
                    listComponentsResponse.m_components.value().push_back(listOfComponentsItem.value());
                }
            }
        }

        const char *ListComponentsResponse::MODEL_NAME = "aws.greengrass#ListComponentsResponse";

        Aws::Crt::String ListComponentsResponse::GetModelName() const noexcept
        {
            return ListComponentsResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> ListComponentsResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<ListComponentsResponse> shape(
                Aws::Crt::New<ListComponentsResponse>(allocator), ListComponentsResponse::s_customDeleter);
            shape->m_allocator = allocator;
            ListComponentsResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void ListComponentsResponse::s_customDeleter(ListComponentsResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void ListComponentsRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            (void)payloadObject;
        }

        void ListComponentsRequest::s_loadFromJsonView(
            ListComponentsRequest &listComponentsRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            (void)listComponentsRequest;
            (void)jsonView;
        }

        const char *ListComponentsRequest::MODEL_NAME = "aws.greengrass#ListComponentsRequest";

        Aws::Crt::String ListComponentsRequest::GetModelName() const noexcept
        {
            return ListComponentsRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> ListComponentsRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<ListComponentsRequest> shape(
                Aws::Crt::New<ListComponentsRequest>(allocator), ListComponentsRequest::s_customDeleter);
            shape->m_allocator = allocator;
            ListComponentsRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void ListComponentsRequest::s_customDeleter(ListComponentsRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void GetThingShadowResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_payload.has_value())
            {
                if (m_payload.value().size() > 0)
                {
                    payloadObject.WithString("payload", Aws::Crt::Base64Encode(m_payload.value()));
                }
            }
        }

        void GetThingShadowResponse::s_loadFromJsonView(
            GetThingShadowResponse &getThingShadowResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("payload"))
            {
                if (jsonView.GetString("payload").size() > 0)
                {
                    getThingShadowResponse.m_payload = Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>>(
                        Aws::Crt::Base64Decode(jsonView.GetString("payload")));
                }
            }
        }

        const char *GetThingShadowResponse::MODEL_NAME = "aws.greengrass#GetThingShadowResponse";

        Aws::Crt::String GetThingShadowResponse::GetModelName() const noexcept
        {
            return GetThingShadowResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> GetThingShadowResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<GetThingShadowResponse> shape(
                Aws::Crt::New<GetThingShadowResponse>(allocator), GetThingShadowResponse::s_customDeleter);
            shape->m_allocator = allocator;
            GetThingShadowResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void GetThingShadowResponse::s_customDeleter(GetThingShadowResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void GetThingShadowRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_thingName.has_value())
            {
                payloadObject.WithString("thingName", m_thingName.value());
            }
            if (m_shadowName.has_value())
            {
                payloadObject.WithString("shadowName", m_shadowName.value());
            }
        }

        void GetThingShadowRequest::s_loadFromJsonView(
            GetThingShadowRequest &getThingShadowRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("thingName"))
            {
                getThingShadowRequest.m_thingName =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("thingName"));
            }
            if (jsonView.ValueExists("shadowName"))
            {
                getThingShadowRequest.m_shadowName =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("shadowName"));
            }
        }

        const char *GetThingShadowRequest::MODEL_NAME = "aws.greengrass#GetThingShadowRequest";

        Aws::Crt::String GetThingShadowRequest::GetModelName() const noexcept
        {
            return GetThingShadowRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> GetThingShadowRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<GetThingShadowRequest> shape(
                Aws::Crt::New<GetThingShadowRequest>(allocator), GetThingShadowRequest::s_customDeleter);
            shape->m_allocator = allocator;
            GetThingShadowRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void GetThingShadowRequest::s_customDeleter(GetThingShadowRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void GetSecretValueResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_secretId.has_value())
            {
                payloadObject.WithString("secretId", m_secretId.value());
            }
            if (m_versionId.has_value())
            {
                payloadObject.WithString("versionId", m_versionId.value());
            }
            if (m_versionStage.has_value())
            {
                Aws::Crt::JsonObject secretVersionList;
                Aws::Crt::Vector<Aws::Crt::JsonObject> secretVersionListJsonArray;
                for (const auto &secretVersionListItem : m_versionStage.value())
                {
                    Aws::Crt::JsonObject secretVersionListJsonArrayItem;
                    secretVersionListJsonArrayItem.AsString(secretVersionListItem);
                    secretVersionListJsonArray.emplace_back(std::move(secretVersionListJsonArrayItem));
                }
                secretVersionList.AsArray(std::move(secretVersionListJsonArray));
                payloadObject.WithObject("versionStage", std::move(secretVersionList));
            }
            if (m_secretValue.has_value())
            {
                Aws::Crt::JsonObject secretValueValue;
                m_secretValue.value().SerializeToJsonObject(secretValueValue);
                payloadObject.WithObject("secretValue", std::move(secretValueValue));
            }
        }

        void GetSecretValueResponse::s_loadFromJsonView(
            GetSecretValueResponse &getSecretValueResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("secretId"))
            {
                getSecretValueResponse.m_secretId =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("secretId"));
            }
            if (jsonView.ValueExists("versionId"))
            {
                getSecretValueResponse.m_versionId =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("versionId"));
            }
            if (jsonView.ValueExists("versionStage"))
            {
                getSecretValueResponse.m_versionStage = Aws::Crt::Vector<Aws::Crt::String>();
                for (const Aws::Crt::JsonView &secretVersionListJsonView : jsonView.GetArray("versionStage"))
                {
                    Aws::Crt::Optional<Aws::Crt::String> secretVersionListItem;
                    secretVersionListItem = Aws::Crt::Optional<Aws::Crt::String>(secretVersionListJsonView.AsString());
                    getSecretValueResponse.m_versionStage.value().push_back(secretVersionListItem.value());
                }
            }
            if (jsonView.ValueExists("secretValue"))
            {
                getSecretValueResponse.m_secretValue = SecretValue();
                SecretValue::s_loadFromJsonView(
                    getSecretValueResponse.m_secretValue.value(), jsonView.GetJsonObject("secretValue"));
            }
        }

        const char *GetSecretValueResponse::MODEL_NAME = "aws.greengrass#GetSecretValueResponse";

        Aws::Crt::String GetSecretValueResponse::GetModelName() const noexcept
        {
            return GetSecretValueResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> GetSecretValueResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<GetSecretValueResponse> shape(
                Aws::Crt::New<GetSecretValueResponse>(allocator), GetSecretValueResponse::s_customDeleter);
            shape->m_allocator = allocator;
            GetSecretValueResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void GetSecretValueResponse::s_customDeleter(GetSecretValueResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void GetSecretValueRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_secretId.has_value())
            {
                payloadObject.WithString("secretId", m_secretId.value());
            }
            if (m_versionId.has_value())
            {
                payloadObject.WithString("versionId", m_versionId.value());
            }
            if (m_versionStage.has_value())
            {
                payloadObject.WithString("versionStage", m_versionStage.value());
            }
            if (m_refresh.has_value())
            {
                payloadObject.WithBool("refresh", m_refresh.value());
            }
        }

        void GetSecretValueRequest::s_loadFromJsonView(
            GetSecretValueRequest &getSecretValueRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("secretId"))
            {
                getSecretValueRequest.m_secretId = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("secretId"));
            }
            if (jsonView.ValueExists("versionId"))
            {
                getSecretValueRequest.m_versionId =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("versionId"));
            }
            if (jsonView.ValueExists("versionStage"))
            {
                getSecretValueRequest.m_versionStage =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("versionStage"));
            }
            if (jsonView.ValueExists("refresh"))
            {
                getSecretValueRequest.m_refresh = Aws::Crt::Optional<bool>(jsonView.GetBool("refresh"));
            }
        }

        const char *GetSecretValueRequest::MODEL_NAME = "aws.greengrass#GetSecretValueRequest";

        Aws::Crt::String GetSecretValueRequest::GetModelName() const noexcept
        {
            return GetSecretValueRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> GetSecretValueRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<GetSecretValueRequest> shape(
                Aws::Crt::New<GetSecretValueRequest>(allocator), GetSecretValueRequest::s_customDeleter);
            shape->m_allocator = allocator;
            GetSecretValueRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void GetSecretValueRequest::s_customDeleter(GetSecretValueRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void GetLocalDeploymentStatusResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_deployment.has_value())
            {
                Aws::Crt::JsonObject localDeploymentValue;
                m_deployment.value().SerializeToJsonObject(localDeploymentValue);
                payloadObject.WithObject("deployment", std::move(localDeploymentValue));
            }
        }

        void GetLocalDeploymentStatusResponse::s_loadFromJsonView(
            GetLocalDeploymentStatusResponse &getLocalDeploymentStatusResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("deployment"))
            {
                getLocalDeploymentStatusResponse.m_deployment = LocalDeployment();
                LocalDeployment::s_loadFromJsonView(
                    getLocalDeploymentStatusResponse.m_deployment.value(), jsonView.GetJsonObject("deployment"));
            }
        }

        const char *GetLocalDeploymentStatusResponse::MODEL_NAME = "aws.greengrass#GetLocalDeploymentStatusResponse";

        Aws::Crt::String GetLocalDeploymentStatusResponse::GetModelName() const noexcept
        {
            return GetLocalDeploymentStatusResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> GetLocalDeploymentStatusResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<GetLocalDeploymentStatusResponse> shape(
                Aws::Crt::New<GetLocalDeploymentStatusResponse>(allocator),
                GetLocalDeploymentStatusResponse::s_customDeleter);
            shape->m_allocator = allocator;
            GetLocalDeploymentStatusResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void GetLocalDeploymentStatusResponse::s_customDeleter(GetLocalDeploymentStatusResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void GetLocalDeploymentStatusRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_deploymentId.has_value())
            {
                payloadObject.WithString("deploymentId", m_deploymentId.value());
            }
        }

        void GetLocalDeploymentStatusRequest::s_loadFromJsonView(
            GetLocalDeploymentStatusRequest &getLocalDeploymentStatusRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("deploymentId"))
            {
                getLocalDeploymentStatusRequest.m_deploymentId =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("deploymentId"));
            }
        }

        const char *GetLocalDeploymentStatusRequest::MODEL_NAME = "aws.greengrass#GetLocalDeploymentStatusRequest";

        Aws::Crt::String GetLocalDeploymentStatusRequest::GetModelName() const noexcept
        {
            return GetLocalDeploymentStatusRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> GetLocalDeploymentStatusRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<GetLocalDeploymentStatusRequest> shape(
                Aws::Crt::New<GetLocalDeploymentStatusRequest>(allocator),
                GetLocalDeploymentStatusRequest::s_customDeleter);
            shape->m_allocator = allocator;
            GetLocalDeploymentStatusRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void GetLocalDeploymentStatusRequest::s_customDeleter(GetLocalDeploymentStatusRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void GetConfigurationResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_componentName.has_value())
            {
                payloadObject.WithString("componentName", m_componentName.value());
            }
            if (m_value.has_value())
            {
                payloadObject.WithObject("value", m_value.value());
            }
        }

        void GetConfigurationResponse::s_loadFromJsonView(
            GetConfigurationResponse &getConfigurationResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("componentName"))
            {
                getConfigurationResponse.m_componentName =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("componentName"));
            }
            if (jsonView.ValueExists("value"))
            {
                getConfigurationResponse.m_value =
                    Aws::Crt::Optional<Aws::Crt::JsonObject>(jsonView.GetJsonObject("value").Materialize());
            }
        }

        const char *GetConfigurationResponse::MODEL_NAME = "aws.greengrass#GetConfigurationResponse";

        Aws::Crt::String GetConfigurationResponse::GetModelName() const noexcept
        {
            return GetConfigurationResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> GetConfigurationResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<GetConfigurationResponse> shape(
                Aws::Crt::New<GetConfigurationResponse>(allocator), GetConfigurationResponse::s_customDeleter);
            shape->m_allocator = allocator;
            GetConfigurationResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void GetConfigurationResponse::s_customDeleter(GetConfigurationResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void GetConfigurationRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_componentName.has_value())
            {
                payloadObject.WithString("componentName", m_componentName.value());
            }
            if (m_keyPath.has_value())
            {
                Aws::Crt::JsonObject keyPath;
                Aws::Crt::Vector<Aws::Crt::JsonObject> keyPathJsonArray;
                for (const auto &keyPathItem : m_keyPath.value())
                {
                    Aws::Crt::JsonObject keyPathJsonArrayItem;
                    keyPathJsonArrayItem.AsString(keyPathItem);
                    keyPathJsonArray.emplace_back(std::move(keyPathJsonArrayItem));
                }
                keyPath.AsArray(std::move(keyPathJsonArray));
                payloadObject.WithObject("keyPath", std::move(keyPath));
            }
        }

        void GetConfigurationRequest::s_loadFromJsonView(
            GetConfigurationRequest &getConfigurationRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("componentName"))
            {
                getConfigurationRequest.m_componentName =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("componentName"));
            }
            if (jsonView.ValueExists("keyPath"))
            {
                getConfigurationRequest.m_keyPath = Aws::Crt::Vector<Aws::Crt::String>();
                for (const Aws::Crt::JsonView &keyPathJsonView : jsonView.GetArray("keyPath"))
                {
                    Aws::Crt::Optional<Aws::Crt::String> keyPathItem;
                    keyPathItem = Aws::Crt::Optional<Aws::Crt::String>(keyPathJsonView.AsString());
                    getConfigurationRequest.m_keyPath.value().push_back(keyPathItem.value());
                }
            }
        }

        const char *GetConfigurationRequest::MODEL_NAME = "aws.greengrass#GetConfigurationRequest";

        Aws::Crt::String GetConfigurationRequest::GetModelName() const noexcept
        {
            return GetConfigurationRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> GetConfigurationRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<GetConfigurationRequest> shape(
                Aws::Crt::New<GetConfigurationRequest>(allocator), GetConfigurationRequest::s_customDeleter);
            shape->m_allocator = allocator;
            GetConfigurationRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void GetConfigurationRequest::s_customDeleter(GetConfigurationRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void GetComponentDetailsResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_componentDetails.has_value())
            {
                Aws::Crt::JsonObject componentDetailsValue;
                m_componentDetails.value().SerializeToJsonObject(componentDetailsValue);
                payloadObject.WithObject("componentDetails", std::move(componentDetailsValue));
            }
        }

        void GetComponentDetailsResponse::s_loadFromJsonView(
            GetComponentDetailsResponse &getComponentDetailsResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("componentDetails"))
            {
                getComponentDetailsResponse.m_componentDetails = ComponentDetails();
                ComponentDetails::s_loadFromJsonView(
                    getComponentDetailsResponse.m_componentDetails.value(), jsonView.GetJsonObject("componentDetails"));
            }
        }

        const char *GetComponentDetailsResponse::MODEL_NAME = "aws.greengrass#GetComponentDetailsResponse";

        Aws::Crt::String GetComponentDetailsResponse::GetModelName() const noexcept
        {
            return GetComponentDetailsResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> GetComponentDetailsResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<GetComponentDetailsResponse> shape(
                Aws::Crt::New<GetComponentDetailsResponse>(allocator), GetComponentDetailsResponse::s_customDeleter);
            shape->m_allocator = allocator;
            GetComponentDetailsResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void GetComponentDetailsResponse::s_customDeleter(GetComponentDetailsResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void GetComponentDetailsRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_componentName.has_value())
            {
                payloadObject.WithString("componentName", m_componentName.value());
            }
        }

        void GetComponentDetailsRequest::s_loadFromJsonView(
            GetComponentDetailsRequest &getComponentDetailsRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("componentName"))
            {
                getComponentDetailsRequest.m_componentName =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("componentName"));
            }
        }

        const char *GetComponentDetailsRequest::MODEL_NAME = "aws.greengrass#GetComponentDetailsRequest";

        Aws::Crt::String GetComponentDetailsRequest::GetModelName() const noexcept
        {
            return GetComponentDetailsRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> GetComponentDetailsRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<GetComponentDetailsRequest> shape(
                Aws::Crt::New<GetComponentDetailsRequest>(allocator), GetComponentDetailsRequest::s_customDeleter);
            shape->m_allocator = allocator;
            GetComponentDetailsRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void GetComponentDetailsRequest::s_customDeleter(GetComponentDetailsRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void InvalidCredentialError::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_message.has_value())
            {
                payloadObject.WithString("message", m_message.value());
            }
        }

        void InvalidCredentialError::s_loadFromJsonView(
            InvalidCredentialError &invalidCredentialError,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("message"))
            {
                invalidCredentialError.m_message = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("message"));
            }
        }

        const char *InvalidCredentialError::MODEL_NAME = "aws.greengrass#InvalidCredentialError";

        Aws::Crt::String InvalidCredentialError::GetModelName() const noexcept
        {
            return InvalidCredentialError::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<OperationError> InvalidCredentialError::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<InvalidCredentialError> shape(
                Aws::Crt::New<InvalidCredentialError>(allocator), InvalidCredentialError::s_customDeleter);
            shape->m_allocator = allocator;
            InvalidCredentialError::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<OperationError *>(shape.release());
            return Aws::Crt::ScopedResource<OperationError>(operationResponse, OperationError::s_customDeleter);
        }

        void InvalidCredentialError::s_customDeleter(InvalidCredentialError *shape) noexcept
        {
            OperationError::s_customDeleter(static_cast<OperationError *>(shape));
        }

        void GetClientDeviceAuthTokenResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_clientDeviceAuthToken.has_value())
            {
                payloadObject.WithString("clientDeviceAuthToken", m_clientDeviceAuthToken.value());
            }
        }

        void GetClientDeviceAuthTokenResponse::s_loadFromJsonView(
            GetClientDeviceAuthTokenResponse &getClientDeviceAuthTokenResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("clientDeviceAuthToken"))
            {
                getClientDeviceAuthTokenResponse.m_clientDeviceAuthToken =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("clientDeviceAuthToken"));
            }
        }

        const char *GetClientDeviceAuthTokenResponse::MODEL_NAME = "aws.greengrass#GetClientDeviceAuthTokenResponse";

        Aws::Crt::String GetClientDeviceAuthTokenResponse::GetModelName() const noexcept
        {
            return GetClientDeviceAuthTokenResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> GetClientDeviceAuthTokenResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<GetClientDeviceAuthTokenResponse> shape(
                Aws::Crt::New<GetClientDeviceAuthTokenResponse>(allocator),
                GetClientDeviceAuthTokenResponse::s_customDeleter);
            shape->m_allocator = allocator;
            GetClientDeviceAuthTokenResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void GetClientDeviceAuthTokenResponse::s_customDeleter(GetClientDeviceAuthTokenResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void GetClientDeviceAuthTokenRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_credential.has_value())
            {
                Aws::Crt::JsonObject credentialDocumentValue;
                m_credential.value().SerializeToJsonObject(credentialDocumentValue);
                payloadObject.WithObject("credential", std::move(credentialDocumentValue));
            }
        }

        void GetClientDeviceAuthTokenRequest::s_loadFromJsonView(
            GetClientDeviceAuthTokenRequest &getClientDeviceAuthTokenRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("credential"))
            {
                getClientDeviceAuthTokenRequest.m_credential = CredentialDocument();
                CredentialDocument::s_loadFromJsonView(
                    getClientDeviceAuthTokenRequest.m_credential.value(), jsonView.GetJsonObject("credential"));
            }
        }

        const char *GetClientDeviceAuthTokenRequest::MODEL_NAME = "aws.greengrass#GetClientDeviceAuthTokenRequest";

        Aws::Crt::String GetClientDeviceAuthTokenRequest::GetModelName() const noexcept
        {
            return GetClientDeviceAuthTokenRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> GetClientDeviceAuthTokenRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<GetClientDeviceAuthTokenRequest> shape(
                Aws::Crt::New<GetClientDeviceAuthTokenRequest>(allocator),
                GetClientDeviceAuthTokenRequest::s_customDeleter);
            shape->m_allocator = allocator;
            GetClientDeviceAuthTokenRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void GetClientDeviceAuthTokenRequest::s_customDeleter(GetClientDeviceAuthTokenRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void DeleteThingShadowResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_payload.has_value())
            {
                if (m_payload.value().size() > 0)
                {
                    payloadObject.WithString("payload", Aws::Crt::Base64Encode(m_payload.value()));
                }
            }
        }

        void DeleteThingShadowResponse::s_loadFromJsonView(
            DeleteThingShadowResponse &deleteThingShadowResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("payload"))
            {
                if (jsonView.GetString("payload").size() > 0)
                {
                    deleteThingShadowResponse.m_payload = Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>>(
                        Aws::Crt::Base64Decode(jsonView.GetString("payload")));
                }
            }
        }

        const char *DeleteThingShadowResponse::MODEL_NAME = "aws.greengrass#DeleteThingShadowResponse";

        Aws::Crt::String DeleteThingShadowResponse::GetModelName() const noexcept
        {
            return DeleteThingShadowResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> DeleteThingShadowResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<DeleteThingShadowResponse> shape(
                Aws::Crt::New<DeleteThingShadowResponse>(allocator), DeleteThingShadowResponse::s_customDeleter);
            shape->m_allocator = allocator;
            DeleteThingShadowResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void DeleteThingShadowResponse::s_customDeleter(DeleteThingShadowResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void DeleteThingShadowRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_thingName.has_value())
            {
                payloadObject.WithString("thingName", m_thingName.value());
            }
            if (m_shadowName.has_value())
            {
                payloadObject.WithString("shadowName", m_shadowName.value());
            }
        }

        void DeleteThingShadowRequest::s_loadFromJsonView(
            DeleteThingShadowRequest &deleteThingShadowRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("thingName"))
            {
                deleteThingShadowRequest.m_thingName =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("thingName"));
            }
            if (jsonView.ValueExists("shadowName"))
            {
                deleteThingShadowRequest.m_shadowName =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("shadowName"));
            }
        }

        const char *DeleteThingShadowRequest::MODEL_NAME = "aws.greengrass#DeleteThingShadowRequest";

        Aws::Crt::String DeleteThingShadowRequest::GetModelName() const noexcept
        {
            return DeleteThingShadowRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> DeleteThingShadowRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<DeleteThingShadowRequest> shape(
                Aws::Crt::New<DeleteThingShadowRequest>(allocator), DeleteThingShadowRequest::s_customDeleter);
            shape->m_allocator = allocator;
            DeleteThingShadowRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void DeleteThingShadowRequest::s_customDeleter(DeleteThingShadowRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void DeferComponentUpdateResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            (void)payloadObject;
        }

        void DeferComponentUpdateResponse::s_loadFromJsonView(
            DeferComponentUpdateResponse &deferComponentUpdateResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            (void)deferComponentUpdateResponse;
            (void)jsonView;
        }

        const char *DeferComponentUpdateResponse::MODEL_NAME = "aws.greengrass#DeferComponentUpdateResponse";

        Aws::Crt::String DeferComponentUpdateResponse::GetModelName() const noexcept
        {
            return DeferComponentUpdateResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> DeferComponentUpdateResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<DeferComponentUpdateResponse> shape(
                Aws::Crt::New<DeferComponentUpdateResponse>(allocator), DeferComponentUpdateResponse::s_customDeleter);
            shape->m_allocator = allocator;
            DeferComponentUpdateResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void DeferComponentUpdateResponse::s_customDeleter(DeferComponentUpdateResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void DeferComponentUpdateRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_deploymentId.has_value())
            {
                payloadObject.WithString("deploymentId", m_deploymentId.value());
            }
            if (m_message.has_value())
            {
                payloadObject.WithString("message", m_message.value());
            }
            if (m_recheckAfterMs.has_value())
            {
                payloadObject.WithInt64("recheckAfterMs", m_recheckAfterMs.value());
            }
        }

        void DeferComponentUpdateRequest::s_loadFromJsonView(
            DeferComponentUpdateRequest &deferComponentUpdateRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("deploymentId"))
            {
                deferComponentUpdateRequest.m_deploymentId =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("deploymentId"));
            }
            if (jsonView.ValueExists("message"))
            {
                deferComponentUpdateRequest.m_message =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("message"));
            }
            if (jsonView.ValueExists("recheckAfterMs"))
            {
                deferComponentUpdateRequest.m_recheckAfterMs =
                    Aws::Crt::Optional<int64_t>(jsonView.GetInt64("recheckAfterMs"));
            }
        }

        const char *DeferComponentUpdateRequest::MODEL_NAME = "aws.greengrass#DeferComponentUpdateRequest";

        Aws::Crt::String DeferComponentUpdateRequest::GetModelName() const noexcept
        {
            return DeferComponentUpdateRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> DeferComponentUpdateRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<DeferComponentUpdateRequest> shape(
                Aws::Crt::New<DeferComponentUpdateRequest>(allocator), DeferComponentUpdateRequest::s_customDeleter);
            shape->m_allocator = allocator;
            DeferComponentUpdateRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void DeferComponentUpdateRequest::s_customDeleter(DeferComponentUpdateRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void InvalidArtifactsDirectoryPathError::SerializeToJsonObject(
            Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_message.has_value())
            {
                payloadObject.WithString("message", m_message.value());
            }
        }

        void InvalidArtifactsDirectoryPathError::s_loadFromJsonView(
            InvalidArtifactsDirectoryPathError &invalidArtifactsDirectoryPathError,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("message"))
            {
                invalidArtifactsDirectoryPathError.m_message =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("message"));
            }
        }

        const char *InvalidArtifactsDirectoryPathError::MODEL_NAME =
            "aws.greengrass#InvalidArtifactsDirectoryPathError";

        Aws::Crt::String InvalidArtifactsDirectoryPathError::GetModelName() const noexcept
        {
            return InvalidArtifactsDirectoryPathError::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<OperationError> InvalidArtifactsDirectoryPathError::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<InvalidArtifactsDirectoryPathError> shape(
                Aws::Crt::New<InvalidArtifactsDirectoryPathError>(allocator),
                InvalidArtifactsDirectoryPathError::s_customDeleter);
            shape->m_allocator = allocator;
            InvalidArtifactsDirectoryPathError::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<OperationError *>(shape.release());
            return Aws::Crt::ScopedResource<OperationError>(operationResponse, OperationError::s_customDeleter);
        }

        void InvalidArtifactsDirectoryPathError::s_customDeleter(InvalidArtifactsDirectoryPathError *shape) noexcept
        {
            OperationError::s_customDeleter(static_cast<OperationError *>(shape));
        }

        void InvalidRecipeDirectoryPathError::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_message.has_value())
            {
                payloadObject.WithString("message", m_message.value());
            }
        }

        void InvalidRecipeDirectoryPathError::s_loadFromJsonView(
            InvalidRecipeDirectoryPathError &invalidRecipeDirectoryPathError,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("message"))
            {
                invalidRecipeDirectoryPathError.m_message =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("message"));
            }
        }

        const char *InvalidRecipeDirectoryPathError::MODEL_NAME = "aws.greengrass#InvalidRecipeDirectoryPathError";

        Aws::Crt::String InvalidRecipeDirectoryPathError::GetModelName() const noexcept
        {
            return InvalidRecipeDirectoryPathError::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<OperationError> InvalidRecipeDirectoryPathError::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<InvalidRecipeDirectoryPathError> shape(
                Aws::Crt::New<InvalidRecipeDirectoryPathError>(allocator),
                InvalidRecipeDirectoryPathError::s_customDeleter);
            shape->m_allocator = allocator;
            InvalidRecipeDirectoryPathError::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<OperationError *>(shape.release());
            return Aws::Crt::ScopedResource<OperationError>(operationResponse, OperationError::s_customDeleter);
        }

        void InvalidRecipeDirectoryPathError::s_customDeleter(InvalidRecipeDirectoryPathError *shape) noexcept
        {
            OperationError::s_customDeleter(static_cast<OperationError *>(shape));
        }

        void CreateLocalDeploymentResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_deploymentId.has_value())
            {
                payloadObject.WithString("deploymentId", m_deploymentId.value());
            }
        }

        void CreateLocalDeploymentResponse::s_loadFromJsonView(
            CreateLocalDeploymentResponse &createLocalDeploymentResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("deploymentId"))
            {
                createLocalDeploymentResponse.m_deploymentId =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("deploymentId"));
            }
        }

        const char *CreateLocalDeploymentResponse::MODEL_NAME = "aws.greengrass#CreateLocalDeploymentResponse";

        Aws::Crt::String CreateLocalDeploymentResponse::GetModelName() const noexcept
        {
            return CreateLocalDeploymentResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> CreateLocalDeploymentResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<CreateLocalDeploymentResponse> shape(
                Aws::Crt::New<CreateLocalDeploymentResponse>(allocator),
                CreateLocalDeploymentResponse::s_customDeleter);
            shape->m_allocator = allocator;
            CreateLocalDeploymentResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void CreateLocalDeploymentResponse::s_customDeleter(CreateLocalDeploymentResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void CreateLocalDeploymentRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_groupName.has_value())
            {
                payloadObject.WithString("groupName", m_groupName.value());
            }
            if (m_rootComponentVersionsToAdd.has_value())
            {
                Aws::Crt::JsonObject componentToVersionMapValue;
                for (const auto &componentToVersionMapItem : m_rootComponentVersionsToAdd.value())
                {
                    Aws::Crt::JsonObject componentToVersionMapJsonObject;
                    componentToVersionMapJsonObject.AsString(componentToVersionMapItem.second);
                    componentToVersionMapValue.WithObject(
                        componentToVersionMapItem.first, std::move(componentToVersionMapJsonObject));
                }
                payloadObject.WithObject("rootComponentVersionsToAdd", std::move(componentToVersionMapValue));
            }
            if (m_rootComponentsToRemove.has_value())
            {
                Aws::Crt::JsonObject componentList;
                Aws::Crt::Vector<Aws::Crt::JsonObject> componentListJsonArray;
                for (const auto &componentListItem : m_rootComponentsToRemove.value())
                {
                    Aws::Crt::JsonObject componentListJsonArrayItem;
                    componentListJsonArrayItem.AsString(componentListItem);
                    componentListJsonArray.emplace_back(std::move(componentListJsonArrayItem));
                }
                componentList.AsArray(std::move(componentListJsonArray));
                payloadObject.WithObject("rootComponentsToRemove", std::move(componentList));
            }
            if (m_componentToConfiguration.has_value())
            {
                Aws::Crt::JsonObject componentToConfigurationValue;
                for (const auto &componentToConfigurationItem : m_componentToConfiguration.value())
                {
                    Aws::Crt::JsonObject componentToConfigurationJsonObject;
                    componentToConfigurationJsonObject.AsObject(componentToConfigurationItem.second);
                    componentToConfigurationValue.WithObject(
                        componentToConfigurationItem.first, std::move(componentToConfigurationJsonObject));
                }
                payloadObject.WithObject("componentToConfiguration", std::move(componentToConfigurationValue));
            }
            if (m_componentToRunWithInfo.has_value())
            {
                Aws::Crt::JsonObject componentToRunWithInfoValue;
                for (const auto &componentToRunWithInfoItem : m_componentToRunWithInfo.value())
                {
                    Aws::Crt::JsonObject componentToRunWithInfoJsonObject;
                    componentToRunWithInfoItem.second.SerializeToJsonObject(componentToRunWithInfoJsonObject);
                    componentToRunWithInfoValue.WithObject(
                        componentToRunWithInfoItem.first, std::move(componentToRunWithInfoJsonObject));
                }
                payloadObject.WithObject("componentToRunWithInfo", std::move(componentToRunWithInfoValue));
            }
            if (m_recipeDirectoryPath.has_value())
            {
                payloadObject.WithString("recipeDirectoryPath", m_recipeDirectoryPath.value());
            }
            if (m_artifactsDirectoryPath.has_value())
            {
                payloadObject.WithString("artifactsDirectoryPath", m_artifactsDirectoryPath.value());
            }
            if (m_failureHandlingPolicy.has_value())
            {
                payloadObject.WithString("failureHandlingPolicy", m_failureHandlingPolicy.value());
            }
        }

        void CreateLocalDeploymentRequest::s_loadFromJsonView(
            CreateLocalDeploymentRequest &createLocalDeploymentRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("groupName"))
            {
                createLocalDeploymentRequest.m_groupName =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("groupName"));
            }
            if (jsonView.ValueExists("rootComponentVersionsToAdd"))
            {
                createLocalDeploymentRequest.m_rootComponentVersionsToAdd =
                    Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String>();
                for (const auto &componentToVersionMapPair :
                     jsonView.GetJsonObject("rootComponentVersionsToAdd").GetAllObjects())
                {
                    Aws::Crt::Optional<Aws::Crt::String> componentToVersionMapValue;
                    componentToVersionMapValue =
                        Aws::Crt::Optional<Aws::Crt::String>(componentToVersionMapPair.second.AsString());
                    createLocalDeploymentRequest.m_rootComponentVersionsToAdd.value()[componentToVersionMapPair.first] =
                        componentToVersionMapValue.value();
                }
            }
            if (jsonView.ValueExists("rootComponentsToRemove"))
            {
                createLocalDeploymentRequest.m_rootComponentsToRemove = Aws::Crt::Vector<Aws::Crt::String>();
                for (const Aws::Crt::JsonView &componentListJsonView : jsonView.GetArray("rootComponentsToRemove"))
                {
                    Aws::Crt::Optional<Aws::Crt::String> componentListItem;
                    componentListItem = Aws::Crt::Optional<Aws::Crt::String>(componentListJsonView.AsString());
                    createLocalDeploymentRequest.m_rootComponentsToRemove.value().push_back(componentListItem.value());
                }
            }
            if (jsonView.ValueExists("componentToConfiguration"))
            {
                createLocalDeploymentRequest.m_componentToConfiguration =
                    Aws::Crt::Map<Aws::Crt::String, Aws::Crt::JsonObject>();
                for (const auto &componentToConfigurationPair :
                     jsonView.GetJsonObject("componentToConfiguration").GetAllObjects())
                {
                    Aws::Crt::Optional<Aws::Crt::JsonObject> componentToConfigurationValue;
                    componentToConfigurationValue = Aws::Crt::Optional<Aws::Crt::JsonObject>(
                        componentToConfigurationPair.second.AsObject().Materialize());
                    createLocalDeploymentRequest.m_componentToConfiguration
                        .value()[componentToConfigurationPair.first] = componentToConfigurationValue.value();
                }
            }
            if (jsonView.ValueExists("componentToRunWithInfo"))
            {
                createLocalDeploymentRequest.m_componentToRunWithInfo = Aws::Crt::Map<Aws::Crt::String, RunWithInfo>();
                for (const auto &componentToRunWithInfoPair :
                     jsonView.GetJsonObject("componentToRunWithInfo").GetAllObjects())
                {
                    Aws::Crt::Optional<RunWithInfo> componentToRunWithInfoValue;
                    componentToRunWithInfoValue = RunWithInfo();
                    RunWithInfo::s_loadFromJsonView(
                        componentToRunWithInfoValue.value(), componentToRunWithInfoPair.second);
                    createLocalDeploymentRequest.m_componentToRunWithInfo.value()[componentToRunWithInfoPair.first] =
                        componentToRunWithInfoValue.value();
                }
            }
            if (jsonView.ValueExists("recipeDirectoryPath"))
            {
                createLocalDeploymentRequest.m_recipeDirectoryPath =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("recipeDirectoryPath"));
            }
            if (jsonView.ValueExists("artifactsDirectoryPath"))
            {
                createLocalDeploymentRequest.m_artifactsDirectoryPath =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("artifactsDirectoryPath"));
            }
            if (jsonView.ValueExists("failureHandlingPolicy"))
            {
                createLocalDeploymentRequest.m_failureHandlingPolicy =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("failureHandlingPolicy"));
            }
        }

        void CreateLocalDeploymentRequest::SetFailureHandlingPolicy(
            FailureHandlingPolicy failureHandlingPolicy) noexcept
        {
            switch (failureHandlingPolicy)
            {
                case FAILURE_HANDLING_POLICY_ROLLBACK:
                    m_failureHandlingPolicy = Aws::Crt::String("ROLLBACK");
                    break;
                case FAILURE_HANDLING_POLICY_DO_NOTHING:
                    m_failureHandlingPolicy = Aws::Crt::String("DO_NOTHING");
                    break;
                default:
                    break;
            }
        }

        Aws::Crt::Optional<FailureHandlingPolicy> CreateLocalDeploymentRequest::GetFailureHandlingPolicy() noexcept
        {
            if (!m_failureHandlingPolicy.has_value())
                return Aws::Crt::Optional<FailureHandlingPolicy>();
            if (m_failureHandlingPolicy.value() == Aws::Crt::String("ROLLBACK"))
            {
                return Aws::Crt::Optional<FailureHandlingPolicy>(FAILURE_HANDLING_POLICY_ROLLBACK);
            }
            if (m_failureHandlingPolicy.value() == Aws::Crt::String("DO_NOTHING"))
            {
                return Aws::Crt::Optional<FailureHandlingPolicy>(FAILURE_HANDLING_POLICY_DO_NOTHING);
            }

            return Aws::Crt::Optional<FailureHandlingPolicy>();
        }

        const char *CreateLocalDeploymentRequest::MODEL_NAME = "aws.greengrass#CreateLocalDeploymentRequest";

        Aws::Crt::String CreateLocalDeploymentRequest::GetModelName() const noexcept
        {
            return CreateLocalDeploymentRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> CreateLocalDeploymentRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<CreateLocalDeploymentRequest> shape(
                Aws::Crt::New<CreateLocalDeploymentRequest>(allocator), CreateLocalDeploymentRequest::s_customDeleter);
            shape->m_allocator = allocator;
            CreateLocalDeploymentRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void CreateLocalDeploymentRequest::s_customDeleter(CreateLocalDeploymentRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void CreateDebugPasswordResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_password.has_value())
            {
                payloadObject.WithString("password", m_password.value());
            }
            if (m_username.has_value())
            {
                payloadObject.WithString("username", m_username.value());
            }
            if (m_passwordExpiration.has_value())
            {
                payloadObject.WithDouble("passwordExpiration", m_passwordExpiration.value().SecondsWithMSPrecision());
            }
            if (m_certificateSHA256Hash.has_value())
            {
                payloadObject.WithString("certificateSHA256Hash", m_certificateSHA256Hash.value());
            }
            if (m_certificateSHA1Hash.has_value())
            {
                payloadObject.WithString("certificateSHA1Hash", m_certificateSHA1Hash.value());
            }
        }

        void CreateDebugPasswordResponse::s_loadFromJsonView(
            CreateDebugPasswordResponse &createDebugPasswordResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("password"))
            {
                createDebugPasswordResponse.m_password =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("password"));
            }
            if (jsonView.ValueExists("username"))
            {
                createDebugPasswordResponse.m_username =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("username"));
            }
            if (jsonView.ValueExists("passwordExpiration"))
            {
                createDebugPasswordResponse.m_passwordExpiration = Aws::Crt::Optional<Aws::Crt::DateTime>(
                    Aws::Crt::DateTime(jsonView.GetDouble("passwordExpiration")));
            }
            if (jsonView.ValueExists("certificateSHA256Hash"))
            {
                createDebugPasswordResponse.m_certificateSHA256Hash =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("certificateSHA256Hash"));
            }
            if (jsonView.ValueExists("certificateSHA1Hash"))
            {
                createDebugPasswordResponse.m_certificateSHA1Hash =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("certificateSHA1Hash"));
            }
        }

        const char *CreateDebugPasswordResponse::MODEL_NAME = "aws.greengrass#CreateDebugPasswordResponse";

        Aws::Crt::String CreateDebugPasswordResponse::GetModelName() const noexcept
        {
            return CreateDebugPasswordResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> CreateDebugPasswordResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<CreateDebugPasswordResponse> shape(
                Aws::Crt::New<CreateDebugPasswordResponse>(allocator), CreateDebugPasswordResponse::s_customDeleter);
            shape->m_allocator = allocator;
            CreateDebugPasswordResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void CreateDebugPasswordResponse::s_customDeleter(CreateDebugPasswordResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void CreateDebugPasswordRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            (void)payloadObject;
        }

        void CreateDebugPasswordRequest::s_loadFromJsonView(
            CreateDebugPasswordRequest &createDebugPasswordRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            (void)createDebugPasswordRequest;
            (void)jsonView;
        }

        const char *CreateDebugPasswordRequest::MODEL_NAME = "aws.greengrass#CreateDebugPasswordRequest";

        Aws::Crt::String CreateDebugPasswordRequest::GetModelName() const noexcept
        {
            return CreateDebugPasswordRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> CreateDebugPasswordRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<CreateDebugPasswordRequest> shape(
                Aws::Crt::New<CreateDebugPasswordRequest>(allocator), CreateDebugPasswordRequest::s_customDeleter);
            shape->m_allocator = allocator;
            CreateDebugPasswordRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void CreateDebugPasswordRequest::s_customDeleter(CreateDebugPasswordRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void CancelLocalDeploymentResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_message.has_value())
            {
                payloadObject.WithString("message", m_message.value());
            }
        }

        void CancelLocalDeploymentResponse::s_loadFromJsonView(
            CancelLocalDeploymentResponse &cancelLocalDeploymentResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("message"))
            {
                cancelLocalDeploymentResponse.m_message =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("message"));
            }
        }

        const char *CancelLocalDeploymentResponse::MODEL_NAME = "aws.greengrass#CancelLocalDeploymentResponse";

        Aws::Crt::String CancelLocalDeploymentResponse::GetModelName() const noexcept
        {
            return CancelLocalDeploymentResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> CancelLocalDeploymentResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<CancelLocalDeploymentResponse> shape(
                Aws::Crt::New<CancelLocalDeploymentResponse>(allocator),
                CancelLocalDeploymentResponse::s_customDeleter);
            shape->m_allocator = allocator;
            CancelLocalDeploymentResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void CancelLocalDeploymentResponse::s_customDeleter(CancelLocalDeploymentResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void CancelLocalDeploymentRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_deploymentId.has_value())
            {
                payloadObject.WithString("deploymentId", m_deploymentId.value());
            }
        }

        void CancelLocalDeploymentRequest::s_loadFromJsonView(
            CancelLocalDeploymentRequest &cancelLocalDeploymentRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("deploymentId"))
            {
                cancelLocalDeploymentRequest.m_deploymentId =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("deploymentId"));
            }
        }

        const char *CancelLocalDeploymentRequest::MODEL_NAME = "aws.greengrass#CancelLocalDeploymentRequest";

        Aws::Crt::String CancelLocalDeploymentRequest::GetModelName() const noexcept
        {
            return CancelLocalDeploymentRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> CancelLocalDeploymentRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<CancelLocalDeploymentRequest> shape(
                Aws::Crt::New<CancelLocalDeploymentRequest>(allocator), CancelLocalDeploymentRequest::s_customDeleter);
            shape->m_allocator = allocator;
            CancelLocalDeploymentRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void CancelLocalDeploymentRequest::s_customDeleter(CancelLocalDeploymentRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void InvalidClientDeviceAuthTokenError::SerializeToJsonObject(
            Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_message.has_value())
            {
                payloadObject.WithString("message", m_message.value());
            }
        }

        void InvalidClientDeviceAuthTokenError::s_loadFromJsonView(
            InvalidClientDeviceAuthTokenError &invalidClientDeviceAuthTokenError,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("message"))
            {
                invalidClientDeviceAuthTokenError.m_message =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("message"));
            }
        }

        const char *InvalidClientDeviceAuthTokenError::MODEL_NAME = "aws.greengrass#InvalidClientDeviceAuthTokenError";

        Aws::Crt::String InvalidClientDeviceAuthTokenError::GetModelName() const noexcept
        {
            return InvalidClientDeviceAuthTokenError::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<OperationError> InvalidClientDeviceAuthTokenError::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<InvalidClientDeviceAuthTokenError> shape(
                Aws::Crt::New<InvalidClientDeviceAuthTokenError>(allocator),
                InvalidClientDeviceAuthTokenError::s_customDeleter);
            shape->m_allocator = allocator;
            InvalidClientDeviceAuthTokenError::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<OperationError *>(shape.release());
            return Aws::Crt::ScopedResource<OperationError>(operationResponse, OperationError::s_customDeleter);
        }

        void InvalidClientDeviceAuthTokenError::s_customDeleter(InvalidClientDeviceAuthTokenError *shape) noexcept
        {
            OperationError::s_customDeleter(static_cast<OperationError *>(shape));
        }

        void AuthorizeClientDeviceActionResponse::SerializeToJsonObject(
            Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_isAuthorized.has_value())
            {
                payloadObject.WithBool("isAuthorized", m_isAuthorized.value());
            }
        }

        void AuthorizeClientDeviceActionResponse::s_loadFromJsonView(
            AuthorizeClientDeviceActionResponse &authorizeClientDeviceActionResponse,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("isAuthorized"))
            {
                authorizeClientDeviceActionResponse.m_isAuthorized =
                    Aws::Crt::Optional<bool>(jsonView.GetBool("isAuthorized"));
            }
        }

        const char *AuthorizeClientDeviceActionResponse::MODEL_NAME =
            "aws.greengrass#AuthorizeClientDeviceActionResponse";

        Aws::Crt::String AuthorizeClientDeviceActionResponse::GetModelName() const noexcept
        {
            return AuthorizeClientDeviceActionResponse::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> AuthorizeClientDeviceActionResponse::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<AuthorizeClientDeviceActionResponse> shape(
                Aws::Crt::New<AuthorizeClientDeviceActionResponse>(allocator),
                AuthorizeClientDeviceActionResponse::s_customDeleter);
            shape->m_allocator = allocator;
            AuthorizeClientDeviceActionResponse::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void AuthorizeClientDeviceActionResponse::s_customDeleter(AuthorizeClientDeviceActionResponse *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void AuthorizeClientDeviceActionRequest::SerializeToJsonObject(
            Aws::Crt::JsonObject &payloadObject) const noexcept
        {
            if (m_clientDeviceAuthToken.has_value())
            {
                payloadObject.WithString("clientDeviceAuthToken", m_clientDeviceAuthToken.value());
            }
            if (m_operation.has_value())
            {
                payloadObject.WithString("operation", m_operation.value());
            }
            if (m_resource.has_value())
            {
                payloadObject.WithString("resource", m_resource.value());
            }
        }

        void AuthorizeClientDeviceActionRequest::s_loadFromJsonView(
            AuthorizeClientDeviceActionRequest &authorizeClientDeviceActionRequest,
            const Aws::Crt::JsonView &jsonView) noexcept
        {
            if (jsonView.ValueExists("clientDeviceAuthToken"))
            {
                authorizeClientDeviceActionRequest.m_clientDeviceAuthToken =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("clientDeviceAuthToken"));
            }
            if (jsonView.ValueExists("operation"))
            {
                authorizeClientDeviceActionRequest.m_operation =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("operation"));
            }
            if (jsonView.ValueExists("resource"))
            {
                authorizeClientDeviceActionRequest.m_resource =
                    Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("resource"));
            }
        }

        const char *AuthorizeClientDeviceActionRequest::MODEL_NAME =
            "aws.greengrass#AuthorizeClientDeviceActionRequest";

        Aws::Crt::String AuthorizeClientDeviceActionRequest::GetModelName() const noexcept
        {
            return AuthorizeClientDeviceActionRequest::MODEL_NAME;
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> AuthorizeClientDeviceActionRequest::s_allocateFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) noexcept
        {
            Aws::Crt::String payload = {stringView.begin(), stringView.end()};
            Aws::Crt::JsonObject jsonObject(payload);
            Aws::Crt::JsonView jsonView(jsonObject);

            Aws::Crt::ScopedResource<AuthorizeClientDeviceActionRequest> shape(
                Aws::Crt::New<AuthorizeClientDeviceActionRequest>(allocator),
                AuthorizeClientDeviceActionRequest::s_customDeleter);
            shape->m_allocator = allocator;
            AuthorizeClientDeviceActionRequest::s_loadFromJsonView(*shape, jsonView);
            auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
            return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
        }

        void AuthorizeClientDeviceActionRequest::s_customDeleter(AuthorizeClientDeviceActionRequest *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
        }

        void SubscribeToIoTCoreStreamHandler::OnStreamEvent(Aws::Crt::ScopedResource<AbstractShapeBase> response)
        {
            OnStreamEvent(static_cast<IoTCoreMessage *>(response.get()));
        }

        bool SubscribeToIoTCoreStreamHandler::OnStreamError(
            Aws::Crt::ScopedResource<OperationError> operationError,
            RpcError rpcError)
        {
            bool streamShouldTerminate = false;
            if (rpcError.baseStatus != EVENT_STREAM_RPC_SUCCESS)
            {
                streamShouldTerminate = OnStreamError(rpcError);
            }
            if (operationError != nullptr &&
                operationError->GetModelName() == Aws::Crt::String("aws.greengrass#ServiceError") &&
                !streamShouldTerminate)
            {
                streamShouldTerminate = OnStreamError(static_cast<ServiceError *>(operationError.get()));
            }
            if (operationError != nullptr &&
                operationError->GetModelName() == Aws::Crt::String("aws.greengrass#UnauthorizedError") &&
                !streamShouldTerminate)
            {
                streamShouldTerminate = OnStreamError(static_cast<UnauthorizedError *>(operationError.get()));
            }
            if (operationError != nullptr && !streamShouldTerminate)
                streamShouldTerminate = OnStreamError(operationError.get());
            return streamShouldTerminate;
        }

        SubscribeToIoTCoreOperationContext::SubscribeToIoTCoreOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SubscribeToIoTCoreOperationContext::
            AllocateInitialResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return SubscribeToIoTCoreResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SubscribeToIoTCoreOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return IoTCoreMessage::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::String SubscribeToIoTCoreOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#SubscribeToIoTCoreRequest");
        }

        Aws::Crt::String SubscribeToIoTCoreOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#SubscribeToIoTCoreResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> SubscribeToIoTCoreOperationContext::GetStreamingResponseModelName()
            const noexcept
        {
            return Aws::Crt::String("aws.greengrass#IoTCoreMessage");
        }

        Aws::Crt::String SubscribeToIoTCoreOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#SubscribeToIoTCore");
        }

        std::future<SubscribeToIoTCoreResult> SubscribeToIoTCoreOperation::GetResult() noexcept
        {
            return std::async(
                m_asyncLaunchMode, [this]() { return SubscribeToIoTCoreResult(GetOperationResult().get()); });
        }

        SubscribeToIoTCoreOperation::SubscribeToIoTCoreOperation(
            ClientConnection &connection,
            std::shared_ptr<SubscribeToIoTCoreStreamHandler> streamHandler,
            const SubscribeToIoTCoreOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, streamHandler, operationContext, allocator)
        {
        }

        std::future<RpcError> SubscribeToIoTCoreOperation::Activate(
            const SubscribeToIoTCoreRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String SubscribeToIoTCoreOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        ResumeComponentOperationContext::ResumeComponentOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> ResumeComponentOperationContext::AllocateInitialResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) const noexcept
        {
            return ResumeComponentResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> ResumeComponentOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String ResumeComponentOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#ResumeComponentRequest");
        }

        Aws::Crt::String ResumeComponentOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#ResumeComponentResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> ResumeComponentOperationContext::GetStreamingResponseModelName()
            const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String ResumeComponentOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#ResumeComponent");
        }

        std::future<ResumeComponentResult> ResumeComponentOperation::GetResult() noexcept
        {
            return std::async(
                m_asyncLaunchMode, [this]() { return ResumeComponentResult(GetOperationResult().get()); });
        }

        ResumeComponentOperation::ResumeComponentOperation(
            ClientConnection &connection,
            const ResumeComponentOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> ResumeComponentOperation::Activate(
            const ResumeComponentRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String ResumeComponentOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        PublishToIoTCoreOperationContext::PublishToIoTCoreOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> PublishToIoTCoreOperationContext::
            AllocateInitialResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return PublishToIoTCoreResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> PublishToIoTCoreOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String PublishToIoTCoreOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#PublishToIoTCoreRequest");
        }

        Aws::Crt::String PublishToIoTCoreOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#PublishToIoTCoreResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> PublishToIoTCoreOperationContext::GetStreamingResponseModelName()
            const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String PublishToIoTCoreOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#PublishToIoTCore");
        }

        std::future<PublishToIoTCoreResult> PublishToIoTCoreOperation::GetResult() noexcept
        {
            return std::async(
                m_asyncLaunchMode, [this]() { return PublishToIoTCoreResult(GetOperationResult().get()); });
        }

        PublishToIoTCoreOperation::PublishToIoTCoreOperation(
            ClientConnection &connection,
            const PublishToIoTCoreOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> PublishToIoTCoreOperation::Activate(
            const PublishToIoTCoreRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String PublishToIoTCoreOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        void SubscribeToConfigurationUpdateStreamHandler::OnStreamEvent(
            Aws::Crt::ScopedResource<AbstractShapeBase> response)
        {
            OnStreamEvent(static_cast<ConfigurationUpdateEvents *>(response.get()));
        }

        bool SubscribeToConfigurationUpdateStreamHandler::OnStreamError(
            Aws::Crt::ScopedResource<OperationError> operationError,
            RpcError rpcError)
        {
            bool streamShouldTerminate = false;
            if (rpcError.baseStatus != EVENT_STREAM_RPC_SUCCESS)
            {
                streamShouldTerminate = OnStreamError(rpcError);
            }
            if (operationError != nullptr &&
                operationError->GetModelName() == Aws::Crt::String("aws.greengrass#ServiceError") &&
                !streamShouldTerminate)
            {
                streamShouldTerminate = OnStreamError(static_cast<ServiceError *>(operationError.get()));
            }
            if (operationError != nullptr &&
                operationError->GetModelName() == Aws::Crt::String("aws.greengrass#ResourceNotFoundError") &&
                !streamShouldTerminate)
            {
                streamShouldTerminate = OnStreamError(static_cast<ResourceNotFoundError *>(operationError.get()));
            }
            if (operationError != nullptr && !streamShouldTerminate)
                streamShouldTerminate = OnStreamError(operationError.get());
            return streamShouldTerminate;
        }

        SubscribeToConfigurationUpdateOperationContext::SubscribeToConfigurationUpdateOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SubscribeToConfigurationUpdateOperationContext::
            AllocateInitialResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return SubscribeToConfigurationUpdateResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SubscribeToConfigurationUpdateOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return ConfigurationUpdateEvents::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::String SubscribeToConfigurationUpdateOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#SubscribeToConfigurationUpdateRequest");
        }

        Aws::Crt::String SubscribeToConfigurationUpdateOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#SubscribeToConfigurationUpdateResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> SubscribeToConfigurationUpdateOperationContext::
            GetStreamingResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#ConfigurationUpdateEvents");
        }

        Aws::Crt::String SubscribeToConfigurationUpdateOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#SubscribeToConfigurationUpdate");
        }

        std::future<SubscribeToConfigurationUpdateResult> SubscribeToConfigurationUpdateOperation::GetResult() noexcept
        {
            return std::async(m_asyncLaunchMode, [this]() {
                return SubscribeToConfigurationUpdateResult(GetOperationResult().get());
            });
        }

        SubscribeToConfigurationUpdateOperation::SubscribeToConfigurationUpdateOperation(
            ClientConnection &connection,
            std::shared_ptr<SubscribeToConfigurationUpdateStreamHandler> streamHandler,
            const SubscribeToConfigurationUpdateOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, streamHandler, operationContext, allocator)
        {
        }

        std::future<RpcError> SubscribeToConfigurationUpdateOperation::Activate(
            const SubscribeToConfigurationUpdateRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String SubscribeToConfigurationUpdateOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        DeleteThingShadowOperationContext::DeleteThingShadowOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> DeleteThingShadowOperationContext::
            AllocateInitialResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return DeleteThingShadowResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> DeleteThingShadowOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String DeleteThingShadowOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#DeleteThingShadowRequest");
        }

        Aws::Crt::String DeleteThingShadowOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#DeleteThingShadowResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> DeleteThingShadowOperationContext::GetStreamingResponseModelName()
            const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String DeleteThingShadowOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#DeleteThingShadow");
        }

        std::future<DeleteThingShadowResult> DeleteThingShadowOperation::GetResult() noexcept
        {
            return std::async(
                m_asyncLaunchMode, [this]() { return DeleteThingShadowResult(GetOperationResult().get()); });
        }

        DeleteThingShadowOperation::DeleteThingShadowOperation(
            ClientConnection &connection,
            const DeleteThingShadowOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> DeleteThingShadowOperation::Activate(
            const DeleteThingShadowRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String DeleteThingShadowOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        PutComponentMetricOperationContext::PutComponentMetricOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> PutComponentMetricOperationContext::
            AllocateInitialResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return PutComponentMetricResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> PutComponentMetricOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String PutComponentMetricOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#PutComponentMetricRequest");
        }

        Aws::Crt::String PutComponentMetricOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#PutComponentMetricResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> PutComponentMetricOperationContext::GetStreamingResponseModelName()
            const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String PutComponentMetricOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#PutComponentMetric");
        }

        std::future<PutComponentMetricResult> PutComponentMetricOperation::GetResult() noexcept
        {
            return std::async(
                m_asyncLaunchMode, [this]() { return PutComponentMetricResult(GetOperationResult().get()); });
        }

        PutComponentMetricOperation::PutComponentMetricOperation(
            ClientConnection &connection,
            const PutComponentMetricOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> PutComponentMetricOperation::Activate(
            const PutComponentMetricRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String PutComponentMetricOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        DeferComponentUpdateOperationContext::DeferComponentUpdateOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> DeferComponentUpdateOperationContext::
            AllocateInitialResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return DeferComponentUpdateResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> DeferComponentUpdateOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String DeferComponentUpdateOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#DeferComponentUpdateRequest");
        }

        Aws::Crt::String DeferComponentUpdateOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#DeferComponentUpdateResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> DeferComponentUpdateOperationContext::GetStreamingResponseModelName()
            const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String DeferComponentUpdateOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#DeferComponentUpdate");
        }

        std::future<DeferComponentUpdateResult> DeferComponentUpdateOperation::GetResult() noexcept
        {
            return std::async(
                m_asyncLaunchMode, [this]() { return DeferComponentUpdateResult(GetOperationResult().get()); });
        }

        DeferComponentUpdateOperation::DeferComponentUpdateOperation(
            ClientConnection &connection,
            const DeferComponentUpdateOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> DeferComponentUpdateOperation::Activate(
            const DeferComponentUpdateRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String DeferComponentUpdateOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        void SubscribeToValidateConfigurationUpdatesStreamHandler::OnStreamEvent(
            Aws::Crt::ScopedResource<AbstractShapeBase> response)
        {
            OnStreamEvent(static_cast<ValidateConfigurationUpdateEvents *>(response.get()));
        }

        bool SubscribeToValidateConfigurationUpdatesStreamHandler::OnStreamError(
            Aws::Crt::ScopedResource<OperationError> operationError,
            RpcError rpcError)
        {
            bool streamShouldTerminate = false;
            if (rpcError.baseStatus != EVENT_STREAM_RPC_SUCCESS)
            {
                streamShouldTerminate = OnStreamError(rpcError);
            }
            if (operationError != nullptr &&
                operationError->GetModelName() == Aws::Crt::String("aws.greengrass#ServiceError") &&
                !streamShouldTerminate)
            {
                streamShouldTerminate = OnStreamError(static_cast<ServiceError *>(operationError.get()));
            }
            if (operationError != nullptr && !streamShouldTerminate)
                streamShouldTerminate = OnStreamError(operationError.get());
            return streamShouldTerminate;
        }

        SubscribeToValidateConfigurationUpdatesOperationContext::
            SubscribeToValidateConfigurationUpdatesOperationContext(
                const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SubscribeToValidateConfigurationUpdatesOperationContext::
            AllocateInitialResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return SubscribeToValidateConfigurationUpdatesResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SubscribeToValidateConfigurationUpdatesOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return ValidateConfigurationUpdateEvents::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::String SubscribeToValidateConfigurationUpdatesOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#SubscribeToValidateConfigurationUpdatesRequest");
        }

        Aws::Crt::String SubscribeToValidateConfigurationUpdatesOperationContext::GetInitialResponseModelName()
            const noexcept
        {
            return Aws::Crt::String("aws.greengrass#SubscribeToValidateConfigurationUpdatesResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> SubscribeToValidateConfigurationUpdatesOperationContext::
            GetStreamingResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#ValidateConfigurationUpdateEvents");
        }

        Aws::Crt::String SubscribeToValidateConfigurationUpdatesOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#SubscribeToValidateConfigurationUpdates");
        }

        std::future<SubscribeToValidateConfigurationUpdatesResult> SubscribeToValidateConfigurationUpdatesOperation::
            GetResult() noexcept
        {
            return std::async(m_asyncLaunchMode, [this]() {
                return SubscribeToValidateConfigurationUpdatesResult(GetOperationResult().get());
            });
        }

        SubscribeToValidateConfigurationUpdatesOperation::SubscribeToValidateConfigurationUpdatesOperation(
            ClientConnection &connection,
            std::shared_ptr<SubscribeToValidateConfigurationUpdatesStreamHandler> streamHandler,
            const SubscribeToValidateConfigurationUpdatesOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, streamHandler, operationContext, allocator)
        {
        }

        std::future<RpcError> SubscribeToValidateConfigurationUpdatesOperation::Activate(
            const SubscribeToValidateConfigurationUpdatesRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String SubscribeToValidateConfigurationUpdatesOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        GetConfigurationOperationContext::GetConfigurationOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> GetConfigurationOperationContext::
            AllocateInitialResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return GetConfigurationResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> GetConfigurationOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String GetConfigurationOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#GetConfigurationRequest");
        }

        Aws::Crt::String GetConfigurationOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#GetConfigurationResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> GetConfigurationOperationContext::GetStreamingResponseModelName()
            const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String GetConfigurationOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#GetConfiguration");
        }

        std::future<GetConfigurationResult> GetConfigurationOperation::GetResult() noexcept
        {
            return std::async(
                m_asyncLaunchMode, [this]() { return GetConfigurationResult(GetOperationResult().get()); });
        }

        GetConfigurationOperation::GetConfigurationOperation(
            ClientConnection &connection,
            const GetConfigurationOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> GetConfigurationOperation::Activate(
            const GetConfigurationRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String GetConfigurationOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        void SubscribeToTopicStreamHandler::OnStreamEvent(Aws::Crt::ScopedResource<AbstractShapeBase> response)
        {
            OnStreamEvent(static_cast<SubscriptionResponseMessage *>(response.get()));
        }

        bool SubscribeToTopicStreamHandler::OnStreamError(
            Aws::Crt::ScopedResource<OperationError> operationError,
            RpcError rpcError)
        {
            bool streamShouldTerminate = false;
            if (rpcError.baseStatus != EVENT_STREAM_RPC_SUCCESS)
            {
                streamShouldTerminate = OnStreamError(rpcError);
            }
            if (operationError != nullptr &&
                operationError->GetModelName() == Aws::Crt::String("aws.greengrass#InvalidArgumentsError") &&
                !streamShouldTerminate)
            {
                streamShouldTerminate = OnStreamError(static_cast<InvalidArgumentsError *>(operationError.get()));
            }
            if (operationError != nullptr &&
                operationError->GetModelName() == Aws::Crt::String("aws.greengrass#ServiceError") &&
                !streamShouldTerminate)
            {
                streamShouldTerminate = OnStreamError(static_cast<ServiceError *>(operationError.get()));
            }
            if (operationError != nullptr &&
                operationError->GetModelName() == Aws::Crt::String("aws.greengrass#UnauthorizedError") &&
                !streamShouldTerminate)
            {
                streamShouldTerminate = OnStreamError(static_cast<UnauthorizedError *>(operationError.get()));
            }
            if (operationError != nullptr && !streamShouldTerminate)
                streamShouldTerminate = OnStreamError(operationError.get());
            return streamShouldTerminate;
        }

        SubscribeToTopicOperationContext::SubscribeToTopicOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SubscribeToTopicOperationContext::
            AllocateInitialResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return SubscribeToTopicResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SubscribeToTopicOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return SubscriptionResponseMessage::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::String SubscribeToTopicOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#SubscribeToTopicRequest");
        }

        Aws::Crt::String SubscribeToTopicOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#SubscribeToTopicResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> SubscribeToTopicOperationContext::GetStreamingResponseModelName()
            const noexcept
        {
            return Aws::Crt::String("aws.greengrass#SubscriptionResponseMessage");
        }

        Aws::Crt::String SubscribeToTopicOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#SubscribeToTopic");
        }

        std::future<SubscribeToTopicResult> SubscribeToTopicOperation::GetResult() noexcept
        {
            return std::async(
                m_asyncLaunchMode, [this]() { return SubscribeToTopicResult(GetOperationResult().get()); });
        }

        SubscribeToTopicOperation::SubscribeToTopicOperation(
            ClientConnection &connection,
            std::shared_ptr<SubscribeToTopicStreamHandler> streamHandler,
            const SubscribeToTopicOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, streamHandler, operationContext, allocator)
        {
        }

        std::future<RpcError> SubscribeToTopicOperation::Activate(
            const SubscribeToTopicRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String SubscribeToTopicOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        GetComponentDetailsOperationContext::GetComponentDetailsOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> GetComponentDetailsOperationContext::
            AllocateInitialResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return GetComponentDetailsResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> GetComponentDetailsOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String GetComponentDetailsOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#GetComponentDetailsRequest");
        }

        Aws::Crt::String GetComponentDetailsOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#GetComponentDetailsResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> GetComponentDetailsOperationContext::GetStreamingResponseModelName()
            const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String GetComponentDetailsOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#GetComponentDetails");
        }

        std::future<GetComponentDetailsResult> GetComponentDetailsOperation::GetResult() noexcept
        {
            return std::async(
                m_asyncLaunchMode, [this]() { return GetComponentDetailsResult(GetOperationResult().get()); });
        }

        GetComponentDetailsOperation::GetComponentDetailsOperation(
            ClientConnection &connection,
            const GetComponentDetailsOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> GetComponentDetailsOperation::Activate(
            const GetComponentDetailsRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String GetComponentDetailsOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        GetClientDeviceAuthTokenOperationContext::GetClientDeviceAuthTokenOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> GetClientDeviceAuthTokenOperationContext::
            AllocateInitialResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return GetClientDeviceAuthTokenResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> GetClientDeviceAuthTokenOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String GetClientDeviceAuthTokenOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#GetClientDeviceAuthTokenRequest");
        }

        Aws::Crt::String GetClientDeviceAuthTokenOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#GetClientDeviceAuthTokenResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> GetClientDeviceAuthTokenOperationContext::GetStreamingResponseModelName()
            const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String GetClientDeviceAuthTokenOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#GetClientDeviceAuthToken");
        }

        std::future<GetClientDeviceAuthTokenResult> GetClientDeviceAuthTokenOperation::GetResult() noexcept
        {
            return std::async(
                m_asyncLaunchMode, [this]() { return GetClientDeviceAuthTokenResult(GetOperationResult().get()); });
        }

        GetClientDeviceAuthTokenOperation::GetClientDeviceAuthTokenOperation(
            ClientConnection &connection,
            const GetClientDeviceAuthTokenOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> GetClientDeviceAuthTokenOperation::Activate(
            const GetClientDeviceAuthTokenRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String GetClientDeviceAuthTokenOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        PublishToTopicOperationContext::PublishToTopicOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> PublishToTopicOperationContext::AllocateInitialResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) const noexcept
        {
            return PublishToTopicResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> PublishToTopicOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String PublishToTopicOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#PublishToTopicRequest");
        }

        Aws::Crt::String PublishToTopicOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#PublishToTopicResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> PublishToTopicOperationContext::GetStreamingResponseModelName()
            const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String PublishToTopicOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#PublishToTopic");
        }

        std::future<PublishToTopicResult> PublishToTopicOperation::GetResult() noexcept
        {
            return std::async(m_asyncLaunchMode, [this]() { return PublishToTopicResult(GetOperationResult().get()); });
        }

        PublishToTopicOperation::PublishToTopicOperation(
            ClientConnection &connection,
            const PublishToTopicOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> PublishToTopicOperation::Activate(
            const PublishToTopicRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String PublishToTopicOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        void SubscribeToCertificateUpdatesStreamHandler::OnStreamEvent(
            Aws::Crt::ScopedResource<AbstractShapeBase> response)
        {
            OnStreamEvent(static_cast<CertificateUpdateEvent *>(response.get()));
        }

        bool SubscribeToCertificateUpdatesStreamHandler::OnStreamError(
            Aws::Crt::ScopedResource<OperationError> operationError,
            RpcError rpcError)
        {
            bool streamShouldTerminate = false;
            if (rpcError.baseStatus != EVENT_STREAM_RPC_SUCCESS)
            {
                streamShouldTerminate = OnStreamError(rpcError);
            }
            if (operationError != nullptr &&
                operationError->GetModelName() == Aws::Crt::String("aws.greengrass#ServiceError") &&
                !streamShouldTerminate)
            {
                streamShouldTerminate = OnStreamError(static_cast<ServiceError *>(operationError.get()));
            }
            if (operationError != nullptr &&
                operationError->GetModelName() == Aws::Crt::String("aws.greengrass#UnauthorizedError") &&
                !streamShouldTerminate)
            {
                streamShouldTerminate = OnStreamError(static_cast<UnauthorizedError *>(operationError.get()));
            }
            if (operationError != nullptr &&
                operationError->GetModelName() == Aws::Crt::String("aws.greengrass#InvalidArgumentsError") &&
                !streamShouldTerminate)
            {
                streamShouldTerminate = OnStreamError(static_cast<InvalidArgumentsError *>(operationError.get()));
            }
            if (operationError != nullptr && !streamShouldTerminate)
                streamShouldTerminate = OnStreamError(operationError.get());
            return streamShouldTerminate;
        }

        SubscribeToCertificateUpdatesOperationContext::SubscribeToCertificateUpdatesOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SubscribeToCertificateUpdatesOperationContext::
            AllocateInitialResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return SubscribeToCertificateUpdatesResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SubscribeToCertificateUpdatesOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return CertificateUpdateEvent::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::String SubscribeToCertificateUpdatesOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#SubscribeToCertificateUpdatesRequest");
        }

        Aws::Crt::String SubscribeToCertificateUpdatesOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#SubscribeToCertificateUpdatesResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> SubscribeToCertificateUpdatesOperationContext::
            GetStreamingResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#CertificateUpdateEvent");
        }

        Aws::Crt::String SubscribeToCertificateUpdatesOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#SubscribeToCertificateUpdates");
        }

        std::future<SubscribeToCertificateUpdatesResult> SubscribeToCertificateUpdatesOperation::GetResult() noexcept
        {
            return std::async(m_asyncLaunchMode, [this]() {
                return SubscribeToCertificateUpdatesResult(GetOperationResult().get());
            });
        }

        SubscribeToCertificateUpdatesOperation::SubscribeToCertificateUpdatesOperation(
            ClientConnection &connection,
            std::shared_ptr<SubscribeToCertificateUpdatesStreamHandler> streamHandler,
            const SubscribeToCertificateUpdatesOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, streamHandler, operationContext, allocator)
        {
        }

        std::future<RpcError> SubscribeToCertificateUpdatesOperation::Activate(
            const SubscribeToCertificateUpdatesRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String SubscribeToCertificateUpdatesOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        VerifyClientDeviceIdentityOperationContext::VerifyClientDeviceIdentityOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> VerifyClientDeviceIdentityOperationContext::
            AllocateInitialResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return VerifyClientDeviceIdentityResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> VerifyClientDeviceIdentityOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String VerifyClientDeviceIdentityOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#VerifyClientDeviceIdentityRequest");
        }

        Aws::Crt::String VerifyClientDeviceIdentityOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#VerifyClientDeviceIdentityResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> VerifyClientDeviceIdentityOperationContext::GetStreamingResponseModelName()
            const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String VerifyClientDeviceIdentityOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#VerifyClientDeviceIdentity");
        }

        std::future<VerifyClientDeviceIdentityResult> VerifyClientDeviceIdentityOperation::GetResult() noexcept
        {
            return std::async(
                m_asyncLaunchMode, [this]() { return VerifyClientDeviceIdentityResult(GetOperationResult().get()); });
        }

        VerifyClientDeviceIdentityOperation::VerifyClientDeviceIdentityOperation(
            ClientConnection &connection,
            const VerifyClientDeviceIdentityOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> VerifyClientDeviceIdentityOperation::Activate(
            const VerifyClientDeviceIdentityRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String VerifyClientDeviceIdentityOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        AuthorizeClientDeviceActionOperationContext::AuthorizeClientDeviceActionOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> AuthorizeClientDeviceActionOperationContext::
            AllocateInitialResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return AuthorizeClientDeviceActionResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> AuthorizeClientDeviceActionOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String AuthorizeClientDeviceActionOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#AuthorizeClientDeviceActionRequest");
        }

        Aws::Crt::String AuthorizeClientDeviceActionOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#AuthorizeClientDeviceActionResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> AuthorizeClientDeviceActionOperationContext::
            GetStreamingResponseModelName() const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String AuthorizeClientDeviceActionOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#AuthorizeClientDeviceAction");
        }

        std::future<AuthorizeClientDeviceActionResult> AuthorizeClientDeviceActionOperation::GetResult() noexcept
        {
            return std::async(
                m_asyncLaunchMode, [this]() { return AuthorizeClientDeviceActionResult(GetOperationResult().get()); });
        }

        AuthorizeClientDeviceActionOperation::AuthorizeClientDeviceActionOperation(
            ClientConnection &connection,
            const AuthorizeClientDeviceActionOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> AuthorizeClientDeviceActionOperation::Activate(
            const AuthorizeClientDeviceActionRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String AuthorizeClientDeviceActionOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        ListComponentsOperationContext::ListComponentsOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> ListComponentsOperationContext::AllocateInitialResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) const noexcept
        {
            return ListComponentsResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> ListComponentsOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String ListComponentsOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#ListComponentsRequest");
        }

        Aws::Crt::String ListComponentsOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#ListComponentsResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> ListComponentsOperationContext::GetStreamingResponseModelName()
            const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String ListComponentsOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#ListComponents");
        }

        std::future<ListComponentsResult> ListComponentsOperation::GetResult() noexcept
        {
            return std::async(m_asyncLaunchMode, [this]() { return ListComponentsResult(GetOperationResult().get()); });
        }

        ListComponentsOperation::ListComponentsOperation(
            ClientConnection &connection,
            const ListComponentsOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> ListComponentsOperation::Activate(
            const ListComponentsRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String ListComponentsOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        CreateDebugPasswordOperationContext::CreateDebugPasswordOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> CreateDebugPasswordOperationContext::
            AllocateInitialResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return CreateDebugPasswordResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> CreateDebugPasswordOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String CreateDebugPasswordOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#CreateDebugPasswordRequest");
        }

        Aws::Crt::String CreateDebugPasswordOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#CreateDebugPasswordResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> CreateDebugPasswordOperationContext::GetStreamingResponseModelName()
            const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String CreateDebugPasswordOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#CreateDebugPassword");
        }

        std::future<CreateDebugPasswordResult> CreateDebugPasswordOperation::GetResult() noexcept
        {
            return std::async(
                m_asyncLaunchMode, [this]() { return CreateDebugPasswordResult(GetOperationResult().get()); });
        }

        CreateDebugPasswordOperation::CreateDebugPasswordOperation(
            ClientConnection &connection,
            const CreateDebugPasswordOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> CreateDebugPasswordOperation::Activate(
            const CreateDebugPasswordRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String CreateDebugPasswordOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        GetThingShadowOperationContext::GetThingShadowOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> GetThingShadowOperationContext::AllocateInitialResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) const noexcept
        {
            return GetThingShadowResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> GetThingShadowOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String GetThingShadowOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#GetThingShadowRequest");
        }

        Aws::Crt::String GetThingShadowOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#GetThingShadowResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> GetThingShadowOperationContext::GetStreamingResponseModelName()
            const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String GetThingShadowOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#GetThingShadow");
        }

        std::future<GetThingShadowResult> GetThingShadowOperation::GetResult() noexcept
        {
            return std::async(m_asyncLaunchMode, [this]() { return GetThingShadowResult(GetOperationResult().get()); });
        }

        GetThingShadowOperation::GetThingShadowOperation(
            ClientConnection &connection,
            const GetThingShadowOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> GetThingShadowOperation::Activate(
            const GetThingShadowRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String GetThingShadowOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        SendConfigurationValidityReportOperationContext::SendConfigurationValidityReportOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SendConfigurationValidityReportOperationContext::
            AllocateInitialResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return SendConfigurationValidityReportResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SendConfigurationValidityReportOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String SendConfigurationValidityReportOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#SendConfigurationValidityReportRequest");
        }

        Aws::Crt::String SendConfigurationValidityReportOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#SendConfigurationValidityReportResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> SendConfigurationValidityReportOperationContext::
            GetStreamingResponseModelName() const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String SendConfigurationValidityReportOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#SendConfigurationValidityReport");
        }

        std::future<SendConfigurationValidityReportResult> SendConfigurationValidityReportOperation::
            GetResult() noexcept
        {
            return std::async(m_asyncLaunchMode, [this]() {
                return SendConfigurationValidityReportResult(GetOperationResult().get());
            });
        }

        SendConfigurationValidityReportOperation::SendConfigurationValidityReportOperation(
            ClientConnection &connection,
            const SendConfigurationValidityReportOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> SendConfigurationValidityReportOperation::Activate(
            const SendConfigurationValidityReportRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String SendConfigurationValidityReportOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        UpdateThingShadowOperationContext::UpdateThingShadowOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> UpdateThingShadowOperationContext::
            AllocateInitialResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return UpdateThingShadowResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> UpdateThingShadowOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String UpdateThingShadowOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#UpdateThingShadowRequest");
        }

        Aws::Crt::String UpdateThingShadowOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#UpdateThingShadowResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> UpdateThingShadowOperationContext::GetStreamingResponseModelName()
            const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String UpdateThingShadowOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#UpdateThingShadow");
        }

        std::future<UpdateThingShadowResult> UpdateThingShadowOperation::GetResult() noexcept
        {
            return std::async(
                m_asyncLaunchMode, [this]() { return UpdateThingShadowResult(GetOperationResult().get()); });
        }

        UpdateThingShadowOperation::UpdateThingShadowOperation(
            ClientConnection &connection,
            const UpdateThingShadowOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> UpdateThingShadowOperation::Activate(
            const UpdateThingShadowRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String UpdateThingShadowOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        UpdateConfigurationOperationContext::UpdateConfigurationOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> UpdateConfigurationOperationContext::
            AllocateInitialResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return UpdateConfigurationResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> UpdateConfigurationOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String UpdateConfigurationOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#UpdateConfigurationRequest");
        }

        Aws::Crt::String UpdateConfigurationOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#UpdateConfigurationResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> UpdateConfigurationOperationContext::GetStreamingResponseModelName()
            const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String UpdateConfigurationOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#UpdateConfiguration");
        }

        std::future<UpdateConfigurationResult> UpdateConfigurationOperation::GetResult() noexcept
        {
            return std::async(
                m_asyncLaunchMode, [this]() { return UpdateConfigurationResult(GetOperationResult().get()); });
        }

        UpdateConfigurationOperation::UpdateConfigurationOperation(
            ClientConnection &connection,
            const UpdateConfigurationOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> UpdateConfigurationOperation::Activate(
            const UpdateConfigurationRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String UpdateConfigurationOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        ValidateAuthorizationTokenOperationContext::ValidateAuthorizationTokenOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> ValidateAuthorizationTokenOperationContext::
            AllocateInitialResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return ValidateAuthorizationTokenResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> ValidateAuthorizationTokenOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String ValidateAuthorizationTokenOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#ValidateAuthorizationTokenRequest");
        }

        Aws::Crt::String ValidateAuthorizationTokenOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#ValidateAuthorizationTokenResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> ValidateAuthorizationTokenOperationContext::GetStreamingResponseModelName()
            const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String ValidateAuthorizationTokenOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#ValidateAuthorizationToken");
        }

        std::future<ValidateAuthorizationTokenResult> ValidateAuthorizationTokenOperation::GetResult() noexcept
        {
            return std::async(
                m_asyncLaunchMode, [this]() { return ValidateAuthorizationTokenResult(GetOperationResult().get()); });
        }

        ValidateAuthorizationTokenOperation::ValidateAuthorizationTokenOperation(
            ClientConnection &connection,
            const ValidateAuthorizationTokenOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> ValidateAuthorizationTokenOperation::Activate(
            const ValidateAuthorizationTokenRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String ValidateAuthorizationTokenOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        RestartComponentOperationContext::RestartComponentOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> RestartComponentOperationContext::
            AllocateInitialResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return RestartComponentResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> RestartComponentOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String RestartComponentOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#RestartComponentRequest");
        }

        Aws::Crt::String RestartComponentOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#RestartComponentResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> RestartComponentOperationContext::GetStreamingResponseModelName()
            const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String RestartComponentOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#RestartComponent");
        }

        std::future<RestartComponentResult> RestartComponentOperation::GetResult() noexcept
        {
            return std::async(
                m_asyncLaunchMode, [this]() { return RestartComponentResult(GetOperationResult().get()); });
        }

        RestartComponentOperation::RestartComponentOperation(
            ClientConnection &connection,
            const RestartComponentOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> RestartComponentOperation::Activate(
            const RestartComponentRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String RestartComponentOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        GetLocalDeploymentStatusOperationContext::GetLocalDeploymentStatusOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> GetLocalDeploymentStatusOperationContext::
            AllocateInitialResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return GetLocalDeploymentStatusResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> GetLocalDeploymentStatusOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String GetLocalDeploymentStatusOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#GetLocalDeploymentStatusRequest");
        }

        Aws::Crt::String GetLocalDeploymentStatusOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#GetLocalDeploymentStatusResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> GetLocalDeploymentStatusOperationContext::GetStreamingResponseModelName()
            const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String GetLocalDeploymentStatusOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#GetLocalDeploymentStatus");
        }

        std::future<GetLocalDeploymentStatusResult> GetLocalDeploymentStatusOperation::GetResult() noexcept
        {
            return std::async(
                m_asyncLaunchMode, [this]() { return GetLocalDeploymentStatusResult(GetOperationResult().get()); });
        }

        GetLocalDeploymentStatusOperation::GetLocalDeploymentStatusOperation(
            ClientConnection &connection,
            const GetLocalDeploymentStatusOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> GetLocalDeploymentStatusOperation::Activate(
            const GetLocalDeploymentStatusRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String GetLocalDeploymentStatusOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        GetSecretValueOperationContext::GetSecretValueOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> GetSecretValueOperationContext::AllocateInitialResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) const noexcept
        {
            return GetSecretValueResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> GetSecretValueOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String GetSecretValueOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#GetSecretValueRequest");
        }

        Aws::Crt::String GetSecretValueOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#GetSecretValueResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> GetSecretValueOperationContext::GetStreamingResponseModelName()
            const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String GetSecretValueOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#GetSecretValue");
        }

        std::future<GetSecretValueResult> GetSecretValueOperation::GetResult() noexcept
        {
            return std::async(m_asyncLaunchMode, [this]() { return GetSecretValueResult(GetOperationResult().get()); });
        }

        GetSecretValueOperation::GetSecretValueOperation(
            ClientConnection &connection,
            const GetSecretValueOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> GetSecretValueOperation::Activate(
            const GetSecretValueRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String GetSecretValueOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        UpdateStateOperationContext::UpdateStateOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> UpdateStateOperationContext::AllocateInitialResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) const noexcept
        {
            return UpdateStateResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> UpdateStateOperationContext::AllocateStreamingResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String UpdateStateOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#UpdateStateRequest");
        }

        Aws::Crt::String UpdateStateOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#UpdateStateResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> UpdateStateOperationContext::GetStreamingResponseModelName() const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String UpdateStateOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#UpdateState");
        }

        std::future<UpdateStateResult> UpdateStateOperation::GetResult() noexcept
        {
            return std::async(m_asyncLaunchMode, [this]() { return UpdateStateResult(GetOperationResult().get()); });
        }

        UpdateStateOperation::UpdateStateOperation(
            ClientConnection &connection,
            const UpdateStateOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> UpdateStateOperation::Activate(
            const UpdateStateRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String UpdateStateOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        CancelLocalDeploymentOperationContext::CancelLocalDeploymentOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> CancelLocalDeploymentOperationContext::
            AllocateInitialResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return CancelLocalDeploymentResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> CancelLocalDeploymentOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String CancelLocalDeploymentOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#CancelLocalDeploymentRequest");
        }

        Aws::Crt::String CancelLocalDeploymentOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#CancelLocalDeploymentResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> CancelLocalDeploymentOperationContext::GetStreamingResponseModelName()
            const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String CancelLocalDeploymentOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#CancelLocalDeployment");
        }

        std::future<CancelLocalDeploymentResult> CancelLocalDeploymentOperation::GetResult() noexcept
        {
            return std::async(
                m_asyncLaunchMode, [this]() { return CancelLocalDeploymentResult(GetOperationResult().get()); });
        }

        CancelLocalDeploymentOperation::CancelLocalDeploymentOperation(
            ClientConnection &connection,
            const CancelLocalDeploymentOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> CancelLocalDeploymentOperation::Activate(
            const CancelLocalDeploymentRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String CancelLocalDeploymentOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        ListNamedShadowsForThingOperationContext::ListNamedShadowsForThingOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> ListNamedShadowsForThingOperationContext::
            AllocateInitialResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return ListNamedShadowsForThingResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> ListNamedShadowsForThingOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String ListNamedShadowsForThingOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#ListNamedShadowsForThingRequest");
        }

        Aws::Crt::String ListNamedShadowsForThingOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#ListNamedShadowsForThingResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> ListNamedShadowsForThingOperationContext::GetStreamingResponseModelName()
            const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String ListNamedShadowsForThingOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#ListNamedShadowsForThing");
        }

        std::future<ListNamedShadowsForThingResult> ListNamedShadowsForThingOperation::GetResult() noexcept
        {
            return std::async(
                m_asyncLaunchMode, [this]() { return ListNamedShadowsForThingResult(GetOperationResult().get()); });
        }

        ListNamedShadowsForThingOperation::ListNamedShadowsForThingOperation(
            ClientConnection &connection,
            const ListNamedShadowsForThingOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> ListNamedShadowsForThingOperation::Activate(
            const ListNamedShadowsForThingRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String ListNamedShadowsForThingOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        void SubscribeToComponentUpdatesStreamHandler::OnStreamEvent(
            Aws::Crt::ScopedResource<AbstractShapeBase> response)
        {
            OnStreamEvent(static_cast<ComponentUpdatePolicyEvents *>(response.get()));
        }

        bool SubscribeToComponentUpdatesStreamHandler::OnStreamError(
            Aws::Crt::ScopedResource<OperationError> operationError,
            RpcError rpcError)
        {
            bool streamShouldTerminate = false;
            if (rpcError.baseStatus != EVENT_STREAM_RPC_SUCCESS)
            {
                streamShouldTerminate = OnStreamError(rpcError);
            }
            if (operationError != nullptr &&
                operationError->GetModelName() == Aws::Crt::String("aws.greengrass#ServiceError") &&
                !streamShouldTerminate)
            {
                streamShouldTerminate = OnStreamError(static_cast<ServiceError *>(operationError.get()));
            }
            if (operationError != nullptr &&
                operationError->GetModelName() == Aws::Crt::String("aws.greengrass#ResourceNotFoundError") &&
                !streamShouldTerminate)
            {
                streamShouldTerminate = OnStreamError(static_cast<ResourceNotFoundError *>(operationError.get()));
            }
            if (operationError != nullptr && !streamShouldTerminate)
                streamShouldTerminate = OnStreamError(operationError.get());
            return streamShouldTerminate;
        }

        SubscribeToComponentUpdatesOperationContext::SubscribeToComponentUpdatesOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SubscribeToComponentUpdatesOperationContext::
            AllocateInitialResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return SubscribeToComponentUpdatesResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> SubscribeToComponentUpdatesOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return ComponentUpdatePolicyEvents::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::String SubscribeToComponentUpdatesOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#SubscribeToComponentUpdatesRequest");
        }

        Aws::Crt::String SubscribeToComponentUpdatesOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#SubscribeToComponentUpdatesResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> SubscribeToComponentUpdatesOperationContext::
            GetStreamingResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#ComponentUpdatePolicyEvents");
        }

        Aws::Crt::String SubscribeToComponentUpdatesOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#SubscribeToComponentUpdates");
        }

        std::future<SubscribeToComponentUpdatesResult> SubscribeToComponentUpdatesOperation::GetResult() noexcept
        {
            return std::async(
                m_asyncLaunchMode, [this]() { return SubscribeToComponentUpdatesResult(GetOperationResult().get()); });
        }

        SubscribeToComponentUpdatesOperation::SubscribeToComponentUpdatesOperation(
            ClientConnection &connection,
            std::shared_ptr<SubscribeToComponentUpdatesStreamHandler> streamHandler,
            const SubscribeToComponentUpdatesOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, streamHandler, operationContext, allocator)
        {
        }

        std::future<RpcError> SubscribeToComponentUpdatesOperation::Activate(
            const SubscribeToComponentUpdatesRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String SubscribeToComponentUpdatesOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        ListLocalDeploymentsOperationContext::ListLocalDeploymentsOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> ListLocalDeploymentsOperationContext::
            AllocateInitialResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return ListLocalDeploymentsResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> ListLocalDeploymentsOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String ListLocalDeploymentsOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#ListLocalDeploymentsRequest");
        }

        Aws::Crt::String ListLocalDeploymentsOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#ListLocalDeploymentsResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> ListLocalDeploymentsOperationContext::GetStreamingResponseModelName()
            const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String ListLocalDeploymentsOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#ListLocalDeployments");
        }

        std::future<ListLocalDeploymentsResult> ListLocalDeploymentsOperation::GetResult() noexcept
        {
            return std::async(
                m_asyncLaunchMode, [this]() { return ListLocalDeploymentsResult(GetOperationResult().get()); });
        }

        ListLocalDeploymentsOperation::ListLocalDeploymentsOperation(
            ClientConnection &connection,
            const ListLocalDeploymentsOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> ListLocalDeploymentsOperation::Activate(
            const ListLocalDeploymentsRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String ListLocalDeploymentsOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        StopComponentOperationContext::StopComponentOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> StopComponentOperationContext::AllocateInitialResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) const noexcept
        {
            return StopComponentResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> StopComponentOperationContext::AllocateStreamingResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String StopComponentOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#StopComponentRequest");
        }

        Aws::Crt::String StopComponentOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#StopComponentResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> StopComponentOperationContext::GetStreamingResponseModelName()
            const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String StopComponentOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#StopComponent");
        }

        std::future<StopComponentResult> StopComponentOperation::GetResult() noexcept
        {
            return std::async(m_asyncLaunchMode, [this]() { return StopComponentResult(GetOperationResult().get()); });
        }

        StopComponentOperation::StopComponentOperation(
            ClientConnection &connection,
            const StopComponentOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> StopComponentOperation::Activate(
            const StopComponentRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String StopComponentOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        PauseComponentOperationContext::PauseComponentOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> PauseComponentOperationContext::AllocateInitialResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) const noexcept
        {
            return PauseComponentResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> PauseComponentOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String PauseComponentOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#PauseComponentRequest");
        }

        Aws::Crt::String PauseComponentOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#PauseComponentResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> PauseComponentOperationContext::GetStreamingResponseModelName()
            const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String PauseComponentOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#PauseComponent");
        }

        std::future<PauseComponentResult> PauseComponentOperation::GetResult() noexcept
        {
            return std::async(m_asyncLaunchMode, [this]() { return PauseComponentResult(GetOperationResult().get()); });
        }

        PauseComponentOperation::PauseComponentOperation(
            ClientConnection &connection,
            const PauseComponentOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> PauseComponentOperation::Activate(
            const PauseComponentRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String PauseComponentOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        CreateLocalDeploymentOperationContext::CreateLocalDeploymentOperationContext(
            const GreengrassCoreIpcServiceModel &serviceModel) noexcept
            : OperationModelContext(serviceModel)
        {
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> CreateLocalDeploymentOperationContext::
            AllocateInitialResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            return CreateLocalDeploymentResponse::s_allocateFromPayload(stringView, allocator);
        }

        Aws::Crt::ScopedResource<AbstractShapeBase> CreateLocalDeploymentOperationContext::
            AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator)
                const noexcept
        {
            (void)stringView;
            (void)allocator;
            return nullptr;
        }

        Aws::Crt::String CreateLocalDeploymentOperationContext::GetRequestModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#CreateLocalDeploymentRequest");
        }

        Aws::Crt::String CreateLocalDeploymentOperationContext::GetInitialResponseModelName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#CreateLocalDeploymentResponse");
        }

        Aws::Crt::Optional<Aws::Crt::String> CreateLocalDeploymentOperationContext::GetStreamingResponseModelName()
            const noexcept
        {
            return Aws::Crt::Optional<Aws::Crt::String>();
        }

        Aws::Crt::String CreateLocalDeploymentOperationContext::GetOperationName() const noexcept
        {
            return Aws::Crt::String("aws.greengrass#CreateLocalDeployment");
        }

        std::future<CreateLocalDeploymentResult> CreateLocalDeploymentOperation::GetResult() noexcept
        {
            return std::async(
                m_asyncLaunchMode, [this]() { return CreateLocalDeploymentResult(GetOperationResult().get()); });
        }

        CreateLocalDeploymentOperation::CreateLocalDeploymentOperation(
            ClientConnection &connection,
            const CreateLocalDeploymentOperationContext &operationContext,
            Aws::Crt::Allocator *allocator) noexcept
            : ClientOperation(connection, nullptr, operationContext, allocator)
        {
        }

        std::future<RpcError> CreateLocalDeploymentOperation::Activate(
            const CreateLocalDeploymentRequest &request,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
        }

        Aws::Crt::String CreateLocalDeploymentOperation::GetModelName() const noexcept
        {
            return m_operationModelContext.GetOperationName();
        }

        GreengrassCoreIpcServiceModel::GreengrassCoreIpcServiceModel() noexcept
            : m_subscribeToIoTCoreOperationContext(*this), m_resumeComponentOperationContext(*this),
              m_publishToIoTCoreOperationContext(*this), m_subscribeToConfigurationUpdateOperationContext(*this),
              m_deleteThingShadowOperationContext(*this), m_putComponentMetricOperationContext(*this),
              m_deferComponentUpdateOperationContext(*this),
              m_subscribeToValidateConfigurationUpdatesOperationContext(*this),
              m_getConfigurationOperationContext(*this), m_subscribeToTopicOperationContext(*this),
              m_getComponentDetailsOperationContext(*this), m_getClientDeviceAuthTokenOperationContext(*this),
              m_publishToTopicOperationContext(*this), m_subscribeToCertificateUpdatesOperationContext(*this),
              m_verifyClientDeviceIdentityOperationContext(*this), m_authorizeClientDeviceActionOperationContext(*this),
              m_listComponentsOperationContext(*this), m_createDebugPasswordOperationContext(*this),
              m_getThingShadowOperationContext(*this), m_sendConfigurationValidityReportOperationContext(*this),
              m_updateThingShadowOperationContext(*this), m_updateConfigurationOperationContext(*this),
              m_validateAuthorizationTokenOperationContext(*this), m_restartComponentOperationContext(*this),
              m_getLocalDeploymentStatusOperationContext(*this), m_getSecretValueOperationContext(*this),
              m_updateStateOperationContext(*this), m_cancelLocalDeploymentOperationContext(*this),
              m_listNamedShadowsForThingOperationContext(*this), m_subscribeToComponentUpdatesOperationContext(*this),
              m_listLocalDeploymentsOperationContext(*this), m_stopComponentOperationContext(*this),
              m_pauseComponentOperationContext(*this), m_createLocalDeploymentOperationContext(*this)
        {
        }

        Aws::Crt::ScopedResource<OperationError> GreengrassCoreIpcServiceModel::AllocateOperationErrorFromPayload(
            const Aws::Crt::String &errorModelName,
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator) const noexcept
        {
            auto it = m_modelNameToErrorResponse.find(errorModelName);
            if (it == m_modelNameToErrorResponse.end())
            {
                return nullptr;
            }
            else
            {
                return it->second(stringView, allocator);
            }
        }

        void GreengrassCoreIpcServiceModel::AssignModelNameToErrorResponse(
            Aws::Crt::String modelName,
            ErrorResponseFactory factory) noexcept
        {
            m_modelNameToErrorResponse[modelName] = factory;
        }
    } // namespace Greengrass
} // namespace Aws
