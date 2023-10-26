/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include "aws/common/error.h"
#include "aws/crt/Types.h"
#include "aws/iotdevice/device_defender.h"
#include <aws/common/clock.h>
#include <aws/iotdevicedefender/DeviceDefender.h>

namespace Aws
{
    namespace Crt
    {

    }

    namespace Iotdevicedefenderv1
    {
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

        // Custom number metric setup and metric function getter.
        CustomMetricNumber::CustomMetricNumber(CustomMetricNumberFunction inputFunction, Crt::Allocator *inputAllocator)
        {
            m_metricFunction = std::move(inputFunction);
            m_allocator = inputAllocator;
        }
        int CustomMetricNumber::GetMetricFunction(double *output, void *data)
        {
            CustomMetricNumber *metric = (CustomMetricNumber *)data;
            return metric->m_metricFunction(output);
        }

        // Custom number list metric setup and metric function getter.
        CustomMetricNumberList::CustomMetricNumberList(
            CustomMetricNumberListFunction inputFunction,
            Crt::Allocator *inputAllocator)
        {
            m_metricFunction = std::move(inputFunction);
            m_allocator = inputAllocator;
        }
        int CustomMetricNumberList::GetMetricFunction(aws_array_list *output, void *data)
        {
            CustomMetricNumberList *metric = (CustomMetricNumberList *)data;
            Crt::Vector<double> function_data = Crt::Vector<double>();
            int returnValue = metric->m_metricFunction(&function_data);
            std::for_each(function_data.begin(), function_data.end(), [output](double &i) {
                aws_array_list_push_back(output, &i);
            });
            return returnValue;
        }

        // Custom string list metric setup and metric function getter.
        CustomMetricStringList::CustomMetricStringList(
            CustomMetricStringListFunction inputFunction,
            Crt::Allocator *inputAllocator)
        {
            m_metricFunction = std::move(inputFunction);
            m_allocator = inputAllocator;
        }
        int CustomMetricStringList::GetMetricFunction(aws_array_list *output, void *data)
        {
            CustomMetricStringList *metric = (CustomMetricStringList *)data;
            Crt::Vector<Crt::String> function_data = Crt::Vector<Crt::String>();
            int returnValue = metric->m_metricFunction(&function_data);
            std::for_each(function_data.begin(), function_data.end(), [output, metric](Crt::String &i) {
                aws_string *tmp_str = aws_string_new_from_c_str(metric->m_allocator, i.c_str());
                aws_array_list_push_back(output, &tmp_str);
            });
            return returnValue;
        }

        // Custom ip list metric setup and metric function getter.
        CustomMetricIpList::CustomMetricIpList(CustomMetricIpListFunction inputFunction, Crt::Allocator *inputAllocator)
        {
            m_metricFunction = std::move(inputFunction);
            m_allocator = inputAllocator;
        }
        int CustomMetricIpList::GetMetricFunction(aws_array_list *output, void *data)
        {
            CustomMetricIpList *metric = (CustomMetricIpList *)data;
            Crt::Vector<Crt::String> function_data = Crt::Vector<Crt::String>();
            int returnValue = metric->m_metricFunction(&function_data);
            std::for_each(function_data.begin(), function_data.end(), [output, metric](Crt::String &i) {
                aws_string *tmp_str = aws_string_new_from_c_str(metric->m_allocator, i.c_str());
                aws_array_list_push_back(output, &tmp_str);
            });
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
            const Crt::String metricName,
            CustomMetricNumberFunction metricFunc) noexcept
        {
            std::shared_ptr<CustomMetricNumber> data =
                Aws::Crt::MakeShared<CustomMetricNumber>(m_allocator, std::move(metricFunc), m_allocator);
            storedCustomMetrics.push_back(data);
            aws_byte_cursor cursor = aws_byte_cursor_from_c_str(metricName.c_str());
            aws_iotdevice_defender_config_register_number_metric(
                m_taskConfig, &cursor, CustomMetricNumber::GetMetricFunction, data.get());
        }

        void ReportTask::RegisterCustomMetricNumberList(
            const Crt::String metricName,
            CustomMetricNumberListFunction metricFunc) noexcept
        {
            std::shared_ptr<CustomMetricNumberList> data =
                Aws::Crt::MakeShared<CustomMetricNumberList>(m_allocator, std::move(metricFunc), m_allocator);
            storedCustomMetrics.push_back(data);
            aws_byte_cursor cursor = aws_byte_cursor_from_c_str(metricName.c_str());
            aws_iotdevice_defender_config_register_number_list_metric(
                m_taskConfig, &cursor, CustomMetricNumberList::GetMetricFunction, data.get());
        }

        void ReportTask::RegisterCustomMetricStringList(
            const Crt::String metricName,
            CustomMetricStringListFunction metricFunc) noexcept
        {
            std::shared_ptr<CustomMetricStringList> data =
                Aws::Crt::MakeShared<CustomMetricStringList>(m_allocator, std::move(metricFunc), m_allocator);
            storedCustomMetrics.push_back(data);
            aws_byte_cursor cursor = aws_byte_cursor_from_c_str(metricName.c_str());
            aws_iotdevice_defender_config_register_string_list_metric(
                m_taskConfig, &cursor, CustomMetricStringList::GetMetricFunction, data.get());
        }

        void ReportTask::RegisterCustomMetricIpAddressList(
            const Crt::String metricName,
            CustomMetricIpListFunction metricFunc) noexcept
        {
            std::shared_ptr<CustomMetricIpList> data =
                Aws::Crt::MakeShared<CustomMetricIpList>(m_allocator, std::move(metricFunc), m_allocator);
            storedCustomMetrics.push_back(data);
            aws_byte_cursor cursor = aws_byte_cursor_from_c_str(metricName.c_str());
            aws_iotdevice_defender_config_register_ip_list_metric(
                m_taskConfig, &cursor, CustomMetricIpList::GetMetricFunction, data.get());
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

        ReportTaskBuilder::ReportTaskBuilder(
            Crt::Allocator *allocator,
            std::shared_ptr<Crt::Mqtt5::Mqtt5Client> mqtt5Client,
            Crt::Io::EventLoopGroup &eventLoopGroup,
            const Crt::String &thingName)
            : ReportTaskBuilder(
                  allocator,
                  Crt::Mqtt::MqttConnection::NewConnectionFromMqtt5Client(std::move(mqtt5Client)),
                  eventLoopGroup,
                  thingName)
        {
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
