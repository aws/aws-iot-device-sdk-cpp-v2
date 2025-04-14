#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotcommands/Exports.h>

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
    namespace Iotcommands
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

        class CommandExecutionEvent;
        class CommandExecutionsSubscriptionRequest;
        class UpdateCommandExecutionRequest;
        class UpdateCommandExecutionResponse;
        class V2ErrorResponse;

        using UpdateCommandExecutionResult =
            Aws::Iot::RequestResponse::Result<UpdateCommandExecutionResponse, ServiceErrorV2<V2ErrorResponse>>;
        using UpdateCommandExecutionResultHandler = std::function<void(UpdateCommandExecutionResult &&)>;

        /**
         */
        class AWS_IOTCOMMANDS_API IClientV2
        {
          public:
            virtual ~IClientV2() = default;

            /**
             *
             *
             *
             * @param request operation to perform
             * @param handler function object to invoke upon operation completion
             *
             * @return success/failure
             */
            virtual bool UpdateCommandExecution(
                const UpdateCommandExecutionRequest &request,
                const UpdateCommandExecutionResultHandler &handler) = 0;

            /**
             * Creates a stream of CommandExecutionsChanged notifications for a given IoT thing.
             *
             *
             * @param request Modeled streaming operation subscription configuration.
             * @param options Configuration options for the streaming operation.
             *
             * @return A streaming operation which will emit a modeled event every time a message is received on the
             * associated MQTT topic.
             */
            virtual std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation>
                CreateCommandExecutionsCborPayloadStream(
                    const CommandExecutionsSubscriptionRequest &request,
                    const Aws::Iot::RequestResponse::StreamingOperationOptions<CommandExecutionEvent> &options) = 0;

            /**
             * Creates a stream of CommandExecutionsChanged notifications for a given IoT thing.
             *
             *
             * @param request Modeled streaming operation subscription configuration.
             * @param options Configuration options for the streaming operation.
             *
             * @return A streaming operation which will emit a modeled event every time a message is received on the
             * associated MQTT topic.
             */
            virtual std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation>
                CreateCommandExecutionsGenericPayloadStream(
                    const CommandExecutionsSubscriptionRequest &request,
                    const Aws::Iot::RequestResponse::StreamingOperationOptions<CommandExecutionEvent> &options) = 0;

            /**
             * Creates a stream of CommandExecutionsChanged notifications for a given IoT thing.
             *
             *
             * @param request Modeled streaming operation subscription configuration.
             * @param options Configuration options for the streaming operation.
             *
             * @return A streaming operation which will emit a modeled event every time a message is received on the
             * associated MQTT topic.
             */
            virtual std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation>
                CreateCommandExecutionsJsonPayloadStream(
                    const CommandExecutionsSubscriptionRequest &request,
                    const Aws::Iot::RequestResponse::StreamingOperationOptions<CommandExecutionEvent> &options) = 0;
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
        AWS_IOTCOMMANDS_API std::shared_ptr<IClientV2> NewClientFrom5(
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
        AWS_IOTCOMMANDS_API std::shared_ptr<IClientV2> NewClientFrom311(
            const Aws::Crt::Mqtt::MqttConnection &protocolClient,
            const Aws::Iot::RequestResponse::RequestResponseClientOptions &options,
            Aws::Crt::Allocator *allocator = Aws::Crt::ApiAllocator());

    } // namespace Iotcommands
} // namespace Aws
