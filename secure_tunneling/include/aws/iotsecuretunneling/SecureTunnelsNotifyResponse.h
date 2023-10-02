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
         * Data needed sent as a response after subscribing to the secure tunnel notify topic
         *
         */
        class AWS_IOTSECURETUNNELING_API SecureTunnelsNotifyResponse final
        {
          public:
            SecureTunnelsNotifyResponse() = default;

            SecureTunnelsNotifyResponse(const Crt::JsonView &doc);
            SecureTunnelsNotifyResponse &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * The access token that the AWS IoT thing can use through the secure tunnel
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ClientAccessToken;

            /**
             * The services that the AWS IoT thing can access through the secure tunnel
             *
             */
            Aws::Crt::Optional<Aws::Crt::Vector<Aws::Crt::String>> Services;

            /**
             * The client mode sent to the AWS IoT thing
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ClientMode;

            /**
             * The region that the AWS IoT thing can access through the secure tunnel
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> Region;

          private:
            static void LoadFromObject(SecureTunnelsNotifyResponse &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotsecuretunneling
} // namespace Aws
