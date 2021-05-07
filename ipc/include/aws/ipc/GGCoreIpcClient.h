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
                BinaryMessage(const Crt::Optional<Crt::Vector<uint8_t>> &message) noexcept;
                BinaryMessage(Crt::Optional<Crt::Vector<uint8_t>> &&message) noexcept;
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
                JsonMessage(Crt::Optional<Crt::JsonObject> &&jsonObject) noexcept;
                JsonMessage(const Crt::Optional<Crt::JsonObject> &jsonObject) noexcept;
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
                PublishMessage(
                    const Crt::Optional<JsonMessage> &jsonMessage,
                    const Crt::Optional<BinaryMessage> &optionalMessage) noexcept;
                PublishMessage(
                    Crt::Optional<JsonMessage> &&jsonMessage,
                    Crt::Optional<BinaryMessage> &&optionalMessage) noexcept;
                void SerializeToJsonObject(Crt::JsonObject &payloadObject) const override;

              private:
                Crt::Optional<JsonMessage> m_jsonMessage;
                Crt::Optional<BinaryMessage> m_optionalMessage;
            };
            class PublishToTopicRequest : public OperationRequest
            {
              public:
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
                PublishToTopicResponse() noexcept;
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
                void Activate(
                    const PublishToTopicRequest &request,
                    OnMessageFlushCallback onMessageFlushCallback) noexcept;

              protected:
                Crt::String GetModelName() const noexcept override;
            };
        } // namespace Ipc

    } // namespace Eventstreamrpc
} // namespace Aws