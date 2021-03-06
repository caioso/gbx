/*#pragma once

#include <string>

#include "interfaces/ServerProtocolParameters.h"

namespace gbx
{

class BoostAsioServerProtocolParameters : public interfaces::ServerProtocolParameters
{
public:
    BoostAsioServerProtocolParameters(std::string, int, bool);
    ~BoostAsioServerProtocolParameters() = default;

    std::string IP();
    int Port();
    bool Verbose();

private:
    std::string _ip;
    int _port;
    bool _verbose;
};

}*/