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
         * Data needed to subscribe to UpdateJobExecution responses.
         *
         */
        class AWS_IOTJOBS_API UpdateJobExecutionSubscriptionRequest final
        {
          public:
            UpdateJobExecutionSubscriptionRequest() = default;

            UpdateJobExecutionSubscriptionRequest(const Crt::JsonView &doc);
            UpdateJobExecutionSubscriptionRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * Name of the IoT Thing that you want to subscribe to UpdateJobExecution response events for.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ThingName;

            /**
             * Job ID that you want to subscribe to UpdateJobExecution response events for.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> JobId;

          private:
            static void LoadFromObject(UpdateJobExecutionSubscriptionRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotjobs
} // namespace Aws
