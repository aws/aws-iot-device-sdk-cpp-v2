/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/common/clock.h>
#include <aws/iotdevicedefender/DeviceDefender.h>

namespace Aws
{
    namespace Crt
    {

    }

    namespace Iotdevice
    {
        DeviceApiHandle::DeviceApiHandle(Crt::Allocator *allocator) noexcept { aws_iotdevice_library_init(allocator); }

        DeviceApiHandle::~DeviceApiHandle() { aws_iotdevice_library_clean_up(); }
    } // namespace Iotdevice

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
            uint64_t taskPeriodSeconds,
            uint64_t networkConnectionSamplePeriodSeconds,
            OnTaskCancelledHandler &&onCancelled,
            void *cancellationUserdata) noexcept
            : OnTaskCancelled(std::move(onCancelled)), cancellationUserdata(cancellationUserdata),
              m_allocator(allocator), m_status(ReportTaskStatus::Ready),
              m_taskConfig{
                  mqttConnection.get()->GetUnderlyingConnection(),
                  ByteCursorFromString(thingName),
                  aws_event_loop_group_get_next_loop(eventLoopGroup.GetUnderlyingHandle()),
                  reportFormat,
                  aws_timestamp_convert(taskPeriodSeconds, AWS_TIMESTAMP_SECS, AWS_TIMESTAMP_NANOS, NULL),
                  aws_timestamp_convert(
                      networkConnectionSamplePeriodSeconds,
                      AWS_TIMESTAMP_SECS,
                      AWS_TIMESTAMP_NANOS,
                      NULL),
                  ReportTask::s_onDefenderV1TaskCancelled,
                  this},
              m_lastError(0)
        {
            aws_iotdevice_library_init(allocator);
        }

        ReportTask::ReportTask(ReportTask &&toMove) noexcept
            : OnTaskCancelled(std::move(toMove.OnTaskCancelled)), cancellationUserdata(toMove.cancellationUserdata),
              m_allocator(toMove.m_allocator), m_status(toMove.m_status), m_taskConfig(std::move(toMove.m_taskConfig)),
              m_owningTask(toMove.m_owningTask), m_lastError(toMove.m_lastError)
        {
            toMove.OnTaskCancelled = nullptr;
            toMove.cancellationUserdata = nullptr;
            toMove.m_allocator = nullptr;
            toMove.m_status = ReportTaskStatus::Stopped;
            toMove.m_taskConfig = {0};
            toMove.m_owningTask = nullptr;
            toMove.m_lastError = AWS_ERROR_UNKNOWN;
        }

        ReportTask &ReportTask::operator=(ReportTask &&toMove) noexcept
        {
            OnTaskCancelled = std::move(toMove.OnTaskCancelled);
            cancellationUserdata = toMove.cancellationUserdata;
            m_allocator = toMove.m_allocator;
            m_status = toMove.m_status;
            m_taskConfig = std::move(toMove.m_taskConfig);
            m_owningTask = toMove.m_owningTask;
            m_lastError = toMove.m_lastError;

            toMove.OnTaskCancelled = nullptr;
            toMove.cancellationUserdata = nullptr;
            toMove.m_allocator = nullptr;
            toMove.m_status = ReportTaskStatus::Stopped;
            toMove.m_taskConfig = {0};
            toMove.m_owningTask = nullptr;
            toMove.m_lastError = AWS_ERROR_UNKNOWN;

            return *this;
        }

        ReportTaskStatus ReportTask::GetStatus() noexcept { return this->m_status; }

        int ReportTask::StartTask() noexcept
        {
            if (this->GetStatus() == ReportTaskStatus::Ready || this->GetStatus() == ReportTaskStatus::Stopped)
            {

                this->m_owningTask = aws_iotdevice_defender_v1_report_task(this->m_allocator, &this->m_taskConfig);

                if (this->m_owningTask == nullptr)
                {
                    this->m_lastError = aws_last_error();
                    aws_raise_error(this->m_lastError);
                    return AWS_OP_ERR;
                }
                else
                {
                    this->m_status = ReportTaskStatus::Running;
                }
            }
            return AWS_OP_SUCCESS;
        }

        void ReportTask::StopTask() noexcept
        {
            if (this->GetStatus() == ReportTaskStatus::Running)
            {
                aws_iotdevice_defender_v1_stop_task(this->m_owningTask);
                this->m_owningTask = nullptr;
            }
        }

        ReportTask::~ReportTask()
        {
            StopTask();
            this->m_owningTask = nullptr;
            this->m_allocator = nullptr;
            this->OnTaskCancelled = nullptr;
            this->cancellationUserdata = nullptr;
            aws_iotdevice_library_clean_up();
        }

        ReportTaskBuilder::ReportTaskBuilder(
            Aws::Crt::Allocator *allocator,
            std::shared_ptr<Crt::Mqtt::MqttConnection> mqttConnection,
            Crt::Io::EventLoopGroup &eventLoopGroup,
            const Crt::String &thingName)
            : m_allocator(allocator), m_mqttConnection(mqttConnection), m_thingName(thingName),
              m_eventLoopGroup(std::move(eventLoopGroup))
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

        ReportTaskBuilder &ReportTaskBuilder::WithTaskPeriodSeconds(uint64_t taskPeriodSeconds) noexcept
        {
            m_taskPeriodSeconds = taskPeriodSeconds;
            return *this;
        }

        ReportTaskBuilder &ReportTaskBuilder::WithNetworkConnectionSamplePeriodSeconds(
            uint64_t networkConnectionSamplePeriodSeconds) noexcept
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