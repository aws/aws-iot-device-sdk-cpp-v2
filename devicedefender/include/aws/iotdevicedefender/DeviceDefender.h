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

        using CustomMetricNumberFunction = std::function<int(double *)>;
        using CustomMetricNumberListFunction = std::function<int(Crt::Vector<double> *)>;
        using CustomMetricStringListFunction = std::function<int(Crt::Vector<Crt::String> *)>;
        using CustomMetricIpListFunction = std::function<int(Crt::Vector<Crt::String> *)>;

        /**
         * A base class used to store all custom metrics in the same container. Only used internally.
         */
        class AWS_IOTDEVICEDEFENDER_API CustomMetricBase
        {
          public:
            Crt::Allocator *m_allocator;

            virtual ~CustomMetricBase(){};
        };
        /**
         * A base class used to store all custom number metrics. Only used internally.
         */
        class AWS_IOTDEVICEDEFENDER_API CustomMetricNumber : public CustomMetricBase
        {
          public:
            CustomMetricNumberFunction m_metricFunction;
            CustomMetricNumber(CustomMetricNumberFunction inputFunction, Crt::Allocator *inputAllocator);
            static int GetMetricFunction(double *output, void *data);
        };
        /**
         * A base class used to store all custom number list metrics. Only used internally.
         */
        class AWS_IOTDEVICEDEFENDER_API CustomMetricNumberList : public CustomMetricBase
        {
          public:
            CustomMetricNumberListFunction m_metricFunction;
            CustomMetricNumberList(CustomMetricNumberListFunction inputFunction, Crt::Allocator *inputAllocator);
            static int GetMetricFunction(aws_array_list *output, void *data);
        };
        /**
         * A base class used to store all custom string list metrics. Only used internally.
         */
        class AWS_IOTDEVICEDEFENDER_API CustomMetricStringList : public CustomMetricBase
        {
          public:
            CustomMetricStringListFunction m_metricFunction;
            CustomMetricStringList(CustomMetricStringListFunction inputFunction, Crt::Allocator *inputAllocator);
            static int GetMetricFunction(aws_array_list *output, void *data);
        };
        /**
         * A base class used to store all custom ip list metrics. Only used internally.
         */
        class AWS_IOTDEVICEDEFENDER_API CustomMetricIpList : public CustomMetricBase
        {
          public:
            CustomMetricIpListFunction m_metricFunction;
            CustomMetricIpList(CustomMetricIpListFunction inputFunction, Crt::Allocator *inputAllocator);
            static int GetMetricFunction(aws_array_list *output, void *data);
        };
        // ========

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

            /**
             * Registers a custom metric number function to the Device Defender result. Will call the "metricFunc"
             * function that is passed in each time a report is generated so it's data can be passed along with the
             * other device defender payload data with the metric name of "metricName".
             * @param metricName The key name for the data.
             * @param metricFunc The function that is called to get the number data.
             */
            void RegisterCustomMetricNumber(
                const Crt::String &&metricName,
                CustomMetricNumberFunction &&metricFunc) noexcept;

            /**
             * Registers a custom metric number list function to the Device Defender result. Will call the "metricFunc"
             * function that is passed in each time a report is generated so it's data can be passed along with the
             * other device defender payload data with the metric name of "metricName".
             *
             * @param metricName The key name for the data.
             * @param metricFunc The function that is called to get the number list data.
             */
            void RegisterCustomMetricNumberList(
                const Crt::String &&metricName,
                CustomMetricNumberListFunction &&metricFunc) noexcept;

            /**
             * Registers a custom metric string list function to the Device Defender result. Will call the "metricFunc"
             * function that is passed in each time a report is generated so it's data can be passed along with the
             * other device defender payload data with the metric name of "metricName".
             *
             * Only valid IP addresses will show up in the Device Defender metrics even if it sends correctly.
             * @param metricName The key name for the data.
             * @param metricFunc The function that is called to get the string list data.
             */
            void RegisterCustomMetricStringList(
                const Crt::String &&metricName,
                CustomMetricStringListFunction &&metricFunc) noexcept;

            /**
             * Registers a custom metric IP address list function to the Device Defender result. Will call the
             * "metricFunc" function that is passed in each time a report is generated so it's data can be passed along
             * with the other device defender payload data with the metric name of "metricName".
             *
             * @param metricName The key name for the data.
             * @param metricFunc The function that is called to get the IP address list data.
             */
            void RegisterCustomMetricIpAddressList(
                const Crt::String &&metricName,
                CustomMetricIpListFunction &&metricFunc) noexcept;

            /**
             * Registers a custom metric number that will report the CPU usage automatically into a custom metric called
             * "cpu_usage". Calling this function will make the task report CPU usage each time a report is generated.
             *
             * Note: The CPU usage reported is in percentage ("12.0" = 12% CPU)
             */
            void RegisterCustomMetricCpuUsage() noexcept;

            /**
             * Registers a custom metric number that will report the RAM memory usage automatically into a custom metric
             * called "memory_usage". Calling this function will make the task report memory usage each time a report is
             * generated.
             *
             * Note: The memory usage reported is in kilobytes.
             */
            void RegisterCustomMetricMemoryUsage() noexcept;

            /**
             * Registers a custom metric number that will report the number of processes automatically into a custom
             * metric called "process_count". Calling this function will make the task report processor count each
             * time a report is generated.
             */
            void RegisterCustomMetricProcessCount() noexcept;

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

            // Holds all of the custom metrics created for this task. These are pointers that will be
            // automatically created and cleaned by ReportTask when it is destroyed.
            Crt::Vector<std::shared_ptr<CustomMetricBase>> storedCustomMetrics;

            /**
             * Reports CPU usage to the custom metric
             */
            int getCustomMetricCpuUsage(double *output);

            /**
             * A helper function to get the CPU usage from the computer and populate the passed-in uint64_t
             * pointers.
             * @param totalUser The total user CPU usage
             * @param totalUserLow The low total user CPU usage
             * @param totalSystem The total system CPU usage
             * @param totalIdle The total idle CPU usage
             */
            void getCurrentCpuUsage(
                uint64_t *totalUser,
                uint64_t *totalUserLow,
                uint64_t *totalSystem,
                uint64_t *totalIdle);

            /** Static variables to store the last CPU usage call. */
            uint64_t m_cpuLastTotalUser;
            uint64_t m_cpuLastTotalUserLow;
            uint64_t m_cpuLastTotalSystem;
            uint64_t m_cpuLastTotalIdle;

            /**
             * Reports physical memory usage to the custom metric.
             */
            int getCustomMetricMemoryUsage(double *output);

            /**
             * Reports process count to the custom metric
             */
            int getCustomMetricProcessCount(double *output);
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
