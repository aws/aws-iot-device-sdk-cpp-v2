#pragma once
/* Copyright 2010-2019 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

* This file is generated
*/
#include <aws/crt/DateTime.h>
#include <aws/iotjobs/JobExecutionState.h>
#include <aws/iotjobs/RejectedErrorCode.h>

#include <aws/iotjobs/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
namespace Iotjobs
{

    class AWS_IOTJOBS_API RejectedError final
    {
    public:
        RejectedError() = default;

        RejectedError(const Crt::JsonView& doc);
        RejectedError& operator=(const Crt::JsonView& doc);

        void SerializeToObject(Crt::JsonObject& doc) const;


        Aws::Crt::Optional<Aws::Crt::DateTime> Timestamp;
        Aws::Crt::Optional<Aws::Iotjobs::RejectedErrorCode> Code;
        Aws::Crt::Optional<Aws::Crt::String> Message;
        Aws::Crt::Optional<Aws::Crt::String> ClientToken;
        Aws::Crt::Optional<Aws::Iotjobs::JobExecutionState> ExecutionState;

    private:
        static void LoadFromObject(RejectedError& obj, const Crt::JsonView &doc);
    };
}
}

