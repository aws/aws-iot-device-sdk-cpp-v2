/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include "CommandLineUtils.h"
#include <aws/crt/Types.h>
#include <iostream>

namespace Utils
{
    void CommandLineUtils::RegisterProgramName(Aws::Crt::String newProgramName)
    {
        m_programName = std::move(newProgramName);
    }

    void CommandLineUtils::RegisterCommand(CommandLineOption option)
    {
        if (m_registeredCommands.count(option.m_commandName))
        {
            fprintf(stdout, "Cannot register command: %s: Command already registered!", option.m_commandName.c_str());
            return;
        }
        m_registeredCommands.insert({option.m_commandName, option});
    }

    void CommandLineUtils::RegisterCommand(
        Aws::Crt::String commandName,
        Aws::Crt::String exampleInput,
        Aws::Crt::String helpOutput)
    {
        RegisterCommand(CommandLineOption(commandName, exampleInput, helpOutput));
    }

    void CommandLineUtils::RemoveCommand(Aws::Crt::String commandName)
    {
        if (m_registeredCommands.count(commandName))
        {
            m_registeredCommands.erase(commandName);
        }
    }

    void CommandLineUtils::UpdateCommandHelp(Aws::Crt::String commandName, Aws::Crt::String newCommandHelp)
    {
        if (m_registeredCommands.count(commandName))
        {
            m_registeredCommands.at(commandName).m_helpOutput = std::move(newCommandHelp);
        }
    }

    void CommandLineUtils::SendArguments(const char **argv, const char **argc)
    {
        if (m_beginPosition != nullptr || m_endPosition != nullptr)
        {
            fprintf(stdout, "Arguments already sent!");
            return;
        }
        m_beginPosition = argv;
        m_endPosition = argc;
    }

    bool CommandLineUtils::HasCommand(Aws::Crt::String command)
    {
        return std::find(m_beginPosition, m_endPosition, "--" + command) != m_endPosition;
    }

    Aws::Crt::String CommandLineUtils::GetCommand(Aws::Crt::String command)
    {
        const char **itr = std::find(m_beginPosition, m_endPosition, "--" + command);
        if (itr != m_endPosition && ++itr != m_endPosition)
        {
            return Aws::Crt::String(*itr);
        }
        return "";
    }

    Aws::Crt::String CommandLineUtils::GetCommandOrDefault(Aws::Crt::String command, Aws::Crt::String commandDefault)
    {
        if (HasCommand(command))
        {
            return Aws::Crt::String(GetCommand(command));
        }
        return commandDefault;
    }

    Aws::Crt::String CommandLineUtils::GetCommandRequired(
        Aws::Crt::String command,
        Aws::Crt::String optionalAdditionalMessage)
    {
        if (HasCommand(command))
        {
            return GetCommand(command);
        }
        PrintHelp();
        fprintf(stderr, "Missing required argument: --%s\n", command.c_str());
        if (optionalAdditionalMessage != "")
        {
            fprintf(stderr, "%s\n", optionalAdditionalMessage.c_str());
        }
        exit(-1);
    }

    void CommandLineUtils::PrintHelp()
    {
        fprintf(stdout, "Usage:\n");
        fprintf(stdout, "%s", m_programName.c_str());

        for (auto const &pair : m_registeredCommands)
        {
            fprintf(stdout, " --%s %s", pair.first.c_str(), pair.second.m_exampleInput.c_str());
        }

        fprintf(stdout, "\n\n");

        for (auto const &pair : m_registeredCommands)
        {
            fprintf(stdout, "* %s:\t\t%s\n", pair.first.c_str(), pair.second.m_helpOutput.c_str());
        }

        fprintf(stdout, "\n");
    }

    void CommandLineUtils::AddCommonMQTTCommands()
    {
        RegisterCommand(
            Utils::CommandLineOption("endpoint", "<str>", "The endpoint of the mqtt server not including a port."));
        RegisterCommand(Utils::CommandLineOption("key", "<path>", "Path to your key in PEM format."));
        RegisterCommand(Utils::CommandLineOption("cert", "<path>", "Path to your client certificate in PEM format."));
        RegisterCommand(Utils::CommandLineOption(
            "ca_file", "<path>", "Path to AmazonRootCA1.pem (optional, system trust store used by default)."));
    }
} // namespace Utils
