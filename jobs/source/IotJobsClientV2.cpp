/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotjobs/IotJobsClientV2.h>

#include <aws/crt/UUID.h>

#include <aws/iotjobs/DescribeJobExecutionRequest.h>
#include <aws/iotjobs/DescribeJobExecutionResponse.h>
#include <aws/iotjobs/GetPendingJobExecutionsRequest.h>
#include <aws/iotjobs/GetPendingJobExecutionsResponse.h>
#include <aws/iotjobs/JobExecutionsChangedEvent.h>
#include <aws/iotjobs/JobExecutionsChangedSubscriptionRequest.h>
#include <aws/iotjobs/NextJobExecutionChangedEvent.h>
#include <aws/iotjobs/NextJobExecutionChangedSubscriptionRequest.h>
#include <aws/iotjobs/StartNextJobExecutionResponse.h>
#include <aws/iotjobs/StartNextPendingJobExecutionRequest.h>
#include <aws/iotjobs/UpdateJobExecutionRequest.h>
#include <aws/iotjobs/UpdateJobExecutionResponse.h>
#include <aws/iotjobs/V2ErrorResponse.h>

namespace Aws
{
    namespace Iotjobs
    {

        class ClientV2 : public IClientV2
        {
          public:
            ClientV2(
                Aws::Crt::Allocator *allocator,
                std::shared_ptr<Aws::Iot::RequestResponse::IMqttRequestResponseClient> bindingClient);
            virtual ~ClientV2() = default;

            bool DescribeJobExecution(
                const DescribeJobExecutionRequest &request,
                const DescribeJobExecutionResultHandler &handler) override;

            bool GetPendingJobExecutions(
                const GetPendingJobExecutionsRequest &request,
                const GetPendingJobExecutionsResultHandler &handler) override;

            bool StartNextPendingJobExecution(
                const StartNextPendingJobExecutionRequest &request,
                const StartNextPendingJobExecutionResultHandler &handler) override;

            bool UpdateJobExecution(
                const UpdateJobExecutionRequest &request,
                const UpdateJobExecutionResultHandler &handler) override;

            std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> CreateJobExecutionsChangedStream(
                const JobExecutionsChangedSubscriptionRequest &request,
                const Aws::Iot::RequestResponse::StreamingOperationOptions<JobExecutionsChangedEvent> &options)
                override;

            std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> CreateNextJobExecutionChangedStream(
                const NextJobExecutionChangedSubscriptionRequest &request,
                const Aws::Iot::RequestResponse::StreamingOperationOptions<NextJobExecutionChangedEvent> &options)
                override;

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

        static void s_DescribeJobExecutionResponseHandler(
            Aws::Iot::RequestResponse::UnmodeledResult &&result,
            const DescribeJobExecutionResultHandler &handler,
            const Aws::Crt::String &successPathTopic,
            const Aws::Crt::String &failurePathTopic)
        {
            using E = V2ErrorResponse;
            using R = Aws::Iot::RequestResponse::Result<DescribeJobExecutionResponse, ServiceErrorV2<E>>;

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
                DescribeJobExecutionResponse modeledResponse(jsonObject);
                Aws::Iot::RequestResponse::Result<DescribeJobExecutionResponse, ServiceErrorV2<E>> finalResult(
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

        bool ClientV2::DescribeJobExecution(
            const DescribeJobExecutionRequest &request,
            const DescribeJobExecutionResultHandler &handler)
        {
            Aws::Crt::StringStream publishTopicStream;
            publishTopicStream << "$aws/things/" << *request.ThingName << "/jobs/" << *request.JobId << "/get";
            Aws::Crt::String publishTopic = publishTopicStream.str();

            Aws::Crt::StringStream subscriptionTopicStream0;
            subscriptionTopicStream0 << "$aws/things/" << *request.ThingName << "/jobs/" << *request.JobId << "/get/+";
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
                s_DescribeJobExecutionResponseHandler(
                    std::move(result), handler, responsePathTopicAccepted, responsePathTopicRejected);
            };

            int submitResult = m_bindingClient->SubmitRequest(options, std::move(resultHandler));

            return submitResult == AWS_OP_SUCCESS;
        }

        static void s_GetPendingJobExecutionsResponseHandler(
            Aws::Iot::RequestResponse::UnmodeledResult &&result,
            const GetPendingJobExecutionsResultHandler &handler,
            const Aws::Crt::String &successPathTopic,
            const Aws::Crt::String &failurePathTopic)
        {
            using E = V2ErrorResponse;
            using R = Aws::Iot::RequestResponse::Result<GetPendingJobExecutionsResponse, ServiceErrorV2<E>>;

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
                GetPendingJobExecutionsResponse modeledResponse(jsonObject);
                Aws::Iot::RequestResponse::Result<GetPendingJobExecutionsResponse, ServiceErrorV2<E>> finalResult(
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

        bool ClientV2::GetPendingJobExecutions(
            const GetPendingJobExecutionsRequest &request,
            const GetPendingJobExecutionsResultHandler &handler)
        {
            Aws::Crt::StringStream publishTopicStream;
            publishTopicStream << "$aws/things/" << *request.ThingName << "/jobs/get";
            Aws::Crt::String publishTopic = publishTopicStream.str();

            Aws::Crt::StringStream subscriptionTopicStream0;
            subscriptionTopicStream0 << "$aws/things/" << *request.ThingName << "/jobs/get/+";
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
                s_GetPendingJobExecutionsResponseHandler(
                    std::move(result), handler, responsePathTopicAccepted, responsePathTopicRejected);
            };

            int submitResult = m_bindingClient->SubmitRequest(options, std::move(resultHandler));

            return submitResult == AWS_OP_SUCCESS;
        }

        static void s_StartNextPendingJobExecutionResponseHandler(
            Aws::Iot::RequestResponse::UnmodeledResult &&result,
            const StartNextPendingJobExecutionResultHandler &handler,
            const Aws::Crt::String &successPathTopic,
            const Aws::Crt::String &failurePathTopic)
        {
            using E = V2ErrorResponse;
            using R = Aws::Iot::RequestResponse::Result<StartNextJobExecutionResponse, ServiceErrorV2<E>>;

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
                StartNextJobExecutionResponse modeledResponse(jsonObject);
                Aws::Iot::RequestResponse::Result<StartNextJobExecutionResponse, ServiceErrorV2<E>> finalResult(
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

        bool ClientV2::StartNextPendingJobExecution(
            const StartNextPendingJobExecutionRequest &request,
            const StartNextPendingJobExecutionResultHandler &handler)
        {
            Aws::Crt::StringStream publishTopicStream;
            publishTopicStream << "$aws/things/" << *request.ThingName << "/jobs/start-next";
            Aws::Crt::String publishTopic = publishTopicStream.str();

            Aws::Crt::StringStream subscriptionTopicStream0;
            subscriptionTopicStream0 << "$aws/things/" << *request.ThingName << "/jobs/start-next/+";
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
                s_StartNextPendingJobExecutionResponseHandler(
                    std::move(result), handler, responsePathTopicAccepted, responsePathTopicRejected);
            };

            int submitResult = m_bindingClient->SubmitRequest(options, std::move(resultHandler));

            return submitResult == AWS_OP_SUCCESS;
        }

        static void s_UpdateJobExecutionResponseHandler(
            Aws::Iot::RequestResponse::UnmodeledResult &&result,
            const UpdateJobExecutionResultHandler &handler,
            const Aws::Crt::String &successPathTopic,
            const Aws::Crt::String &failurePathTopic)
        {
            using E = V2ErrorResponse;
            using R = Aws::Iot::RequestResponse::Result<UpdateJobExecutionResponse, ServiceErrorV2<E>>;

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
                UpdateJobExecutionResponse modeledResponse(jsonObject);
                Aws::Iot::RequestResponse::Result<UpdateJobExecutionResponse, ServiceErrorV2<E>> finalResult(
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

        bool ClientV2::UpdateJobExecution(
            const UpdateJobExecutionRequest &request,
            const UpdateJobExecutionResultHandler &handler)
        {
            Aws::Crt::StringStream publishTopicStream;
            publishTopicStream << "$aws/things/" << *request.ThingName << "/jobs/" << *request.JobId << "/update";
            Aws::Crt::String publishTopic = publishTopicStream.str();

            Aws::Crt::StringStream subscriptionTopicStream0;
            subscriptionTopicStream0 << "$aws/things/" << *request.ThingName << "/jobs/" << *request.JobId
                                     << "/update/+";
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
                s_UpdateJobExecutionResponseHandler(
                    std::move(result), handler, responsePathTopicAccepted, responsePathTopicRejected);
            };

            int submitResult = m_bindingClient->SubmitRequest(options, std::move(resultHandler));

            return submitResult == AWS_OP_SUCCESS;
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
                    [options](Aws::Iot::RequestResponse::IncomingPublishEvent &&event)
                {
                    const auto &payload = event.GetPayload();
                    Aws::Crt::String objectStr(reinterpret_cast<char *>(payload.ptr), payload.len);
                    Aws::Crt::JsonObject jsonObject(objectStr);
                    if (!jsonObject.WasParseSuccessful())
                    {
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

        std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> ClientV2::CreateJobExecutionsChangedStream(
            const JobExecutionsChangedSubscriptionRequest &request,
            const Aws::Iot::RequestResponse::StreamingOperationOptions<JobExecutionsChangedEvent> &options)
        {
            Aws::Crt::StringStream topicStream;
            topicStream << "$aws/things/" << *request.ThingName << "/jobs/notify";
            Aws::Crt::String topic = topicStream.str();

            return ServiceStreamingOperation<JobExecutionsChangedEvent>::Create(
                m_allocator, m_bindingClient, topic, options);
        }

        std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> ClientV2::CreateNextJobExecutionChangedStream(
            const NextJobExecutionChangedSubscriptionRequest &request,
            const Aws::Iot::RequestResponse::StreamingOperationOptions<NextJobExecutionChangedEvent> &options)
        {
            Aws::Crt::StringStream topicStream;
            topicStream << "$aws/things/" << *request.ThingName << "/jobs/notify-next";
            Aws::Crt::String topic = topicStream.str();

            return ServiceStreamingOperation<NextJobExecutionChangedEvent>::Create(
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

    } // namespace Iotjobs
} // namespace Aws
