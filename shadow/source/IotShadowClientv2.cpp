/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
* SPDX-License-Identifier: Apache-2.0.
*
* This file is generated
*/

#include <aws/iotshadow/IotShadowClientv2.h>

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

        class Clientv2 : public IClientV2
        {
          public:

            Clientv2(Aws::Crt::Allocator *allocator, Aws::Iot::RequestResponse::IMqttRequestResponseClient *bindingClient);
            virtual ~Clientv2() = default;

            virtual bool DeleteNamedShadow(const Aws::Iotshadow::DeleteNamedShadowRequest &request, const std::function<void(DeleteShadowResult &&)> &handler);

            virtual bool GetNamedShadow(const Aws::Iotshadow::GetNamedShadowRequest &request, const std::function<void(GetShadowResult &&)> &handler);

            virtual bool UpdateNamedShadow(const Aws::Iotshadow::UpdateNamedShadowRequest &request, const std::function<void(UpdateShadowResult &&)> &handler);

            virtual std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> createNamedShadowDeltaUpdatedStream(const Aws::Iotshadow::NamedShadowDeltaUpdatedSubscriptionRequest &request, const Aws::Iot::RequestResponse::StreamingOperationOptions &options, const std::function<void(ShadowDeltaUpdatedEvent &&)> &handler);

            virtual std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> createNamedShadowUpdatedStream(const Aws::Iotshadow::NamedShadowUpdatedSubscriptionRequest &request, const Aws::Iot::RequestResponse::StreamingOperationOptions &options, const std::function<void(ShadowUpdatedEvent &&)> &handler);

          private:

            Aws::Crt::Allocator *m_allocator;

            Aws::Crt::ScopedResource<Aws::Iot::RequestResponse::IMqttRequestResponseClient> m_bindingClient;
        };

        Clientv2::Clientv2(Aws::Crt::Allocator *allocator, Aws::Iot::RequestResponse::IMqttRequestResponseClient *bindingClient) :
            m_allocator(allocator),
            m_bindingClient(bindingClient, [allocator](Aws::Iot::RequestResponse::IMqttRequestResponseClient *bindingClient) { Aws::Crt::Delete(bindingClient, allocator); })
        {
        }

        static void s_applyUnmodeledErrorToDeleteNamedShadowHandler(const std::function<void(DeleteShadowResult &&)> &handler, int errorCode) {
            ServiceErrorV2<ErrorResponse> error(errorCode);
            Aws::Iotshadow::DeleteShadowResult finalResult(std::move(error));
            handler(std::move(finalResult));
        }

        static void s_DeleteNamedShadowResponseHandler(Aws::Iot::RequestResponse::UnmodeledResult &&result, const std::function<void(DeleteShadowResult &&)> &handler, const Aws::Crt::String &successPathTopic, const Aws::Crt::String &failurePathTopic) {
            if (!result.isSuccess()) {
                s_applyUnmodeledErrorToDeleteNamedShadowHandler(handler, result.getError());
                return;
            }

            auto response = result.getResponse();
            Aws::Crt::String objectStr(reinterpret_cast<char *>(response.payload.ptr), response.payload.len);
            Aws::Crt::JsonObject jsonObject(objectStr);
            if (!jsonObject.WasParseSuccessful()) {
                s_applyUnmodeledErrorToDeleteNamedShadowHandler(handler, AWS_ERROR_MQTT_REQUEST_RESPONSE_INTERNAL_ERROR);  // TBI: Payload Parse failure
                return;
            }

            auto responseTopic = Aws::Crt::String((const char *)response.topic.ptr, response.topic.len);
            if (responseTopic == successPathTopic) {
                Aws::Iotshadow::DeleteShadowResponse modeledResponse(jsonObject);
                Aws::Iotshadow::DeleteShadowResult finalResult(std::move(modeledResponse));
                handler(std::move(finalResult));
            } else if (responseTopic == failurePathTopic) {
                Aws::Iotshadow::ErrorResponse modeledResponse(jsonObject);

                Aws::Iotshadow::ServiceErrorV2<ErrorResponse> modeledError(AWS_ERROR_MQTT_REQUEST_RESPONSE_INTERNAL_ERROR, std::move(modeledResponse));  // TBI: MQTT Service error
                Aws::Iotshadow::DeleteShadowResult finalResult(std::move(modeledError));
                handler(std::move(finalResult));
            } else {
                s_applyUnmodeledErrorToDeleteNamedShadowHandler(handler, AWS_ERROR_MQTT_REQUEST_RESPONSE_INTERNAL_ERROR); // TBI: invalid response path
            }
        }

        bool Clientv2::DeleteNamedShadow(const Aws::Iotshadow::DeleteNamedShadowRequest &request, const std::function<void(DeleteShadowResult &&)> &handler) {
            Aws::Crt::StringStream publishTopicStream;
            publishTopicStream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName << "/delete";
            Aws::Crt::String publishTopic = publishTopicStream.str();

            Aws::Crt::StringStream subscriptionTopicStream;
            subscriptionTopicStream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName << "/delete/+";
            Aws::Crt::String subscriptionTopic = subscriptionTopicStream.str();

            struct aws_byte_cursor subscriptionTopicFilters[1] = {
                Aws::Crt::ByteCursorFromString(subscriptionTopic)
            };

            Aws::Crt::StringStream responsePathTopic1Stream;
            responsePathTopic1Stream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName << "/delete/accepted";
            Aws::Crt::String responsePathTopic1 = responsePathTopic1Stream.str();

            Aws::Crt::StringStream responsePathTopic2Stream;
            responsePathTopic2Stream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName << "/delete/rejected";
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
                s_DeleteNamedShadowResponseHandler(std::move(result), handler, responsePathTopic1, responsePathTopic2);
            };

            int submitResult = m_bindingClient->submitRequest(options, std::move(resultHandler));

            return submitResult == AWS_OP_SUCCESS;
        }

        static void s_applyUnmodeledErrorToGetNamedShadowHandler(const std::function<void(GetShadowResult &&)> &handler, int errorCode) {
            ServiceErrorV2<ErrorResponse> error(errorCode);
            Aws::Iotshadow::GetShadowResult finalResult(std::move(error));
            handler(std::move(finalResult));
        }

        static void s_GetNamedShadowResponseHandler(Aws::Iot::RequestResponse::UnmodeledResult &&result, const std::function<void(GetShadowResult &&)> &handler, const Aws::Crt::String &successPathTopic, const Aws::Crt::String &failurePathTopic) {
            if (!result.isSuccess()) {
                s_applyUnmodeledErrorToGetNamedShadowHandler(handler, result.getError());
                return;
            }

            auto response = result.getResponse();
            Aws::Crt::String objectStr(reinterpret_cast<char *>(response.payload.ptr), response.payload.len);
            Aws::Crt::JsonObject jsonObject(objectStr);
            if (!jsonObject.WasParseSuccessful()) {
                s_applyUnmodeledErrorToGetNamedShadowHandler(handler, AWS_ERROR_MQTT_REQUEST_RESPONSE_INTERNAL_ERROR);  // TBI: Payload Parse failure
                return;
            }

            auto responseTopic = Aws::Crt::String((const char *)response.topic.ptr, response.topic.len);
            if (responseTopic == successPathTopic) {
                Aws::Iotshadow::GetShadowResponse modeledResponse(jsonObject);
                Aws::Iotshadow::GetShadowResult finalResult(std::move(modeledResponse));
                handler(std::move(finalResult));
            } else if (responseTopic == failurePathTopic) {
                Aws::Iotshadow::ErrorResponse modeledResponse(jsonObject);

                Aws::Iotshadow::ServiceErrorV2<ErrorResponse> modeledError(AWS_ERROR_MQTT_REQUEST_RESPONSE_INTERNAL_ERROR, std::move(modeledResponse));  // TBI: MQTT Service error
                Aws::Iotshadow::GetShadowResult finalResult(std::move(modeledError));
                handler(std::move(finalResult));
            } else {
                s_applyUnmodeledErrorToGetNamedShadowHandler(handler, AWS_ERROR_MQTT_REQUEST_RESPONSE_INTERNAL_ERROR); // TBI: invalid response path
            }
        }

        bool Clientv2::GetNamedShadow(const Aws::Iotshadow::GetNamedShadowRequest &request, const std::function<void(GetShadowResult &&)> &handler) {
            Aws::Crt::StringStream publishTopicStream;
            publishTopicStream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName << "/get";
            Aws::Crt::String publishTopic = publishTopicStream.str();

            Aws::Crt::StringStream subscriptionTopicStream;
            subscriptionTopicStream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName << "/get/+";
            Aws::Crt::String subscriptionTopic = subscriptionTopicStream.str();

            struct aws_byte_cursor subscriptionTopicFilters[1] = {
                Aws::Crt::ByteCursorFromString(subscriptionTopic)
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

            int submitResult = m_bindingClient->submitRequest(options, std::move(resultHandler));

            return submitResult == AWS_OP_SUCCESS;
        }

        static void s_applyUnmodeledErrorToUpdateNamedShadowHandler(const std::function<void(UpdateShadowResult &&)> &handler, int errorCode) {
            ServiceErrorV2<ErrorResponse> error(errorCode);
            Aws::Iotshadow::UpdateShadowResult finalResult(std::move(error));
            handler(std::move(finalResult));
        }

        static void s_UpdateNamedShadowResponseHandler(Aws::Iot::RequestResponse::UnmodeledResult &&result, const std::function<void(UpdateShadowResult &&)> &handler, const Aws::Crt::String &successPathTopic, const Aws::Crt::String &failurePathTopic) {
            if (!result.isSuccess()) {
                s_applyUnmodeledErrorToUpdateNamedShadowHandler(handler, result.getError());
                return;
            }

            auto response = result.getResponse();
            Aws::Crt::String objectStr(reinterpret_cast<char *>(response.payload.ptr), response.payload.len);
            Aws::Crt::JsonObject jsonObject(objectStr);
            if (!jsonObject.WasParseSuccessful()) {
                s_applyUnmodeledErrorToUpdateNamedShadowHandler(handler, AWS_ERROR_MQTT_REQUEST_RESPONSE_INTERNAL_ERROR);  // TBI: Payload Parse failure
                return;
            }

            auto responseTopic = Aws::Crt::String((const char *)response.topic.ptr, response.topic.len);
            if (responseTopic == successPathTopic) {
                Aws::Iotshadow::UpdateShadowResponse modeledResponse(jsonObject);
                Aws::Iotshadow::UpdateShadowResult finalResult(std::move(modeledResponse));
                handler(std::move(finalResult));
            } else if (responseTopic == failurePathTopic) {
                Aws::Iotshadow::ErrorResponse modeledResponse(jsonObject);

                Aws::Iotshadow::ServiceErrorV2<ErrorResponse> modeledError(AWS_ERROR_MQTT_REQUEST_RESPONSE_INTERNAL_ERROR, std::move(modeledResponse));  // TBI: MQTT Service error
                Aws::Iotshadow::UpdateShadowResult finalResult(std::move(modeledError));
                handler(std::move(finalResult));
            } else {
                s_applyUnmodeledErrorToUpdateNamedShadowHandler(handler, AWS_ERROR_MQTT_REQUEST_RESPONSE_INTERNAL_ERROR); // TBI: invalid response path
            }
        }

        bool Clientv2::UpdateNamedShadow(const Aws::Iotshadow::UpdateNamedShadowRequest &request, const std::function<void(UpdateShadowResult &&)> &handler) {
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
                Aws::Crt::ByteCursorFromString(subscriptionTopic2)
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

            int submitResult = m_bindingClient->submitRequest(options, std::move(resultHandler));

            return submitResult == AWS_OP_SUCCESS;
        }

        template<typename T> class ShadowStreamingOperation : public Aws::Iot::RequestResponse::IStreamingOperation {
          public:

            explicit ShadowStreamingOperation(std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> stream) :
                  m_stream(std::move(stream))
            {
            }

            static std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> create(Aws::Crt::Allocator *allocator, Aws::Iot::RequestResponse::IMqttRequestResponseClient *bindingClient, const Aws::Crt::String &subscriptionTopicFilter, const Aws::Iot::RequestResponse::StreamingOperationOptions &options, const std::function<void(T &&)> &handler) {

                std::function<void(Aws::Iot::RequestResponse::IncomingPublishEvent &&)> unmodeledHandler = [handler](Aws::Iot::RequestResponse::IncomingPublishEvent &&event){
                    Aws::Crt::String objectStr(reinterpret_cast<char *>(event.payload.ptr), event.payload.len);
                    Aws::Crt::JsonObject jsonObject(objectStr);
                    if (!jsonObject.WasParseSuccessful()) {
                        return;
                    }

                    T modeledEvent(jsonObject);
                    handler(std::move(modeledEvent));
                };

                Aws::Iot::RequestResponse::StreamingOperationOptionsInternal internalOptions;
                internalOptions.subscriptionTopicFilter = Aws::Crt::ByteCursorFromString(subscriptionTopicFilter);
                internalOptions.subscriptionStatusEventHandler = options.subscriptionStatusEventHandler;
                internalOptions.incomingPublishEventHandler = unmodeledHandler;

                auto unmodeledStream = bindingClient->createStream(internalOptions);
                if (!unmodeledStream) {
                    return nullptr;
                }

                return Aws::Crt::MakeShared<ShadowStreamingOperation>(allocator, unmodeledStream);
            }

            virtual void activate() {
                m_stream->activate();
            }

          private:

            std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> m_stream;
        };

        std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> Clientv2::createNamedShadowDeltaUpdatedStream(const Aws::Iotshadow::NamedShadowDeltaUpdatedSubscriptionRequest &request, const Aws::Iot::RequestResponse::StreamingOperationOptions &options, const std::function<void(ShadowDeltaUpdatedEvent &&)> &handler) {
            Aws::Crt::StringStream topicStream;
            topicStream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName << "/update/delta";
            Aws::Crt::String topic = topicStream.str();

            return ShadowStreamingOperation<ShadowDeltaUpdatedEvent>::create(m_allocator, m_bindingClient.get(), topic, options, handler);
        }

        std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> Clientv2::createNamedShadowUpdatedStream(const Aws::Iotshadow::NamedShadowUpdatedSubscriptionRequest &request, const Aws::Iot::RequestResponse::StreamingOperationOptions &options, const std::function<void(ShadowUpdatedEvent &&)> &handler) {
            Aws::Crt::StringStream topicStream;
            topicStream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName << "/update/documents";
            Aws::Crt::String topic = topicStream.str();

            return ShadowStreamingOperation<ShadowUpdatedEvent>::create(m_allocator, m_bindingClient.get(), topic, options, handler);
        }

        std::shared_ptr<IClientV2> IClientV2::newFrom5(const Aws::Crt::Mqtt5::Mqtt5Client &protocolClient,
                                   const Aws::Iot::RequestResponse::RequestResponseClientOptions &options,
                                   Aws::Crt::Allocator *allocator) {

            Aws::Iot::RequestResponse::IMqttRequestResponseClient *bindingClient = Aws::Iot::RequestResponse::IMqttRequestResponseClient::newFrom5(protocolClient, options, allocator);
            if (!bindingClient) {
                return nullptr;
            }

            return Aws::Crt::MakeShared<Clientv2>(allocator, allocator, bindingClient);
        }

        std::shared_ptr<IClientV2> IClientV2::newFrom311(const Aws::Crt::Mqtt::MqttConnection &protocolClient,
                                     const Aws::Iot::RequestResponse::RequestResponseClientOptions &options,
                                     Aws::Crt::Allocator *allocator) {

            Aws::Iot::RequestResponse::IMqttRequestResponseClient *bindingClient = Aws::Iot::RequestResponse::IMqttRequestResponseClient::newFrom311(protocolClient, options, allocator);
            if (!bindingClient) {
                return nullptr;
            }

            return Aws::Crt::MakeShared<Clientv2>(allocator, allocator, bindingClient);
        }

    }
}