#pragma once

#include <string>

namespace gbxasm::interfaces
{

class Pass
{
public:
    virtual void Process(std::string) = 0;
    virtual std::string Result() = 0;
};  

}