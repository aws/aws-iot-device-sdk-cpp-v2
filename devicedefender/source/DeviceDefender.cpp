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

            fprintf(stdout, "\n\nTask finished!\n\n");
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

        void ReportTask::RegisterCustomMetricNumber(
            aws_byte_cursor metricName,
            std::function<int(double*)> metricFunc)
        {
            m_storedCustomMetricsNumberFunctions.push_back(metricFunc);
            customMetricData data;
            data.index = m_storedCustomMetricsNumberFunctions.size() - 1;
            data.task = this;
            storedCustomMetricData.push_back(data);
            aws_iotdevice_defender_config_register_number_metric(
                m_taskConfig, &metricName, s_getCustomMetricNumber, &data);
        }

        void ReportTask::RegisterCustomMetricNumberList(
            aws_byte_cursor metricName,
            std::function<int(std::vector<double> *)> &metricFunc)
        {
            m_storedCustomMetricsNumberListFunctions.push_back(metricFunc);
            customMetricData data;
            data.index = m_storedCustomMetricsNumberListFunctions.size() - 1;
            data.task = this;
            storedCustomMetricData.push_back(data);
            aws_iotdevice_defender_config_register_number_list_metric(
                m_taskConfig, &metricName, s_getCustomMetricNumberList, &data);
        }

        void ReportTask::RegisterCustomMetricStringList(
            aws_byte_cursor metricName,
            std::function<int(std::vector<std::string> *)> &metricFunc)
        {
            m_storedCustomMetricsStringListFunctions.push_back(metricFunc);
            customMetricData data;
            data.index = m_storedCustomMetricsStringListFunctions.size() - 1;
            data.task = this;
            storedCustomMetricData.push_back(data);
            aws_iotdevice_defender_config_register_string_list_metric(
                m_taskConfig, &metricName, s_getCustomMetricStringList, &data);
        }

        void ReportTask::RegisterCustomMetricIpAddressList(
            aws_byte_cursor metricName,
            std::function<int(std::vector<std::string> *)> &metricFunc)
        {
            m_storedCustomMetricsIpListFunctions.push_back(metricFunc);
            customMetricData data;
            data.index = m_storedCustomMetricsIpListFunctions.size() - 1;
            data.task = this;
            storedCustomMetricData.push_back(data);
            aws_iotdevice_defender_config_register_ip_list_metric(
                m_taskConfig, &metricName, s_getCustomMetricIpList, &data);
        }

        std::function<int(double*)> *ReportTask::GetStoredCustomMetricNumber(size_t &index)
        {
            if (index >= 0 && index < m_storedCustomMetricsNumberFunctions.size())
            {
                return &m_storedCustomMetricsNumberFunctions[index];
            }
            return nullptr;
        }

        std::function<int(std::vector<double>*)> *ReportTask::GetStoredCustomMetricNumberList(size_t &index)
        {
            if (index >= 0 && index < m_storedCustomMetricsNumberListFunctions.size())
            {
                return &m_storedCustomMetricsNumberListFunctions[index];
            }
            return nullptr;
        }

        std::function<int(std::vector<std::string>*)> *ReportTask::GetStoredCustomMetricStringList(size_t &index)
        {
            if (index >= 0 && index < m_storedCustomMetricsStringListFunctions.size())
            {
                return &m_storedCustomMetricsStringListFunctions[index];
            }
            return nullptr;
        }

        std::function<int(std::vector<std::string>*)> *ReportTask::GetStoredCustomMetricIpList(size_t &index)
        {
            if (index >= 0 && index < m_storedCustomMetricsIpListFunctions.size())
            {
                return &m_storedCustomMetricsIpListFunctions[index];
            }
            return nullptr;
        }

        int ReportTask::s_getCustomMetricNumber(double *output, void *data)
        {
            customMetricData *storedData = (customMetricData*)data;
            std::function<int(double*)> *tmp = storedData->task->GetStoredCustomMetricNumber(storedData->index);
            if (tmp == nullptr) {
                return AWS_OP_ERR;
            }
            std::function<int(double*)> tmpRef = *tmp;
            int returnValue = tmpRef(output);
            return returnValue;
        }

        int ReportTask::s_getCustomMetricNumberList(aws_array_list *output, void *data)
        {
            customMetricData *storedData = (customMetricData*)data;
            std::function<int(std::vector<double>*)> *tmp = storedData->task->GetStoredCustomMetricNumberList(storedData->index);
            if (tmp == nullptr) {
                return AWS_OP_ERR;
            }
            std::function<int(std::vector<double>*)> tmpRef = *tmp;

            std::vector<double> function_data = std::vector<double>();
            int returnValue = tmpRef(&function_data);

            for (size_t i = 0; i < function_data.size(); i++)
            {
                aws_array_list_push_back(output, &function_data.at(i));
            }
            
            return returnValue;
        }

        int ReportTask::s_getCustomMetricStringList(aws_array_list *output, void *data)
        {
            customMetricData *storedData = (customMetricData*)data;
            std::function<int(std::vector<std::string>*)> *tmp = storedData->task->GetStoredCustomMetricStringList(storedData->index);
            if (tmp == nullptr) {
                return AWS_OP_ERR;
            }
            std::function<int(std::vector<std::string>*)> tmpRef = *tmp;

            std::vector<std::string> function_data = std::vector<std::string>();
            int returnValue = tmpRef(&function_data);

            for (size_t i = 0; i < function_data.size(); i++)
            {
                aws_string *tmp_str = aws_string_new_from_c_str(storedData->task->m_allocator, function_data[i].c_str());
                aws_array_list_push_back(output, &tmp_str);
            }
            
            return returnValue;
        }

        int ReportTask::s_getCustomMetricIpList(aws_array_list *output, void *data)
        {
            customMetricData *storedData = (customMetricData*)data;
            std::function<int(std::vector<std::string>*)> *tmp = storedData->task->GetStoredCustomMetricIpList(storedData->index);
            if (tmp == nullptr) {
                return AWS_OP_ERR;
            }
            std::function<int(std::vector<std::string>*)> tmpRef = *tmp;

            std::vector<std::string> function_data = std::vector<std::string>();
            int returnValue = tmpRef(&function_data);

            for (size_t i = 0; i < function_data.size(); i++)
            {
                aws_string *tmp_str = aws_string_new_from_c_str(storedData->task->m_allocator, function_data[i].c_str());
                aws_array_list_push_back(output, &tmp_str);
            }
            return returnValue;
        }

        void ReportTask::RegisterCustomMetricCpuUsage()
        {
            //std::function<int(double *, void *)> func = ReportTask::s_getCustomMetricCpuUsage;
            //RegisterCustomMetricNumber(aws_byte_cursor_from_c_str("cpu_usage"), func);
        }

        int ReportTask::s_getCustomMetricCpuUsage(double *output, void *data)
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
                    *output = percent;
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

        void ReportTask::RegisterCustomMetricMemoryUsage()
        {
            //std::function<int(double *, void *)> func = ReportTask::s_getCustomMetricMemoryUsage;
            //RegisterCustomMetricNumber(aws_byte_cursor_from_c_str("memory_usage"), func);
        }

        int ReportTask::s_getCustomMetricMemoryUsage(double *output, void *data)
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
            *output = (double)physicalMemoryUsed;
            return AWS_OP_SUCCESS;
#endif

            // OS not supported? Just return an error and set the output to 0
            *output = 0;
            return AWS_OP_ERR;
        }

        void ReportTask::RegisterCustomMetricProcessCount()
        {
            //std::function<int(double *, void *)> func = ReportTask::s_getCustomMetricProcessCount;
            //RegisterCustomMetricNumber(aws_byte_cursor_from_c_str("process_count"), func);
        }

        int ReportTask::s_getCustomMetricProcessCount(double *output, void *data)
        {
            (void)(data); // prevent warnings over unused parameter
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
