#include "ArgumentsParser.h"

using namespace std;

namespace gbxcommons
{

void ArgumentsParser::Parse(char** arguments, int count)
{
    _parsedOptions.resize(0);

    for (auto i = 0; i < count; ++i)
    {
        auto match = find_if(begin(_registeredOptions), end(_registeredOptions), 
        [&](const CommandLineOption& c)
        {
            if (c.ShortVersion.compare(arguments[i]) == 0 || c.LongVersion.compare(arguments[i]) == 0)
                return true;
            return false;
        });

        if (match != end(_registeredOptions))
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
    {
        if (requiredOption.Requirement == OptionRequirement::Required)
        {
            auto match = find_if(begin(_parsedOptions), end(_parsedOptions), 
            [&](const ParsedOption& c)
            {
                if (c.ShortVersion.compare(requiredOption.ShortVersion) == 0 || c.LongVersion.compare(requiredOption.LongVersion) == 0)
                    return true;
                return false;
            });     

            if (match == end(_parsedOptions))
            {
                stringstream ss;
                ss << "Option " << requiredOption.ShortVersion << '/' << requiredOption.LongVersion << " is required";
                throw ArgumentsParserException(ss.str());
            }
        }
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

}