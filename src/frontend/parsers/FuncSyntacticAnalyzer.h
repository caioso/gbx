#pragma once

#include <iostream>
#include <memory>
#include <vector>

#include "../../interfaces/ConstructionSyntacticAnalyzer.h"
#include "../Token.h"

namespace gbxasm::frontend::parsers
{

class FuncSyntacticAnalyzer : public interfaces::ConstructionSyntacticAnalyzer
{
public:
    FuncSyntacticAnalyzer() = default;
    virtual ~FuncSyntacticAnalyzer() = default;

    virtual std::shared_ptr<gbxasm::intermediate_representation::IntermediateRepresentation> TryToAccept(std::vector<Token>::iterator&, std::vector<Token>::iterator&) override;

private:
    void ExtractSymbols(std::vector<Token>::iterator&, std::vector<Token>::iterator&);
    size_t CountEndWithinFunctionBody(std::vector<Token>::iterator&, std::vector<Token>::iterator&);
};

}