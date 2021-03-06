#pragma once

#include <string>

namespace gbxasm::interfaces
{

class Pass
{
public:
    virtual ~Pass() = default;

    virtual void Process(std::string) = 0;
    [[nodiscard]] virtual std::string Result() = 0;
};  

}