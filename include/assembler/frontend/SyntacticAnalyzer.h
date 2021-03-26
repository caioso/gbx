#pragma once

#include <vector>

#include "IntermediateRepresentation.h"
#include "ConstructionSyntacticAnalyzer.h"
#include "PACKSyntacticAnalyzer.h"

#include "Token.h"

namespace gbxasm::frontend
{

class SyntacticAnalyzer
{
public:
    SyntacticAnalyzer() = default;
    ~SyntacticAnalyzer() = default;

    SyntacticAnalyzer(const SyntacticAnalyzer&) = default;
    SyntacticAnalyzer(SyntacticAnalyzer&&) = default;
    SyntacticAnalyzer& operator=(const SyntacticAnalyzer&) = default;
    SyntacticAnalyzer& operator=(SyntacticAnalyzer&&) = default;

    void Parse(std::vector<Token>);
    
    std::vector<std::shared_ptr<gbxasm::intermediate_representation::IntermediateRepresentation> > AcceptedStructures();

private:
    std::shared_ptr<interfaces::ConstructionSyntacticAnalyzer> ChooseParser(std::vector<Token>::iterator);
    std::vector<std::shared_ptr<gbxasm::intermediate_representation::IntermediateRepresentation>> _intermediateRepresentations;
};

}