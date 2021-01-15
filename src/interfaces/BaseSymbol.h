#pragma once

#include <cstdlib>
#include <string>

namespace gbxasm::interfaces
{

class BaseSymbol
{
public:
    BaseSymbol(std::string, size_t, size_t);
    virtual ~BaseSymbol() = default;

    size_t Line();
    size_t Column();
    std::string RawToken();
    
    virtual void Process() = 0;

protected:
    size_t _line;
    size_t _column;
    std::string _rawToken;

};

}