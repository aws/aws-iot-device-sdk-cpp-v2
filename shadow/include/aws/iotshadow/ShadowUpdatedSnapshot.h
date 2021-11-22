#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotshadow/ShadowMetadata.h>
#include <aws/iotshadow/ShadowState.h>

#include <aws/iotshadow/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotshadow
    {

        /**
         * Complete state of the (classic) shadow of an AWS IoT Thing.
         *
         */
        class AWS_IOTSHADOW_API ShadowUpdatedSnapshot final
        {
          public:
            ShadowUpdatedSnapshot() = default;

            ShadowUpdatedSnapshot(const Crt::JsonView &doc);
            ShadowUpdatedSnapshot &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * Current shadow state.
             *
             */
            Aws::Crt::Optional<Aws::Iotshadow::ShadowState> State;

            /**
             * Contains the timestamps for each attribute in the desired and reported sections of the state.
             *
             */
            Aws::Crt::Optional<Aws::Iotshadow::ShadowMetadata> Metadata;

            /**
             * The current version of the document for the device's shadow.
             *
             */
            Aws::Crt::Optional<int32_t> Version;

          private:
            static void LoadFromObject(ShadowUpdatedSnapshot &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotshadow
} // namespace Aws
