#pragma once

#include <algorithm>
#include <iostream>
#include <optional>
#include <string>
#include <sstream>
#include <vector>

#include "ApplicationOptions.h"
#include "GBXCommonsExceptions.h"

namespace gbxcommons
{

struct ApplicationConfiguration
{
    bool Verbose;
    bool IsDebug;
    std::string IPAddress;
    std::string Port;
};

enum class OptionType
{
    Flag,
    Pair
};

enum class OptionRequirement
{
    Optional,
    Required
};

struct CommandLineOption
{
    std::string ShortVersion;
    std::string LongVersion;
    std::string Description;
    OptionType Type;
    OptionRequirement Requirement;
};

struct ParsedOption
{
    std::string ShortVersion;
    std::string LongVersion;
    std::optional<std::string> Value;
};

class ArgumentsParser
{
public:
    ArgumentsParser() = default;
    ~ArgumentsParser() = default;

    void Parse(char**, int);
    void RegisterOption(std::string, std::string, std::string, OptionType, OptionRequirement);
    void CheckForMandatoryOptions();
    
    bool HasBeenFound(std::string);

    ParsedOption RetrieveOption(std::string);

private:
    std::vector<CommandLineOption> _registeredOptions;
    std::vector<ParsedOption> _parsedOptions;
};

}