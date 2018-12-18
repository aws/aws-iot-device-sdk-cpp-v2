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
#include <aws/iotsdk/jobs/JobsError.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StringUtils.h>

namespace Aws
{
    namespace Crt
    {
        class JsonObject;
        class JsonView;
    }

    namespace IotSdk
    {
        namespace Jobs
        {
            InvalidTopic::InvalidTopic(const Crt::JsonView& doc)
            {
                if (doc.ValueExists("clientToken"))
                {
                    ClientToken = doc.GetString("clientToken");
                }

                if (doc.ValueExists("message"))
                {
                    Message = doc.GetString("message");
                }

                if (doc.ValueExists("timestamp"))
                {
                    Timestamp = doc.GetDouble("timestamp");
                }
            }

            InvalidJson::InvalidJson(const Crt::JsonView& doc)
            {
                if (doc.ValueExists("clientToken"))
                {
                    ClientToken = doc.GetString("clientToken");
                }

                if (doc.ValueExists("message"))
                {
                    Message = doc.GetString("message");
                }

                if (doc.ValueExists("timestamp"))
                {
                    Timestamp = doc.GetDouble("timestamp");
                }
            }

            InvalidRequest::InvalidRequest(const Crt::JsonView& doc)
            {
                if (doc.ValueExists("clientToken"))
                {
                    ClientToken = doc.GetString("clientToken");
                }

                if (doc.ValueExists("message"))
                {
                    Message = doc.GetString("message");
                }

                if (doc.ValueExists("timestamp"))
                {
                    Timestamp = doc.GetDouble("timestamp");
                }
            }

            InvalidStateTransition::InvalidStateTransition(const Crt::JsonView& doc)
            {
                if (doc.ValueExists("clientToken"))
                {
                    ClientToken = doc.GetString("clientToken");
                }

                if (doc.ValueExists("message"))
                {
                    Message = doc.GetString("message");
                }

                if (doc.ValueExists("timestamp"))
                {
                    Timestamp = doc.GetDouble("timestamp");
                }

                if (doc.ValueExists("executionState"))
                {
                    ExecutionState = doc.GetJsonObject("executionState");
                }
            }

            ResourceNotFound::ResourceNotFound(const Crt::JsonView& doc)
            {
                if (doc.ValueExists("clientToken"))
                {
                    ClientToken = doc.GetString("clientToken");
                }

                if (doc.ValueExists("message"))
                {
                    Message = doc.GetString("message");
                }

                if (doc.ValueExists("timestamp"))
                {
                    Timestamp = doc.GetDouble("timestamp");
                }
            }

            VersionMismatch::VersionMismatch(const Crt::JsonView& doc)
            {
                if (doc.ValueExists("clientToken"))
                {
                    ClientToken = doc.GetString("clientToken");
                }

                if (doc.ValueExists("message"))
                {
                    Message = doc.GetString("message");
                }

                if (doc.ValueExists("timestamp"))
                {
                    Timestamp = doc.GetDouble("timestamp");
                }

                if (doc.ValueExists("executionState"))
                {
                    ExecutionState = doc.GetJsonObject("executionState");
                }
            }

            InternalError::InternalError(const Crt::JsonView& doc)
            {
                if (doc.ValueExists("clientToken"))
                {
                    ClientToken = doc.GetString("clientToken");
                }

                if (doc.ValueExists("message"))
                {
                    Message = doc.GetString("message");
                }

                if (doc.ValueExists("timestamp"))
                {
                    Timestamp = doc.GetDouble("timestamp");
                }

                if (doc.ValueExists("executionState"))
                {
                    ExecutionState = doc.GetJsonObject("executionState");
                }
            }

            RequestThrottled::RequestThrottled(const Crt::JsonView& doc)
            {
                if (doc.ValueExists("clientToken"))
                {
                    ClientToken = doc.GetString("clientToken");
                }

                if (doc.ValueExists("message"))
                {
                    Message = doc.GetString("message");
                }

                if (doc.ValueExists("timestamp"))
                {
                    Timestamp = doc.GetDouble("timestamp");
                }

                if (doc.ValueExists("executionState"))
                {
                    ExecutionState = doc.GetJsonObject("executionState");
                }
            }

            TerminalStateReached::TerminalStateReached(const Crt::JsonView& doc)
            {
                if (doc.ValueExists("clientToken"))
                {
                    ClientToken = doc.GetString("clientToken");
                }

                if (doc.ValueExists("message"))
                {
                    Message = doc.GetString("message");
                }

                if (doc.ValueExists("timestamp"))
                {
                    Timestamp = doc.GetDouble("timestamp");
                }

                if (doc.ValueExists("executionState"))
                {
                    ExecutionState = doc.GetJsonObject("executionState");
                }
            }

            UnknownError::UnknownError(const Crt::JsonView& doc)
            {
                if (doc.ValueExists("clientToken"))
                {
                    ClientToken = doc.GetString("clientToken");
                }

                if (doc.ValueExists("message"))
                {
                    Message = doc.GetString("message");
                }       
            }

            static const size_t INVALID_TOPIC_HASH = Aws::Crt::HashString("InvalidTopic");
            static const size_t INVALID_JSON_HASH = Aws::Crt::HashString("InvalidJson");
            static const size_t INVALID_REQUEST_HASH = Aws::Crt::HashString("InvalidRequest");
            static const size_t INVALID_STATE_TRANSITION_HASH = Aws::Crt::HashString("InvalidStateTransition");
            static const size_t RESOURCE_NOT_FOUND_HASH = Aws::Crt::HashString("ResourceNotFound");
            static const size_t VERSION_MISMATCH_HASH = Aws::Crt::HashString("VersionMismatch");
            static const size_t INTERNAL_ERROR_HASH = Aws::Crt::HashString("InternalError");
            static const size_t REQUEST_THROTTLED_HASH = Aws::Crt::HashString("RequestThrottled");
            static const size_t TERMINAL_STATE_REACHED_HASH = Aws::Crt::HashString("TerminalStateReached");

            JobsError::JobsError(const Crt::JsonView& doc)
            {
                if (doc.ValueExists("code"))
                {
                    size_t errorHash = Aws::Crt::HashString(doc.GetString("code").c_str());

                    if (errorHash == INVALID_TOPIC_HASH)
                    {
                        ErrorCode = Jobs::InvalidTopic::Code;
                        new(&m_errorStorage)InvalidTopic(doc);
                        return;
                    }

                    if (errorHash == INVALID_JSON_HASH)
                    {
                        ErrorCode = Jobs::InvalidJson::Code;
                        new(&m_errorStorage)InvalidJson(doc);
                        return;
                    }

                    if (errorHash == INVALID_TOPIC_HASH)
                    {
                        ErrorCode = Jobs::InvalidTopic::Code;
                        new(&m_errorStorage)InvalidTopic(doc);
                        return;
                    }

                    if (errorHash == INVALID_REQUEST_HASH)
                    {
                        ErrorCode = Jobs::InvalidRequest::Code;
                        new(&m_errorStorage)InvalidRequest(doc);
                        return;
                    }

                    if (errorHash == INVALID_STATE_TRANSITION_HASH)
                    {
                        ErrorCode = Jobs::InvalidStateTransition::Code;

                        new(&m_errorStorage)InvalidStateTransition(doc);
                        return;
                    }

                    if (errorHash == RESOURCE_NOT_FOUND_HASH)
                    {
                        ErrorCode = Jobs::ResourceNotFound::Code;

                        new(&m_errorStorage)ResourceNotFound(doc);
                        return;
                    }

                    if (errorHash == VERSION_MISMATCH_HASH)
                    {
                        ErrorCode = Jobs::VersionMismatch::Code;

                        new(&m_errorStorage)VersionMismatch(doc);
                        return;
                    }

                    if (errorHash == INTERNAL_ERROR_HASH)
                    {
                        ErrorCode = Jobs::InternalError::Code;
                        new(&m_errorStorage)InternalError(doc);
                        return;
                    }

                    if (errorHash == REQUEST_THROTTLED_HASH)
                    {
                        ErrorCode = Jobs::RequestThrottled::Code;

                        new(&m_errorStorage)RequestThrottled(doc);
                        return;
                    }

                    if (errorHash == TERMINAL_STATE_REACHED_HASH)
                    {
                        ErrorCode = Jobs::TerminalStateReached::Code;

                        new(&m_errorStorage)TerminalStateReached(doc);
                        return;
                    }
                }

                ErrorCode = Jobs::UnknownError::Code;
                new(&m_errorStorage)Jobs::UnknownError(doc);
            }
        }
    }
}
