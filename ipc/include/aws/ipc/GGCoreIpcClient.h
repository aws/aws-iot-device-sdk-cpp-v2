/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/eventstreamrpc/EventStreamClient.h>

#include <aws/ipc/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Eventstreamrpc
    {
        namespace Ipc
        {
            class BinaryMessage : public AbstractShapeBase
            {
              public:
                BinaryMessage(const Crt::Optional<Crt::Vector<uint8_t>> &message, Crt::Allocator* allocator = Crt::g_allocator) noexcept;
                BinaryMessage(Crt::Optional<Crt::Vector<uint8_t>> &&message, Crt::Allocator* allocator = Crt::g_allocator) noexcept;
                BinaryMessage(const BinaryMessage &jsonMessage) = default;
                BinaryMessage(BinaryMessage &&jsonMessage) = default;
                void SerializeToJsonObject(Crt::JsonObject &payloadObject) const override;
                static void LoadFromJsonView(BinaryMessage &message, const Crt::JsonView &payloadView) noexcept;

              protected:
                Crt::String GetModelName() const noexcept override;

              private:
                Crt::Optional<Crt::Vector<uint8_t>> m_message;
            };
            class JsonMessage : public AbstractShapeBase
            {
              public:
                JsonMessage(Crt::Optional<Crt::JsonObject> &&jsonObject, Crt::Allocator* allocator = Crt::g_allocator) noexcept;
                JsonMessage(const Crt::Optional<Crt::JsonObject> &jsonObject, Crt::Allocator* allocator = Crt::g_allocator) noexcept;
                JsonMessage(const JsonMessage &jsonMessage) = default;
                JsonMessage(JsonMessage &&jsonMessage) = default;
                void SerializeToJsonObject(Crt::JsonObject &payloadObject) const override;
                static void LoadFromJsonView(JsonMessage &message, const Crt::JsonView &payloadView) noexcept;

              protected:
                Crt::String GetModelName() const noexcept override;

              private:
                Crt::Optional<Crt::JsonObject> m_jsonObject;
            };
            class PublishMessage : public AbstractShapeBase
            {
              public:
                PublishMessage(Crt::Allocator* allocator = Crt::g_allocator) noexcept;
                PublishMessage(const PublishMessage &publishMessage) = default;
                PublishMessage(PublishMessage &&publishMessage) = default;
                PublishMessage(
                    const Crt::Optional<JsonMessage> &jsonMessage,
                    const Crt::Optional<BinaryMessage> &binaryMessage,
                    Crt::Allocator* allocator = Crt::g_allocator) noexcept;
                PublishMessage(
                    Crt::Optional<JsonMessage> &&jsonMessage,
                    Crt::Optional<BinaryMessage> &&binaryMessage,
                    Crt::Allocator* allocator = Crt::g_allocator) noexcept;
                void SerializeToJsonObject(Crt::JsonObject &payloadObject) const override;

              private:
                Crt::Optional<JsonMessage> m_jsonMessage;
                Crt::Optional<BinaryMessage> m_binaryMessage;
            };
            class PublishToTopicRequest : public OperationRequest
            {
              public:
                PublishToTopicRequest(const PublishToTopicRequest &) noexcept = default;
                PublishToTopicRequest(PublishToTopicRequest &&) noexcept = default;
                PublishToTopicRequest(
                    const Crt::Optional<Crt::String> &topic,
                    const Crt::Optional<PublishMessage> &publishMessage,
                    Crt::Allocator* allocator = Crt::g_allocator) noexcept;
                PublishToTopicRequest(
                    Crt::Optional<Crt::String> &&topic,
                    Crt::Optional<PublishMessage> &&publishMessage,
                    Crt::Allocator* allocator = Crt::g_allocator) noexcept;
                void SerializeToJsonObject(Crt::JsonObject &payloadObject) const override;

              protected:
                Crt::String GetModelName() const noexcept override;

              private:
                Crt::Optional<Crt::String> m_topic;
                Crt::Optional<PublishMessage> m_publishMessage;
            };

            class PublishToTopicResponse : public OperationResponse
            {
              public:
                PublishToTopicResponse(Crt::Allocator* allocator = Crt::g_allocator) noexcept;
                static Crt::ScopedResource<OperationResponse> s_loadFromPayload(
                    Crt::StringView stringView,
                    Crt::Allocator *allocator) noexcept;
                static void s_customDeleter(PublishToTopicResponse *response) noexcept;

              protected:
                Crt::String GetModelName() const noexcept override;
            };

            class SubscribeToTopicStreamHandler : public StreamResponseHandler
            {
            };

            class PublishToTopicOperation : public ClientOperation
            {
              public:
                PublishToTopicOperation(ClientConnection &connection, Crt::Allocator* allocator) noexcept;
                void Activate(
                    const PublishToTopicRequest &request,
                    OnMessageFlushCallback onMessageFlushCallback) noexcept;

              protected:
                Crt::String GetModelName() const noexcept override;
            };

            class GreengrassIpcClient
            {
              public:
                GreengrassIpcClient(ClientConnection &&connection, Crt::Allocator* allocator = Crt::g_allocator) noexcept;
                PublishToTopicOperation NewPublishToTopic() noexcept;

              private:
                ClientConnection m_connection;
                Crt::Allocator* m_allocator;
                Crt::Map<Crt::String, ExpectedResponseFactory> m_ModelNameToSingleResponseObject;
                Crt::Map<Crt::String, ExpectedResponseFactory> m_ModelNameToStreamingResponseObject;
                Crt::Map<Crt::String, ErrorResponseFactory> m_ErrorNameToObject;
            };
        } // namespace Ipc

    } // namespace Eventstreamrpc
} // namespace Aws