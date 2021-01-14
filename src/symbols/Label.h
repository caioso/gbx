#pragma once

#include "../interfaces/BaseSymbol.h"

namespace gbxasm::symbols
{

class Label : public interfaces::BaseSymbol
{
public:
    Label(std::string, size_t);
    virtual ~Label() = default;

    virtual void Process() override;
    std::string Name();

private:
    std::string _name;
};

}