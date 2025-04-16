/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotcommands/IotCommandsClientV2.h>

#include <aws/crt/UUID.h>

#include <aws/iotcommands/CommandExecutionEvent.h>
#include <aws/iotcommands/CommandExecutionsSubscriptionRequest.h>
#include <aws/iotcommands/UpdateCommandExecutionRequest.h>
#include <aws/iotcommands/UpdateCommandExecutionResponse.h>
#include <aws/iotcommands/V2ErrorResponse.h>

namespace Aws
{
    namespace Iotcommands
    {

        class ClientV2 : public IClientV2
        {
          public:
            ClientV2(
                Aws::Crt::Allocator *allocator,
                std::shared_ptr<Aws::Iot::RequestResponse::IMqttRequestResponseClient> bindingClient);
            virtual ~ClientV2() = default;

            bool UpdateCommandExecution(
                const UpdateCommandExecutionRequest &request,
                const UpdateCommandExecutionResultHandler &handler) override;

            std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> CreateCommandExecutionsCborPayloadStream(
                const CommandExecutionsSubscriptionRequest &request,
                const Aws::Iot::RequestResponse::StreamingOperationOptions<CommandExecutionEvent> &options) override;

            std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> CreateCommandExecutionsGenericPayloadStream(
                const CommandExecutionsSubscriptionRequest &request,
                const Aws::Iot::RequestResponse::StreamingOperationOptions<CommandExecutionEvent> &options) override;

            std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> CreateCommandExecutionsJsonPayloadStream(
                const CommandExecutionsSubscriptionRequest &request,
                const Aws::Iot::RequestResponse::StreamingOperationOptions<CommandExecutionEvent> &options) override;

          private:
            Aws::Crt::Allocator *m_allocator;

            std::shared_ptr<Aws::Iot::RequestResponse::IMqttRequestResponseClient> m_bindingClient;
        };

        ClientV2::ClientV2(
            Aws::Crt::Allocator *allocator,
            std::shared_ptr<Aws::Iot::RequestResponse::IMqttRequestResponseClient> bindingClient)
            : m_allocator(allocator), m_bindingClient(std::move(bindingClient))
        {
            // It's simpler to do this than branch the codegen based on the presence of streaming operations
            (void)m_allocator;
        }

        template <typename R, typename E>
        static void s_applyUnmodeledErrorToHandler(const std::function<void(R &&)> &handler, int errorCode)
        {
            ServiceErrorV2<E> error(errorCode);
            R finalResult(std::move(error));
            handler(std::move(finalResult));
        }

        template <typename R, typename E>
        static void s_applyModeledErrorToHandler(const std::function<void(R &&)> &handler, E &&modeledError)
        {
            ServiceErrorV2<E> error(AWS_ERROR_MQTT_REQUEST_RESPONSE_MODELED_SERVICE_ERROR, std::move(modeledError));
            R finalResult(std::move(error));
            handler(std::move(finalResult));
        }

        static void s_UpdateCommandExecutionResponseHandler(
            Aws::Iot::RequestResponse::UnmodeledResult &&result,
            const UpdateCommandExecutionResultHandler &handler,
            const Aws::Crt::String &successPathTopic,
            const Aws::Crt::String &failurePathTopic)
        {
            using E = V2ErrorResponse;
            using R = Aws::Iot::RequestResponse::Result<UpdateCommandExecutionResponse, ServiceErrorV2<E>>;

            if (!result.IsSuccess())
            {
                s_applyUnmodeledErrorToHandler<R, E>(handler, result.GetError());
                return;
            }

            auto response = result.GetResponse();
            const auto &payload = response.GetPayload();
            Aws::Crt::String objectStr(reinterpret_cast<char *>(payload.ptr), payload.len);
            Aws::Crt::JsonObject jsonObject(objectStr);
            if (!jsonObject.WasParseSuccessful())
            {
                s_applyUnmodeledErrorToHandler<R, E>(handler, AWS_ERROR_MQTT_REQUEST_RESPONSE_PAYLOAD_PARSE_ERROR);
                return;
            }

            const auto &topic = response.GetTopic();
            auto responseTopic = Aws::Crt::String((const char *)topic.ptr, topic.len);
            if (responseTopic == successPathTopic)
            {
                UpdateCommandExecutionResponse modeledResponse(jsonObject);
                Aws::Iot::RequestResponse::Result<UpdateCommandExecutionResponse, ServiceErrorV2<E>> finalResult(
                    std::move(modeledResponse));
                handler(std::move(finalResult));
            }
            else if (responseTopic == failurePathTopic)
            {
                V2ErrorResponse modeledError(jsonObject);
                s_applyModeledErrorToHandler<R, E>(handler, std::move(modeledError));
            }
            else
            {
                s_applyUnmodeledErrorToHandler<R, E>(handler, AWS_ERROR_MQTT_REQUEST_RESPONSE_INVALID_RESPONSE_PATH);
            }
        }

        bool ClientV2::UpdateCommandExecution(
            const UpdateCommandExecutionRequest &request,
            const UpdateCommandExecutionResultHandler &handler)
        {
            Aws::Crt::StringStream publishTopicStream;
            publishTopicStream << "$aws/commands/" << DeviceTypeMarshaller::ToString(*request.DeviceType) << "/"
                               << *request.DeviceId << "/executions/" << *request.ExecutionId << "/response/json";
            Aws::Crt::String publishTopic = publishTopicStream.str();

            Aws::Crt::StringStream subscriptionTopicStream0;
            subscriptionTopicStream0 << "$aws/commands/" << DeviceTypeMarshaller::ToString(*request.DeviceType) << "/"
                                     << *request.DeviceId << "/executions/" << *request.ExecutionId
                                     << "/response/accepted/json";
            Aws::Crt::String subscriptionTopic0 = subscriptionTopicStream0.str();

            Aws::Crt::StringStream subscriptionTopicStream1;
            subscriptionTopicStream1 << "$aws/commands/" << DeviceTypeMarshaller::ToString(*request.DeviceType) << "/"
                                     << *request.DeviceId << "/executions/" << *request.ExecutionId
                                     << "/response/rejected/json";
            Aws::Crt::String subscriptionTopic1 = subscriptionTopicStream1.str();

            struct aws_byte_cursor subscriptionTopicFilters[2] = {
                Aws::Crt::ByteCursorFromString(subscriptionTopic0),
                Aws::Crt::ByteCursorFromString(subscriptionTopic1),
            };

            Aws::Crt::StringStream responsePathTopicAcceptedStream;
            responsePathTopicAcceptedStream << "$aws/commands/" << DeviceTypeMarshaller::ToString(*request.DeviceType)
                                            << "/" << *request.DeviceId << "/executions/" << *request.ExecutionId
                                            << "/response/accepted/json";
            Aws::Crt::String responsePathTopicAccepted = responsePathTopicAcceptedStream.str();

            Aws::Crt::StringStream responsePathTopicRejectedStream;
            responsePathTopicRejectedStream << "$aws/commands/" << DeviceTypeMarshaller::ToString(*request.DeviceType)
                                            << "/" << *request.DeviceId << "/executions/" << *request.ExecutionId
                                            << "/response/rejected/json";
            Aws::Crt::String responsePathTopicRejected = responsePathTopicRejectedStream.str();

            struct aws_mqtt_request_operation_response_path responsePaths[2];
            responsePaths[0].topic = Aws::Crt::ByteCursorFromString(responsePathTopicAccepted);
            responsePaths[1].topic = Aws::Crt::ByteCursorFromString(responsePathTopicRejected);
            AWS_ZERO_STRUCT(responsePaths[0].correlation_token_json_path);
            AWS_ZERO_STRUCT(responsePaths[1].correlation_token_json_path);

            Aws::Crt::JsonObject jsonObject;
            request.SerializeToObject(jsonObject);

            Aws::Crt::String outgoingJson = jsonObject.View().WriteCompact(true);

            struct aws_mqtt_request_operation_options options;
            AWS_ZERO_STRUCT(options);
            options.subscription_topic_filters = subscriptionTopicFilters;
            options.subscription_topic_filter_count = 2;
            options.response_paths = responsePaths;
            options.response_path_count = 2;
            options.publish_topic = Aws::Crt::ByteCursorFromString(publishTopic);
            options.serialized_request =
                Aws::Crt::ByteCursorFromArray((uint8_t *)outgoingJson.data(), outgoingJson.length());

            auto resultHandler = [handler, responsePathTopicAccepted, responsePathTopicRejected](
                                     Aws::Iot::RequestResponse::UnmodeledResult &&result)
            {
                s_UpdateCommandExecutionResponseHandler(
                    std::move(result), handler, responsePathTopicAccepted, responsePathTopicRejected);
            };

            int submitResult = m_bindingClient->SubmitRequest(options, std::move(resultHandler));

            return submitResult == AWS_OP_SUCCESS;
        }

        static struct aws_byte_cursor s_getSegmentFromTopic(const Aws::Crt::ByteCursor &topic, int segment)
        {
            struct aws_byte_cursor topicSegment;
            AWS_ZERO_STRUCT(topicSegment);
            int segmentPosition = 0;
            while (aws_byte_cursor_next_split(&topic, '/', &topicSegment))
            {
                if (segmentPosition == segment)
                {
                    return topicSegment;
                }
                ++segmentPosition;
            }
            return {};
        }

        static bool s_initModeledEvent(
            const Aws::Iot::RequestResponse::IncomingPublishEvent &publishEvent,
            CommandExecutionEvent &modeledEvent)
        {
            auto segmentExecutionId = s_getSegmentFromTopic(publishEvent.GetTopic(), 5);
            modeledEvent.SetExecutionId(segmentExecutionId);
            modeledEvent.SetPayload(publishEvent.GetPayload());
            auto contentType = publishEvent.GetContentType();
            if (contentType)
            {
                modeledEvent.SetContentType(*contentType);
            }
            auto messageExpiryIntervalSeconds = publishEvent.GetMessageExpiryIntervalSeconds();
            if (messageExpiryIntervalSeconds)
            {
                modeledEvent.SetTimeout(*messageExpiryIntervalSeconds);
            }
            return true;
        }

        template <typename T> class ServiceStreamingOperation : public Aws::Iot::RequestResponse::IStreamingOperation
        {
          public:
            explicit ServiceStreamingOperation(std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> stream)
                : m_stream(std::move(stream))
            {
            }

            static std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> Create(
                Aws::Crt::Allocator *allocator,
                const std::shared_ptr<Aws::Iot::RequestResponse::IMqttRequestResponseClient> &bindingClient,
                const Aws::Crt::String &subscriptionTopicFilter,
                const Aws::Iot::RequestResponse::StreamingOperationOptions<T> &options)
            {

                std::function<void(Aws::Iot::RequestResponse::IncomingPublishEvent &&)> unmodeledHandler =
                    [options](Aws::Iot::RequestResponse::IncomingPublishEvent &&publishEvent)
                {
                    T modeledEvent;
                    if (!s_initModeledEvent(publishEvent, modeledEvent))
                    {
                        return;
                    }
                    options.GetStreamHandler()(std::move(modeledEvent));
                };

                Aws::Iot::RequestResponse::StreamingOperationOptionsInternal internalOptions;
                internalOptions.subscriptionTopicFilter = Aws::Crt::ByteCursorFromString(subscriptionTopicFilter);
                internalOptions.subscriptionStatusEventHandler = options.GetSubscriptionStatusEventHandler();
                internalOptions.incomingPublishEventHandler = unmodeledHandler;

                auto unmodeledStream = bindingClient->CreateStream(internalOptions);
                if (!unmodeledStream)
                {
                    return nullptr;
                }

                return Aws::Crt::MakeShared<ServiceStreamingOperation<T>>(allocator, unmodeledStream);
            }

            void Open() override { m_stream->Open(); }

          private:
            std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> m_stream;
        };

        std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> ClientV2::
            CreateCommandExecutionsCborPayloadStream(
                const CommandExecutionsSubscriptionRequest &request,
                const Aws::Iot::RequestResponse::StreamingOperationOptions<CommandExecutionEvent> &options)
        {
            Aws::Crt::StringStream topicStream;
            topicStream << "$aws/commands/" << DeviceTypeMarshaller::ToString(*request.DeviceType) << "/"
                        << *request.DeviceId << "/executions/+/request/cbor";
            Aws::Crt::String topic = topicStream.str();

            return ServiceStreamingOperation<CommandExecutionEvent>::Create(
                m_allocator, m_bindingClient, topic, options);
        }

        std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> ClientV2::
            CreateCommandExecutionsGenericPayloadStream(
                const CommandExecutionsSubscriptionRequest &request,
                const Aws::Iot::RequestResponse::StreamingOperationOptions<CommandExecutionEvent> &options)
        {
            Aws::Crt::StringStream topicStream;
            topicStream << "$aws/commands/" << DeviceTypeMarshaller::ToString(*request.DeviceType) << "/"
                        << *request.DeviceId << "/executions/+/request";
            Aws::Crt::String topic = topicStream.str();

            return ServiceStreamingOperation<CommandExecutionEvent>::Create(
                m_allocator, m_bindingClient, topic, options);
        }

        std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> ClientV2::
            CreateCommandExecutionsJsonPayloadStream(
                const CommandExecutionsSubscriptionRequest &request,
                const Aws::Iot::RequestResponse::StreamingOperationOptions<CommandExecutionEvent> &options)
        {
            Aws::Crt::StringStream topicStream;
            topicStream << "$aws/commands/" << DeviceTypeMarshaller::ToString(*request.DeviceType) << "/"
                        << *request.DeviceId << "/executions/+/request/json";
            Aws::Crt::String topic = topicStream.str();

            return ServiceStreamingOperation<CommandExecutionEvent>::Create(
                m_allocator, m_bindingClient, topic, options);
        }

        std::shared_ptr<IClientV2> NewClientFrom5(
            const Aws::Crt::Mqtt5::Mqtt5Client &protocolClient,
            const Aws::Iot::RequestResponse::RequestResponseClientOptions &options,
            Aws::Crt::Allocator *allocator)
        {

            std::shared_ptr<Aws::Iot::RequestResponse::IMqttRequestResponseClient> bindingClient =
                Aws::Iot::RequestResponse::NewClientFrom5(protocolClient, options, allocator);
            if (nullptr == bindingClient)
            {
                return nullptr;
            }

            return Aws::Crt::MakeShared<ClientV2>(allocator, allocator, bindingClient);
        }

        std::shared_ptr<IClientV2> NewClientFrom311(
            const Aws::Crt::Mqtt::MqttConnection &protocolClient,
            const Aws::Iot::RequestResponse::RequestResponseClientOptions &options,
            Aws::Crt::Allocator *allocator)
        {

            std::shared_ptr<Aws::Iot::RequestResponse::IMqttRequestResponseClient> bindingClient =
                Aws::Iot::RequestResponse::NewClientFrom311(protocolClient, options, allocator);
            if (nullptr == bindingClient)
            {
                return nullptr;
            }

            return Aws::Crt::MakeShared<ClientV2>(allocator, allocator, bindingClient);
        }

    } // namespace Iotcommands
} // namespace Aws
