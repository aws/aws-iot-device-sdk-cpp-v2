#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/crt/JsonObject.h>

#include <aws/iotshadow/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotshadow
    {

        /**
         * Contains the last-updated timestamps for each attribute in the desired and reported sections of the shadow
         * state.
         *
         */
        class AWS_IOTSHADOW_API ShadowMetadata final
        {
          public:
            ShadowMetadata() = default;

            ShadowMetadata(const Crt::JsonView &doc);
            ShadowMetadata &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * Contains the timestamps for each attribute in the desired section of a shadow's state.
             *
             */
            Aws::Crt::Optional<Aws::Crt::JsonObject> Desired;

            /**
             * Contains the timestamps for each attribute in the reported section of a shadow's state.
             *
             */
            Aws::Crt::Optional<Aws::Crt::JsonObject> Reported;

          private:
            static void LoadFromObject(ShadowMetadata &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotshadow
} // namespace Aws
