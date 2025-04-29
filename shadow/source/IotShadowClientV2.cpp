/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotshadow/IotShadowClientV2.h>

#include <aws/crt/UUID.h>

#include <aws/iotshadow/DeleteNamedShadowRequest.h>
#include <aws/iotshadow/DeleteShadowRequest.h>
#include <aws/iotshadow/DeleteShadowResponse.h>
#include <aws/iotshadow/GetNamedShadowRequest.h>
#include <aws/iotshadow/GetShadowRequest.h>
#include <aws/iotshadow/GetShadowResponse.h>
#include <aws/iotshadow/NamedShadowDeltaUpdatedSubscriptionRequest.h>
#include <aws/iotshadow/NamedShadowUpdatedSubscriptionRequest.h>
#include <aws/iotshadow/ShadowDeltaUpdatedEvent.h>
#include <aws/iotshadow/ShadowDeltaUpdatedSubscriptionRequest.h>
#include <aws/iotshadow/ShadowUpdatedEvent.h>
#include <aws/iotshadow/ShadowUpdatedSubscriptionRequest.h>
#include <aws/iotshadow/UpdateNamedShadowRequest.h>
#include <aws/iotshadow/UpdateShadowRequest.h>
#include <aws/iotshadow/UpdateShadowResponse.h>
#include <aws/iotshadow/V2ErrorResponse.h>

namespace Aws
{
    namespace Iotshadow
    {

        class ClientV2 : public IClientV2
        {
          public:
            ClientV2(
                Aws::Crt::Allocator *allocator,
                std::shared_ptr<Aws::Iot::RequestResponse::IMqttRequestResponseClient> bindingClient);
            virtual ~ClientV2() = default;

            bool DeleteNamedShadow(
                const DeleteNamedShadowRequest &request,
                const DeleteNamedShadowResultHandler &handler) override;

            bool DeleteShadow(const DeleteShadowRequest &request, const DeleteShadowResultHandler &handler) override;

            bool GetNamedShadow(const GetNamedShadowRequest &request, const GetNamedShadowResultHandler &handler)
                override;

            bool GetShadow(const GetShadowRequest &request, const GetShadowResultHandler &handler) override;

            bool UpdateNamedShadow(
                const UpdateNamedShadowRequest &request,
                const UpdateNamedShadowResultHandler &handler) override;

            bool UpdateShadow(const UpdateShadowRequest &request, const UpdateShadowResultHandler &handler) override;

            std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> CreateNamedShadowDeltaUpdatedStream(
                const NamedShadowDeltaUpdatedSubscriptionRequest &request,
                const Aws::Iot::RequestResponse::StreamingOperationOptions<ShadowDeltaUpdatedEvent> &options) override;

            std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> CreateNamedShadowUpdatedStream(
                const NamedShadowUpdatedSubscriptionRequest &request,
                const Aws::Iot::RequestResponse::StreamingOperationOptions<ShadowUpdatedEvent> &options) override;

            std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> CreateShadowDeltaUpdatedStream(
                const ShadowDeltaUpdatedSubscriptionRequest &request,
                const Aws::Iot::RequestResponse::StreamingOperationOptions<ShadowDeltaUpdatedEvent> &options) override;

            std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> CreateShadowUpdatedStream(
                const ShadowUpdatedSubscriptionRequest &request,
                const Aws::Iot::RequestResponse::StreamingOperationOptions<ShadowUpdatedEvent> &options) override;

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

        static void s_DeleteNamedShadowResponseHandler(
            Aws::Iot::RequestResponse::UnmodeledResult &&result,
            const DeleteNamedShadowResultHandler &handler,
            const Aws::Crt::String &successPathTopic,
            const Aws::Crt::String &failurePathTopic)
        {
            using E = V2ErrorResponse;
            using R = Aws::Iot::RequestResponse::Result<DeleteShadowResponse, ServiceErrorV2<E>>;

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
                DeleteShadowResponse modeledResponse(jsonObject);
                Aws::Iot::RequestResponse::Result<DeleteShadowResponse, ServiceErrorV2<E>> finalResult(
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

        bool ClientV2::DeleteNamedShadow(
            const DeleteNamedShadowRequest &request,
            const DeleteNamedShadowResultHandler &handler)
        {
            Aws::Crt::StringStream publishTopicStream;
            publishTopicStream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName
                               << "/delete";
            Aws::Crt::String publishTopic = publishTopicStream.str();

            Aws::Crt::StringStream subscriptionTopicStream0;
            subscriptionTopicStream0 << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName
                                     << "/delete/+";
            Aws::Crt::String subscriptionTopic0 = subscriptionTopicStream0.str();

            struct aws_byte_cursor subscriptionTopicFilters[1] = {
                Aws::Crt::ByteCursorFromString(subscriptionTopic0),
            };

            Aws::Crt::StringStream responsePathTopicAcceptedStream;
            responsePathTopicAcceptedStream << publishTopic << "/accepted";
            Aws::Crt::String responsePathTopicAccepted = responsePathTopicAcceptedStream.str();

            Aws::Crt::StringStream responsePathTopicRejectedStream;
            responsePathTopicRejectedStream << publishTopic << "/rejected";
            Aws::Crt::String responsePathTopicRejected = responsePathTopicRejectedStream.str();

            struct aws_mqtt_request_operation_response_path responsePaths[2];
            responsePaths[0].topic = Aws::Crt::ByteCursorFromString(responsePathTopicAccepted);
            responsePaths[1].topic = Aws::Crt::ByteCursorFromString(responsePathTopicRejected);
            responsePaths[0].correlation_token_json_path = Aws::Crt::ByteCursorFromCString("clientToken");
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

            auto resultHandler = [handler, responsePathTopicAccepted, responsePathTopicRejected](
                                     Aws::Iot::RequestResponse::UnmodeledResult &&result)
            {
                s_DeleteNamedShadowResponseHandler(
                    std::move(result), handler, responsePathTopicAccepted, responsePathTopicRejected);
            };

            int submitResult = m_bindingClient->SubmitRequest(options, std::move(resultHandler));

            return submitResult == AWS_OP_SUCCESS;
        }

        static void s_DeleteShadowResponseHandler(
            Aws::Iot::RequestResponse::UnmodeledResult &&result,
            const DeleteShadowResultHandler &handler,
            const Aws::Crt::String &successPathTopic,
            const Aws::Crt::String &failurePathTopic)
        {
            using E = V2ErrorResponse;
            using R = Aws::Iot::RequestResponse::Result<DeleteShadowResponse, ServiceErrorV2<E>>;

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
                DeleteShadowResponse modeledResponse(jsonObject);
                Aws::Iot::RequestResponse::Result<DeleteShadowResponse, ServiceErrorV2<E>> finalResult(
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

        bool ClientV2::DeleteShadow(const DeleteShadowRequest &request, const DeleteShadowResultHandler &handler)
        {
            Aws::Crt::StringStream publishTopicStream;
            publishTopicStream << "$aws/things/" << *request.ThingName << "/shadow/delete";
            Aws::Crt::String publishTopic = publishTopicStream.str();

            Aws::Crt::StringStream subscriptionTopicStream0;
            subscriptionTopicStream0 << "$aws/things/" << *request.ThingName << "/shadow/delete/+";
            Aws::Crt::String subscriptionTopic0 = subscriptionTopicStream0.str();

            struct aws_byte_cursor subscriptionTopicFilters[1] = {
                Aws::Crt::ByteCursorFromString(subscriptionTopic0),
            };

            Aws::Crt::StringStream responsePathTopicAcceptedStream;
            responsePathTopicAcceptedStream << publishTopic << "/accepted";
            Aws::Crt::String responsePathTopicAccepted = responsePathTopicAcceptedStream.str();

            Aws::Crt::StringStream responsePathTopicRejectedStream;
            responsePathTopicRejectedStream << publishTopic << "/rejected";
            Aws::Crt::String responsePathTopicRejected = responsePathTopicRejectedStream.str();

            struct aws_mqtt_request_operation_response_path responsePaths[2];
            responsePaths[0].topic = Aws::Crt::ByteCursorFromString(responsePathTopicAccepted);
            responsePaths[1].topic = Aws::Crt::ByteCursorFromString(responsePathTopicRejected);
            responsePaths[0].correlation_token_json_path = Aws::Crt::ByteCursorFromCString("clientToken");
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

            auto resultHandler = [handler, responsePathTopicAccepted, responsePathTopicRejected](
                                     Aws::Iot::RequestResponse::UnmodeledResult &&result) {
                s_DeleteShadowResponseHandler(
                    std::move(result), handler, responsePathTopicAccepted, responsePathTopicRejected);
            };

            int submitResult = m_bindingClient->SubmitRequest(options, std::move(resultHandler));

            return submitResult == AWS_OP_SUCCESS;
        }

        static void s_GetNamedShadowResponseHandler(
            Aws::Iot::RequestResponse::UnmodeledResult &&result,
            const GetNamedShadowResultHandler &handler,
            const Aws::Crt::String &successPathTopic,
            const Aws::Crt::String &failurePathTopic)
        {
            using E = V2ErrorResponse;
            using R = Aws::Iot::RequestResponse::Result<GetShadowResponse, ServiceErrorV2<E>>;

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
                GetShadowResponse modeledResponse(jsonObject);
                Aws::Iot::RequestResponse::Result<GetShadowResponse, ServiceErrorV2<E>> finalResult(
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

        bool ClientV2::GetNamedShadow(const GetNamedShadowRequest &request, const GetNamedShadowResultHandler &handler)
        {
            Aws::Crt::StringStream publishTopicStream;
            publishTopicStream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName
                               << "/get";
            Aws::Crt::String publishTopic = publishTopicStream.str();

            Aws::Crt::StringStream subscriptionTopicStream0;
            subscriptionTopicStream0 << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName
                                     << "/get/+";
            Aws::Crt::String subscriptionTopic0 = subscriptionTopicStream0.str();

            struct aws_byte_cursor subscriptionTopicFilters[1] = {
                Aws::Crt::ByteCursorFromString(subscriptionTopic0),
            };

            Aws::Crt::StringStream responsePathTopicAcceptedStream;
            responsePathTopicAcceptedStream << publishTopic << "/accepted";
            Aws::Crt::String responsePathTopicAccepted = responsePathTopicAcceptedStream.str();

            Aws::Crt::StringStream responsePathTopicRejectedStream;
            responsePathTopicRejectedStream << publishTopic << "/rejected";
            Aws::Crt::String responsePathTopicRejected = responsePathTopicRejectedStream.str();

            struct aws_mqtt_request_operation_response_path responsePaths[2];
            responsePaths[0].topic = Aws::Crt::ByteCursorFromString(responsePathTopicAccepted);
            responsePaths[1].topic = Aws::Crt::ByteCursorFromString(responsePathTopicRejected);
            responsePaths[0].correlation_token_json_path = Aws::Crt::ByteCursorFromCString("clientToken");
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

            auto resultHandler = [handler, responsePathTopicAccepted, responsePathTopicRejected](
                                     Aws::Iot::RequestResponse::UnmodeledResult &&result) {
                s_GetNamedShadowResponseHandler(
                    std::move(result), handler, responsePathTopicAccepted, responsePathTopicRejected);
            };

            int submitResult = m_bindingClient->SubmitRequest(options, std::move(resultHandler));

            return submitResult == AWS_OP_SUCCESS;
        }

        static void s_GetShadowResponseHandler(
            Aws::Iot::RequestResponse::UnmodeledResult &&result,
            const GetShadowResultHandler &handler,
            const Aws::Crt::String &successPathTopic,
            const Aws::Crt::String &failurePathTopic)
        {
            using E = V2ErrorResponse;
            using R = Aws::Iot::RequestResponse::Result<GetShadowResponse, ServiceErrorV2<E>>;

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
                GetShadowResponse modeledResponse(jsonObject);
                Aws::Iot::RequestResponse::Result<GetShadowResponse, ServiceErrorV2<E>> finalResult(
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

        bool ClientV2::GetShadow(const GetShadowRequest &request, const GetShadowResultHandler &handler)
        {
            Aws::Crt::StringStream publishTopicStream;
            publishTopicStream << "$aws/things/" << *request.ThingName << "/shadow/get";
            Aws::Crt::String publishTopic = publishTopicStream.str();

            Aws::Crt::StringStream subscriptionTopicStream0;
            subscriptionTopicStream0 << "$aws/things/" << *request.ThingName << "/shadow/get/+";
            Aws::Crt::String subscriptionTopic0 = subscriptionTopicStream0.str();

            struct aws_byte_cursor subscriptionTopicFilters[1] = {
                Aws::Crt::ByteCursorFromString(subscriptionTopic0),
            };

            Aws::Crt::StringStream responsePathTopicAcceptedStream;
            responsePathTopicAcceptedStream << publishTopic << "/accepted";
            Aws::Crt::String responsePathTopicAccepted = responsePathTopicAcceptedStream.str();

            Aws::Crt::StringStream responsePathTopicRejectedStream;
            responsePathTopicRejectedStream << publishTopic << "/rejected";
            Aws::Crt::String responsePathTopicRejected = responsePathTopicRejectedStream.str();

            struct aws_mqtt_request_operation_response_path responsePaths[2];
            responsePaths[0].topic = Aws::Crt::ByteCursorFromString(responsePathTopicAccepted);
            responsePaths[1].topic = Aws::Crt::ByteCursorFromString(responsePathTopicRejected);
            responsePaths[0].correlation_token_json_path = Aws::Crt::ByteCursorFromCString("clientToken");
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

            auto resultHandler = [handler, responsePathTopicAccepted, responsePathTopicRejected](
                                     Aws::Iot::RequestResponse::UnmodeledResult &&result) {
                s_GetShadowResponseHandler(
                    std::move(result), handler, responsePathTopicAccepted, responsePathTopicRejected);
            };

            int submitResult = m_bindingClient->SubmitRequest(options, std::move(resultHandler));

            return submitResult == AWS_OP_SUCCESS;
        }

        static void s_UpdateNamedShadowResponseHandler(
            Aws::Iot::RequestResponse::UnmodeledResult &&result,
            const UpdateNamedShadowResultHandler &handler,
            const Aws::Crt::String &successPathTopic,
            const Aws::Crt::String &failurePathTopic)
        {
            using E = V2ErrorResponse;
            using R = Aws::Iot::RequestResponse::Result<UpdateShadowResponse, ServiceErrorV2<E>>;

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
                UpdateShadowResponse modeledResponse(jsonObject);
                Aws::Iot::RequestResponse::Result<UpdateShadowResponse, ServiceErrorV2<E>> finalResult(
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

        bool ClientV2::UpdateNamedShadow(
            const UpdateNamedShadowRequest &request,
            const UpdateNamedShadowResultHandler &handler)
        {
            Aws::Crt::StringStream publishTopicStream;
            publishTopicStream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName
                               << "/update";
            Aws::Crt::String publishTopic = publishTopicStream.str();

            Aws::Crt::StringStream subscriptionTopicStream0;
            subscriptionTopicStream0 << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName
                                     << "/update/accepted";
            Aws::Crt::String subscriptionTopic0 = subscriptionTopicStream0.str();

            Aws::Crt::StringStream subscriptionTopicStream1;
            subscriptionTopicStream1 << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName
                                     << "/update/rejected";
            Aws::Crt::String subscriptionTopic1 = subscriptionTopicStream1.str();

            struct aws_byte_cursor subscriptionTopicFilters[2] = {
                Aws::Crt::ByteCursorFromString(subscriptionTopic0),
                Aws::Crt::ByteCursorFromString(subscriptionTopic1),
            };

            Aws::Crt::StringStream responsePathTopicAcceptedStream;
            responsePathTopicAcceptedStream << publishTopic << "/accepted";
            Aws::Crt::String responsePathTopicAccepted = responsePathTopicAcceptedStream.str();

            Aws::Crt::StringStream responsePathTopicRejectedStream;
            responsePathTopicRejectedStream << publishTopic << "/rejected";
            Aws::Crt::String responsePathTopicRejected = responsePathTopicRejectedStream.str();

            struct aws_mqtt_request_operation_response_path responsePaths[2];
            responsePaths[0].topic = Aws::Crt::ByteCursorFromString(responsePathTopicAccepted);
            responsePaths[1].topic = Aws::Crt::ByteCursorFromString(responsePathTopicRejected);
            responsePaths[0].correlation_token_json_path = Aws::Crt::ByteCursorFromCString("clientToken");
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
            options.serialized_request =
                Aws::Crt::ByteCursorFromArray((uint8_t *)outgoingJson.data(), outgoingJson.length());

            options.correlation_token = Aws::Crt::ByteCursorFromString(uuid);

            auto resultHandler = [handler, responsePathTopicAccepted, responsePathTopicRejected](
                                     Aws::Iot::RequestResponse::UnmodeledResult &&result)
            {
                s_UpdateNamedShadowResponseHandler(
                    std::move(result), handler, responsePathTopicAccepted, responsePathTopicRejected);
            };

            int submitResult = m_bindingClient->SubmitRequest(options, std::move(resultHandler));

            return submitResult == AWS_OP_SUCCESS;
        }

        static void s_UpdateShadowResponseHandler(
            Aws::Iot::RequestResponse::UnmodeledResult &&result,
            const UpdateShadowResultHandler &handler,
            const Aws::Crt::String &successPathTopic,
            const Aws::Crt::String &failurePathTopic)
        {
            using E = V2ErrorResponse;
            using R = Aws::Iot::RequestResponse::Result<UpdateShadowResponse, ServiceErrorV2<E>>;

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
                UpdateShadowResponse modeledResponse(jsonObject);
                Aws::Iot::RequestResponse::Result<UpdateShadowResponse, ServiceErrorV2<E>> finalResult(
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

        bool ClientV2::UpdateShadow(const UpdateShadowRequest &request, const UpdateShadowResultHandler &handler)
        {
            Aws::Crt::StringStream publishTopicStream;
            publishTopicStream << "$aws/things/" << *request.ThingName << "/shadow/update";
            Aws::Crt::String publishTopic = publishTopicStream.str();

            Aws::Crt::StringStream subscriptionTopicStream0;
            subscriptionTopicStream0 << "$aws/things/" << *request.ThingName << "/shadow/update/accepted";
            Aws::Crt::String subscriptionTopic0 = subscriptionTopicStream0.str();

            Aws::Crt::StringStream subscriptionTopicStream1;
            subscriptionTopicStream1 << "$aws/things/" << *request.ThingName << "/shadow/update/rejected";
            Aws::Crt::String subscriptionTopic1 = subscriptionTopicStream1.str();

            struct aws_byte_cursor subscriptionTopicFilters[2] = {
                Aws::Crt::ByteCursorFromString(subscriptionTopic0),
                Aws::Crt::ByteCursorFromString(subscriptionTopic1),
            };

            Aws::Crt::StringStream responsePathTopicAcceptedStream;
            responsePathTopicAcceptedStream << publishTopic << "/accepted";
            Aws::Crt::String responsePathTopicAccepted = responsePathTopicAcceptedStream.str();

            Aws::Crt::StringStream responsePathTopicRejectedStream;
            responsePathTopicRejectedStream << publishTopic << "/rejected";
            Aws::Crt::String responsePathTopicRejected = responsePathTopicRejectedStream.str();

            struct aws_mqtt_request_operation_response_path responsePaths[2];
            responsePaths[0].topic = Aws::Crt::ByteCursorFromString(responsePathTopicAccepted);
            responsePaths[1].topic = Aws::Crt::ByteCursorFromString(responsePathTopicRejected);
            responsePaths[0].correlation_token_json_path = Aws::Crt::ByteCursorFromCString("clientToken");
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
            options.serialized_request =
                Aws::Crt::ByteCursorFromArray((uint8_t *)outgoingJson.data(), outgoingJson.length());

            options.correlation_token = Aws::Crt::ByteCursorFromString(uuid);

            auto resultHandler = [handler, responsePathTopicAccepted, responsePathTopicRejected](
                                     Aws::Iot::RequestResponse::UnmodeledResult &&result) {
                s_UpdateShadowResponseHandler(
                    std::move(result), handler, responsePathTopicAccepted, responsePathTopicRejected);
            };

            int submitResult = m_bindingClient->SubmitRequest(options, std::move(resultHandler));

            return submitResult == AWS_OP_SUCCESS;
        }

        static bool s_initModeledEvent(
            const Aws::Iot::RequestResponse::IncomingPublishEvent &publishEvent,
            ShadowDeltaUpdatedEvent &modeledEvent)
        {
            const auto &payload = publishEvent.GetPayload();
            Aws::Crt::String objectStr(reinterpret_cast<char *>(payload.ptr), payload.len);
            Aws::Crt::JsonObject jsonObject(objectStr);
            if (!jsonObject.WasParseSuccessful())
            {
                return false;
            }

            modeledEvent = ShadowDeltaUpdatedEvent(jsonObject);
            return true;
        }

        static bool s_initModeledEvent(
            const Aws::Iot::RequestResponse::IncomingPublishEvent &publishEvent,
            ShadowUpdatedEvent &modeledEvent)
        {
            const auto &payload = publishEvent.GetPayload();
            Aws::Crt::String objectStr(reinterpret_cast<char *>(payload.ptr), payload.len);
            Aws::Crt::JsonObject jsonObject(objectStr);
            if (!jsonObject.WasParseSuccessful())
            {
                return false;
            }

            modeledEvent = ShadowUpdatedEvent(jsonObject);
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

        std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> ClientV2::CreateNamedShadowDeltaUpdatedStream(
            const NamedShadowDeltaUpdatedSubscriptionRequest &request,
            const Aws::Iot::RequestResponse::StreamingOperationOptions<ShadowDeltaUpdatedEvent> &options)
        {
            Aws::Crt::StringStream topicStream;
            topicStream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName
                        << "/update/delta";
            Aws::Crt::String topic = topicStream.str();

            return ServiceStreamingOperation<ShadowDeltaUpdatedEvent>::Create(
                m_allocator, m_bindingClient, topic, options);
        }

        std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> ClientV2::CreateNamedShadowUpdatedStream(
            const NamedShadowUpdatedSubscriptionRequest &request,
            const Aws::Iot::RequestResponse::StreamingOperationOptions<ShadowUpdatedEvent> &options)
        {
            Aws::Crt::StringStream topicStream;
            topicStream << "$aws/things/" << *request.ThingName << "/shadow/name/" << *request.ShadowName
                        << "/update/documents";
            Aws::Crt::String topic = topicStream.str();

            return ServiceStreamingOperation<ShadowUpdatedEvent>::Create(m_allocator, m_bindingClient, topic, options);
        }

        std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> ClientV2::CreateShadowDeltaUpdatedStream(
            const ShadowDeltaUpdatedSubscriptionRequest &request,
            const Aws::Iot::RequestResponse::StreamingOperationOptions<ShadowDeltaUpdatedEvent> &options)
        {
            Aws::Crt::StringStream topicStream;
            topicStream << "$aws/things/" << *request.ThingName << "/shadow/update/delta";
            Aws::Crt::String topic = topicStream.str();

            return ServiceStreamingOperation<ShadowDeltaUpdatedEvent>::Create(
                m_allocator, m_bindingClient, topic, options);
        }

        std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> ClientV2::CreateShadowUpdatedStream(
            const ShadowUpdatedSubscriptionRequest &request,
            const Aws::Iot::RequestResponse::StreamingOperationOptions<ShadowUpdatedEvent> &options)
        {
            Aws::Crt::StringStream topicStream;
            topicStream << "$aws/things/" << *request.ThingName << "/shadow/update/documents";
            Aws::Crt::String topic = topicStream.str();

            return ServiceStreamingOperation<ShadowUpdatedEvent>::Create(m_allocator, m_bindingClient, topic, options);
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

    } // namespace Iotshadow
} // namespace Aws
