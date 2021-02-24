#pragma once

#include <string>

namespace gbx::interfaces
{

class CommunicationChannel
{
public:
    virtual void Initialize() = 0;
    virtual void Send(std::string) = 0;
    virtual void Receive(std::string&) = 0;
};

}