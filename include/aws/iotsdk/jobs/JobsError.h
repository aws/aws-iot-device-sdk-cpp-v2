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

namespace Aws
{
    namespace IotSdk
    {
        namespace Jobs
        {
            enum class JobsErrorCode
            {
                RejectedError,
                UnknownError,
            };

            class AWS_CRT_CPP_API RejectedError
            {
            public:
                RejectedError() = default;
                RejectedError(const Crt::JsonView& doc);

                Crt::Optional<Crt::String> ClientToken;
                Crt::Optional<Crt::String> Message;
                Crt::Optional<Crt::DateTime> Timestamp;
                Crt::Optional<JobExecutionState> ExecutionState;

                static const JobsErrorCode Code = JobsErrorCode::RejectedError;
            };

            class AWS_CRT_CPP_API UnknownError
            {
            public:
                UnknownError() = default;
                UnknownError(const Crt::JsonView& doc);

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
                    std::aligned_storage<sizeof(RejectedError)>::type rejectedError;
                    std::aligned_storage<sizeof(UnknownError)>::type unknownError;
                } m_errorStorage;
            };
        }
    }
}
