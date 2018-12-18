#pragma once
/*
* Copyright 2010-2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License").
* You may not use this file except in compliance with the License.
* A copy of the License is located at
*
*  http://aws.amazon.com/apache2.0
*
* or in the "license" file accompanying this file. This file is distributed
* on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
* express or implied. See the License for the specific language governing
* permissions and limitations under the License.
*/
#include <aws/crt/Types.h>
#include <aws/iotsdk/jobs/JobExecutionState.h>
#include <aws/crt/mqtt/RpcDispatch.h>

namespace Aws
{
    namespace IotSdk
    {
        namespace Jobs
        {
            enum class JobsErrorCode
            {
                InvalidTopic,
                InvalidJson,
                InvalidRequest,
                InvalidStateTransition,
                ResourceNotFound,
                VersionMismatch,
                InternalError,
                RequestThrottled,
                TerminalStateReached,
                UnknownError,
            };

            class AWS_CRT_CPP_API InvalidTopic final
            {
            public:
                InvalidTopic() = default;
                InvalidTopic(const Crt::JsonView& doc);

                Crt::Optional<Crt::String> ClientToken;
                Crt::Optional<Crt::String> Message;
                Crt::Optional<Crt::DateTime> Timestamp;
                
                static const JobsErrorCode Code = JobsErrorCode::InvalidTopic;
            };

            class AWS_CRT_CPP_API InvalidJson final
            {
            public:
                InvalidJson() = default;
                InvalidJson(const Crt::JsonView& doc);

                Crt::Optional<Crt::String> ClientToken;
                Crt::Optional<Crt::String> Message;
                Crt::Optional<Crt::DateTime> Timestamp;

                static const JobsErrorCode Code = JobsErrorCode::InvalidJson;
            };

            class AWS_CRT_CPP_API InvalidRequest final
            {
            public:
                InvalidRequest() = default;
                InvalidRequest(const Crt::JsonView& doc);

                Crt::Optional<Crt::String> ClientToken;
                Crt::Optional<Crt::String> Message;
                Crt::Optional<Crt::DateTime> Timestamp;

                static const JobsErrorCode Code = JobsErrorCode::InvalidRequest;
            };

            class AWS_CRT_CPP_API InvalidStateTransition final
            {
            public:
                InvalidStateTransition() = default;
                InvalidStateTransition(const Crt::JsonView& doc);

                Crt::Optional<Crt::String> ClientToken;
                Crt::Optional<Crt::String> Message;
                Crt::Optional<Crt::DateTime> Timestamp;
                Crt::Optional<JobExecutionState> ExecutionState;

                static const JobsErrorCode Code = JobsErrorCode::InvalidStateTransition;
            };

            class AWS_CRT_CPP_API ResourceNotFound final
            {
            public:
                ResourceNotFound() = default;
                ResourceNotFound(const Crt::JsonView& doc);

                Crt::Optional<Crt::String> ClientToken;
                Crt::Optional<Crt::String> Message;
                Crt::Optional<Crt::DateTime> Timestamp;

                static const JobsErrorCode Code = JobsErrorCode::ResourceNotFound;
            };

            class AWS_CRT_CPP_API VersionMismatch final
            {
            public:
                VersionMismatch() = default;
                VersionMismatch(const Crt::JsonView& doc);

                Crt::Optional<Crt::String> ClientToken;
                Crt::Optional<Crt::String> Message;
                Crt::Optional<Crt::DateTime> Timestamp;
                Crt::Optional<JobExecutionState> ExecutionState;
                
                static const JobsErrorCode Code = JobsErrorCode::VersionMismatch;
            };

            class AWS_CRT_CPP_API InternalError final
            {
            public:
                InternalError() = default;
                InternalError(const Crt::JsonView& doc);

                Crt::Optional<Crt::String> ClientToken;
                Crt::Optional<Crt::String> Message;
                Crt::Optional<Crt::DateTime> Timestamp;
                Crt::Optional<JobExecutionState> ExecutionState;

                static const JobsErrorCode Code = JobsErrorCode::InternalError;
            };

            class AWS_CRT_CPP_API RequestThrottled final
            {
            public:
                RequestThrottled() = default;
                RequestThrottled(const Crt::JsonView& doc);

                Crt::Optional<Crt::String> ClientToken;
                Crt::Optional<Crt::String> Message;
                Crt::Optional<Crt::DateTime> Timestamp;
                Crt::Optional<JobExecutionState> ExecutionState;
                
                static const JobsErrorCode Code = JobsErrorCode::RequestThrottled;
            };

            class AWS_CRT_CPP_API TerminalStateReached final
            {
            public:
                TerminalStateReached() = default;
                TerminalStateReached(const Crt::JsonView& doc);

                Crt::Optional<Crt::String> ClientToken;
                Crt::Optional<Crt::String> Message;
                Crt::Optional<Crt::DateTime> Timestamp;
                Crt::Optional<JobExecutionState> ExecutionState;
                
                static const JobsErrorCode Code = JobsErrorCode::TerminalStateReached;
            };

            class AWS_CRT_CPP_API UnknownError final
            {
            public:
                UnknownError() = default;
                UnknownError(const Crt::JsonView& doc);

                Crt::Optional<Crt::String> ClientToken;
                Crt::String Message;
                
                static const JobsErrorCode Code = JobsErrorCode::UnknownError;
            };

            class AWS_CRT_CPP_API JobsError final
            {
            public:
                JobsError(const Crt::JsonView& doc);

                JobsErrorCode ErrorCode;

                template<typename U>
                const U* GetErrorInstance() const
                {
                    if (U::Code != ErrorCode)
                    {
                        return nullptr;
                    }

                    return reinterpret_cast<const U*>(&m_errorStorage);
                }

            private:
                union
                {
                    std::aligned_storage<sizeof(InvalidTopic)>::type invalidTopic;
                    std::aligned_storage<sizeof(InvalidJson)>::type invalidJson;
                    std::aligned_storage<sizeof(InvalidRequest)>::type invalidRequest;
                    std::aligned_storage<sizeof(InvalidStateTransition)>::type invalidStateTransition;
                    std::aligned_storage<sizeof(ResourceNotFound)>::type resourceNotFound;
                    std::aligned_storage<sizeof(VersionMismatch)>::type versionMismatch;
                    std::aligned_storage<sizeof(RequestThrottled)>::type requestThrottled;
                    std::aligned_storage<sizeof(TerminalStateReached)>::type terminalStateReached;
                    std::aligned_storage<sizeof(UnknownError)>::type unknownError;
                } m_errorStorage;
            };
        }
    }
}
