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
         * Data needed to make a GetPendingJobExecutions request.
         *
         */
        class AWS_IOTJOBS_API GetPendingJobExecutionsRequest final
        {
          public:
            GetPendingJobExecutionsRequest() = default;

            GetPendingJobExecutionsRequest(const Crt::JsonView &doc);
            GetPendingJobExecutionsRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * IoT Thing the request is relative to.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ThingName;

            /**
             * Optional. A client token used to correlate requests and responses. Enter an arbitrary value here and it
             * is reflected in the response.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ClientToken;

          private:
            static void LoadFromObject(GetPendingJobExecutionsRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotjobs
} // namespace Aws
