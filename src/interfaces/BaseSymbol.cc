#include "BaseSymbol.h"

using namespace std;

namespace gbxasm::interfaces
{

BaseSymbol::BaseSymbol(string rawToken, size_t lineNumber)
    : _lineNumber(lineNumber)
    , _rawToken(rawToken)
{}

size_t BaseSymbol::LineNumber()
{
    return _lineNumber;
}

string BaseSymbol::RawToken()
{
    return _rawToken;
}

}