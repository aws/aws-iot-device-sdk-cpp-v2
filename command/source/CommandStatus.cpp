/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotcommand/CommandStatus.h>

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
    namespace Iotcommand
    {

        namespace CommandStatusMarshaller
        {
            const char *ToString(CommandStatus status)
            {
                switch (status)
                {
                    case CommandStatus::IN_PROGRESS:
                        return "IN_PROGRESS";
                    case CommandStatus::SUCCEEDED:
                        return "SUCCEEDED";
                    case CommandStatus::FAILED:
                        return "FAILED";
                    case CommandStatus::REJECTED:
                        return "REJECTED";
                    case CommandStatus::TIMED_OUT:
                        return "TIMED_OUT";
                    default:
                        assert(0);
                        return "UNKNOWN_VALUE";
                }
            }

            CommandStatus FromString(const Crt::String &str)
            {
                size_t hash = Crt::HashString(str.c_str());

                if (hash == IN_PROGRESS_HASH)
                {
                    return CommandStatus::IN_PROGRESS;
                }

                if (hash == SUCCEEDED_HASH)
                {
                    return CommandStatus::SUCCEEDED;
                }

                if (hash == FAILED_HASH)
                {
                    return CommandStatus::FAILED;
                }

                if (hash == REJECTED_HASH)
                {
                    return CommandStatus::REJECTED;
                }

                if (hash == TIMED_OUT_HASH)
                {
                    return CommandStatus::TIMED_OUT;
                }

                assert(0);
                return static_cast<CommandStatus>(-1);
            }
        } // namespace CommandStatusMarshaller
    } // namespace Iotcommand
} // namespace Aws
