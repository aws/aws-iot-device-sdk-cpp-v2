/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include "aws/common/error.h"
#include "aws/crt/Types.h"
#include "aws/iotdevice/device_defender.h"
#include <aws/common/clock.h>
#include <aws/iotdevicedefender/DeviceDefender.h>

#if defined(__linux__) || defined(__unix__)
#    include <sys/sysinfo.h>
#    include <sys/types.h>
#endif

#include <thread>

namespace Aws
{
    namespace Crt
    {

    }

    namespace Iotdevicedefenderv1
    {
        // Define statics
        unsigned long long ReportTask::s_cpuLastTotalUser = 0;
        unsigned long long ReportTask::s_cpuLastTotalUserLow = 0;
        unsigned long long ReportTask::s_cpuLastTotalSystem = 0;
        unsigned long long ReportTask::s_cpuLastTotalIdle = 0;

        void ReportTask::s_onDefenderV1TaskCancelled(void *userData)
        {
            auto *taskWrapper = reinterpret_cast<ReportTask *>(userData);
            taskWrapper->m_status = ReportTaskStatus::Stopped;

            if (taskWrapper->OnTaskCancelled)
            {
                taskWrapper->OnTaskCancelled(taskWrapper->cancellationUserdata);
            }
        }

        ReportTask::ReportTask(
            Aws::Crt::Allocator *allocator,
            std::shared_ptr<Crt::Mqtt::MqttConnection> mqttConnection,
            const Crt::String &thingName,
            Crt::Io::EventLoopGroup &eventLoopGroup,
            ReportFormat reportFormat,
            uint32_t taskPeriodSeconds,
            uint32_t networkConnectionSamplePeriodSeconds,
            OnTaskCancelledHandler &&onCancelled,
            void *cancellationUserdata) noexcept
            : OnTaskCancelled(std::move(onCancelled)), cancellationUserdata(cancellationUserdata),
              m_allocator(allocator), m_status(ReportTaskStatus::Ready), m_taskConfig{nullptr}, m_owningTask{nullptr},
              m_lastError(0), m_mqttConnection{mqttConnection}, m_eventLoopGroup(eventLoopGroup)
        {
            (void)networkConnectionSamplePeriodSeconds;
            struct aws_byte_cursor thingNameCursor = Crt::ByteCursorFromString(thingName);
            int return_code =
                aws_iotdevice_defender_config_create(&m_taskConfig, allocator, &thingNameCursor, reportFormat);
            if (AWS_OP_SUCCESS == return_code)
            {
                aws_iotdevice_defender_config_set_task_cancelation_fn(m_taskConfig, s_onDefenderV1TaskCancelled);
                aws_iotdevice_defender_config_set_callback_userdata(m_taskConfig, this);
                aws_iotdevice_defender_config_set_task_period_ns(
                    m_taskConfig,
                    aws_timestamp_convert(taskPeriodSeconds, AWS_TIMESTAMP_SECS, AWS_TIMESTAMP_NANOS, NULL));
            }
            else
            {
                m_lastError = aws_last_error();
            }

            // Cache initial CPU usage
            s_getCurrentCpuUsage(
                &s_cpuLastTotalUser, &s_cpuLastTotalUserLow, &s_cpuLastTotalSystem, &s_cpuLastTotalIdle);
        }

        ReportTaskStatus ReportTask::GetStatus() noexcept { return this->m_status; }

        int ReportTask::StartTask() noexcept
        {
            int return_code = AWS_OP_ERR;
            if (m_taskConfig != nullptr && !m_lastError &&
                (this->GetStatus() == ReportTaskStatus::Ready || this->GetStatus() == ReportTaskStatus::Stopped))
            {
                if (AWS_OP_SUCCESS != aws_iotdevice_defender_task_create(
                                          &m_owningTask,
                                          this->m_taskConfig,
                                          m_mqttConnection->GetUnderlyingConnection(),
                                          aws_event_loop_group_get_next_loop(m_eventLoopGroup.GetUnderlyingHandle())))
                {
                    this->m_lastError = aws_last_error();
                }
                else
                {
                    this->m_status = ReportTaskStatus::Running;
                    return_code = AWS_OP_SUCCESS;
                }
            }
            else
            {
                aws_raise_error(AWS_ERROR_INVALID_STATE);
            }
            return return_code;
        }

        void ReportTask::StopTask() noexcept
        {
            if (this->GetStatus() == ReportTaskStatus::Running)
            {
                aws_iotdevice_defender_task_clean_up(this->m_owningTask);
                this->m_owningTask = nullptr;
                m_status = ReportTaskStatus::Stopped;
            }
        }

        ReportTask::~ReportTask()
        {
            StopTask();
            if (m_taskConfig)
            {
                aws_iotdevice_defender_config_clean_up(m_taskConfig);
                this->m_taskConfig = nullptr;
            }
            this->m_owningTask = nullptr;
            this->m_allocator = nullptr;
            this->OnTaskCancelled = nullptr;
            this->cancellationUserdata = nullptr;
        }

        int ReportTask::RegisterCustomMetricNumber(
            aws_byte_cursor metricName,
            aws_iotdevice_defender_get_number_fn *metricFunc)
        {
            return aws_iotdevice_defender_config_register_number_metric(m_taskConfig, &metricName, metricFunc, this);
        }

        int ReportTask::RegisterCustomMetricNumber(
            aws_byte_cursor metricName,
            std::function<int(int64_t *, void *)> *metricFunc)
        {
            return aws_iotdevice_defender_config_register_number_metric(
                m_taskConfig, &metricName, *metricFunc->target<aws_iotdevice_defender_get_number_fn *>(), this);
        }

        int ReportTask::RegisterCustomMetricNumberList(
            aws_byte_cursor metricName,
            aws_iotdevice_defender_get_number_list_fn *metricFunc)
        {
            return aws_iotdevice_defender_config_register_number_list_metric(
                m_taskConfig, &metricName, metricFunc, this);
        }

        int ReportTask::RegisterCustomMetricNumberList(
            aws_byte_cursor metricName,
            std::function<int(aws_array_list *, void *)> *metricFunc)
        {
            return aws_iotdevice_defender_config_register_number_list_metric(
                m_taskConfig, &metricName, *metricFunc->target<aws_iotdevice_defender_get_number_list_fn *>(), this);
        }

        int ReportTask::RegisterCustomMetricStringList(
            aws_byte_cursor metricName,
            aws_iotdevice_defender_get_string_list_fn *metricFunc)
        {
            return aws_iotdevice_defender_config_register_string_list_metric(
                m_taskConfig, &metricName, metricFunc, this);
        }

        int ReportTask::RegisterCustomMetricStringList(
            aws_byte_cursor metricName,
            std::function<int(aws_array_list *, void *)> *metricFunc)
        {
            return aws_iotdevice_defender_config_register_string_list_metric(
                m_taskConfig, &metricName, *metricFunc->target<aws_iotdevice_defender_get_string_list_fn *>(), this);
        }

        int ReportTask::RegisterCustomMetricIpAddressList(
            aws_byte_cursor metricName,
            aws_iotdevice_defender_get_ip_list_fn *metricFunc)
        {
            return aws_iotdevice_defender_config_register_ip_list_metric(m_taskConfig, &metricName, metricFunc, this);
        }
        int ReportTask::RegisterCustomMetricIpAddressList(
            aws_byte_cursor metricName,
            std::function<int(aws_array_list *, void *)> *metricFunc)
        {
            return aws_iotdevice_defender_config_register_ip_list_metric(
                m_taskConfig, &metricName, *metricFunc->target<aws_iotdevice_defender_get_ip_list_fn *>(), this);
        }

        int ReportTask::RegisterCustomMetricCpuUsage()
        {
            return RegisterCustomMetricNumber(aws_byte_cursor_from_c_str("cpu_usage"), &s_getCustomMetricCpuUsage);
        }

        int ReportTask::s_getCustomMetricCpuUsage(int64_t *output, void *data)
        {
            (void)(data); // prevent warnings over unused parameter
// Get the CPU usage from Linux
#if defined(__linux__) || defined(__unix__)
            int return_result = AWS_OP_ERR;
            unsigned long long totalUser, totalUserLow, totalSystem, totalIdle, total;
            s_getCurrentCpuUsage(&totalUser, &totalUserLow, &totalSystem, &totalIdle);
            double percent;

            // Overflow detection
            if (totalUser < s_cpuLastTotalUser || totalUserLow < s_cpuLastTotalUserLow ||
                totalSystem < s_cpuLastTotalSystem || totalIdle < s_cpuLastTotalIdle)
            {
                *output = 0;
            }
            else
            {
                total = (totalUser - s_cpuLastTotalUser) + (totalUserLow - s_cpuLastTotalUserLow) +
                        (totalSystem - s_cpuLastTotalSystem);
                percent = total;
                total += totalIdle - s_cpuLastTotalIdle;
                percent = (percent / total) * 100;

                // If percent is negative, then there was an error (overflow?)
                if (percent < 0)
                {
                    *output = 0;
                    return_result = AWS_OP_ERR;
                }
                else
                {
                    *output = (int64_t)percent;
                    return_result = AWS_OP_SUCCESS;
                }
            }

            s_cpuLastTotalUser = totalUser;
            s_cpuLastTotalUserLow = totalUserLow;
            s_cpuLastTotalSystem = totalSystem;
            s_cpuLastTotalIdle = totalIdle;

            return return_result;
#endif

            // OS not supported? Just return an error and set the output to 0
            *output = 0;
            return AWS_OP_ERR;
        }

        void ReportTask::s_getCurrentCpuUsage(
            unsigned long long *totalUser,
            unsigned long long *totalUserLow,
            unsigned long long *totalSystem,
            unsigned long long *totalIdle)
        {
            *totalUser = 0;    // prevent warnings over unused parameter on Windows and Mac
            *totalUserLow = 0; // prevent warnings over unused parameter on Windows and Mac
            *totalSystem = 0;  // prevent warnings over unused parameter on Windows and Mac
            *totalIdle = 0;    // prevent warnings over unused parameter on Windows and Mac

// Get the CPU usage from Linux
#if defined(__linux__) || defined(__unix__)
            FILE *file;
            int matchedResults;
            file = fopen("/proc/stat", "r");
            matchedResults = fscanf(file, "cpu %llu %llu %llu %llu", totalUser, totalUserLow, totalSystem, totalIdle);
            fclose(file);
            if (matchedResults == EOF || matchedResults != 4)
            {
                aws_raise_error(AWS_ERROR_IOTDEVICE_DEFENDER_INVALID_REPORT_INTERVAL);
            }
            return;
#endif
        }

        int ReportTask::RegisterCustomMetricMemoryUsage()
        {
            return RegisterCustomMetricNumber(
                aws_byte_cursor_from_c_str("memory_usage"), &s_getCustomMetricMemoryUsage);
        }

        int ReportTask::s_getCustomMetricMemoryUsage(int64_t *output, void *data)
        {
            (void)(data); // prevent warnings over unused parameter
// Get the Memory usage from Linux
#if defined(__linux__) || defined(__unix__)
            struct sysinfo memoryInfo;
            sysinfo(&memoryInfo);
            unsigned long long physicalMemoryUsed = memoryInfo.totalram - memoryInfo.freeram;
            physicalMemoryUsed *= memoryInfo.mem_unit;
            // Return data in Kilobytes
            physicalMemoryUsed = physicalMemoryUsed / (1024);
            *output = (int64_t)physicalMemoryUsed;
            return AWS_OP_SUCCESS;
#endif

            // OS not supported? Just return an error and set the output to 0
            *output = 0;
            return AWS_OP_ERR;
        }

        int ReportTask::RegisterCustomMetricProcessorCount()
        {
            return RegisterCustomMetricNumber(
                aws_byte_cursor_from_c_str("processor_count"), &s_getCustomMetricProcessorCount);
        }

        int ReportTask::s_getCustomMetricProcessorCount(int64_t *output, void *data)
        {
            (void)(data); // prevent warnings over unused parameter
            *output = (int64_t)std::thread::hardware_concurrency();
            return AWS_OP_SUCCESS;
        }

        ReportTaskBuilder::ReportTaskBuilder(
            Aws::Crt::Allocator *allocator,
            std::shared_ptr<Crt::Mqtt::MqttConnection> mqttConnection,
            Crt::Io::EventLoopGroup &eventLoopGroup,
            const Crt::String &thingName)
            : m_allocator(allocator), m_mqttConnection(mqttConnection), m_thingName(thingName),
              m_eventLoopGroup(eventLoopGroup)
        {
            m_reportFormat = ReportFormat::AWS_IDDRF_JSON;
            m_taskPeriodSeconds = 5UL * 60UL;
            m_networkConnectionSamplePeriodSeconds = 5UL * 60UL;
            m_onCancelled = nullptr;
            m_cancellationUserdata = nullptr;
        }

        ReportTaskBuilder &ReportTaskBuilder::WithReportFormat(ReportFormat reportFormat) noexcept
        {
            m_reportFormat = reportFormat;
            return *this;
        }

        ReportTaskBuilder &ReportTaskBuilder::WithTaskPeriodSeconds(uint32_t taskPeriodSeconds) noexcept
        {
            m_taskPeriodSeconds = taskPeriodSeconds;
            return *this;
        }

        ReportTaskBuilder &ReportTaskBuilder::WithNetworkConnectionSamplePeriodSeconds(
            uint32_t networkConnectionSamplePeriodSeconds) noexcept
        {
            m_networkConnectionSamplePeriodSeconds = networkConnectionSamplePeriodSeconds;
            return *this;
        }

        ReportTaskBuilder &ReportTaskBuilder::WithTaskCancelledHandler(OnTaskCancelledHandler &&onCancelled) noexcept
        {
            m_onCancelled = std::move(onCancelled);
            return *this;
        }

        ReportTaskBuilder &ReportTaskBuilder::WithTaskCancellationUserData(void *cancellationUserdata) noexcept
        {
            m_cancellationUserdata = cancellationUserdata;
            return *this;
        }

        std::shared_ptr<ReportTask> ReportTaskBuilder::Build() noexcept
        {
            return std::shared_ptr<ReportTask>(new ReportTask(
                m_allocator,
                m_mqttConnection,
                m_thingName,
                m_eventLoopGroup,
                m_reportFormat,
                m_taskPeriodSeconds,
                m_networkConnectionSamplePeriodSeconds,
                static_cast<OnTaskCancelledHandler &&>(m_onCancelled),
                m_cancellationUserdata));
        }

    } // namespace Iotdevicedefenderv1
} // namespace Aws
