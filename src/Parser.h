#pragma once

#include <vector>

#include "constructions/ConstructionPack.h"
#include "interfaces/ConstructionParser.h"
#include "parsers/PackParser.h"

#include "AcceptedConstruction.h"
#include "Token.h"

namespace gbxasm
{

class Parser
{
public:
    void Parse(std::vector<Token>);
    std::vector<AcceptedConstruction> AcceptedStructures();

private:
    std::shared_ptr<interfaces::ConstructionParser> ChooseParser(std::vector<Token>::iterator);

    std::vector<AcceptedConstruction> _acceptedStructures;
};

}