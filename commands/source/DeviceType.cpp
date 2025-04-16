/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotcommands/DeviceType.h>

#include <aws/crt/StlAllocator.h>
#include <aws/crt/StringUtils.h>

#include <assert.h>

static const size_t THING_HASH = Aws::Crt::HashString("things");
static const size_t CLIENT_HASH = Aws::Crt::HashString("clients");

namespace Aws
{
    namespace Iotcommands
    {

        namespace DeviceTypeMarshaller
        {
            const char *ToString(DeviceType status)
            {
                switch (status)
                {
                    case DeviceType::THING:
                        return "things";
                    case DeviceType::CLIENT:
                        return "clients";
                    default:
                        assert(0);
                        return "UNKNOWN_VALUE";
                }
            }

            DeviceType FromString(const Crt::String &str)
            {
                size_t hash = Crt::HashString(str.c_str());

                if (hash == THING_HASH)
                {
                    return DeviceType::THING;
                }

                if (hash == CLIENT_HASH)
                {
                    return DeviceType::CLIENT;
                }

                assert(0);
                return static_cast<DeviceType>(-1);
            }
        } // namespace DeviceTypeMarshaller
    } // namespace Iotcommands
} // namespace Aws
