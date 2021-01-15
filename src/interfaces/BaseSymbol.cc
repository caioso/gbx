#include "BaseSymbol.h"

using namespace std;

namespace gbxasm::interfaces
{

BaseSymbol::BaseSymbol(string rawToken, size_t line, size_t column)
    : _line(line)
    , _column(column)
    , _rawToken(rawToken)
{}

size_t BaseSymbol::Line()
{
    return _line;
}

size_t BaseSymbol::Column()
{
    return _column;
}

string BaseSymbol::RawToken()
{
    return _rawToken;
}

}