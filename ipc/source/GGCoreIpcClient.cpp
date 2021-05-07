#include <aws/ipc/GGCoreIpcClient.h>

namespace Aws
{
    namespace Eventstreamrpc
    {
        namespace Ipc
        {
            BinaryMessage::BinaryMessage(Crt::Optional<Crt::Vector<uint8_t>> &&message) noexcept : m_message(message) {}

            BinaryMessage::BinaryMessage(const Crt::Optional<Crt::Vector<uint8_t>> &message) noexcept
                : m_message(message)
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

            JsonMessage::JsonMessage(Crt::Optional<Crt::JsonObject> &&jsonObject) noexcept : m_jsonObject(jsonObject) {}

            JsonMessage::JsonMessage(const Crt::Optional<Crt::JsonObject> &jsonObject) noexcept
                : m_jsonObject(jsonObject)
            {
            }

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

            PublishMessage::PublishMessage(
                const Crt::Optional<JsonMessage> &jsonMessage,
                const Crt::Optional<BinaryMessage> &optionalMessage) noexcept
                : m_jsonMessage(jsonMessage), m_optionalMessage(optionalMessage)
            {
            }

            PublishMessage::PublishMessage(
                Crt::Optional<JsonMessage> &&jsonMessage,
                Crt::Optional<BinaryMessage> &&optionalMessage) noexcept
                : m_jsonMessage(jsonMessage), m_optionalMessage(optionalMessage)
            {
            }

            void PublishToTopicOperation::Activate(
                const PublishToTopicRequest &request,
                OnMessageFlushCallback onMessageFlushCallback) noexcept
            {
                ClientOperation::Activate((const OperationRequest *)&request, onMessageFlushCallback);
            }

            Crt::String PublishToTopicRequest::GetModelName() const noexcept
            {
                return Crt::String("aws.greengrass#PublishToTopicRequest");
            }

            Crt::String PublishToTopicOperation::GetModelName() const noexcept
            {
                return Crt::String("aws.greengrass#PublishToTopic");
            }

            PublishToTopicResponse::PublishToTopicResponse() noexcept {}

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

        } // namespace Ipc
    }     // namespace Eventstreamrpc
} // namespace Aws
