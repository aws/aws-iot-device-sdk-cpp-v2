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
            RejectedError::RejectedError(const Crt::JsonView& doc)
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
                    ExecutionState = doc.GetObject("executionState");
                }
            }

            UnknownError::UnknownError(const Crt::JsonView& doc)
            {
                if (doc.ValueExists("message"))
                {
                    Message = doc.GetString("message");
                }
            }

            static const size_t REJECTED_ERROR_HASH = Aws::Crt::HashString("RejectedError");
            JobsError::JobsError(const Crt::JsonView& doc)
            {
                if (doc.ValueExists("code"))
                {
                    size_t errorHash = Aws::Crt::HashString(doc.GetString("code").c_str());

                    if (errorHash == REJECTED_ERROR_HASH)
                    {
                        ErrorCode = Jobs::RejectedError::Code;
                        new(&m_errorStorage)RejectedError(doc);
                        return;
                    }
                }

                ErrorCode = Jobs::UnknownError::Code;
                new(&m_errorStorage)Jobs::UnknownError(doc);
            }
        }
    }
}
