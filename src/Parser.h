#pragma once

#include <vector>

#include "constructions/ConstructionPack.h"
#include "AcceptedConstruction.h"
#include "Token.h"

namespace gbxasm
{

class Parser
{
public:
    void Parse(std::vector<Token>);
    std::vector<interfaces::AcceptedConstruction> AcceptedStructures();

};

}