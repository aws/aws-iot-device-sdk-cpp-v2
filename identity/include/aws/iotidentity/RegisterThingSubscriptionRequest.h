#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotidentity/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotidentity
    {

        /**
         * Data needed to subscribe to the responses of the RegisterThing operation.
         *
         */
        class AWS_IOTIDENTITY_API RegisterThingSubscriptionRequest final
        {
          public:
            RegisterThingSubscriptionRequest() = default;

            RegisterThingSubscriptionRequest(const Crt::JsonView &doc);
            RegisterThingSubscriptionRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * Name of the provisioning template to listen for RegisterThing responses for.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> TemplateName;

          private:
            static void LoadFromObject(RegisterThingSubscriptionRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotidentity
} // namespace Aws
