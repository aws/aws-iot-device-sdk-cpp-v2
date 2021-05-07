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
                BinaryMessage(
                    const Crt::Optional<Crt::Vector<uint8_t>> &message,
                    Crt::Allocator *allocator = Crt::g_allocator) noexcept;
                BinaryMessage(
                    Crt::Optional<Crt::Vector<uint8_t>> &&message,
                    Crt::Allocator *allocator = Crt::g_allocator) noexcept;
                BinaryMessage(const BinaryMessage &binaryMessage) = default;
                BinaryMessage(BinaryMessage &&binaryMessage) = default;
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
                JsonMessage(Crt::Allocator *allocator = Crt::g_allocator) noexcept;
                JsonMessage(
                    Crt::Optional<Crt::JsonObject> &&jsonObject,
                    Crt::Allocator *allocator = Crt::g_allocator) noexcept;
                JsonMessage(
                    const Crt::Optional<Crt::JsonObject> &jsonObject,
                    Crt::Allocator *allocator = Crt::g_allocator) noexcept;
                JsonMessage(const JsonMessage &jsonMessage) = default;
                JsonMessage(JsonMessage &&jsonMessage) = default;
                void SerializeToJsonObject(Crt::JsonObject &payloadObject) const override;
                static void LoadFromJsonView(JsonMessage &message, const Crt::JsonView &payloadView) noexcept;
                void SetJsonObject();

              protected:
                Crt::String GetModelName() const noexcept override;

              private:
                Crt::Optional<Crt::JsonObject> m_jsonObject;
            };
            class PublishMessage : public AbstractShapeBase
            {
              public:
                PublishMessage(Crt::Allocator *allocator = Crt::g_allocator) noexcept;
                PublishMessage(const PublishMessage &publishMessage) = default;
                PublishMessage(PublishMessage &&publishMessage) = default;
                PublishMessage(
                    const Crt::Optional<BinaryMessage> &binaryMessage,
                    Crt::Allocator *allocator = Crt::g_allocator) noexcept;
                PublishMessage(
                    const Crt::Optional<JsonMessage> &jsonMessage,
                    Crt::Allocator *allocator = Crt::g_allocator) noexcept;
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
                    Crt::Allocator *allocator = Crt::g_allocator) noexcept;
                PublishToTopicRequest(
                    Crt::Optional<Crt::String> &&topic,
                    Crt::Optional<PublishMessage> &&publishMessage,
                    Crt::Allocator *allocator = Crt::g_allocator) noexcept;
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
                PublishToTopicResponse(Crt::Allocator *allocator = Crt::g_allocator) noexcept;
                static Crt::ScopedResource<OperationResponse> s_loadFromPayload(
                    Crt::StringView stringView,
                    Crt::Allocator *allocator) noexcept;
                static void s_customDeleter(PublishToTopicResponse *response) noexcept;

              protected:
                Crt::String GetModelName() const noexcept override;
            };

            class PublishToTopicOperation : public ClientOperation
            {
              public:
                PublishToTopicOperation(ClientConnection &connection, Crt::Allocator *allocator) noexcept;
                void Activate(
                    const PublishToTopicRequest &request,
                    OnMessageFlushCallback onMessageFlushCallback) noexcept;

              protected:
                Crt::String GetModelName() const noexcept override;
            };

            class SubscriptionResponseMessage : public OperationResponse
            {
              public:
                SubscriptionResponseMessage(Crt::Allocator *allocator = Crt::g_allocator) noexcept;
                SubscriptionResponseMessage(const SubscriptionResponseMessage &publishMessage) = default;
                SubscriptionResponseMessage(SubscriptionResponseMessage &&publishMessage) = default;
                SubscriptionResponseMessage(
                    const Crt::Optional<JsonMessage> &jsonMessage,
                    Crt::Allocator *allocator = Crt::g_allocator) noexcept;
                SubscriptionResponseMessage(
                    const Crt::Optional<BinaryMessage> &binaryMessage,
                    Crt::Allocator *allocator = Crt::g_allocator) noexcept;
                SubscriptionResponseMessage(
                    Crt::Optional<JsonMessage> &&jsonMessage,
                    Crt::Allocator *allocator = Crt::g_allocator) noexcept;
                SubscriptionResponseMessage(
                    Crt::Optional<BinaryMessage> &&binaryMessage,
                    Crt::Allocator *allocator = Crt::g_allocator) noexcept;
                static Crt::ScopedResource<OperationResponse> s_loadFromPayload(
                    Crt::StringView stringView,
                    Crt::Allocator *allocator) noexcept;
                static void s_customDeleter(SubscriptionResponseMessage *response) noexcept;

              protected:
                Crt::String GetModelName() const noexcept override;

              private:
                Crt::Optional<JsonMessage> m_jsonMessage;
                Crt::Optional<BinaryMessage> m_binaryMessage;
            };

            class SubscribeToTopicStreamHandler : public StreamResponseHandler
            {
              public:
                virtual void OnStreamEvent(SubscriptionResponseMessage *response);
                /* TODO: Overload OnStreamError for every error type. */
              private:
                /**
                 * Invoked when a message is received on this continuation.
                 */
                void OnStreamEvent(Crt::ScopedResource<OperationResponse> response) override;
                /**
                 * Invoked when a message is received on this continuation but results in an error.
                 *
                 * This callback can return true so that the stream is closed afterwards.
                 */
                bool OnStreamError(Crt::ScopedResource<OperationError> error) override;
            };

            class SubscribeToTopicRequest : OperationRequest
            {
              public:
                SubscribeToTopicRequest(const SubscribeToTopicRequest &) noexcept = default;
                SubscribeToTopicRequest(SubscribeToTopicRequest &&) noexcept = default;
                SubscribeToTopicRequest(
                    const Crt::Optional<Crt::String> &topic,
                    Crt::Allocator *allocator = Crt::g_allocator) noexcept;
                SubscribeToTopicRequest(
                    Crt::Optional<Crt::String> &&topic,
                    Crt::Allocator *allocator = Crt::g_allocator) noexcept;
                void SerializeToJsonObject(Crt::JsonObject &payloadObject) const override;

              protected:
                Crt::String GetModelName() const noexcept override;

              private:
                Crt::Optional<Crt::String> m_topic;
            };

            class SubscribeToTopicResponse : OperationResponse
            {
              public:
                SubscribeToTopicResponse(
                    const Crt::Optional<Crt::String> &topic,
                    Crt::Allocator *allocator = Crt::g_allocator) noexcept;
                SubscribeToTopicResponse(
                    Crt::Optional<Crt::String> &&topic,
                    Crt::Allocator *allocator = Crt::g_allocator) noexcept;
                static Crt::ScopedResource<OperationResponse> s_loadFromPayload(
                    Crt::StringView stringView,
                    Crt::Allocator *allocator) noexcept;
                static void s_customDeleter(SubscribeToTopicResponse *response) noexcept;

              protected:
                Crt::String GetModelName() const noexcept override;

              private:
                Crt::Optional<Crt::String> m_topic;
            };

            class SubscribeToTopicOperation : public ClientOperation
            {
              public:
                SubscribeToTopicOperation(
                    ClientConnection &connection,
                    SubscribeToTopicStreamHandler *m_streamHandler,
                    Crt::Allocator *allocator) noexcept;
                void Activate(
                    const SubscribeToTopicRequest &request,
                    OnMessageFlushCallback onMessageFlushCallback) noexcept;

              protected:
                Crt::String GetModelName() const noexcept override;
            };

            class GreengrassIpcClient
            {
              public:
                GreengrassIpcClient(
                    ClientConnection &&connection,
                    Crt::Allocator *allocator = Crt::g_allocator) noexcept;
                PublishToTopicOperation NewPublishToTopic() noexcept;
                SubscribeToTopicOperation NewSubscribeToTopic(SubscribeToTopicStreamHandler *streamHandler) noexcept;

              private:
                ClientConnection m_connection;
                Crt::Allocator *m_allocator;
                Crt::Map<Crt::String, ExpectedResponseFactory> m_ModelNameToSingleResponseObject;
                Crt::Map<Crt::String, ExpectedResponseFactory> m_ModelNameToStreamingResponseObject;
                Crt::Map<Crt::String, ErrorResponseFactory> m_ErrorNameToObject;
            };
        } // namespace Ipc

    } // namespace Eventstreamrpc
} // namespace Aws