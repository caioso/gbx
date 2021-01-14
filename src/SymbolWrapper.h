#pragma once

#include <memory>
#include <stdlib.h>

#include "interfaces/BaseSymbol.h"

namespace gbxasm
{

enum class SymbolType
{
    Label,
    Unknown,
};

struct SymbolWrapper
{
    SymbolType Type;
    std::shared_ptr<interfaces::BaseSymbol> Symbol;
};

SymbolWrapper MakeSymbol(SymbolType, std::shared_ptr<interfaces::BaseSymbol>);

}