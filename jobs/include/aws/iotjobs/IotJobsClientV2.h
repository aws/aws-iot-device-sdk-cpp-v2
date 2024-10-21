#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotjobs/Exports.h>

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
    namespace Iotjobs
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

        class DescribeJobExecutionRequest;
        class DescribeJobExecutionResponse;
        class GetPendingJobExecutionsRequest;
        class GetPendingJobExecutionsResponse;
        class JobExecutionsChangedEvent;
        class JobExecutionsChangedSubscriptionRequest;
        class NextJobExecutionChangedEvent;
        class NextJobExecutionChangedSubscriptionRequest;
        class StartNextJobExecutionResponse;
        class StartNextPendingJobExecutionRequest;
        class UpdateJobExecutionRequest;
        class UpdateJobExecutionResponse;
        class V2ServiceError;

        using DescribeJobExecutionResult =
            Aws::Iot::RequestResponse::Result<DescribeJobExecutionResponse, ServiceErrorV2<V2ServiceError>>;
        using DescribeJobExecutionResultHandler = std::function<void(DescribeJobExecutionResult &&)>;
        using GetPendingJobExecutionsResult =
            Aws::Iot::RequestResponse::Result<GetPendingJobExecutionsResponse, ServiceErrorV2<V2ServiceError>>;
        using GetPendingJobExecutionsResultHandler = std::function<void(GetPendingJobExecutionsResult &&)>;
        using StartNextPendingJobExecutionResult =
            Aws::Iot::RequestResponse::Result<StartNextJobExecutionResponse, ServiceErrorV2<V2ServiceError>>;
        using StartNextPendingJobExecutionResultHandler = std::function<void(StartNextPendingJobExecutionResult &&)>;
        using UpdateJobExecutionResult =
            Aws::Iot::RequestResponse::Result<UpdateJobExecutionResponse, ServiceErrorV2<V2ServiceError>>;
        using UpdateJobExecutionResultHandler = std::function<void(UpdateJobExecutionResult &&)>;

        /**
         * The AWS IoT jobs service can be used to define a set of remote operations that are sent to and executed on
         * one or more devices connected to AWS IoT.
         *
         * https://docs.aws.amazon.com/iot/latest/developerguide/jobs-api.html#jobs-mqtt-api
         *
         */
        class AWS_IOTJOBS_API IClientV2
        {
          public:
            virtual ~IClientV2() = default;

            /**
             * Gets detailed information about a job execution.
             *
             *
             * AWS documentation:
             * https://docs.aws.amazon.com/iot/latest/developerguide/jobs-api.html#mqtt-describejobexecution
             *
             * @param request operation to perform
             * @param handler function object to invoke upon operation completion
             *
             * @returns success/failure
             */
            virtual bool DescribeJobExecution(
                const DescribeJobExecutionRequest &request,
                const DescribeJobExecutionResultHandler &handler) = 0;

            /**
             * Gets the list of all jobs for a thing that are not in a terminal state.
             *
             *
             * AWS documentation:
             * https://docs.aws.amazon.com/iot/latest/developerguide/jobs-api.html#mqtt-getpendingjobexecutions
             *
             * @param request operation to perform
             * @param handler function object to invoke upon operation completion
             *
             * @returns success/failure
             */
            virtual bool GetPendingJobExecutions(
                const GetPendingJobExecutionsRequest &request,
                const GetPendingJobExecutionsResultHandler &handler) = 0;

            /**
             * Gets and starts the next pending job execution for a thing (status IN_PROGRESS or QUEUED).
             *
             *
             * AWS documentation:
             * https://docs.aws.amazon.com/iot/latest/developerguide/jobs-api.html#mqtt-startnextpendingjobexecution
             *
             * @param request operation to perform
             * @param handler function object to invoke upon operation completion
             *
             * @returns success/failure
             */
            virtual bool StartNextPendingJobExecution(
                const StartNextPendingJobExecutionRequest &request,
                const StartNextPendingJobExecutionResultHandler &handler) = 0;

            /**
             * Updates the status of a job execution. You can optionally create a step timer by setting a value for the
             * stepTimeoutInMinutes property. If you don't update the value of this property by running
             * UpdateJobExecution again, the job execution times out when the step timer expires.
             *
             *
             * AWS documentation:
             * https://docs.aws.amazon.com/iot/latest/developerguide/jobs-api.html#mqtt-updatejobexecution
             *
             * @param request operation to perform
             * @param handler function object to invoke upon operation completion
             *
             * @returns success/failure
             */
            virtual bool UpdateJobExecution(
                const UpdateJobExecutionRequest &request,
                const UpdateJobExecutionResultHandler &handler) = 0;

            /**
             * Creates a stream of JobExecutionsChanged notifications for a given IoT thing.
             *
             *
             * AWS documentation:
             * https://docs.aws.amazon.com/iot/latest/developerguide/jobs-api.html#mqtt-jobexecutionschanged
             *
             * @param request modeled streaming operation subscription configuration
             * @param options
             *
             * @returns a streaming operation which will emit a modeled event every time a message is received on the
             *    associated MQTT topic
             */
            virtual std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> CreateJobExecutionsChangedStream(
                const JobExecutionsChangedSubscriptionRequest &request,
                const Aws::Iot::RequestResponse::StreamingOperationOptions<JobExecutionsChangedEvent> &options) = 0;

            /**
             *
             *
             *
             * AWS documentation:
             * https://docs.aws.amazon.com/iot/latest/developerguide/jobs-api.html#mqtt-nextjobexecutionchanged
             *
             * @param request modeled streaming operation subscription configuration
             * @param options
             *
             * @returns a streaming operation which will emit a modeled event every time a message is received on the
             *    associated MQTT topic
             */
            virtual std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> CreateNextJobExecutionChangedStream(
                const NextJobExecutionChangedSubscriptionRequest &request,
                const Aws::Iot::RequestResponse::StreamingOperationOptions<NextJobExecutionChangedEvent> &options) = 0;
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
        AWS_IOTJOBS_API std::shared_ptr<IClientV2> NewClientFrom5(
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
        AWS_IOTJOBS_API std::shared_ptr<IClientV2> NewClientFrom311(
            const Aws::Crt::Mqtt::MqttConnection &protocolClient,
            const Aws::Iot::RequestResponse::RequestResponseClientOptions &options,
            Aws::Crt::Allocator *allocator = Aws::Crt::ApiAllocator());

    } // namespace Iotjobs
} // namespace Aws
