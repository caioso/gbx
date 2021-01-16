#include "Number.h"

using namespace gbxasm::interfaces;
using namespace std;

namespace gbxasm::symbols
{

Number::Number(string rawToken, size_t line, size_t column)
    : BaseSymbol(rawToken, line, column)
{}

void Number::Process()
{
    _numericValue = 100;
}

uint32_t Number::NumericValue()
{
    return _numericValue;
}

}