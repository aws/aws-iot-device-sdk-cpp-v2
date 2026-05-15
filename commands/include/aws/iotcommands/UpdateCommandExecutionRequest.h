#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotcommands/CommandExecutionResult.h>
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
             * The type of a target device. Determine if the device should subscribe for commands addressed to an IoT
             * Thing or MQTT client.
             *
             */
            Aws::Crt::Optional<Aws::Iotcommands::DeviceType> DeviceType;

            /**
             * Depending on device type value, this field is either an IoT Thing name or a client ID.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> DeviceId;

            /**
             * ID of the command execution that needs to be updated.
             *
             */
            Aws::Crt::Optional<Aws::Crt::String> ExecutionId;

            /**
             * The status of the command execution.
             *
             */
            Aws::Crt::Optional<Aws::Iotcommands::CommandExecutionStatus> Status;

            /**
             * A reason for the updated status. Can provide additional information on failures. Should be used when
             * status is one of the following: FAILED, REJECTED, TIMED_OUT.
             *
             */
            Aws::Crt::Optional<Aws::Iotcommands::StatusReason> StatusReason;

            /**
             * The result value for the current state of the command execution. The status provides information about
             * the progress of the command execution. The device can use the result field to share additional details
             * about the execution such as a return value of a remote function call.
             *
             */
            Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, Aws::Iotcommands::CommandExecutionResult>> Result;

          private:
            static void LoadFromObject(UpdateCommandExecutionRequest &obj, const Crt::JsonView &doc);
        };
    } // namespace Iotcommands
} // namespace Aws
