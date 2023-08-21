#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotjobs/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotjobs
    {

        /**
         * Data needed to subscribe to JobExecutionsChanged events.
         *
         */
        class AWS_IOTJOBS_API JobExecutionsChangedSubscriptionRequest final
        {
          public:
            JobExecutionsChangedSubscriptionRequest() = default;

            JobExecutionsChangedSubscriptionRequest(const Crt::JsonView &doc);
            JobExecutionsChangedSubscriptionRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * Name of the IoT Thing that you want to subscribe to JobExecutionsChanged events for.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ThingName;

          private:
            static void LoadFromObject(JobExecutionsChangedSubscriptionRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotjobs
} // namespace Aws
