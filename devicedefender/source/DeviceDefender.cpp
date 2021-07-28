/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
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
            m_lastError =
                aws_iotdevice_defender_config_create(&m_taskConfig, allocator, &thingNameCursor, reportFormat);
            if (AWS_OP_SUCCESS == m_lastError)
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

        ReportTask::ReportTask(ReportTask &&toMove) noexcept
            : OnTaskCancelled(std::move(toMove.OnTaskCancelled)), cancellationUserdata(toMove.cancellationUserdata),
              m_allocator(toMove.m_allocator), m_status(toMove.m_status), m_taskConfig(std::move(toMove.m_taskConfig)),
              m_owningTask(toMove.m_owningTask), m_lastError(toMove.m_lastError),
              m_mqttConnection(toMove.m_mqttConnection), m_eventLoopGroup(toMove.m_eventLoopGroup)
        {
            toMove.m_taskConfig = m_taskConfig;
            toMove.m_owningTask = m_owningTask;
            m_owningTask = nullptr;
            toMove.m_status = ReportTaskStatus::Stopped;
            toMove.m_owningTask = nullptr;
            toMove.m_lastError = AWS_ERROR_UNKNOWN;
        }

        ReportTask &ReportTask::operator=(ReportTask &&toMove) noexcept
        {
            if (this != &toMove)
            {
                this->~ReportTask();

                OnTaskCancelled = std::move(toMove.OnTaskCancelled);
                cancellationUserdata = toMove.cancellationUserdata;
                m_allocator = toMove.m_allocator;
                m_status = toMove.m_status;
                m_taskConfig = toMove.m_taskConfig;
                m_owningTask = toMove.m_owningTask;
                m_lastError = toMove.m_lastError;

                toMove.OnTaskCancelled = nullptr;
                toMove.cancellationUserdata = nullptr;
                toMove.m_allocator = nullptr;
                toMove.m_status = ReportTaskStatus::Stopped;
                toMove.m_owningTask = nullptr;
                toMove.m_lastError = AWS_ERROR_UNKNOWN;
            }

            return *this;
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
                    aws_raise_error(this->m_lastError);
                }
                else
                {
                    this->m_status = ReportTaskStatus::Running;
                    return_code = AWS_OP_SUCCESS;
                }
            }
            return return_code;
        }

        void ReportTask::StopTask() noexcept
        {
            if (this->GetStatus() == ReportTaskStatus::Running)
            {
                aws_iotdevice_defender_task_clean_up(this->m_owningTask);
                this->m_owningTask = nullptr;
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

        ReportTask ReportTaskBuilder::Build() noexcept
        {

            return ReportTask(
                m_allocator,
                m_mqttConnection,
                m_thingName,
                m_eventLoopGroup,
                m_reportFormat,
                m_taskPeriodSeconds,
                m_networkConnectionSamplePeriodSeconds,
                static_cast<OnTaskCancelledHandler &&>(m_onCancelled),
                m_cancellationUserdata);
        }

    } // namespace Iotdevicedefenderv1
} // namespace Aws
