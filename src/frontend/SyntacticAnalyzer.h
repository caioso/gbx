#pragma once

#include <vector>

#include "../intermediate-representation/IntermediateRepresentation.h"
#include "../language/ConstructionPack.h"
#include "../interfaces/ConstructionSyntacticAnalyzer.h"
#include "parsers/PackSyntacticAnalyzer.h"

#include "Token.h"

namespace gbxasm::frontend
{

class SyntacticAnalyzer
{
public:
    void Parse(std::vector<Token>);
    std::vector<std::shared_ptr<gbxasm::intermediate_representation::IntermediateRepresentation> > AcceptedStructures();

private:
    std::shared_ptr<interfaces::ConstructionSyntacticAnalyzer> ChooseParser(std::vector<Token>::iterator);
    std::vector<std::shared_ptr<gbxasm::intermediate_representation::IntermediateRepresentation>> _intermediateRepresentations;
};

}