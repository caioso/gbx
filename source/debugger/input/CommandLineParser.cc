#include "CommandLineParser.h"

using namespace gbxdb;
using namespace gbxdb::interfaces;
using namespace gbxdb::protocol;
using namespace std;

namespace gbxdb::input
{

void CommandLineParser::Initialize()
{
    Register("register-bank", "rb", CommandType::Getter, ClientCommandID::CommandRegisterBankSummary, nullopt);
}

DebugMessage CommandLineParser::Parse(string line)
{
    auto commandAndArguments = SplitLine(line);
    auto command = ParseCommand(commandAndArguments);

    if (command.CommandID == ClientCommandID::CommandRegisterBankSummary)
    {
        DebugMessage message(make_shared<std::array<uint8_t, MaxMessageBufferSize>>());
        (*message.Buffer())[0] = 0xFD;
        (*message.Buffer())[1] = 0xFF;
        return message;
    }

    return make_shared<std::array<uint8_t, MaxMessageBufferSize>>();
}

void CommandLineParser::Register(string longFormat, string shortFormat, CommandType type, ClientCommandID commandID, optional<size_t> numArgs)
{
    LookForDuplicateCommands(longFormat, shortFormat);

    CLICommandTemplate command = 
    {
        .LongFormat = longFormat,
        .ShortFormat = shortFormat,
        .Type = type,
        .CommandID = commandID, 
        .NumberOfArgument = numArgs
    };

    _commands.emplace_back(std::move(command));
}

void CommandLineParser::LookForDuplicateCommands(string longFormat, string shortFormat)
{
    auto position = std::find_if(_commands.begin(), _commands.end(), [&](CLICommandTemplate c) -> auto
    {
        if (c.LongFormat.compare(longFormat) == 0 || c.ShortFormat.compare(shortFormat) == 0 ||
            c.LongFormat.compare(shortFormat) == 0 || c.ShortFormat.compare(longFormat) == 0)
            return true;

        return false;
    });

    if (position != _commands.end())
    {   
        stringstream ss;
        ss << "Command '" << longFormat << "' has already been registered";
        throw CommandLineInputException(ss.str());
    }
}

ParsedCLICommand CommandLineParser::ParseCommand(vector<string> command)
{
    auto position = FindCommand(command[0]);

    if (position == _commands.end())
    {  
        stringstream ss;
        ss << "Unknown command '" << command[0] << "'";
        throw CommandLineInputException(ss.str());
    }

    return GenerateParsedCommand(command, *position);
}

vector<CLICommandTemplate>::iterator CommandLineParser::FindCommand(string command)
{
    return std::find_if(_commands.begin(), _commands.end(), [&](CLICommandTemplate c) -> auto
    {
        if (c.LongFormat.compare(command) == 0 || c.ShortFormat.compare(command) == 0)
            return true;

        return false;
    }); 
}

vector<string> CommandLineParser::SplitLine(string line)
{
    stringstream stream(line);
    vector<string> commandAndArguments;
    auto bit = string("");

    while (stream >> bit)
        commandAndArguments.push_back(bit);

    return commandAndArguments;
}

ParsedCLICommand CommandLineParser::GenerateParsedCommand(vector<string> commandAndArguments, CLICommandTemplate temp)
{
    ParsedCLICommand command;
    command.CommandID = temp.CommandID;
    vector<string> args;

    auto it = (commandAndArguments.begin() + 1);
    for (auto arg = it; arg != commandAndArguments.end(); ++arg)
        args.push_back(*arg);
    
    command.Arguments = make_optional(args);
    return command;
}

}