#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotshadow/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotshadow
    {

        /**
         * Data needed to make a GetNamedShadow request.
         *
         */
        class AWS_IOTSHADOW_API GetNamedShadowRequest final
        {
          public:
            GetNamedShadowRequest() = default;

            GetNamedShadowRequest(const Crt::JsonView &doc);
            GetNamedShadowRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * AWS IoT thing to get the named shadow for.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ThingName;

            /**
             * Name of the shadow to get.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ShadowName;

            /**
             * Optional. A client token used to correlate requests and responses. Enter an arbitrary value here and it
             * is reflected in the response.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ClientToken;

          private:
            static void LoadFromObject(GetNamedShadowRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotshadow
} // namespace Aws
