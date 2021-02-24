#include "ArgumentsParser.h"

using namespace std;

namespace gbx
{

void ArgumentsParser::Parse(char** arguments, int count)
{
    _configuration = {};

    for (auto i = 0; i < count; ++i)
    {
        auto option = string(arguments[i]);

        if (option.compare(OptionHelpFlag) == 0 || option.compare(OptionHelpFlagExtended) == 0)
            throw GBXException(ApplicationHelp::Help());
        if (option.compare(OptionVerboseFlag) == 0 || option.compare(OptionVerboseFlagExtended) == 0)
            _configuration.Verbose = true;
        if (option.compare(OptionDebugFlag) == 0 || option.compare(OptionDebugFlagExtended) == 0)
            _configuration.IsDebug = true;
        else if ((option.compare(OptionIPAddressFlag) == 0 && i < count - 1) ||
                 (option.compare(OptionIPAddressFlagExtended) == 0 && i < count - 1))
            _configuration.IPAddress = string(arguments[++i]);
        else if ((option.compare(OptionPortFlag) == 0 && i < count - 1) ||
                 (option.compare(OptionPortFlagExtended) == 0 && i < count - 1))
            _configuration.Port = string(arguments[++i]);
    }

    EvaluateConfiguration();
}

void ArgumentsParser::EvaluateConfiguration()
{
    if (!_configuration.IsDebug && (_configuration.IPAddress.compare("") != 0 || _configuration.Port.compare("") != 0))
        throw GBXException("-d/--debug option expected");
    if (_configuration.IPAddress.compare(OptionDebugFlag) == 0  || _configuration.IPAddress.compare(OptionDebugFlagExtended) == 0 ||
        _configuration.IPAddress.compare(OptionIPAddressFlag) == 0  || _configuration.IPAddress.compare(OptionIPAddressFlagExtended) == 0 ||
        _configuration.IPAddress.compare(OptionPortFlag) == 0  || _configuration.IPAddress.compare(OptionPortFlagExtended) == 0)
    {
        stringstream ss;
        ss << "Invalid IP Address '" << _configuration.IPAddress << "'";
        throw GBXException(ss.str());
    }
    if (_configuration.Port.compare(OptionDebugFlag) == 0  || _configuration.Port.compare(OptionDebugFlagExtended) == 0 ||
        _configuration.Port.compare(OptionIPAddressFlag) == 0  || _configuration.Port.compare(OptionIPAddressFlagExtended) == 0 ||
        _configuration.Port.compare(OptionPortFlag) == 0  || _configuration.Port.compare(OptionPortFlagExtended) == 0)
    {
        stringstream ss;
        ss << "Invalid Port '" << _configuration.Port << "'";
        throw GBXException(ss.str());
    }
    if (_configuration.IsDebug && _configuration.IPAddress.compare("") == 0)
        throw GBXException("-i/--ip option required");
    if (_configuration.IsDebug && _configuration.Port.compare("") == 0)
        throw GBXException("-p/--port option required");
}

ApplicationConfiguration ArgumentsParser::Configuration()
{
    return _configuration;
}

}