#pragma once

#include <string>
#include <vector>

#include "DeclaredMember.h"
#include "IntermediateRepresentation.h"

namespace gbxasm::intermediate_representation
{

class PackIntermediateRepresentation : public IntermediateRepresentation
{
public:
    PackIntermediateRepresentation(std::string, std::vector<DeclaredMember>, size_t, size_t);
    ~PackIntermediateRepresentation() = default;

    std::string Identifier();
    std::vector<DeclaredMember>& Members();

private:
    std::string _identifier{};
    std::vector<DeclaredMember> _members{};
};

}