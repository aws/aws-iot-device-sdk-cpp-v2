#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotshadow/ShadowState.h>

#include <aws/iotshadow/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotshadow
    {

        /**
         * Data needed to make an UpdateShadow request.
         *
         */
        class AWS_IOTSHADOW_API UpdateShadowRequest final
        {
          public:
            UpdateShadowRequest() = default;

            UpdateShadowRequest(const Crt::JsonView &doc);
            UpdateShadowRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * Requested changes to the shadow state.  Updates affect only the fields specified.
             *
             */
            Aws::Crt::Optional<Aws::Iotshadow::ShadowState> State;

            /**
             * Aws IoT thing to update the (classic) shadow of.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ThingName;

            /**
             * (Optional) The Device Shadow service processes the update only if the specified version matches the
             * latest version.
             *
             */
            Aws::Crt::Optional<int32_t> Version;

            /**
             * Optional. A client token used to correlate requests and responses. Enter an arbitrary value here and it
             * is reflected in the response.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ClientToken;

          private:
            static void LoadFromObject(UpdateShadowRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotshadow
} // namespace Aws
