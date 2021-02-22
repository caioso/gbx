#pragma once

#include <vector>

#include "../language/ConstructionPack.h"
#include "../interfaces/ConstructionSyntacticAnalyzer.h"
#include "parsers/PackSyntacticAnalyzer.h"

#include "AcceptedConstruction.h"
#include "Token.h"

namespace gbxasm::frontend
{

class SyntacticAnalyzer
{
public:
    void Parse(std::vector<Token>);
    std::vector<AcceptedConstruction> AcceptedStructures();

private:
    std::shared_ptr<interfaces::ConstructionSyntacticAnalyzer> ChooseParser(std::vector<Token>::iterator);

    std::vector<AcceptedConstruction> _acceptedStructures;
};

}