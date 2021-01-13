#include "Symbol.h"

namespace gbxasm
{

Symbol MakeSymbol(SymbolType type, std::size_t line)
{
    return { .Type = type, .Line = line };
}

}