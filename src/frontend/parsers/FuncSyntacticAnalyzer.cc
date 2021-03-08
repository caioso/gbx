#include "FuncSyntacticAnalyzer.h"

using namespace gbxasm;
using namespace gbxasm::frontend;
using namespace std;

namespace gbxasm::frontend::parsers
{

shared_ptr<gbxasm::intermediate_representation::IntermediateRepresentation> FuncSyntacticAnalyzer::TryToAccept(vector<Token>::iterator& beginIt, vector<Token>::iterator& endIt)
{
    ExtractSymbols(beginIt, endIt);
    return {};
}

void FuncSyntacticAnalyzer::ExtractSymbols(vector<Token>::iterator& beginIt, vector<Token>::iterator& endIt)
{
    auto countEnds = CountEndWithinFunctionBody(beginIt, endIt);
    cout << "End: " << countEnds << '\n';
}

size_t FuncSyntacticAnalyzer::CountEndWithinFunctionBody(vector<Token>::iterator& beginIt, vector<Token>::iterator& endIt)
{
    auto counter = 0;
    auto position = 0;
    for (auto it = beginIt; it != endIt; ++it)
    {
        if ((*it).Type == TokenType::KeywordFUNC || (*it).Type == TokenType::KeywordREPT || 
            (*it).Type == TokenType::KeywordWHEN || (*it).Type == TokenType::KeywordTRY ||
            (*it).Type == TokenType::KeywordIF ||  (*it).Type == TokenType::KeywordPACK)
            counter++;

        if ((*it).Type == TokenType::KeywordEND)
            counter--;

        position++;

        if (counter == 0)
            break;
    }

    return position;
}

}