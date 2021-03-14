#pragma once

#include <memory>
#include <vector>

#include "IntermediateRepresentation.h"
#include "Token.h"
#include "Construction.h"

namespace gbxasm::interfaces
{

class ConstructionSyntacticAnalyzer
{
public:
    ConstructionSyntacticAnalyzer() = default;
    virtual ~ConstructionSyntacticAnalyzer() = default;
    virtual std::shared_ptr<gbxasm::intermediate_representation::IntermediateRepresentation> TryToAccept(std::vector<frontend::Token>::iterator&, std::vector<frontend::Token>::iterator&) = 0;

    ConstructionSyntacticAnalyzer(const ConstructionSyntacticAnalyzer&) = default;
    ConstructionSyntacticAnalyzer(ConstructionSyntacticAnalyzer&&) = default;
    ConstructionSyntacticAnalyzer& operator=(const ConstructionSyntacticAnalyzer&) = default;
    ConstructionSyntacticAnalyzer& operator=(ConstructionSyntacticAnalyzer&&) = default;

    bool IsAccepted();
    bool IsRejected();

protected:
    void Accept();
    void Reject();

    bool _isAccepted{};
    bool _isRejected{};
};

}