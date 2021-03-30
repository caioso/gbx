#pragma once

#include <optional>
#include <string>
#include <sstream>
#include <vector>

#include <iostream>

#include "GBXDebuggerExceptions.h"
#include "CommandID.h"
#include "DebugMessage.h"

namespace gbxdb::input
{

enum class CommandType
{
    Getter = 0,
    Setter = 1,
    Action = 2
};

struct CLICommandTemplate
{
    std::string LongFormat;
    std::string ShortFormat;

    CommandType Type;
    gbxdb::protocol::CommandID CommandID;

    std::optional<size_t> NumberOfArgument;
};

struct ParsedCLICommand
{
    gbxdb::protocol::CommandID CommandID;
    std::optional<std::vector<std::string>> Arguments;
};

class CommandLineParser
{
public:
    CommandLineParser() = default;
    virtual ~CommandLineParser() = default;

    void Initialize();
    interfaces::DebugMessage Parse(std::string);

    CommandLineParser(const CommandLineParser&) = default;
    CommandLineParser(CommandLineParser&&) = default;
    CommandLineParser& operator=(const CommandLineParser&) = default;
    CommandLineParser& operator=(CommandLineParser&&) = default;

private:
    ParsedCLICommand GenerateParsedCommand(std::vector<std::string>, CLICommandTemplate) noexcept(false);
    std::vector<CLICommandTemplate>::iterator FindCommand(std::string);
    void LookForDuplicateCommands(std::string, std::string) noexcept(false);
    ParsedCLICommand ParseCommand(std::vector<std::string>) noexcept(false);
    void Register(std::string, std::string, CommandType, gbxdb::protocol::CommandID, std::optional<size_t>);
    std::vector<std::string> SplitLine(std::string);

    std::vector<CLICommandTemplate> _commands;
};

}