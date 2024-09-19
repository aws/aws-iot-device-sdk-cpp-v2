/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
* SPDX-License-Identifier: Apache-2.0.
*
* This file is generated
*/

#include <aws/iotshadow/IotShadowClientV2.h>

#include <aws/crt/UUID.h>
#include <aws/iotshadow/DeleteNamedShadowRequest.h>
#include <aws/iotshadow/DeleteShadowResponse.h>
#include <aws/iotshadow/ErrorResponse.h>
#include <aws/iotshadow/GetNamedShadowRequest.h>
#include <aws/iotshadow/GetShadowResponse.h>
#include <aws/iotshadow/NamedShadowDeltaUpdatedSubscriptionRequest.h>
#include <aws/iotshadow/NamedShadowUpdatedSubscriptionRequest.h>
#include <aws/iotshadow/ShadowDeltaUpdatedEvent.h>
#include <aws/iotshadow/ShadowUpdatedEvent.h>
#include <aws/iotshadow/UpdateNamedShadowRequest.h>
#include <aws/iotshadow/UpdateShadowResponse.h>

namespace Aws
{

    namespace Iotshadow
    {

        class ClientV2 : public IClientV2
        {
          public:
            ClientV2(Aws::Crt::Allocator *allocator, std::shared_ptr<Aws::Iot::RequestResponse::IMqttRequestResponseClient> bindingClient);
            virtual ~ClientV2() = default;

            bool DeleteNamedShadow(const Aws::Iotshadow::DeleteNamedShadowRequest &request, const std::function<void(DeleteShadowResult &&)> &handler) override;

            bool GetNamedShadow(const Aws::Iotshadow::GetNamedShadowRequest &request, const std::function<void(GetShadowResult &&)> &handler) override;

            bool UpdateNamedShadow(const Aws::Iotshadow::UpdateNamedShadowRequest &request, const std::function<void(UpdateShadowResult &&)> &handler) override;

            std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> CreateNamedShadowDeltaUpdatedStream(const Aws::Iotshadow::NamedShadowDeltaUpdatedSubscriptionRequest &request, const Aws::Iot::RequestResponse::StreamingOperationOptions<ShadowDeltaUpdatedEvent> &options) override;

            std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> CreateNamedShadowUpdatedStream(const Aws::Iotshadow::NamedShadowUpdatedSubscriptionRequest &request, const Aws::Iot::RequestResponse::StreamingOperationOptions<ShadowUpdatedEvent> &options) override;

          private:

            Aws::Crt::Allocator *m_allocator;

            std::shared_ptr<Aws::Iot::RequestResponse::IMqttRequestResponseClient> m_bindingClient;
        };

        ClientV2::ClientV2(Aws::Crt::Allocator *allocator, std::shared_ptr<Aws::Iot::RequestResponse::IMqttRequestResponseClient> bindingClient) :
            m_allocator(allocator),
            m_bindingClient(std::move(bindingClient))
        {
        }

        template<typename R>
        static void s_applyUnmodeledErrorToHandler(const std::function<void(R &&)> &handler, int errorCode) {
            ServiceErrorV2<ErrorResponse> error(errorCode);
            R finalResult(std::move(error));
            handler(std::move(finalResult));
        }

        template<typename R>
        static void s_applyModeledErrorToHandler(const std::function<void(R &&)> &handler, ErrorResponse &&modeledError) {
            ServiceErrorV2<ErrorResponse> error(AWS_ERROR_MQTT_REQUEST_RESPONSE_INTERNAL_ERROR, std::move(modeledError));  // TBI: MQTT Service error
            R finalResult(std::move(error));
            handler(std::move(finalResult));
        }

        static void s_DeleteNamedShadowResponseHandler(Aws::Iot::RequestResponse::UnmodeledResult &&result, const std::function<void(DeleteShadowResult &&)> &handler, const Aws::Crt::String &successPathTopic, const Aws::Crt::String &failurePathTopic) {
            if (!result.IsSuccess()) {
                s_applyUnmodeledErrorToHandler(handler, result.GetError());
                return;
            }

            auto response = result.GetResponse();
            const auto &payload = response.GetPayload();
            Aws::Crt::String objectStr(reinterpret_cast<char *>(payload.ptr), payload.len);
            Aws::Crt::JsonObject jsonObject(objectStr);
            if (!jsonObject.WasParseSuccessful()) {
                s_applyUnmodeledErrorToHandler(handler, AWS_ERROR_MQTT_REQUEST_RESPONSE_INTERNAL_ERROR);  // TBI: Payload Parse failure
                return;
            }

            const auto &topic = response.GetTopic();
            auto responseTopic = Aws::Crt::String((const char *)topic.ptr, topic.len);
            if (responseTopic == successPathTopic) {
                Aws::Iotshadow::DeleteShadowResponse modeledResponse(jsonObject);
                Aws::Iotshadow::DeleteShadowResult finalResult(std::move(modeledResponse));
                handler(std::move(finalResult));
            } else if (responseTopic == failurePathTopic) {
                Aws::Iotshadow::ErrorResponse modeledError(jsonObject);
                s_applyModeledErrorToHandler(handler, std::move(modeledError));
            } else {
                s_applyUnmodeledErrorToHandler(handler, AWS_ERROR_MQTT_REQUEST_RESPONSE_INTERNAL_ERROR); // TBI: invalid response path
            }
        }

        bool ClientV2::DeleteNamedShadow(const Aws::Iotshadow::DeleteNamedShadowRequest &request, const std::function<void(DeleteShadowResult &&)> &handler)
        {
            Aws::Crt::StringStream publishTopicStream;
            publishTopicStream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName
                               << "/delete";
            Aws::Crt::String publishTopic = publishTopicStream.str();

            Aws::Crt::StringStream subscriptionTopicStream;
            subscriptionTopicStream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName
                                    << "/delete/+";
            Aws::Crt::String subscriptionTopic = subscriptionTopicStream.str();

            struct aws_byte_cursor subscriptionTopicFilters[1] = {Aws::Crt::ByteCursorFromString(subscriptionTopic),};

            Aws::Crt::StringStream responsePathTopic1Stream;
            responsePathTopic1Stream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName
                                     << "/delete/accepted";
            Aws::Crt::String responsePathTopic1 = responsePathTopic1Stream.str();

            Aws::Crt::StringStream responsePathTopic2Stream;
            responsePathTopic2Stream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName
                                     << "/delete/rejected";
            Aws::Crt::String responsePathTopic2 = responsePathTopic2Stream.str();

            struct aws_mqtt_request_operation_response_path responsePaths[2];
            responsePaths[0].topic = Aws::Crt::ByteCursorFromString(responsePathTopic1);
            responsePaths[0].correlation_token_json_path = Aws::Crt::ByteCursorFromCString("clientToken");
            responsePaths[1].topic = Aws::Crt::ByteCursorFromString(responsePathTopic2);
            responsePaths[1].correlation_token_json_path = Aws::Crt::ByteCursorFromCString("clientToken");

            Aws::Crt::JsonObject jsonObject;
            request.SerializeToObject(jsonObject);

            auto uuid = Aws::Crt::UUID().ToString();
            jsonObject.WithString("clientToken", uuid);

            Aws::Crt::String outgoingJson = jsonObject.View().WriteCompact(true);

            struct aws_mqtt_request_operation_options options;
            AWS_ZERO_STRUCT(options);
            options.subscription_topic_filters = subscriptionTopicFilters;
            options.subscription_topic_filter_count = 1;
            options.response_paths = responsePaths;
            options.response_path_count = 2;
            options.publish_topic = Aws::Crt::ByteCursorFromString(publishTopic);
            options.serialized_request =
                Aws::Crt::ByteCursorFromArray((uint8_t *)outgoingJson.data(), outgoingJson.length());
            options.correlation_token = Aws::Crt::ByteCursorFromString(uuid);

            auto resultHandler =
                [handler, responsePathTopic1, responsePathTopic2](Aws::Iot::RequestResponse::UnmodeledResult &&result)
            { s_DeleteNamedShadowResponseHandler(std::move(result), handler, responsePathTopic1, responsePathTopic2); };

            int submitResult = m_bindingClient->SubmitRequest(options, std::move(resultHandler));

            return submitResult == AWS_OP_SUCCESS;
        }

        static void s_GetNamedShadowResponseHandler(Aws::Iot::RequestResponse::UnmodeledResult &&result, const std::function<void(GetShadowResult &&)> &handler, const Aws::Crt::String &successPathTopic, const Aws::Crt::String &failurePathTopic) {
            if (!result.IsSuccess()) {
                s_applyUnmodeledErrorToHandler(handler, result.GetError());
                return;
            }

            auto response = result.GetResponse();
            const auto &payload = response.GetPayload();
            Aws::Crt::String objectStr(reinterpret_cast<char *>(payload.ptr), payload.len);
            Aws::Crt::JsonObject jsonObject(objectStr);
            if (!jsonObject.WasParseSuccessful()) {
                s_applyUnmodeledErrorToHandler(handler, AWS_ERROR_MQTT_REQUEST_RESPONSE_INTERNAL_ERROR);  // TBI: Payload Parse failure
                return;
            }

            const auto &topic = response.GetTopic();
            auto responseTopic = Aws::Crt::String((const char *)topic.ptr, topic.len);
            if (responseTopic == successPathTopic) {
                Aws::Iotshadow::GetShadowResponse modeledResponse(jsonObject);
                Aws::Iotshadow::GetShadowResult finalResult(std::move(modeledResponse));
                handler(std::move(finalResult));
            } else if (responseTopic == failurePathTopic) {
                Aws::Iotshadow::ErrorResponse modeledError(jsonObject);
                s_applyModeledErrorToHandler(handler, std::move(modeledError));
            } else {
                s_applyUnmodeledErrorToHandler(handler, AWS_ERROR_MQTT_REQUEST_RESPONSE_INTERNAL_ERROR); // TBI: invalid response path
            }
        }

        bool ClientV2::GetNamedShadow(const Aws::Iotshadow::GetNamedShadowRequest &request, const std::function<void(GetShadowResult &&)> &handler) {
            Aws::Crt::StringStream publishTopicStream;
            publishTopicStream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName << "/get";
            Aws::Crt::String publishTopic = publishTopicStream.str();

            Aws::Crt::StringStream subscriptionTopicStream;
            subscriptionTopicStream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName << "/get/+";
            Aws::Crt::String subscriptionTopic = subscriptionTopicStream.str();

            struct aws_byte_cursor subscriptionTopicFilters[1] = {
                Aws::Crt::ByteCursorFromString(subscriptionTopic),
            };

            Aws::Crt::StringStream responsePathTopic1Stream;
            responsePathTopic1Stream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName << "/get/accepted";
            Aws::Crt::String responsePathTopic1 = responsePathTopic1Stream.str();

            Aws::Crt::StringStream responsePathTopic2Stream;
            responsePathTopic2Stream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName << "/get/rejected";
            Aws::Crt::String responsePathTopic2 = responsePathTopic2Stream.str();

            struct aws_mqtt_request_operation_response_path responsePaths[2];
            responsePaths[0].topic = Aws::Crt::ByteCursorFromString(responsePathTopic1);
            responsePaths[0].correlation_token_json_path = Aws::Crt::ByteCursorFromCString("clientToken");
            responsePaths[1].topic = Aws::Crt::ByteCursorFromString(responsePathTopic2);
            responsePaths[1].correlation_token_json_path = Aws::Crt::ByteCursorFromCString("clientToken");

            Aws::Crt::JsonObject jsonObject;
            request.SerializeToObject(jsonObject);

            auto uuid = Aws::Crt::UUID().ToString();
            jsonObject.WithString("clientToken", uuid);

            Aws::Crt::String outgoingJson = jsonObject.View().WriteCompact(true);

            struct aws_mqtt_request_operation_options options;
            AWS_ZERO_STRUCT(options);
            options.subscription_topic_filters = subscriptionTopicFilters;
            options.subscription_topic_filter_count = 1;
            options.response_paths = responsePaths;
            options.response_path_count = 2;
            options.publish_topic = Aws::Crt::ByteCursorFromString(publishTopic);
            options.serialized_request = Aws::Crt::ByteCursorFromArray((uint8_t *)outgoingJson.data(), outgoingJson.length());
            options.correlation_token = Aws::Crt::ByteCursorFromString(uuid);

            auto resultHandler = [handler, responsePathTopic1, responsePathTopic2](Aws::Iot::RequestResponse::UnmodeledResult &&result){
                s_GetNamedShadowResponseHandler(std::move(result), handler, responsePathTopic1, responsePathTopic2);
            };

            int submitResult = m_bindingClient->SubmitRequest(options, std::move(resultHandler));

            return submitResult == AWS_OP_SUCCESS;
        }

        static void s_UpdateNamedShadowResponseHandler(Aws::Iot::RequestResponse::UnmodeledResult &&result, const std::function<void(UpdateShadowResult &&)> &handler, const Aws::Crt::String &successPathTopic, const Aws::Crt::String &failurePathTopic) {
            if (!result.IsSuccess()) {
                s_applyUnmodeledErrorToHandler(handler, result.GetError());
                return;
            }

            auto response = result.GetResponse();
            const auto &payload = response.GetPayload();
            Aws::Crt::String objectStr(reinterpret_cast<char *>(payload.ptr), payload.len);
            Aws::Crt::JsonObject jsonObject(objectStr);
            if (!jsonObject.WasParseSuccessful()) {
                s_applyUnmodeledErrorToHandler(handler, AWS_ERROR_MQTT_REQUEST_RESPONSE_INTERNAL_ERROR);  // TBI: Payload Parse failure
                return;
            }

            const auto &topic = response.GetTopic();
            auto responseTopic = Aws::Crt::String((const char *)topic.ptr, topic.len);
            if (responseTopic == successPathTopic) {
                Aws::Iotshadow::UpdateShadowResponse modeledResponse(jsonObject);
                Aws::Iotshadow::UpdateShadowResult finalResult(std::move(modeledResponse));
                handler(std::move(finalResult));
            } else if (responseTopic == failurePathTopic) {
                Aws::Iotshadow::ErrorResponse modeledError(jsonObject);
                s_applyModeledErrorToHandler(handler, std::move(modeledError));
            } else {
                s_applyUnmodeledErrorToHandler(handler, AWS_ERROR_MQTT_REQUEST_RESPONSE_INTERNAL_ERROR); // TBI: invalid response path
            }
        }

        bool ClientV2::UpdateNamedShadow(const Aws::Iotshadow::UpdateNamedShadowRequest &request, const std::function<void(UpdateShadowResult &&)> &handler) {
            Aws::Crt::StringStream publishTopicStream;
            publishTopicStream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName << "/update";
            Aws::Crt::String publishTopic = publishTopicStream.str();

            Aws::Crt::StringStream subscriptionTopic1Stream;
            subscriptionTopic1Stream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName << "/update/accepted";
            Aws::Crt::String subscriptionTopic1 = subscriptionTopic1Stream.str();

            Aws::Crt::StringStream subscriptionTopic2Stream;
            subscriptionTopic2Stream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName << "/update/rejected";
            Aws::Crt::String subscriptionTopic2 = subscriptionTopic2Stream.str();

            struct aws_byte_cursor subscriptionTopicFilters[2] = {
                Aws::Crt::ByteCursorFromString(subscriptionTopic1),
                Aws::Crt::ByteCursorFromString(subscriptionTopic2),
            };

            Aws::Crt::StringStream responsePathTopic1Stream;
            responsePathTopic1Stream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName << "/update/accepted";
            Aws::Crt::String responsePathTopic1 = responsePathTopic1Stream.str();

            Aws::Crt::StringStream responsePathTopic2Stream;
            responsePathTopic2Stream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName << "/update/rejected";
            Aws::Crt::String responsePathTopic2 = responsePathTopic2Stream.str();

            struct aws_mqtt_request_operation_response_path responsePaths[2];
            responsePaths[0].topic = Aws::Crt::ByteCursorFromString(responsePathTopic1);
            responsePaths[0].correlation_token_json_path = Aws::Crt::ByteCursorFromCString("clientToken");
            responsePaths[1].topic = Aws::Crt::ByteCursorFromString(responsePathTopic2);
            responsePaths[1].correlation_token_json_path = Aws::Crt::ByteCursorFromCString("clientToken");

            Aws::Crt::JsonObject jsonObject;
            request.SerializeToObject(jsonObject);

            auto uuid = Aws::Crt::UUID().ToString();
            jsonObject.WithString("clientToken", uuid);

            Aws::Crt::String outgoingJson = jsonObject.View().WriteCompact(true);

            struct aws_mqtt_request_operation_options options;
            AWS_ZERO_STRUCT(options);
            options.subscription_topic_filters = subscriptionTopicFilters;
            options.subscription_topic_filter_count = 2;
            options.response_paths = responsePaths;
            options.response_path_count = 2;
            options.publish_topic = Aws::Crt::ByteCursorFromString(publishTopic);
            options.serialized_request = Aws::Crt::ByteCursorFromArray((uint8_t *)outgoingJson.data(), outgoingJson.length());
            options.correlation_token = Aws::Crt::ByteCursorFromString(uuid);

            auto resultHandler = [handler, responsePathTopic1, responsePathTopic2](Aws::Iot::RequestResponse::UnmodeledResult &&result){
                s_UpdateNamedShadowResponseHandler(std::move(result), handler, responsePathTopic1, responsePathTopic2);
            };

            int submitResult = m_bindingClient->SubmitRequest(options, std::move(resultHandler));

            return submitResult == AWS_OP_SUCCESS;
        }

        template<typename T> class ShadowStreamingOperation : public Aws::Iot::RequestResponse::IStreamingOperation {
          public:

            explicit ShadowStreamingOperation(std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> stream) :
                  m_stream(std::move(stream))
            {
            }

            static std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> Create(Aws::Crt::Allocator *allocator, const std::shared_ptr<Aws::Iot::RequestResponse::IMqttRequestResponseClient> &bindingClient, const Aws::Crt::String &subscriptionTopicFilter, const Aws::Iot::RequestResponse::StreamingOperationOptions<T> &options) {

                std::function<void(Aws::Iot::RequestResponse::IncomingPublishEvent &&)> unmodeledHandler = [options](Aws::Iot::RequestResponse::IncomingPublishEvent &&event){
                    const auto &payload = event.GetPayload();
                    Aws::Crt::String objectStr(reinterpret_cast<char *>(payload.ptr), payload.len);
                    Aws::Crt::JsonObject jsonObject(objectStr);
                    if (!jsonObject.WasParseSuccessful()) {
                        return;
                    }

                    T modeledEvent(jsonObject);
                    options.GetStreamHandler()(std::move(modeledEvent));
                };

                Aws::Iot::RequestResponse::StreamingOperationOptionsInternal internalOptions;
                internalOptions.subscriptionTopicFilter = Aws::Crt::ByteCursorFromString(subscriptionTopicFilter);
                internalOptions.subscriptionStatusEventHandler = options.GetSubscriptionStatusEventHandler();
                internalOptions.incomingPublishEventHandler = unmodeledHandler;

                auto unmodeledStream = bindingClient->CreateStream(internalOptions);
                if (!unmodeledStream) {
                    return nullptr;
                }

                return Aws::Crt::MakeShared<ShadowStreamingOperation>(allocator, unmodeledStream);
            }

            void Open() override {
                m_stream->Open();
            }

          private:

            std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> m_stream;
        };

        std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> ClientV2::CreateNamedShadowDeltaUpdatedStream(const Aws::Iotshadow::NamedShadowDeltaUpdatedSubscriptionRequest &request, const Aws::Iot::RequestResponse::StreamingOperationOptions<ShadowDeltaUpdatedEvent> &options) {
            Aws::Crt::StringStream topicStream;
            topicStream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName << "/update/delta";
            Aws::Crt::String topic = topicStream.str();

            return ShadowStreamingOperation<ShadowDeltaUpdatedEvent>::Create(m_allocator, m_bindingClient, topic, options);
        }

        std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> ClientV2::CreateNamedShadowUpdatedStream(const Aws::Iotshadow::NamedShadowUpdatedSubscriptionRequest &request, const Aws::Iot::RequestResponse::StreamingOperationOptions<ShadowUpdatedEvent> &options) {
            Aws::Crt::StringStream topicStream;
            topicStream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName << "/update/documents";
            Aws::Crt::String topic = topicStream.str();

            return ShadowStreamingOperation<ShadowUpdatedEvent>::Create(m_allocator, m_bindingClient, topic, options);
        }

        std::shared_ptr<IClientV2> NewClientFrom5(const Aws::Crt::Mqtt5::Mqtt5Client &protocolClient,
                                   const Aws::Iot::RequestResponse::RequestResponseClientOptions &options,
                                   Aws::Crt::Allocator *allocator) {

            std::shared_ptr<Aws::Iot::RequestResponse::IMqttRequestResponseClient> bindingClient = Aws::Iot::RequestResponse::NewClientFrom5(protocolClient, options, allocator);
            if (nullptr == bindingClient) {
                return nullptr;
            }

            return Aws::Crt::MakeShared<ClientV2>(allocator, allocator, bindingClient);
        }

        std::shared_ptr<IClientV2> NewClientFrom311(const Aws::Crt::Mqtt::MqttConnection &protocolClient,
                                     const Aws::Iot::RequestResponse::RequestResponseClientOptions &options,
                                     Aws::Crt::Allocator *allocator) {

            std::shared_ptr<Aws::Iot::RequestResponse::IMqttRequestResponseClient> bindingClient = Aws::Iot::RequestResponse::NewClientFrom311(protocolClient, options, allocator);
            if (nullptr == bindingClient) {
                return nullptr;
            }

            return Aws::Crt::MakeShared<ClientV2>(allocator, allocator, bindingClient);
        }

    }
}