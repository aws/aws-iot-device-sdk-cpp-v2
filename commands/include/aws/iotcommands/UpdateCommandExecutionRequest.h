#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotcommands/CommandExecutionStatus.h>
#include <aws/iotcommands/DeviceType.h>
#include <aws/iotcommands/StatusReason.h>

#include <aws/iotcommands/Exports.h>

#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>

namespace Aws
{
    namespace Iotcommands
    {

        /**
         * Data needed to make an UpdateCommandExecution request.
         *
         */
        class AWS_IOTCOMMANDS_API UpdateCommandExecutionRequest final
        {
          public:
            UpdateCommandExecutionRequest() = default;

            UpdateCommandExecutionRequest(const Crt::JsonView &doc);
            UpdateCommandExecutionRequest &operator=(const Crt::JsonView &doc);

            void SerializeToObject(Crt::JsonObject &doc) const;

            /**
             * The type of a target device.
             *
             */
            Aws::Crt::Optional<Aws::Iotcommands::DeviceType> DeviceType;

            /**
             * The name of the thing associated with the device.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> DeviceId;

            /**
             * ID of the command execution that needs to be updated.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ExecutionId;

            /**
             *
             *
             */
            Aws::Crt::Optional<Aws::Iotcommands::CommandExecutionStatus> Status;

            /**
             * A reason for the updated status.
             *
             */
            Aws::Crt::Optional<Aws::Iotcommands::StatusReason> StatusReason;

          private:
            static void LoadFromObject(UpdateCommandExecutionRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotcommands
} // namespace Aws
