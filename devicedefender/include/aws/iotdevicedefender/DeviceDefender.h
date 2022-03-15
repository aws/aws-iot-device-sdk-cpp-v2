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

            /**
             * Registers a custom metric number function to the Device Defender result. Will call the "metricFunc"
             * function that is passed in each time a report is generated so it's data can be passed along with the
             * other device defender payload data with the metric name of "metricName".
             * @param metricName The key name for the data.
             * @param metricFunc The function that is called to get the number data.
             * @return AWS_OP_SUCCESS if the custom metric was registered successfully.
             *          Will return AWS_OP_ERR if the number cannot be registered or the data passed
             *          ("metricName" or "metricFunc") is incorrect.
             */
            int RegisterCustomMetricNumber(
                aws_byte_cursor metricName,
                aws_iotdevice_defender_get_number_fn *metricFunc);

            /**
             * Registers a custom metric number function to the Device Defender result. Will call the "metricFunc"
             * function that is passed in each time a report is generated so it's data can be passed along with the
             * other device defender payload data with the metric name of "metricName".
             * @param metricName The key name for the data.
             * @param metricFunc The function that is called to get the number data.
             * @return AWS_OP_SUCCESS if the custom metric was registered successfully.
             *          Will return AWS_OP_ERR if the number cannot be registered or the data passed
             *          ("metricName" or "metricFunc") is incorrect.
             */
            int RegisterCustomMetricNumber(
                aws_byte_cursor metricName,
                std::function<int(int64_t *, void *)> *metricFunc);

            /**
             * Registers a custom metric number list function to the Device Defender result. Will call the "metricFunc"
             * function that is passed in each time a report is generated so it's data can be passed along with the
             * other device defender payload data with the metric name of "metricName".
             * @param metricName The key name for the data.
             * @param metricFunc The function that is called to get the number list data.
             * @return AWS_OP_SUCCESS if the custom metric was registered successfully.
             *          Will return AWS_OP_ERR if the number list cannot be registered or the data passed
             *          ("metricName" or "metricFunc") is incorrect.
             */
            int RegisterCustomMetricNumberList(
                aws_byte_cursor metricName,
                aws_iotdevice_defender_get_number_list_fn *metricFunc);

            /**
             * Registers a custom metric number list function to the Device Defender result. Will call the "metricFunc"
             * function that is passed in each time a report is generated so it's data can be passed along with the
             * other device defender payload data with the metric name of "metricName".
             * @param metricName The key name for the data.
             * @param metricFunc The function that is called to get the number list data.
             * @return AWS_OP_SUCCESS if the custom metric was registered successfully.
             *          Will return AWS_OP_ERR if the number list cannot be registered or the data passed
             *          ("metricName" or "metricFunc") is incorrect.
             */
            int RegisterCustomMetricNumberList(
                aws_byte_cursor metricName,
                std::function<int(aws_array_list *, void *)> *metricFunc);

            /**
             * Registers a custom metric string list function to the Device Defender result. Will call the "metricFunc"
             * function that is passed in each time a report is generated so it's data can be passed along with the
             * other device defender payload data with the metric name of "metricName".
             * @param metricName The key name for the data.
             * @param metricFunc The function that is called to get the string list data.
             * @return AWS_OP_SUCCESS if the custom metric was registered successfully.
             *          Will return AWS_OP_ERR if the string list cannot be registered or the data passed
             *          ("metricName" or "metricFunc") is incorrect.
             */
            int RegisterCustomMetricStringList(
                aws_byte_cursor metricName,
                aws_iotdevice_defender_get_string_list_fn *metricFunc);

            /**
             * Registers a custom metric string list function to the Device Defender result. Will call the "metricFunc"
             * function that is passed in each time a report is generated so it's data can be passed along with the
             * other device defender payload data with the metric name of "metricName".
             * @param metricName The key name for the data.
             * @param metricFunc The function that is called to get the string list data.
             * @return AWS_OP_SUCCESS if the custom metric was registered successfully.
             *          Will return AWS_OP_ERR if the string list cannot be registered or the data passed
             *          ("metricName" or "metricFunc") is incorrect.
             */
            int RegisterCustomMetricStringList(
                aws_byte_cursor metricName,
                std::function<int(aws_array_list *, void *)> *metricFunc);

            /**
             * Registers a custom metric IP address list function to the Device Defender result. Will call the
             * "metricFunc" function that is passed in each time a report is generated so it's data can be passed along
             * with the other device defender payload data with the metric name of "metricName".
             * @param metricName The key name for the data.
             * @param metricFunc The function that is called to get the IP address list data.
             * @return AWS_OP_SUCCESS if the custom metric was registered successfully.
             *          Will return AWS_OP_ERR if the IP address list cannot be registered or the data passed
             *          ("metricName" or "metricFunc") is incorrect.
             */
            int RegisterCustomMetricIpAddressList(
                aws_byte_cursor metricName,
                aws_iotdevice_defender_get_ip_list_fn *metricFunc);

            /**
             * Registers a custom metric IP address list function to the Device Defender result. Will call the
             * "metricFunc" function that is passed in each time a report is generated so it's data can be passed along
             * with the other device defender payload data with the metric name of "metricName".
             * @param metricName The key name for the data.
             * @param metricFunc The function that is called to get the IP address list data.
             * @return AWS_OP_SUCCESS if the custom metric was registered successfully.
             *          Will return AWS_OP_ERR if the IP address list cannot be registered or the data passed
             *          ("metricName" or "metricFunc") is incorrect.
             */
            int RegisterCustomMetricIpAddressList(
                aws_byte_cursor metricName,
                std::function<int(aws_array_list *, void *)> *metricFunc);
            
            /**
             * Registers a custom metric number that will report the CPU usage automatically into a custom metric called
             * "cpu_usage". Calling this function will make the task report CPU usage each time a report is generated.
             * 
             * Note: The CPU usage reported is in percentage ("12" = 12% CPU)
             * @return AWS_OP_SUCCESS if the custom metric was registered successfully.
             *      Will return AWS_OP_ERR if the CPU usage cannot be registered
             */
            int RegisterCustomMetricCpuUsage();

            /**
             * Registers a custom metric number that will report the RAM memory usage automatically into a custom metric called
             * "memory_usage". Calling this function will make the task report memory usage each time a report is generated.
             * 
             * Note: The memory usage reported is in kilobytes.
             * @return AWS_OP_SUCCESS if the custom metric was registered successfully.
             *      Will return AWS_OP_ERR if the CPU usage cannot be registered
             */
            int RegisterCustomMetricMemoryUsage();

            /**
             * Registers a custom metric number that will report the number of processors automatically into a custom metric called
             * "processor_count". Calling this function will make the task report processor count each time a report is generated.
             * @return AWS_OP_SUCCESS if the custom metric was registered successfully.
             *      Will return AWS_OP_ERR if the CPU usage cannot be registered
             */
            int RegisterCustomMetricProcessorCount();

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

            /**
             * Reports CPU usage to the custom metric
             */
            static int s_getCustomMetricCpuUsage(int64_t *output, void *data);

            /**
             * A helper function to get the CPU usage from the computer and populate the passed-in unsigned long long pointers.
             * @param totalUser The total user CPU usage
             * @param totalUserLow The low total user CPU usage
             * @param totalSystem The total system CPU usage
             * @param totalIdle The total idle CPU usage
             */
            static void s_getCurrentCpuUsage(
                unsigned long long *totalUser, unsigned long long *totalUserLow,
                unsigned long long *totalSystem, unsigned long long *totalIdle);

            /** Static variables to store the last CPU usage call. */
            static unsigned long long s_cpuLastTotalUser;
            static unsigned long long s_cpuLastTotalUserLow;
            static unsigned long long s_cpuLastTotalSystem;
            static unsigned long long s_cpuLastTotalIdle;

            /**
             * Reports physical memory usage to the custom metric.
             */
            static int s_getCustomMetricMemoryUsage(int64_t *output, void *data);

            /**
             * Reports processor count to the custom metric
             */
            static int s_getCustomMetricProcessorCount(int64_t *output, void *data);
            
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
