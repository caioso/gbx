#pragma once

#include <string>
#include <sstream>

namespace gbx
{

const std::string OptionDebugFlag = "-d";
const std::string OptionDebugFlagExtended = "--debug";
const std::string OptionIPAddressFlag = "-i";
const std::string OptionIPAddressFlagExtended = "--ip";
const std::string OptionPortFlag = "-p";
const std::string OptionPortFlagExtended = "--port";
const std::string OptionHelpFlag = "-h";
const std::string OptionHelpFlagExtended = "--help";

struct ApplicationHelp
{

static std::string Help();

};

}