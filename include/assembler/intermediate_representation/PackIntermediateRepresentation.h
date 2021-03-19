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

    PackIntermediateRepresentation(const PackIntermediateRepresentation&) = default;
    PackIntermediateRepresentation(PackIntermediateRepresentation&&) = default;
    PackIntermediateRepresentation& operator=(const PackIntermediateRepresentation&) = default;
    PackIntermediateRepresentation& operator=(PackIntermediateRepresentation&&) = default;

    std::string_view Identifier();
    std::vector<DeclaredMember>& Members();

private:
    std::string _identifier{};
    std::vector<DeclaredMember> _members{};
};

}