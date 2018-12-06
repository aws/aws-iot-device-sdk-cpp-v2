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

            class RejectedError
            {
            public:
                Crt::Optional<Crt::String> ClientToken;
                Crt::Optional<Crt::String> Message;
                Crt::Optional<Crt::DateTime> Timestamp;
                Crt::Optional<JobExecutionState> ExecutionState;

                static const JobsErrorCode Code = JobsErrorCode::RejectedError;
            };

            class UnknownError
            {
            public:
                Crt::String Message;

                static const JobsErrorCode Code = JobsErrorCode::UnknownError;
            };

            class JobsError final
            {
            public:
                JobsErrorCode ErrorCode;

                template<typename U>
                U* GetErrorInstance() const
                {
                    if (U::Code != ErrorCode)
                    {
                        return nullptr;
                    }

                    return reinterpret_cast<U*>(&m_errorStorage);
                }

            private:
                union
                {
                    RejectedError rejectedError;
                    UnknownError unknownError;
                } m_errorStorage;
            };
        }
    }
}