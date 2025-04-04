#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotcommand/CommandStatus.h>

#include <aws/iotcommand/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotcommand
    {

        /**
         * Data needed to make an UpdateCommandExecution request.
         *
         */
        class AWS_IOTCOMMAND_API UpdateCommandExecutionRequest final
        {
          public:
            UpdateCommandExecutionRequest() = default;

            UpdateCommandExecutionRequest(const Crt::JsonView &doc);
            UpdateCommandExecutionRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             *
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> DeviceType;

            /**
             * The name of the thing associated with the device.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> DeviceId;

            /**
             * Execution ID
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ExecutionId;

            /**
             *
             *
             */
            Aws::Crt::Optional<Aws::Iotcommand::CommandStatus> Status;

          private:
            static void LoadFromObject(UpdateCommandExecutionRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotcommand
} // namespace Aws
