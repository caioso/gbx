#pragma once

#include <string>
#include <vector>

#include "DeclaredMember.h"
#include "IntermediateRepresentation.h"

namespace gbxasm::intermediate_representation
{

class PACKIntermediateRepresentation : public IntermediateRepresentation
{
public:
    PACKIntermediateRepresentation(std::string, std::vector<DeclaredMember>, size_t, size_t);
    ~PACKIntermediateRepresentation() = default;

    PACKIntermediateRepresentation(const PACKIntermediateRepresentation&) = default;
    PACKIntermediateRepresentation(PACKIntermediateRepresentation&&) = default;
    PACKIntermediateRepresentation& operator=(const PACKIntermediateRepresentation&) = default;
    PACKIntermediateRepresentation& operator=(PACKIntermediateRepresentation&&) = default;

    std::string_view Identifier();
    std::vector<DeclaredMember>& Members();

private:
    std::string _identifier{};
    std::vector<DeclaredMember> _members{};
};

}