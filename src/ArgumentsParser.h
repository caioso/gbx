#pragma once

#include <iostream>
#include <string>
#include <sstream>

#include "ApplicationOptions.h"
#include "GBXExceptions.h"

namespace gbx
{

struct ApplicationConfiguration
{
    bool IsDebug;
    std::string IPAddress;
    std::string Port;
};

class ArgumentsParser
{
public:
    ArgumentsParser() = default;
    ~ArgumentsParser() = default;

    void Parse(char**, int);
    ApplicationConfiguration Configuration();

private:
    void EvaluateConfiguration();

    ApplicationConfiguration _configuration{};
};

}