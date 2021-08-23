#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/iotdevicedefender/Exports.h>

#include <aws/crt/io/EventLoopGroup.h>
#include <aws/crt/mqtt/MqttClient.h>

#include <aws/iotdevice/device_defender.h>

namespace Aws
{
    namespace Crt
    {

    }

    namespace Iotdevicedefenderv1
    {

        class ReportTask;
        class ReportTaskBuilder;

        /**
         * Invoked upon DeviceDefender V1 task cancellation.
         */
        using OnTaskCancelledHandler = std::function<void(void *)>;

        using ReportFormat = aws_iotdevice_defender_report_format;

        /**
         * Enum used to expose the status of a DeviceDefenderV1 task.
         */
        enum class ReportTaskStatus
        {
            Ready = 0,
            Running = 1,
            Stopped = 2,
        };

        /**
         * Represents a persistent DeviceDefender V1 task.
         */
        class AWS_IOTDEVICEDEFENDER_API ReportTask final
        {
            friend ReportTaskBuilder;

          public:
            ~ReportTask();
            ReportTask(const ReportTask &) = delete;
            ReportTask &operator=(const ReportTask &) = delete;

            /**
             * Initiates stopping of the Defender V1 task.
             */
            void StopTask() noexcept;

            /**
             * Initiates Defender V1 reporting task.
             */
            int StartTask() noexcept;

            /**
             * Returns the task status.
             */
            ReportTaskStatus GetStatus() noexcept;

            OnTaskCancelledHandler OnTaskCancelled;

            void *cancellationUserdata;

            /**
             * @return the value of the last aws error encountered by operations on this instance.
             */
            int LastError() const noexcept { return m_lastError; }

          private:
            Crt::Allocator *m_allocator;
            ReportTaskStatus m_status;
            aws_iotdevice_defender_task_config *m_taskConfig;
            aws_iotdevice_defender_task *m_owningTask;
            int m_lastError;
            std::shared_ptr<Crt::Mqtt::MqttConnection> m_mqttConnection;
            Crt::Io::EventLoopGroup &m_eventLoopGroup;

            ReportTask(
                Crt::Allocator *allocator,
                std::shared_ptr<Crt::Mqtt::MqttConnection> mqttConnection,
                const Crt::String &thingName,
                Crt::Io::EventLoopGroup &eventLoopGroup,
                ReportFormat reportFormat,
                uint32_t taskPeriodSeconds,
                uint32_t networkConnectionSamplePeriodSeconds,
                OnTaskCancelledHandler &&onCancelled = NULL,
                void *cancellationUserdata = nullptr) noexcept;

            static void s_onDefenderV1TaskCancelled(void *userData);
        };

        /**
         * Represents a builder for creating a ReportTask object.
         */
        class AWS_IOTDEVICEDEFENDER_API ReportTaskBuilder final
        {
          public:
            ReportTaskBuilder(
                Crt::Allocator *allocator,
                std::shared_ptr<Crt::Mqtt::MqttConnection> mqttConnection,
                Crt::Io::EventLoopGroup &eventLoopGroup,
                const Crt::String &thingName);

            /**
             * Sets the device defender report format, or defaults to AWS_IDDRF_JSON.
             */
            ReportTaskBuilder &WithReportFormat(ReportFormat reportFormat) noexcept;

            /**
             * Sets the task period seconds. Defaults to 5 minutes.
             */
            ReportTaskBuilder &WithTaskPeriodSeconds(uint32_t taskPeriodSeconds) noexcept;

            /**
             * Sets the network connection sample period seconds. Defaults to 5 minutes.
             */
            ReportTaskBuilder &WithNetworkConnectionSamplePeriodSeconds(
                uint32_t networkConnectionSamplePeriodSeconds) noexcept;

            /**
             * Sets the task cancelled handler function.
             */
            ReportTaskBuilder &WithTaskCancelledHandler(OnTaskCancelledHandler &&onCancelled) noexcept;

            /**
             * Sets the user data for the task cancelled handler function.
             */
            ReportTaskBuilder &WithTaskCancellationUserData(void *cancellationUserdata) noexcept;

            /**
             * Builds a device defender v1 task object from the set options.
             */
            std::shared_ptr<ReportTask> Build() noexcept;

          private:
            Crt::Allocator *m_allocator;
            std::shared_ptr<Crt::Mqtt::MqttConnection> m_mqttConnection;
            Crt::String m_thingName;
            Crt::Io::EventLoopGroup &m_eventLoopGroup;
            ReportFormat m_reportFormat;
            uint32_t m_taskPeriodSeconds;
            uint32_t m_networkConnectionSamplePeriodSeconds;
            OnTaskCancelledHandler m_onCancelled;
            void *m_cancellationUserdata;
        };

    } // namespace Iotdevicedefenderv1
} // namespace Aws
