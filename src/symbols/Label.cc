#include "Label.h"

using namespace gbxasm::interfaces;
using namespace std;

namespace gbxasm::symbols
{
    
Label::Label(string rawToken, size_t lineNumber)
    : BaseSymbol(rawToken, lineNumber)
{}

void Label::Process()
{
    _name = "label";
}

string Label::Name()
{
    return _name;
}

}