/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include "CommandLineUtils.h"
#include <iostream>
#include <aws/crt/Types.h>

namespace Utils
{
    void CommandLineUtils::RegisterProgramName(Aws::Crt::String NewProgramName)
    {
        ProgramName = NewProgramName;
    }

    void CommandLineUtils::RegisterCommand(CommandLineOption option)
    {
        if (RegisteredCommands.count(option.CommandName))
        {
            fprintf(stdout, "Cannot register command: %s: Command already registered!", option.CommandName.c_str());
            return;
        }
        RegisteredCommands.insert({option.CommandName, option});
    }

    void CommandLineUtils::RegisterCommand(Aws::Crt::String CommandName, Aws::Crt::String ExampleInput, Aws::Crt::String HelpOutput)
    {
        RegisterCommand(CommandLineOption(CommandName, ExampleInput, HelpOutput));
    }

    void CommandLineUtils::RemoveCommand(Aws::Crt::String CommandName)
    {
        if (RegisteredCommands.count(CommandName))
        {
            RegisteredCommands.erase(CommandName);
        }
    }

    void CommandLineUtils::UpdateCommandHelp(Aws::Crt::String CommandName, Aws::Crt::String NewCommandHelp)
    {
        if (RegisteredCommands.count(CommandName))
        {
            RegisteredCommands.at(CommandName).HelpOutput = NewCommandHelp;
        }
    }

    void CommandLineUtils::SendArguments(char** begin, char** end)
    {
        if (BeginPosition != nullptr || EndPosition != nullptr)
        {
            fprintf(stdout, "Arguments already sent!");
            return;
        }
        BeginPosition = begin;
        EndPosition = end;
    }

    bool CommandLineUtils::HasCommand(Aws::Crt::String command)
    {
        return std::find(BeginPosition, EndPosition, "--" + command) != EndPosition;
    }

    Aws::Crt::String CommandLineUtils::GetCommand(Aws::Crt::String command)
    {
        char **itr = std::find(BeginPosition, EndPosition, "--" + command);
        if (itr != EndPosition && ++itr != EndPosition)
        {
            return Aws::Crt::String(*itr);
        }
        return "";
    }

    Aws::Crt::String CommandLineUtils::GetCommandOrDefault(Aws::Crt::String command, Aws::Crt::String CommandDefault)
    {
        if (HasCommand(command))
        {
            return Aws::Crt::String(GetCommand(command));
        }
        return CommandDefault;
    }

    Aws::Crt::String CommandLineUtils::GetCommandRequired(Aws::Crt::String command, Aws::Crt::String OptionalAdditionalMessage)
    {
        if (HasCommand(command))
        {
            return GetCommand(command);
        }
        PrintHelp();
        fprintf(stderr, "Missing required argument: %s\n", command.c_str());
        if (OptionalAdditionalMessage != "")
        {
            fprintf(stderr, "%s\n", OptionalAdditionalMessage.c_str());
        }
        exit(-1);
        return Aws::Crt::String();
    }

    void CommandLineUtils::PrintHelp()
    {
        fprintf(stdout, "Usage:\n");
        fprintf(stdout, "%s", ProgramName.c_str());

        for (auto const& pair : RegisteredCommands)
        {
            fprintf(stdout, " --%s %s", pair.first.c_str(), pair.second.ExampleInput.c_str());
        }

        fprintf(stdout, "\n\n");

        for (auto const& pair : RegisteredCommands)
        {
            fprintf(stdout, "* %s:\t\t%s\n", pair.first.c_str(), pair.second.HelpOutput.c_str());
        }

        fprintf(stdout, "\n");
    }

    void CommandLineUtils::AddCommonMQTTCommands()
    {
        RegisterCommand(Utils::CommandLineOption("endpoint", "<endpoint>", "The endpoint of the mqtt server not including a port."));
        RegisterCommand(Utils::CommandLineOption("key", "<path to key>", "Path to your key in PEM format."));
        RegisterCommand(Utils::CommandLineOption("cert", "<path to cert>", "Path to your client certificate in PEM format."));
        RegisterCommand(Utils::CommandLineOption("ca_file", "<ca file>", "Path to AmazonRootCA1.pem (optional, system trust store used by default)."));
    }
}