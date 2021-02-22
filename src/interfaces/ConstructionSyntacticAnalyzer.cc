#include "ConstructionSyntacticAnalyzer.h"

using namespace gbxasm::frontend;

namespace gbxasm::interfaces
{

bool ConstructionSyntacticAnalyzer::IsAccepted()
{
    return _isAccepted;
}

void ConstructionSyntacticAnalyzer::Accept()
{
    _isAccepted = true;
}

bool ConstructionSyntacticAnalyzer::IsRejected()
{
    return _isRejected;
}

void ConstructionSyntacticAnalyzer::Reject()
{
    _isRejected = true;
}

}