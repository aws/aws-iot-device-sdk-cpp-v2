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

namespace Aws
{
    namespace Crt
    {

    }

    namespace Iotdevicedefenderv1
    {
        // Custom number metric setup and metric function getter.
        void CustomMetricNumber::SetCustomMetricData(CustomMetricNumberFunction inputFunction, Crt::Allocator *inputAllocator)
        {
            function = std::move(inputFunction);
            m_allocator = inputAllocator;
        }
        int CustomMetricNumber::GetMetricFunction(double *output, void* data)
        {
            CustomMetricNumber *metric = (CustomMetricNumber *)data;
            return metric->function(output);
        }

        // Custom number list metric setup and metric function getter.
        void CustomMetricNumberList::SetCustomMetricData(CustomMetricNumberListFunction inputFunction, Crt::Allocator *inputAllocator)
        {
            function = std::move(inputFunction);
            m_allocator = inputAllocator;
        }
        int CustomMetricNumberList::GetMetricFunction(aws_array_list *output, void* data)
        {
            CustomMetricNumberList *metric = (CustomMetricNumberList *)data;
            Crt::Vector<double> function_data = Crt::Vector<double>();
            int returnValue = metric->function(&function_data);
            for (size_t i = 0; i < function_data.size(); i++)
            {
                aws_array_list_push_back(output, &function_data.at(i));
            }
            return returnValue;
        }

        // Custom string list metric setup and metric function getter.
        void CustomMetricStringList::SetCustomMetricData(CustomMetricStringListFunction inputFunction, Crt::Allocator *inputAllocator)
        {
            function = std::move(inputFunction);
            m_allocator = inputAllocator;
        }
        int CustomMetricStringList::GetMetricFunction(aws_array_list *output, void* data)
        {
            CustomMetricStringList *metric = (CustomMetricStringList *)data;
            Crt::Vector<Crt::String> function_data = Crt::Vector<Crt::String>();
            int returnValue = metric->function(&function_data);
            for (size_t i = 0; i < function_data.size(); i++)
            {
                aws_string *tmp_str =
                    aws_string_new_from_c_str(metric->m_allocator, function_data[i].c_str());
                aws_array_list_push_back(output, &tmp_str);
            }
            return returnValue;
        }

        // Custom ip list metric setup and metric function getter.
        void CustomMetricIpList::SetCustomMetricData(CustomMetricIpListFunction inputFunction, Crt::Allocator *inputAllocator)
        {
            function = std::move(inputFunction);
            m_allocator = inputAllocator;
        }
        int CustomMetricIpList::GetMetricFunction(aws_array_list *output, void* data)
        {
            CustomMetricIpList *metric = (CustomMetricIpList *)data;
            Crt::Vector<Crt::String> function_data = Crt::Vector<Crt::String>();
            int returnValue = metric->function(&function_data);
            for (size_t i = 0; i < function_data.size(); i++)
            {
                aws_string *tmp_str =
                    aws_string_new_from_c_str(metric->m_allocator, function_data[i].c_str());
                aws_array_list_push_back(output, &tmp_str);
            }
            return returnValue;
        }

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
            getCurrentCpuUsage(
                &m_cpuLastTotalUser, &m_cpuLastTotalUserLow, &m_cpuLastTotalSystem, &m_cpuLastTotalIdle);
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
            for (size_t i = 0; i < storedCustomMetrics.size(); i++)
            {
                Aws::Crt::Delete(storedCustomMetrics[i], m_allocator);
            }
            this->m_owningTask = nullptr;
            this->m_allocator = nullptr;
            this->OnTaskCancelled = nullptr;
            this->cancellationUserdata = nullptr;
        }

        void ReportTask::RegisterCustomMetricNumber(
            const Crt::String &metricName,
            CustomMetricNumberFunction &metricFunc) noexcept
        {
            CustomMetricNumber* data = Aws::Crt::New<CustomMetricNumber>(m_allocator);
            data->SetCustomMetricData(metricFunc, m_allocator);
            storedCustomMetrics.push_back(data);
            aws_byte_cursor cursor = aws_byte_cursor_from_c_str(metricName.c_str());
            aws_iotdevice_defender_config_register_number_metric(m_taskConfig, &cursor, data->GetMetricFunction, data);
        }

        void ReportTask::RegisterCustomMetricNumberList(
            const Crt::String &metricName,
            CustomMetricNumberListFunction &metricFunc) noexcept
        {
            CustomMetricNumberList* data = Aws::Crt::New<CustomMetricNumberList>(m_allocator);
            data->SetCustomMetricData(metricFunc, m_allocator);
            storedCustomMetrics.push_back(data);
            aws_byte_cursor cursor = aws_byte_cursor_from_c_str(metricName.c_str());
            aws_iotdevice_defender_config_register_number_list_metric(m_taskConfig, &cursor, data->GetMetricFunction, data);
        }

        void ReportTask::RegisterCustomMetricStringList(
            const Crt::String &metricName,
            CustomMetricStringListFunction &metricFunc) noexcept
        {
            CustomMetricStringList* data = Aws::Crt::New<CustomMetricStringList>(m_allocator);
            data->SetCustomMetricData(metricFunc, m_allocator);
            storedCustomMetrics.push_back(data);
            aws_byte_cursor cursor = aws_byte_cursor_from_c_str(metricName.c_str());
            aws_iotdevice_defender_config_register_string_list_metric(m_taskConfig, &cursor, data->GetMetricFunction, data);
        }

        void ReportTask::RegisterCustomMetricIpAddressList(
            const Crt::String &metricName,
            CustomMetricIpListFunction &metricFunc) noexcept
        {
            CustomMetricIpList* data = Aws::Crt::New<CustomMetricIpList>(m_allocator);
            data->SetCustomMetricData(metricFunc, m_allocator);
            storedCustomMetrics.push_back(data);
            aws_byte_cursor cursor = aws_byte_cursor_from_c_str(metricName.c_str());
            aws_iotdevice_defender_config_register_ip_list_metric(m_taskConfig, &cursor, data->GetMetricFunction, data);
        }

        void ReportTask::RegisterCustomMetricCpuUsage() noexcept
        {
            CustomMetricNumberFunction func = std::bind(&ReportTask::getCustomMetricCpuUsage, this, std::placeholders::_1);
            RegisterCustomMetricNumber("cpu_usage", func);
        }

        int ReportTask::getCustomMetricCpuUsage(double *output)
        {
// Get the CPU usage from Linux
#if defined(__linux__) || defined(__unix__)
            int return_result = AWS_OP_ERR;
            uint64_t totalUser, totalUserLow, totalSystem, totalIdle, total;
            getCurrentCpuUsage(&totalUser, &totalUserLow, &totalSystem, &totalIdle);
            double percent;

            // Overflow detection
            if (totalUser < m_cpuLastTotalUser || totalUserLow < m_cpuLastTotalUserLow ||
                totalSystem < m_cpuLastTotalSystem || totalIdle < m_cpuLastTotalIdle)
            {
                *output = 0;
            }
            else
            {
                total = (totalUser - m_cpuLastTotalUser) + (totalUserLow - m_cpuLastTotalUserLow) +
                        (totalSystem - m_cpuLastTotalSystem);
                percent = total;
                total += totalIdle - m_cpuLastTotalIdle;
                percent = (percent / total) * 100;

                // If percent is negative, then there was an error (overflow?)
                if (percent < 0)
                {
                    *output = 0;
                    return_result = AWS_OP_ERR;
                }
                else
                {
                    *output = percent;
                    return_result = AWS_OP_SUCCESS;
                }
            }

            m_cpuLastTotalUser = totalUser;
            m_cpuLastTotalUserLow = totalUserLow;
            m_cpuLastTotalSystem = totalSystem;
            m_cpuLastTotalIdle = totalIdle;

            return return_result;
#endif

            // OS not supported? Just return an error and set the output to 0
            *output = 0;
            return AWS_OP_ERR;
        }

        void ReportTask::getCurrentCpuUsage(
            uint64_t *totalUser,
            uint64_t *totalUserLow,
            uint64_t *totalSystem,
            uint64_t *totalIdle)
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
            matchedResults = fscanf(
                file, "cpu %llu %llu %llu %llu",
                (long long unsigned int *) totalUser,
                (long long unsigned int *) totalUserLow,
                (long long unsigned int *) totalSystem,
                (long long unsigned int *) totalIdle);
            fclose(file);
            if (matchedResults == EOF || matchedResults != 4)
            {
                aws_raise_error(AWS_ERROR_IOTDEVICE_DEFENDER_INVALID_REPORT_INTERVAL);
            }
            return;
#endif
        }

        void ReportTask::RegisterCustomMetricMemoryUsage() noexcept
        {
            CustomMetricNumberFunction func = std::bind(&ReportTask::getCustomMetricMemoryUsage, this, std::placeholders::_1);
            RegisterCustomMetricNumber("memory_usage", func);
        }

        int ReportTask::getCustomMetricMemoryUsage(double *output)
        {
// Get the Memory usage from Linux
#if defined(__linux__) || defined(__unix__)
            struct sysinfo memoryInfo;
            sysinfo(&memoryInfo);
            uint64_t physicalMemoryUsed = memoryInfo.totalram - memoryInfo.freeram;
            physicalMemoryUsed *= memoryInfo.mem_unit;
            // Return data in Kilobytes
            physicalMemoryUsed = physicalMemoryUsed / (1024);
            *output = (double)physicalMemoryUsed;
            return AWS_OP_SUCCESS;
#endif

            // OS not supported? Just return an error and set the output to 0
            *output = 0;
            return AWS_OP_ERR;
        }

        void ReportTask::RegisterCustomMetricProcessCount() noexcept
        {
            CustomMetricNumberFunction func = std::bind(&ReportTask::getCustomMetricProcessCount, this, std::placeholders::_1);
            RegisterCustomMetricNumber("process_count", func);
        }

        int ReportTask::getCustomMetricProcessCount(double *output)
        {
// Get the process count from Linux
#if defined(__linux__) || defined(__unix__)
            struct sysinfo systemInfo;
            sysinfo(&systemInfo);
            *output = (double)systemInfo.procs;
            return AWS_OP_SUCCESS;
#endif
            // OS not supported? Just return an error and set the output to 0
            *output = 0;
            return AWS_OP_ERR;
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
