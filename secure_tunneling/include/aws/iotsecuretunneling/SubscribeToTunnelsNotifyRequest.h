#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotsecuretunneling/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotsecuretunneling
    {

        /**
         * Data needed to subscribe to the secure tunnel notify topic
         *
         */
        class AWS_IOTSECURETUNNELING_API SubscribeToTunnelsNotifyRequest final
        {
          public:
            SubscribeToTunnelsNotifyRequest() = default;

            SubscribeToTunnelsNotifyRequest(const Crt::JsonView &doc);
            SubscribeToTunnelsNotifyRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * Name of the AWS IoT thing that is making the subscription request
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ThingName;

          private:
            static void LoadFromObject(SubscribeToTunnelsNotifyRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotsecuretunneling
} // namespace Aws
