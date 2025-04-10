/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotcommand/CommandDeviceType.h>

#include <aws/crt/StlAllocator.h>
#include <aws/crt/StringUtils.h>

#include <assert.h>

static const size_t THING_HASH = Aws::Crt::HashString("things");
static const size_t CLIENT_HASH = Aws::Crt::HashString("clients");

namespace Aws
{
    namespace Iotcommand
    {

        namespace CommandDeviceTypeMarshaller
        {
            const char *ToString(CommandDeviceType status)
            {
                switch (status)
                {
                    case CommandDeviceType::things:
                        return "things";
                    case CommandDeviceType::clients:
                        return "clients";
                    default:
                        assert(0);
                        return "UNKNOWN_VALUE";
                }
            }

            CommandDeviceType FromString(const Crt::String &str)
            {
                size_t hash = Crt::HashString(str.c_str());

                if (hash == THING_HASH)
                {
                    return CommandDeviceType::things;
                }

                if (hash == CLIENT_HASH)
                {
                    return CommandDeviceType::clients;
                }

                assert(0);
                return static_cast<CommandDeviceType>(-1);
            }
        } // namespace CommandDeviceTypeMarshaller
    } // namespace Iotcommand
} // namespace Aws
