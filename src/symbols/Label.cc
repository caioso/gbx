#include "Label.h"

using namespace gbxasm::interfaces;
using namespace std;

namespace gbxasm::symbols
{
    
Label::Label(string rawToken, size_t line, size_t column)
    : BaseSymbol(rawToken, line, column)
{}

void Label::Process()
{
    if (auto labelEnd = _rawToken.find(":");
        labelEnd != string::npos)
        _id = _rawToken.substr(0, labelEnd);
}

string Label::Identifier()
{
    return _id;
}

}