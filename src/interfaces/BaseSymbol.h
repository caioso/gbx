#pragma once

#include <cstdlib>
#include <string>

namespace gbxasm::interfaces
{

class BaseSymbol
{
public:
    BaseSymbol(std::string, size_t);
    virtual ~BaseSymbol() = default;

    size_t LineNumber();
    std::string RawToken();
    
    virtual void Process() = 0;

protected:
    size_t _lineNumber;
    std::string _rawToken;

};

}