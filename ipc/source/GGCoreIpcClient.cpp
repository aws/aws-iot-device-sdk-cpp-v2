#include <aws/ipc/GGCoreIpcClient.h>

namespace Aws
{
    namespace Eventstreamrpc
    {
        namespace Ipc
        {
            BinaryMessage::BinaryMessage(Crt::Optional<Crt::Vector<uint8_t>> &&message, Crt::Allocator* allocator) noexcept : AbstractShapeBase(allocator), m_message(message) {}

            BinaryMessage::BinaryMessage(const Crt::Optional<Crt::Vector<uint8_t>> &message, Crt::Allocator* allocator) noexcept
                : AbstractShapeBase(allocator), m_message(message)
            {
            }

            void BinaryMessage::SerializeToJsonObject(Crt::JsonObject &payloadObject) const
            {
                if (m_message.has_value())
                {
                    payloadObject.WithString("message", Crt::Base64Encode(m_message.value()));
                }
            }

            void BinaryMessage::LoadFromJsonView(BinaryMessage &message, const Crt::JsonView &jsonView) noexcept
            {
                if (jsonView.ValueExists("message"))
                {
                    message.m_message =
                        Crt::Optional<Crt::Vector<uint8_t>>(Crt::Base64Decode(jsonView.GetString("message")));
                }
            }

            Crt::String BinaryMessage::GetModelName() const noexcept
            {
                return Crt::String("aws.greengrass#BinaryMessage");
            }

            JsonMessage::JsonMessage(Crt::Optional<Crt::JsonObject> &&jsonObject, Crt::Allocator* allocator) noexcept : AbstractShapeBase(allocator), m_jsonObject(jsonObject) {}

            JsonMessage::JsonMessage(const Crt::Optional<Crt::JsonObject> &jsonObject, Crt::Allocator* allocator) noexcept
                : AbstractShapeBase(allocator), m_jsonObject(jsonObject)
            {
            }

            Crt::String JsonMessage::GetModelName() const noexcept { return Crt::String("aws.greengrass#JsonMessage"); }

            void JsonMessage::SerializeToJsonObject(Crt::JsonObject &payloadObject) const
            {
                /* May want to std::move(m_jsonObject) instead, but ideally, it should be up to the application. */
                if (m_jsonObject.has_value())
                {
                    payloadObject.WithObject("message", m_jsonObject.value());
                }
            }

            void JsonMessage::LoadFromJsonView(JsonMessage &message, const Crt::JsonView &jsonView) noexcept
            {
                if (jsonView.ValueExists("message"))
                {
                    message.m_jsonObject =
                        Crt::Optional<Crt::JsonObject>(Crt::JsonObject(jsonView.GetString("message")));
                }
            }

            PublishMessage::PublishMessage(Crt::Allocator* allocator) noexcept : AbstractShapeBase(allocator)
            {
            }

            PublishMessage::PublishMessage(
                const Crt::Optional<JsonMessage> &jsonMessage,
                const Crt::Optional<BinaryMessage> &binaryMessage,
                Crt::Allocator* allocator) noexcept
                : AbstractShapeBase(allocator), m_jsonMessage(jsonMessage), m_binaryMessage(binaryMessage)
            {
            }

            PublishMessage::PublishMessage(
                Crt::Optional<JsonMessage> &&jsonMessage,
                Crt::Optional<BinaryMessage> &&binaryMessage,
                Crt::Allocator* allocator) noexcept
                : AbstractShapeBase(allocator), m_jsonMessage(jsonMessage), m_binaryMessage(binaryMessage)
            {
            }

            PublishToTopicOperation::PublishToTopicOperation(ClientConnection &connection, Crt::Allocator* allocator) noexcept
                : ClientOperation(connection, nullptr, allocator)
            {
            }

            void PublishToTopicOperation::Activate(
                const PublishToTopicRequest &request,
                OnMessageFlushCallback onMessageFlushCallback) noexcept
            {
                ClientOperation::Activate((const OperationRequest *)&request, onMessageFlushCallback);
            }

            void PublishMessage::SerializeToJsonObject(Crt::JsonObject &payloadObject) const
            {
                if (m_jsonMessage.has_value())
                {
                    Aws::Crt::JsonObject jsonObject;
                    m_jsonMessage.value().SerializeToJsonObject(jsonObject);
                    payloadObject.WithObject("jsonMessage", std::move(jsonObject));
                    return;
                }
                if (m_binaryMessage.has_value())
                {
                    Aws::Crt::JsonObject jsonObject;
                    m_binaryMessage.value().SerializeToJsonObject(jsonObject);
                    payloadObject.WithObject("binaryMessage", std::move(jsonObject));
                }
            }

            Crt::String PublishToTopicRequest::GetModelName() const noexcept
            {
                return Crt::String("aws.greengrass#PublishToTopicRequest");
            }

            PublishToTopicRequest::PublishToTopicRequest(
                const Crt::Optional<Crt::String> &topic,
                const Crt::Optional<PublishMessage> &publishMessage,
                Crt::Allocator* allocator) noexcept
                : OperationRequest(allocator), m_topic(topic), m_publishMessage(publishMessage)
            {}
            PublishToTopicRequest::PublishToTopicRequest(
                Crt::Optional<Crt::String> &&topic,
                Crt::Optional<PublishMessage> &&publishMessage,
                Crt::Allocator* allocator) noexcept
                : OperationRequest(allocator), m_topic(topic), m_publishMessage(publishMessage)
            {}

            void PublishToTopicRequest::SerializeToJsonObject(Crt::JsonObject &payloadObject) const
            {
                if (m_topic.has_value())
                {
                    payloadObject.WithString("topic", m_topic.value());
                }
                if (m_publishMessage.has_value())
                {
                    Aws::Crt::JsonObject jsonObject;
                    m_publishMessage.value().SerializeToJsonObject(jsonObject);
                    payloadObject.WithObject("publishMessage", std::move(jsonObject));
                }
            }

            Crt::String PublishToTopicOperation::GetModelName() const noexcept
            {
                return Crt::String("aws.greengrass#PublishToTopic");
            }

            PublishToTopicResponse::PublishToTopicResponse(Crt::Allocator* allocator) noexcept : OperationResponse(allocator) {}

            Crt::String PublishToTopicResponse::GetModelName() const noexcept
            {
                return Crt::String("aws.greengrass#PublishToTopicResponse");
            }

            Crt::ScopedResource<OperationResponse> PublishToTopicResponse::s_loadFromPayload(
                Crt::StringView stringView,
                Crt::Allocator *allocator) noexcept
            {
                Crt::ScopedResource<PublishToTopicResponse> derivedResponse(
                    Crt::New<PublishToTopicResponse>(allocator), PublishToTopicResponse::s_customDeleter);
                auto operationResponse = static_cast<OperationResponse *>(derivedResponse.release());
                return Crt::ScopedResource<OperationResponse>(operationResponse);
            }

            void PublishToTopicResponse::s_customDeleter(PublishToTopicResponse *response) noexcept
            {
                OperationResponse::s_customDeleter((OperationResponse *)response);
            }

            GreengrassIpcClient::GreengrassIpcClient(ClientConnection &&connection, Crt::Allocator* allocator) noexcept
                : m_connection(std::move(connection)), m_allocator(allocator)
            {
                m_ModelNameToSingleResponseObject[Crt::String("aws.greengrass#PublishToTopic")] = PublishToTopicResponse::s_loadFromPayload;
            }

            PublishToTopicOperation GreengrassIpcClient::NewPublishToTopic() noexcept
            {
                return PublishToTopicOperation(m_connection, m_allocator);
            }

        } // namespace Ipc
    }     // namespace Eventstreamrpc
} // namespace Aws
