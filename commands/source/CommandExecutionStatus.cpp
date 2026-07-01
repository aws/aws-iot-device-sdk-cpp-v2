/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotcommands/CommandExecutionStatus.h>

#include <aws/crt/StlAllocator.h>
#include <aws/crt/StringUtils.h>

#include <assert.h>

static const size_t IN_PROGRESS_HASH = Aws::Crt::HashString("IN_PROGRESS");
static const size_t SUCCEEDED_HASH = Aws::Crt::HashString("SUCCEEDED");
static const size_t FAILED_HASH = Aws::Crt::HashString("FAILED");
static const size_t REJECTED_HASH = Aws::Crt::HashString("REJECTED");
static const size_t TIMED_OUT_HASH = Aws::Crt::HashString("TIMED_OUT");

namespace Aws
{
    namespace Iotcommands
    {

        namespace CommandExecutionStatusMarshaller
        {
            const char *ToString(CommandExecutionStatus status)
            {
                switch (status)
                {
                    case CommandExecutionStatus::IN_PROGRESS:
                        return "IN_PROGRESS";
                    case CommandExecutionStatus::SUCCEEDED:
                        return "SUCCEEDED";
                    case CommandExecutionStatus::FAILED:
                        return "FAILED";
                    case CommandExecutionStatus::REJECTED:
                        return "REJECTED";
                    case CommandExecutionStatus::TIMED_OUT:
                        return "TIMED_OUT";
                    default:
                        assert(0);
                        return "UNKNOWN_VALUE";
                }
            }

            CommandExecutionStatus FromString(const Crt::String &str)
            {
                size_t hash = Crt::HashString(str.c_str());

                if (hash == IN_PROGRESS_HASH)
                {
                    return CommandExecutionStatus::IN_PROGRESS;
                }

                if (hash == SUCCEEDED_HASH)
                {
                    return CommandExecutionStatus::SUCCEEDED;
                }

                if (hash == FAILED_HASH)
                {
                    return CommandExecutionStatus::FAILED;
                }

                if (hash == REJECTED_HASH)
                {
                    return CommandExecutionStatus::REJECTED;
                }

                if (hash == TIMED_OUT_HASH)
                {
                    return CommandExecutionStatus::TIMED_OUT;
                }

                assert(0);
                return static_cast<CommandExecutionStatus>(-1);
            }
        } // namespace CommandExecutionStatusMarshaller
    } // namespace Iotcommands
} // namespace Aws
