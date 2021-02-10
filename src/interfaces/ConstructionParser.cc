#include "ConstructionParser.h"

namespace gbxasm::interfaces
{

bool ConstructionParser::IsAccepted()
{
    return _isAccepted;
}

void ConstructionParser::Accept()
{
    _isAccepted = true;
}

bool ConstructionParser::IsRejected()
{
    return _isRejected;
}

void ConstructionParser::Reject()
{
    _isRejected = true;
}

}