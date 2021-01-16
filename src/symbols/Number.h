#pragma once

#include "../interfaces/BaseSymbol.h"

namespace gbxasm::symbols
{

class Number : public interfaces::BaseSymbol
{
public:
    Number(std::string, size_t, size_t);
    virtual ~Number() = default;

    virtual void Process() override;
    uint32_t NumericValue();

private:
    uint32_t _numericValue;
};

}