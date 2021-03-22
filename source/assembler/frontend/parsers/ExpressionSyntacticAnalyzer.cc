#include "ExpressionSyntacticAnalyzer.h"

using namespace gbxasm;
using namespace gbxasm::intermediate_representation;
using namespace std;

namespace gbxasm::frontend::parsers
{

shared_ptr<IntermediateRepresentation> ExpressionSyntacticAnalyzer::TryToAccept(vector<Token>::iterator& beginIt, vector<Token>::iterator& endIt)
{
    ExtractSymbols(beginIt, endIt);

    Accept();

    ExpressionMember member;
    stack<ExpressionMember> expressionStack;
    expressionStack.push(member);
    expressionStack.push(member);
    expressionStack.push(member);

    auto representation = make_shared<ExpressionIntermediateRepresentation>(expressionStack, 1, 1);
    return representation;
}

void ExpressionSyntacticAnalyzer::ExtractSymbols(std::vector<Token>::iterator&, std::vector<Token>::iterator&)
{

}

}