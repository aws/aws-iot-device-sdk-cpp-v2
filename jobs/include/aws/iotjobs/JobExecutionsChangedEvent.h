#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/DateTime.h>
#include <aws/iotjobs/JobExecutionSummary.h>
#include <aws/iotjobs/JobStatus.h>

#include <aws/iotjobs/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotjobs
    {

        class AWS_IOTJOBS_API JobExecutionsChangedEvent final
        {
          public:
            JobExecutionsChangedEvent() = default;

            JobExecutionsChangedEvent(const Crt::JsonView &doc);
            JobExecutionsChangedEvent &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            Aws::Crt::Optional<
                Aws::Crt::Map<Aws::Iotjobs::JobStatus, Aws::Crt::Vector<Aws::Iotjobs::JobExecutionSummary>>>
                Jobs;
            Aws::Crt::Optional<Aws::Crt::DateTime> Timestamp;

          private:
            static void LoadFromObject(JobExecutionsChangedEvent &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotjobs
} // namespace Aws
