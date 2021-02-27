#include "ArgumentsParser.h"

using namespace std;

namespace gbxcommons
{

ArgumentsParser::ArgumentsParser(string usage)
    : _usage(usage)
{}

void ArgumentsParser::Parse(char** arguments, int count)
{
    _parsedOptions.resize(0);

    for (auto i = 0; i < count; ++i)
    {
        if (auto match = FindRegisteredOption(arguments[i]);
            match != end(_registeredOptions))
        {
            // Evaluate option
            ParsedOption option = 
            {
                .ShortVersion = (*match).ShortVersion,
                .LongVersion = (*match).LongVersion,
                .Value = nullopt,
            };

            if ((*match).Type == OptionType::Pair)
            {
                if (i == count -1)
                {
                    stringstream ss;
                    ss << "Option " << (*match).ShortVersion << '/' << (*match).LongVersion << " requires a value";
                    throw ArgumentsParserException(ss.str());
                }
                
                auto value = arguments[++i];
                option.Value = make_optional<string>(string(value));
            }

            _parsedOptions.push_back(option);
        }
    }

    CheckForMandatoryOptions();
}

void ArgumentsParser::CheckForMandatoryOptions()
{
    for (auto requiredOption : _registeredOptions)
        if (requiredOption.Requirement == OptionRequirement::Required)
            if (FindParsedOption(requiredOption) == end(_parsedOptions))
            {
                stringstream ss;
                ss << "Option " << requiredOption.ShortVersion << '/' << requiredOption.LongVersion << " is required";
                throw ArgumentsParserException(ss.str());
            }
}

void ArgumentsParser::RegisterOption(string shortVerison, 
                                     string longVersion, 
                                     string description, 
                                     OptionType type, 
                                     OptionRequirement requirement)
{

    CommandLineOption value = 
    {
        .ShortVersion = shortVerison,
        .LongVersion = longVersion,
        .Description = description,
        .Type = type,
        .Requirement = requirement
    };

    _registeredOptions.push_back(value);
}

bool ArgumentsParser::HasBeenFound(string option)
{
    auto match = find_if(begin(_parsedOptions), end(_parsedOptions), 
    [&](const ParsedOption& c)
    {
        if (c.ShortVersion.compare(option) == 0 || c.LongVersion.compare(option) == 0)
            return true;
        return false;
    });

    return match != end(_parsedOptions);
}

ParsedOption ArgumentsParser::RetrieveOption(string option)
{
    auto match = find_if(begin(_parsedOptions), end(_parsedOptions), 
    [&](const ParsedOption& c)
    {
        if (c.ShortVersion.compare(option) == 0 || c.LongVersion.compare(option) == 0)
            return true;
        return false;
    });

    if (match == end(_parsedOptions))
    {
        stringstream ss;
        ss << "Option " << option << " has not been parsed";
        throw ArgumentsParserException(ss.str());
    }

    return *match;
}

std::string ArgumentsParser::Help()
{
    auto header = GenerateHeader();    
    auto longestOption = FindLongestOptionLength();
    auto optionLines = GenerateOptionLines(longestOption);
    auto helpLine = GenerateHelpLine(longestOption);
    return header + optionLines + helpLine;
}

string ArgumentsParser::GenerateHeader()
{
    stringstream header;
    header << "USAGE: " << _usage << '\n' << GenerateSeparator(header.str().size()) << "\nOPTIONS:\n";
    return header.str();
}

string ArgumentsParser::GenerateSeparator(size_t size)
{
    stringstream separator;
    for (auto i = 0llu; i < size - 1; ++i)
        separator << '-';

    return separator.str();
}

size_t ArgumentsParser::FindLongestOptionLength()
{
    auto longestOption = string("-h/--help").size();
    for (auto option : _registeredOptions)
    {
        auto result = option.ShortVersion + '/' + option.LongVersion;
        longestOption = max(longestOption, result.size());
    }
    return longestOption;
}

string ArgumentsParser::GenerateOptionLines(size_t longestOption)
{
    stringstream optionLines;
    for (auto option : _registeredOptions)
    {
        auto optionPair = GenerateOptionPair(option);
        auto spaces = AddExtraSpaces(longestOption - optionPair.size());

        optionLines << optionPair << spaces << AddValueOrSpaces(option)
                    << option.Description << AddRequiredOrOptional(option);
    }
    return optionLines.str();
}

string ArgumentsParser::GenerateOptionPair(CommandLineOption option)
{
    return option.ShortVersion + '/' + option.LongVersion;
}

string ArgumentsParser::AddExtraSpaces(size_t length)
{
    auto spaces = string("");
    for (auto i = 0llu; i < length; ++i)
        spaces += ' ';
    return spaces;
}

string ArgumentsParser::AddValueOrSpaces(CommandLineOption option)
{
    auto optionLines = string("");
    if (option.Type == OptionType::Pair)
        optionLines += " <value> ";
    else
        optionLines += "         ";
    return optionLines;
}

string ArgumentsParser::AddRequiredOrOptional(CommandLineOption option)
{
    auto optionLines = string("");
    if (option.Requirement == OptionRequirement::Optional)
        optionLines += " (optional)\n";
    else
        optionLines += " (required)\n";
    return optionLines;
}

string ArgumentsParser::GenerateHelpLine(size_t longestOption)
{
    auto helpLine = string("\n-h/--help");
    for (auto i = 0llu; i < longestOption - string("-h/--help").size(); ++i)
            helpLine += ' ';
    helpLine += "         Display available options";
    return helpLine;
}

vector<CommandLineOption>::iterator ArgumentsParser::FindRegisteredOption(string argument)
{
    auto match = find_if(begin(_registeredOptions), end(_registeredOptions), 
    [&](const CommandLineOption& c)
    {
        if (c.ShortVersion.compare(argument) == 0 || c.LongVersion.compare(argument) == 0)
            return true;
        return false;
    });

    return match;
}

vector<ParsedOption>::iterator ArgumentsParser::FindParsedOption(CommandLineOption option)
{
    auto match = find_if(begin(_parsedOptions), end(_parsedOptions), 
    [&](const ParsedOption& c)
    {
        if (c.ShortVersion.compare(option.ShortVersion) == 0 || c.LongVersion.compare(option.LongVersion) == 0)
            return true;
        return false;
    });    

    return match;
}

}