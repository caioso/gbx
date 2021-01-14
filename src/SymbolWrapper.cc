#include "SymbolWrapper.h"

using namespace std;
using namespace gbxasm::interfaces;

namespace gbxasm
{

SymbolWrapper MakeSymbol(SymbolType type, shared_ptr<BaseSymbol> symbol)
{
    return { .Type = type, .Symbol = symbol };
}

}