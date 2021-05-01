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

void ConstructionSyntacticAnalyzer::Exit()
{
    _isExited = true;
}

bool ConstructionSyntacticAnalyzer::IsExited()
{
    return _isExited;
}

bool ConstructionSyntacticAnalyzer::IsOutOfBonds(int position, size_t size)
{
    return position < 0 || position >= static_cast<int>(size);
}

}