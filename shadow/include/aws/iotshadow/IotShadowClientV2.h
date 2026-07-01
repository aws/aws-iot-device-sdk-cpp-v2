#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotshadow/Exports.h>

#include <aws/crt/StlAllocator.h>
#include <aws/crt/Types.h>
#include <aws/iot/MqttRequestResponseClient.h>

#include <functional>

namespace Aws
{
    namespace Crt
    {
        namespace Mqtt
        {
            class MqttConnection;
        }
        namespace Mqtt5
        {
            class Mqtt5Client;
        }
    } // namespace Crt
} // namespace Aws

namespace Aws
{
    namespace Iotshadow
    {

        template <typename E> class ServiceErrorV2
        {
          public:
            ServiceErrorV2() = delete;
            ServiceErrorV2(const ServiceErrorV2<E> &rhs) = default;
            ServiceErrorV2(ServiceErrorV2<E> &&rhs) = default;

            explicit ServiceErrorV2(int errorCode) : m_errorCode(errorCode), m_modeledError() {}

            ServiceErrorV2(int errorCode, E &&modeledError)
                : m_errorCode(errorCode), m_modeledError(std::move(modeledError))
            {
            }

            ~ServiceErrorV2() = default;

            ServiceErrorV2<E> &operator=(const ServiceErrorV2<E> &rhs) = default;
            ServiceErrorV2<E> &operator=(ServiceErrorV2<E> &&rhs) = default;

            int GetErrorCode() const { return m_errorCode; }

            bool HasModeledError() const { return m_modeledError.has_value(); }
            const E &GetModeledError() const { return m_modeledError.value(); }

          private:
            int m_errorCode;

            Aws::Crt::Optional<E> m_modeledError;
        };

        class DeleteNamedShadowRequest;
        class DeleteShadowRequest;
        class DeleteShadowResponse;
        class GetNamedShadowRequest;
        class GetShadowRequest;
        class GetShadowResponse;
        class NamedShadowDeltaUpdatedSubscriptionRequest;
        class NamedShadowUpdatedSubscriptionRequest;
        class ShadowDeltaUpdatedEvent;
        class ShadowDeltaUpdatedSubscriptionRequest;
        class ShadowUpdatedEvent;
        class ShadowUpdatedSubscriptionRequest;
        class UpdateNamedShadowRequest;
        class UpdateShadowRequest;
        class UpdateShadowResponse;
        class V2ErrorResponse;

        using DeleteNamedShadowResult =
            Aws::Iot::RequestResponse::Result<DeleteShadowResponse, ServiceErrorV2<V2ErrorResponse>>;
        using DeleteNamedShadowResultHandler = std::function<void(DeleteNamedShadowResult &&)>;

        using DeleteShadowResult =
            Aws::Iot::RequestResponse::Result<DeleteShadowResponse, ServiceErrorV2<V2ErrorResponse>>;
        using DeleteShadowResultHandler = std::function<void(DeleteShadowResult &&)>;

        using GetNamedShadowResult =
            Aws::Iot::RequestResponse::Result<GetShadowResponse, ServiceErrorV2<V2ErrorResponse>>;
        using GetNamedShadowResultHandler = std::function<void(GetNamedShadowResult &&)>;

        using GetShadowResult = Aws::Iot::RequestResponse::Result<GetShadowResponse, ServiceErrorV2<V2ErrorResponse>>;
        using GetShadowResultHandler = std::function<void(GetShadowResult &&)>;

        using UpdateNamedShadowResult =
            Aws::Iot::RequestResponse::Result<UpdateShadowResponse, ServiceErrorV2<V2ErrorResponse>>;
        using UpdateNamedShadowResultHandler = std::function<void(UpdateNamedShadowResult &&)>;

        using UpdateShadowResult =
            Aws::Iot::RequestResponse::Result<UpdateShadowResponse, ServiceErrorV2<V2ErrorResponse>>;
        using UpdateShadowResultHandler = std::function<void(UpdateShadowResult &&)>;

        /**
         * The AWS IoT Device Shadow service adds shadows to AWS IoT thing objects. Shadows are a simple data store for
         * device properties and state.  Shadows can make a device’s state available to apps and other services whether
         * the device is connected to AWS IoT or not.
         *
         * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html
         *
         */
        class AWS_IOTSHADOW_API IClientV2
        {
          public:
            virtual ~IClientV2() = default;

            /**
             * Deletes a named shadow for an AWS IoT thing.
             *
             *
             * AWS documentation:
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#delete-pub-sub-topic
             *
             * @param request operation to perform
             * @param handler function object to invoke upon operation completion
             *
             * @return success/failure
             */
            virtual bool DeleteNamedShadow(
                const DeleteNamedShadowRequest &request,
                const DeleteNamedShadowResultHandler &handler) = 0;

            /**
             * Deletes the (classic) shadow for an AWS IoT thing.
             *
             *
             * AWS documentation:
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#delete-pub-sub-topic
             *
             * @param request operation to perform
             * @param handler function object to invoke upon operation completion
             *
             * @return success/failure
             */
            virtual bool DeleteShadow(const DeleteShadowRequest &request, const DeleteShadowResultHandler &handler) = 0;

            /**
             * Gets a named shadow for an AWS IoT thing.
             *
             *
             * AWS documentation:
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#get-pub-sub-topic
             *
             * @param request operation to perform
             * @param handler function object to invoke upon operation completion
             *
             * @return success/failure
             */
            virtual bool GetNamedShadow(
                const GetNamedShadowRequest &request,
                const GetNamedShadowResultHandler &handler) = 0;

            /**
             * Gets the (classic) shadow for an AWS IoT thing.
             *
             *
             * AWS documentation:
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#get-pub-sub-topic
             *
             * @param request operation to perform
             * @param handler function object to invoke upon operation completion
             *
             * @return success/failure
             */
            virtual bool GetShadow(const GetShadowRequest &request, const GetShadowResultHandler &handler) = 0;

            /**
             * Update a named shadow for a device.
             *
             *
             * AWS documentation:
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#update-pub-sub-topic
             *
             * @param request operation to perform
             * @param handler function object to invoke upon operation completion
             *
             * @return success/failure
             */
            virtual bool UpdateNamedShadow(
                const UpdateNamedShadowRequest &request,
                const UpdateNamedShadowResultHandler &handler) = 0;

            /**
             * Update a device's (classic) shadow.
             *
             *
             * AWS documentation:
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#update-pub-sub-topic
             *
             * @param request operation to perform
             * @param handler function object to invoke upon operation completion
             *
             * @return success/failure
             */
            virtual bool UpdateShadow(const UpdateShadowRequest &request, const UpdateShadowResultHandler &handler) = 0;

            /**
             * Create a stream for NamedShadowDelta events for a named shadow of an AWS IoT thing.
             *
             *
             * AWS documentation:
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#update-delta-pub-sub-topic
             *
             * @param request Modeled streaming operation subscription configuration.
             * @param options Configuration options for the streaming operation.
             *
             * @return A streaming operation which will emit a modeled event every time a message is received on the
             * associated MQTT topic.
             */
            virtual std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> CreateNamedShadowDeltaUpdatedStream(
                const NamedShadowDeltaUpdatedSubscriptionRequest &request,
                const Aws::Iot::RequestResponse::StreamingOperationOptions<ShadowDeltaUpdatedEvent> &options) = 0;

            /**
             * Create a stream for ShadowUpdated events for a named shadow of an AWS IoT thing.
             *
             *
             * AWS documentation:
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#update-documents-pub-sub-topic
             *
             * @param request Modeled streaming operation subscription configuration.
             * @param options Configuration options for the streaming operation.
             *
             * @return A streaming operation which will emit a modeled event every time a message is received on the
             * associated MQTT topic.
             */
            virtual std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> CreateNamedShadowUpdatedStream(
                const NamedShadowUpdatedSubscriptionRequest &request,
                const Aws::Iot::RequestResponse::StreamingOperationOptions<ShadowUpdatedEvent> &options) = 0;

            /**
             * Create a stream for ShadowDelta events for the (classic) shadow of an AWS IoT thing.
             *
             *
             * AWS documentation:
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#update-delta-pub-sub-topic
             *
             * @param request Modeled streaming operation subscription configuration.
             * @param options Configuration options for the streaming operation.
             *
             * @return A streaming operation which will emit a modeled event every time a message is received on the
             * associated MQTT topic.
             */
            virtual std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> CreateShadowDeltaUpdatedStream(
                const ShadowDeltaUpdatedSubscriptionRequest &request,
                const Aws::Iot::RequestResponse::StreamingOperationOptions<ShadowDeltaUpdatedEvent> &options) = 0;

            /**
             * Create a stream for ShadowUpdated events for the (classic) shadow of an AWS IoT thing.
             *
             *
             * AWS documentation:
             * https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#update-documents-pub-sub-topic
             *
             * @param request Modeled streaming operation subscription configuration.
             * @param options Configuration options for the streaming operation.
             *
             * @return A streaming operation which will emit a modeled event every time a message is received on the
             * associated MQTT topic.
             */
            virtual std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> CreateShadowUpdatedStream(
                const ShadowUpdatedSubscriptionRequest &request,
                const Aws::Iot::RequestResponse::StreamingOperationOptions<ShadowUpdatedEvent> &options) = 0;
        };

        /**
         * Creates a new service client that uses an SDK MQTT5 client for transport.
         *
         * @param protocolClient MQTT client to use as transport
         * @param options request-response MQTT client configuration options
         * @param allocator memory allocator to use for all client functionality
         *
         * @return a new service client
         */
        AWS_IOTSHADOW_API std::shared_ptr<IClientV2> NewClientFrom5(
            const Aws::Crt::Mqtt5::Mqtt5Client &protocolClient,
            const Aws::Iot::RequestResponse::RequestResponseClientOptions &options,
            Aws::Crt::Allocator *allocator = Aws::Crt::ApiAllocator());

        /**
         * Creates a new service client that uses an SDK MQTT311 client for transport.
         *
         * @param protocolClient MQTT client to use as transport
         * @param options request-response MQTT client configuration options
         * @param allocator memory allocator to use for all client functionality
         *
         * @return a new service client
         */
        AWS_IOTSHADOW_API std::shared_ptr<IClientV2> NewClientFrom311(
            const Aws::Crt::Mqtt::MqttConnection &protocolClient,
            const Aws::Iot::RequestResponse::RequestResponseClientOptions &options,
            Aws::Crt::Allocator *allocator = Aws::Crt::ApiAllocator());

    } // namespace Iotshadow
} // namespace Aws
