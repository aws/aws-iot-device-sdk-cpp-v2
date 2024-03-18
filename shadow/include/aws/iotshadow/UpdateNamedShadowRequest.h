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
         * Data needed to make an UpdateNamedShadow request.
         *
         */
        class AWS_IOTSHADOW_API UpdateNamedShadowRequest final
        {
          public:
            UpdateNamedShadowRequest() = default;

            UpdateNamedShadowRequest(const Crt::JsonView &doc);
            UpdateNamedShadowRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * Aws IoT thing to update a named shadow of.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ThingName;

            /**
             * Name of the shadow to update.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ShadowName;

            /**
             * Optional. A client token used to correlate requests and responses. Enter an arbitrary value here and it
             * is reflected in the response.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ClientToken;

            /**
             * Requested changes to shadow state.  Updates affect only the fields specified.
             *
             */
            Aws::Crt::Optional<Aws::Iotshadow::ShadowState> State;

            /**
             * (Optional) The Device Shadow service applies the update only if the specified version matches the latest
             * version.
             *
             */
            Aws::Crt::Optional<int32_t> Version;

          private:
            static void LoadFromObject(UpdateNamedShadowRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotshadow
} // namespace Aws
