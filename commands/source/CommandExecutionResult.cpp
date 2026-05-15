/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotcommands/CommandExecutionResult.h>

namespace Aws
{
    namespace Iotcommands
    {

        void CommandExecutionResult::LoadFromObject(CommandExecutionResult &val, const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("s"))
            {
                val.S = doc.GetString("s");
            }

            if (doc.ValueExists("b"))
            {
                val.B = doc.GetBool("b");
            }

            if (doc.ValueExists("bin"))
            {
                val.Bin = Aws::Crt::Base64Decode(doc.GetString("bin"));
            }
        }

        void CommandExecutionResult::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (S)
            {
                object.WithString("s", *S);
            }

            if (B)
            {
                object.WithBool("b", *B);
            }

            if (Bin)
            {
                object.WithString("bin", Aws::Crt::Base64Encode(*Bin));
            }
        }

        CommandExecutionResult::CommandExecutionResult(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        CommandExecutionResult &CommandExecutionResult::operator=(const Crt::JsonView &doc)
        {
            *this = CommandExecutionResult(doc);
            return *this;
        }

    } // namespace Iotcommands
} // namespace Aws
