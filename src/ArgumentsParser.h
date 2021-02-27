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
    ArgumentsParser(std::string);
    ~ArgumentsParser() = default;

    void Parse(char**, int);
    void RegisterOption(std::string, std::string, std::string, OptionType, OptionRequirement);
    void CheckForMandatoryOptions();
    std::string Help();

    bool HasBeenFound(std::string);

    ParsedOption RetrieveOption(std::string);

private:
    std::vector<CommandLineOption>::iterator FindRegisteredOption(std::string);
    std::vector<ParsedOption>::iterator FindParsedOption(CommandLineOption);

    std::string GenerateHeader();
    std::string GenerateSeparator(size_t);
    std::string GenerateOptionLines(size_t);
    std::string GenerateOptionPair(CommandLineOption);
    std::string AddExtraSpaces(size_t);
    std::string AddValueOrSpaces(CommandLineOption);
    std::string AddRequiredOrOptional(CommandLineOption);
    std::string GenerateHelpLine(size_t);
    size_t FindLongestOptionLength();

    std::vector<CommandLineOption> _registeredOptions;
    std::vector<ParsedOption> _parsedOptions;
    std::string _usage{};
};

}