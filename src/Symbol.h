#pragma once

#include <stdlib.h>

namespace gbxasm
{

enum class SymbolType
{
    Label,
    Unknown,
};

struct Symbol
{
    size_t Line;
    SymbolType Type;
};

Symbol MakeSymbol(SymbolType, std::size_t);

}